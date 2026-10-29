// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * E-stop brake routine (ADR-0012, ADR-0034): safety function, so every
 * behaviour the ADRs ask for has a case here. Steps are 1 ms, as on the
 * controller (port/estop_vesc.c).
 */

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "arc/estop.h"

#define DT 0.001f
#define LOW false
#define HIGH true
#define NO_CMD false
#define CMD true

static float target(void)
{
	return arc_estop_target_brake_current_a();
}

static int ramp_steps(void)
{
	return (int)lroundf(ARC_ESTOP_RAMP_S_TBC / DT);
}

static int near(float a, float b)
{
	return fabsf(a - b) <= 1e-3f * (fabsf(b) > 1.0f ? fabsf(b) : 1.0f);
}

/* Brakes and ignores commands; returns the brake current. */
static float braking(struct arc_estop *e, bool line, bool cmd)
{
	struct arc_estop_output out = arc_estop_step(e, line, cmd, DT);

	assert(out.block_commands);
	assert(out.brake);
	assert(out.brake_current_a >= 0.0f);
	assert(out.brake_current_a <= target() * (1.0f + 1e-6f));
	assert(arc_estop_blocks_commands(e));
	return out.brake_current_a;
}

/* Commands drive; the routine applies nothing. */
static void driving(struct arc_estop *e, bool cmd)
{
	struct arc_estop_output out = arc_estop_step(e, HIGH, cmd, DT);

	assert(!out.block_commands);
	assert(!out.brake);
	assert(out.brake_current_a == 0.0f);
	assert(!arc_estop_blocks_commands(e));
	assert(e->mode == ARC_ESTOP_DRIVE);
}

/* From the start state to driving: release, then a fresh command. */
static void drive(struct arc_estop *e)
{
	arc_estop_init(e);
	braking(e, HIGH, NO_CMD);
	driving(e, CMD);
}

