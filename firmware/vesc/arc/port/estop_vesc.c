// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * The e-stop routine on the controller (ADR-0012, ADR-0034): reads ESTOP on
 * the Servo/PPM pin (HW_ICU_GPIO/HW_ICU_PIN, PB6 on the A50S) with the
 * internal pull-down, steps arc_estop_state every millisecond, and applies
 * its brake current straight to the motor, past the command gate in
 * mc_interface_try_input(). Safety function: a second reviewer on every
 * change.
 */

#include "arc/estop.h"
#include "arc/estop_port.h"

#include "ch.h"
#include "hal.h"
#include "hw.h"
#include "mc_interface.h"
#include "mcpwm.h"
#include "mcpwm_foc.h"

/* Above upstream's CAN threads (NORMALPRIO + 1), so a stream of commands
 * can't starve it; below the fault-stop thread (HIGHPRIO - 3). */
#define ESTOP_THREAD_PRIO (NORMALPRIO + 2)
#define ESTOP_PERIOD_MS 1

static volatile bool m_command_seen;
static THD_WORKING_AREA(estop_thread_wa, 512);

/* True if the pin is still a plain input with the pull-down. Upstream's PPM
 * app or servo output would reconfigure it (they must stay off, README). */
static bool estop_pin_configured(void)
{
	const uint32_t shift = HW_ICU_PIN * 2U;

	return ((HW_ICU_GPIO->MODER >> shift) & 3U) == 0U && /* input */
	       ((HW_ICU_GPIO->PUPDR >> shift) & 3U) == 2U;   /* pull-down */
}

static void estop_pin_configure(void)
{
	chSysLock();
	palSetPadMode(HW_ICU_GPIO, HW_ICU_PIN, PAL_MODE_INPUT_PULLDOWN);
	chSysUnlock();
}

static void estop_apply_brake(float current_a)
{
	/* A fault has stopped the motor: upstream keeps it off for
	 * m_fault_stop_time_ms, so the brake doesn't restart the bridge. */
	if (mc_interface_get_fault() != FAULT_CODE_NONE) {
		return;
	}

	switch (mc_interface_get_configuration()->motor_type) {
	case MOTOR_TYPE_BLDC:
	case MOTOR_TYPE_DC:
		mcpwm_set_brake_current(current_a);
		break;

	case MOTOR_TYPE_FOC:
		mcpwm_foc_set_brake_current(current_a);
		break;

	default:
		break;
	}
}

static THD_FUNCTION(estop_thread, arg)
{
	(void)arg;
	chRegSetThreadName("ARC e-stop");

	systime_t last = chVTGetSystemTimeX();

	for (;;) {
		if (!estop_pin_configured()) {
			estop_pin_configure();
		}

		const bool line_high = palReadPad(HW_ICU_GPIO, HW_ICU_PIN) != 0;

		chSysLock();
		const bool command_seen = m_command_seen;
		m_command_seen = false;
		chSysUnlock();

		const systime_t now = chVTGetSystemTimeX();
		const float dt_s = (float)(systime_t)(now - last) / (float)CH_CFG_ST_FREQUENCY;
		last = now;

		const struct arc_estop_output out =
			arc_estop_step(&arc_estop_state, line_high, command_seen, dt_s);

		if (out.brake) {
			estop_apply_brake(out.brake_current_a);
		}

		chThdSleepMilliseconds(ESTOP_PERIOD_MS);
	}
}

void arc_estop_start(void)
{
	estop_pin_configure();
	chThdCreateStatic(estop_thread_wa, sizeof(estop_thread_wa),
			  ESTOP_THREAD_PRIO, estop_thread, NULL);
}

void arc_estop_note_command(void)
{
	m_command_seen = true;
}

bool arc_estop_input_blocked(void)
{
	return arc_estop_blocks_commands(&arc_estop_state);
}
