// SPDX-License-Identifier: GPL-3.0-or-later
#include "arc/estop.h"

#define ARC_PI 3.14159265f

struct arc_estop arc_estop_state;

/* True for a finite x > 0. */
static bool positive(float x)
{
	return x > 0.0f && x - x == 0.0f;
}

float arc_estop_brake_current_a(float decel_mps2, float mass_kg,
				float wheel_diameter_m, float gear_ratio,
				float motor_kv, int wheels)
{
	if (!positive(decel_mps2) || !positive(mass_kg) ||
	    !positive(wheel_diameter_m) || !positive(gear_ratio) ||
	    !positive(motor_kv) || wheels <= 0) {
		return 0.0f;
	}

	const float wheel_torque_nm =
		mass_kg * decel_mps2 / (float)wheels * (wheel_diameter_m / 2.0f);
	const float kt_nm_per_a = 60.0f / (2.0f * ARC_PI * motor_kv);
	const float current_a = wheel_torque_nm / gear_ratio / kt_nm_per_a;

	return positive(current_a) ? current_a : 0.0f;
}

float arc_estop_target_brake_current_a(void)
{
	return arc_estop_brake_current_a(ARC_ESTOP_DECEL_MPS2_TBC,
					 ARC_ESTOP_CAR_MASS_KG_TBC,
					 ARC_ESTOP_WHEEL_DIAMETER_M_TBC,
					 ARC_ESTOP_GEAR_RATIO_TBC,
					 ARC_ESTOP_MOTOR_KV_TBC,
					 ARC_ESTOP_BRAKED_WHEELS);
}

void arc_estop_init(struct arc_estop *e)
{
	e->mode = ARC_ESTOP_ENGAGED;
	e->ramp = 0.0f;
	e->quiet_s = 0.0f;
}

bool arc_estop_blocks_commands(const struct arc_estop *e)
{
	return e->mode != ARC_ESTOP_DRIVE;
}

struct arc_estop_output arc_estop_step(struct arc_estop *e, bool line_high,
				       bool command_seen, float dt_s)
{
	struct arc_estop_output out = { true, false, 0.0f };

	/* A corrupted state brakes, like an engaged one. */
	if (e->mode != ARC_ESTOP_RELEASED && e->mode != ARC_ESTOP_DRIVE &&
	    e->mode != ARC_ESTOP_TIMEOUT) {
		e->mode = ARC_ESTOP_ENGAGED;
	}
	if (!(e->ramp >= 0.0f && e->ramp <= 1.0f)) {
		e->ramp = 1.0f;
	}
	/* A corrupted quiet time counts as timed out. */
	if (!(e->quiet_s >= 0.0f && e->quiet_s <= ARC_ESTOP_COMMAND_TIMEOUT_S_TBC)) {
		e->quiet_s = ARC_ESTOP_COMMAND_TIMEOUT_S_TBC;
	}

	if (!line_high) {
		if (e->mode == ARC_ESTOP_DRIVE) {
			e->ramp = 0.0f;
		}
		e->mode = ARC_ESTOP_ENGAGED;
	} else if (e->mode == ARC_ESTOP_ENGAGED) {
		/* Released this step: a command seen in the same step may have
		 * arrived before the release, so it doesn't count. */
		e->mode = ARC_ESTOP_RELEASED;
	} else if ((e->mode == ARC_ESTOP_RELEASED || e->mode == ARC_ESTOP_TIMEOUT) &&
		   command_seen) {
		e->mode = ARC_ESTOP_DRIVE;
		e->ramp = 0.0f;
		e->quiet_s = 0.0f;
	} else if (e->mode == ARC_ESTOP_DRIVE) {
		/* The command timeout (ADR-0035) */
		if (command_seen) {
			e->quiet_s = 0.0f;
		} else if (positive(dt_s)) {
			e->quiet_s += dt_s;
		}
		if (!(e->quiet_s < ARC_ESTOP_COMMAND_TIMEOUT_S_TBC)) {
			e->mode = ARC_ESTOP_TIMEOUT;
			e->ramp = 0.0f;
		}
	}

	if (e->mode == ARC_ESTOP_DRIVE) {
		out.block_commands = false;
		return out;
	}

	/* Engaged, released or timed out: the ramp carries on, never back
	 * down, so a release and re-engage (or a press during a timeout)
	 * doesn't drop the brake. */
	if (positive(dt_s)) {
		e->ramp += dt_s / ARC_ESTOP_RAMP_S_TBC;
		if (!(e->ramp <= 1.0f)) {
			e->ramp = 1.0f;
		}
	}

	out.brake = true;
	out.brake_current_a = e->ramp * arc_estop_target_brake_current_a();
	return out;
}

bool arc_estop_engaged(void)
{
	return arc_estop_state.mode != ARC_ESTOP_RELEASED &&
	       arc_estop_state.mode != ARC_ESTOP_DRIVE &&
	       arc_estop_state.mode != ARC_ESTOP_TIMEOUT;
}

bool arc_estop_command_timeout(void)
{
	return arc_estop_state.mode == ARC_ESTOP_TIMEOUT;
}