static void test_target_brake_current(void)
{
	/* 5 kg x 5 m/s^2 / 4 wheels x 0.05 m / 18 / (60 / (2 pi 4400)) */
	const float expected = 5.0f * 5.0f / 4.0f * 0.05f / 18.0f /
			       (60.0f / (2.0f * 3.14159265f * 4400.0f));

	assert(near(target(), expected));
	assert(target() > 7.9f && target() < 8.1f);

	/* Proportional to deceleration and mass, inverse to gearing. */
	assert(near(arc_estop_brake_current_a(10.0f, 5.0f, 0.1f, 18.0f, 4400.0f, 4),
		    2.0f * target()));
	assert(near(arc_estop_brake_current_a(5.0f, 2.5f, 0.1f, 18.0f, 4400.0f, 4),
		    0.5f * target()));
	assert(near(arc_estop_brake_current_a(5.0f, 5.0f, 0.1f, 9.0f, 4400.0f, 4),
		    2.0f * target()));
	assert(near(arc_estop_brake_current_a(5.0f, 5.0f, 0.1f, 18.0f, 4400.0f, 2),
		    2.0f * target()));

	/* Nonsense in, no brake current out (never negative, never NaN). */
	assert(arc_estop_brake_current_a(0.0f, 5.0f, 0.1f, 18.0f, 4400.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(-5.0f, 5.0f, 0.1f, 18.0f, 4400.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(5.0f, NAN, 0.1f, 18.0f, 4400.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(5.0f, 5.0f, INFINITY, 18.0f, 4400.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(5.0f, 5.0f, 0.1f, 0.0f, 4400.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(5.0f, 5.0f, 0.1f, 18.0f, -1.0f, 4) == 0.0f);
	assert(arc_estop_brake_current_a(5.0f, 5.0f, 0.1f, 18.0f, 4400.0f, 0) == 0.0f);
}

/* Before the first step, and zero-initialised: engaged, commands ignored. */
static void test_starts_engaged(void)
{
	struct arc_estop zero;
	struct arc_estop e;

	memset(&zero, 0, sizeof(zero));
	assert(zero.mode == ARC_ESTOP_ENGAGED);
	assert(arc_estop_blocks_commands(&zero));

	arc_estop_init(&e);
	assert(e.mode == ARC_ESTOP_ENGAGED);
	assert(e.ramp == 0.0f);
	assert(arc_estop_blocks_commands(&e));

	/* The controller's own instance, before port/estop_vesc.c steps it. */
	assert(arc_estop_engaged());
	assert(arc_estop_blocks_commands(&arc_estop_state));
}

/* Powering up with ESTOP high still needs a fresh command to move. */
static void test_power_up_waits_for_a_command(void)
{
	struct arc_estop e;

	arc_estop_init(&e);
	for (int i = 0; i < 5000; i++) {
		braking(&e, HIGH, NO_CMD);
	}
	assert(e.mode == ARC_ESTOP_RELEASED);
	driving(&e, CMD);
	driving(&e, NO_CMD);
}

/* An open line reads low (pull-down): from driving, the brake ramps
 * linearly from 0 to the target over the ramp time, then holds. */
static void test_ramp_then_hold(void)
{
	struct arc_estop e;
	const int n = ramp_steps();
	float last = 0.0f;

	drive(&e);
	for (int i = 1; i <= n; i++) {
		const float i_a = braking(&e, LOW, NO_CMD);

		assert(e.mode == ARC_ESTOP_ENGAGED);
		assert(i_a >= last);
		assert(near(i_a, target() * (float)i / (float)n));
		last = i_a;
	}
	assert(near(last, target()));

	/* Not immediate: the first step is a small fraction of the target. */
	drive(&e);
	assert(braking(&e, LOW, NO_CMD) < 0.01f * target());

	/* Half way through the ramp, half the target. */
	for (int i = 1; i < n / 2; i++) {
		braking(&e, LOW, NO_CMD);
	}
	assert(near(e.ramp * target(), 0.5f * target()));
	for (int i = n / 2; i < n; i++) {
		braking(&e, LOW, NO_CMD);
	}

	/* Held at the target well past the stop from top speed (~1.9 s). */
	for (int i = 0; i < 10 * n + 5000; i++) {
		assert(near(braking(&e, LOW, NO_CMD), target()));
	}
}

/* Commands arriving while ESTOP is low are ignored and change nothing. */
static void test_commands_ignored_while_engaged(void)
{
	struct arc_estop e;

	drive(&e);
	for (int i = 0; i < 3 * ramp_steps(); i++) {
		braking(&e, LOW, CMD);
		assert(e.mode == ARC_ESTOP_ENGAGED);
	}
	assert(e.ramp == 1.0f);
}

/* Releasing ESTOP doesn't restart motion: the brake stays on (and keeps
 * ramping) until a fresh command, however long that takes. */
static void test_release_waits_for_a_fresh_command(void)
{
	struct arc_estop e;

	drive(&e);
	for (int i = 0; i < 2 * ramp_steps(); i++) {
		braking(&e, LOW, NO_CMD);
	}

	/* A command in the release step itself may predate the release. */
	assert(near(braking(&e, HIGH, CMD), target()));
	assert(e.mode == ARC_ESTOP_RELEASED);

	for (int i = 0; i < 60000; i++) {
		assert(near(braking(&e, HIGH, NO_CMD), target()));
		assert(e.mode == ARC_ESTOP_RELEASED);
	}

	/* The fresh command: it was ignored when it arrived (the state still
	 * blocked commands); from the step that sees it, commands drive. */
	driving(&e, CMD);
	driving(&e, NO_CMD);
	assert(e.ramp == 0.0f);
}

/* Released part way through the ramp: the brake keeps ramping up. */
static void test_release_mid_ramp_keeps_ramping(void)
{
	struct arc_estop e;
	float last;

	drive(&e);
	for (int i = 0; i < ramp_steps() / 3; i++) {
		braking(&e, LOW, NO_CMD);
	}
	last = e.ramp * target();
	for (int i = 0; i < 2 * ramp_steps(); i++) {
		const float i_a = braking(&e, HIGH, NO_CMD);

		assert(i_a >= last);
		last = i_a;
	}
	assert(near(last, target()));
}

/* A bouncing line (a loose connector) never lowers the brake while it
 * bounces, and lets commands through only after a high step followed by a
 * fresh command. */
static void test_bouncing_line(void)
{
	struct arc_estop e;
	float last = 0.0f;
	unsigned int lfsr = 0xace1u;
	int releases = 0;

	drive(&e);
	braking(&e, LOW, NO_CMD);
	for (int i = 0; i < 20000; i++) {
		const bool line = (lfsr & 1u) != 0u;
		const bool cmd = (lfsr & 6u) == 6u;
		const enum arc_estop_mode before = e.mode;

		lfsr = (lfsr >> 1) ^ (-(lfsr & 1u) & 0xb400u);
		if (before == ARC_ESTOP_DRIVE) {
			/* Let go: engage again and carry on, from a new ramp. */
			driving(&e, cmd);
			last = braking(&e, LOW, cmd);
			releases++;
			continue;
		}
		if (line && cmd && before == ARC_ESTOP_RELEASED) {
			/* The only way out. */
			driving(&e, cmd);
			continue;
		}
		{
			const float i_a = braking(&e, line, cmd);

			assert(i_a >= last);
			last = i_a;
		}
		if (!line) {
			assert(e.mode == ARC_ESTOP_ENGAGED);
		}
	}
	assert(releases > 0);
}

/* One low sample while driving (a glitch) brakes and waits for a command,
 * from a ramp restarted at 0. */
static void test_glitch_while_driving(void)
{
	struct arc_estop e;

	drive(&e);
	assert(braking(&e, LOW, CMD) < 0.01f * target());
	assert(e.mode == ARC_ESTOP_ENGAGED);
	braking(&e, HIGH, CMD);
	assert(e.mode == ARC_ESTOP_RELEASED);
	driving(&e, CMD);
}

/* Re-engaging after a release continues from the brake already applied. */
static void test_reengage_keeps_the_brake(void)
{
	struct arc_estop e;
	float before;

	drive(&e);
	for (int i = 0; i < ramp_steps() / 2; i++) {
		braking(&e, LOW, NO_CMD);
	}
	braking(&e, HIGH, NO_CMD);
	before = e.ramp * target();
	assert(braking(&e, LOW, NO_CMD) >= before);
	assert(e.mode == ARC_ESTOP_ENGAGED);
}

/* Bad time steps never make the brake NaN, negative or above target. */
static void test_bad_dt(void)
{
	struct arc_estop e;
	struct arc_estop_output out;
	const float bad[] = { NAN, -1.0f, -INFINITY, INFINITY, 0.0f };

	for (unsigned int i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
		drive(&e);
		out = arc_estop_step(&e, LOW, NO_CMD, bad[i]);
		assert(out.brake && out.block_commands);
		assert(out.brake_current_a == 0.0f);
		assert(e.ramp == 0.0f);
	}

	/* A long gap (a stalled thread) goes straight to the target. */
	drive(&e);
	out = arc_estop_step(&e, LOW, NO_CMD, 10.0f);
	assert(near(out.brake_current_a, target()));
}

/* A corrupted state brakes. */
static void test_corrupted_state(void)
{
	struct arc_estop e;
	struct arc_estop_output out;

	e.mode = (enum arc_estop_mode)42;
	e.ramp = 0.0f;
	out = arc_estop_step(&e, HIGH, CMD, DT);
	assert(out.brake && out.block_commands);
	assert(e.mode == ARC_ESTOP_RELEASED);

	/* A ramp that isn't in 0..1 while braking goes to the full target. */
	arc_estop_init(&e);
	braking(&e, LOW, NO_CMD);
	e.ramp = NAN;
	out = arc_estop_step(&e, LOW, NO_CMD, DT);
	assert(out.brake && near(out.brake_current_a, target()));

	arc_estop_init(&e);
	e.ramp = -3.0f;
	out = arc_estop_step(&e, LOW, NO_CMD, DT);
	assert(out.brake && near(out.brake_current_a, target()));
}

int main(void)
{
	assert(ARC_ESTOP_RAMP_S_TBC > 0.0f);
	assert(ARC_ESTOP_DECEL_MPS2_TBC > 0.0f);

	test_target_brake_current();
	test_starts_engaged();
	test_power_up_waits_for_a_command();
	test_ramp_then_hold();
	test_commands_ignored_while_engaged();
	test_release_waits_for_a_fresh_command();
	test_release_mid_ramp_keeps_ramping();
	test_bouncing_line();
	test_glitch_while_driving();
	test_reengage_keeps_the_brake();
	test_bad_dt();
	test_corrupted_state();

	puts("arc e-stop: ok");
	return 0;
}
