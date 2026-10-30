// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * E-stop brake routine (ADR-0012, ADR-0034, ICD-corner-connector): reads
 * ESTOP on the Servo/PPM pin with the MCU's pull-down, so an open line is an
 * e-stop. While ESTOP is low it ignores motor commands and ramps brake
 * current to the target deceleration over the ramp time, then holds it until
 * the wheel stops (and after: the power board cuts motor-bus power at T).
 * Releasing ESTOP doesn't restart motion: the brake stays on until a fresh
 * command arrives after the release.
 *
 * The logic here is plain C, tested on the host; port/estop_vesc.c reads the
 * pin, applies the brake and runs it every millisecond on the controller.
 * Safety function: its own tests, and a second reviewer on every change.
 */

#ifndef ARC_ESTOP_H
#define ARC_ESTOP_H

#include <stdbool.h>

/* Target deceleration, m/s^2: TBC ~5 (ADR-0012), the envelope's default
 * acceleration (ADR-0052). Confirmed on the R2 motor bench. */
#define ARC_ESTOP_DECEL_MPS2_TBC 5.0f

/* Time to ramp the brake current from 0 to the target, s: TBC ~150 ms
 * (ADR-0012), so braking isn't immediate. Confirmed on the R2 motor bench. */
#define ARC_ESTOP_RAMP_S_TBC 0.150f

/*
 * What turns the deceleration into a brake current per corner (open loop;
 * see arc_estop_brake_current_a()). Each is TBC until measured; the brake
 * current itself is confirmed on the R2 motor bench.
 */
/* Car mass, kg: TBC 4-5 kg, nominal 4.2 kg; the brake is sized for the upper
 * end (ADR-0057), so the deceleration is at least the target anywhere in that
 * range. */
#define ARC_ESTOP_CAR_MASS_KG_TBC 5.0f
/* Tyre rolling diameter, m: rolling_diameter in config/cars/nominal.yaml. */
#define ARC_ESTOP_WHEEL_DIAMETER_M_TBC 0.100f
/* Motor-to-wheel reduction: gear_ratio_front/rear in config/cars/nominal.yaml. */
#define ARC_ESTOP_GEAR_RATIO_TBC 18.0f
/* Motor kV, rpm/V: the Castle 1010 4400 kV (ADR-0026). Kt is taken from kV
 * for an ideal motor, as in budgets/power-scenarios.md. */
#define ARC_ESTOP_MOTOR_KV_TBC 4400.0f
/* Braked wheels: all four corners brake on the e-stop. */
#define ARC_ESTOP_BRAKED_WHEELS 4

/*
 * Brake current per motor, A, for deceleration decel_mps2 of a car of
 * mass_kg braked by `wheels` motors: the wheel force m*a/wheels, times the
 * wheel radius, divided by the gearing and the motor's torque constant
 * (Kt = 60 / (2*pi*kV)). Ignores rolling resistance and losses, which only
 * add to the deceleration. 0 for any non-positive or non-finite input.
 */
float arc_estop_brake_current_a(float decel_mps2, float mass_kg,
				float wheel_diameter_m, float gear_ratio,
				float motor_kv, int wheels);

/* The target brake current: arc_estop_brake_current_a() of the constants
 * above, ~8 A (TBC). */
float arc_estop_target_brake_current_a(void);

enum arc_estop_mode {
	/* ESTOP is low, or hasn't been read yet: brake, ignore commands. Zero,
	 * so a zero-initialised state is fail-safe. */
	ARC_ESTOP_ENGAGED = 0,
	/* ESTOP released, no fresh command since: keep braking, ignore commands. */
	ARC_ESTOP_RELEASED,
	/* Commands drive the motor; the routine applies nothing. */
	ARC_ESTOP_DRIVE,
};

struct arc_estop {
	enum arc_estop_mode mode;
	/* Fraction of the target brake current reached, 0..1. */
	float ramp;
};

/* What the routine wants for one step. */
struct arc_estop_output {
	/* Ignore every motor command (CAN, USB, apps, upstream's timeout). */
	bool block_commands;
	/* Apply brake_current_a (A, >= 0) to the motor this step. */
	bool brake;
	float brake_current_a;
};

/* The start state: engaged, no brake yet (the ramp starts at 0). */
void arc_estop_init(struct arc_estop *e);

/*
 * One step of the routine. line_high: ESTOP read high (not e-stop).
 * command_seen: a motor command arrived since the last step. dt_s: time
 * since the last step; a negative, NaN or infinite dt counts as 0.
 */
struct arc_estop_output arc_estop_step(struct arc_estop *e, bool line_high,
				       bool command_seen, float dt_s);

/* Motor commands are ignored in this state (anything but DRIVE). */
bool arc_estop_blocks_commands(const struct arc_estop *e);

/* The controller's routine, stepped by port/estop_vesc.c. Zero-initialised:
 * engaged until the first step reads ESTOP high. */
extern struct arc_estop arc_estop_state;

/* True while ESTOP is low or hasn't been read yet (the ESTOP bit of
 * ARC_MOTOR_STATUS, ADR-0035). */
bool arc_estop_engaged(void);

#endif /* ARC_ESTOP_H */
