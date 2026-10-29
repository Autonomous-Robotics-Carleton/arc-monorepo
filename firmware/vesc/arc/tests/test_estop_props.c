// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * E-stop routine (ADR-0012, ADR-0034): properties checked over every input
 * sequence, written from the ADRs and estop.h rather than from estop.c, so
 * they hold whatever the implementation looks like. test_estop.c has the
 * worked cases; this suite is the exhaustive one.
 *
 * Checks don't use assert(): a build with NDEBUG would skip them.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arc/estop.h"

static long failures;

#define CHECK(cond, ...)                                                    \
	do {                                                                \
		if (!(cond)) {                                              \
			if (failures < 20) {                                \
				fprintf(stderr, "%s:%d: %s: ", __FILE__,    \
					__LINE__, #cond);                   \
				fprintf(stderr, __VA_ARGS__);               \
				fputc('\n', stderr);                        \
			}                                                   \
			failures++;                                         \
		}                                                           \
	} while (0)

#define DT 0.001f

static float target(void)
{
	return arc_estop_target_brake_current_a();
}

/*
 * What the ADRs ask for, tracked alongside the routine:
 * - since_low: steps since the line was last low (0 on a low step); at power
 *   on the line hasn't been read, which counts as low.
 * - cmd_after_release: a command arrived on a step after the first high
 *   step of the current release (a command in the release step itself may
 *   have arrived before it, so it doesn't count).
 * - braking_steps: consecutive steps the routine has been braking since it
 *   last drove (or since power on).
 */
struct spec {
	int since_low;
	int cmd_after_release;
	int braking_steps;
	float last_current;
};

static void spec_init(struct spec *s)
{
	s->since_low = 0;
	s->cmd_after_release = 0;
	s->braking_steps = 0;
	s->last_current = 0.0f;
}

/* One step with the line and command given; checks every property. */
static void step_and_check(struct arc_estop *e, struct spec *s, int line_high,
			   int cmd, const char *path)
{
	struct arc_estop_output out = arc_estop_step(e, line_high, cmd, DT);

	if (!line_high) {
		s->since_low = 0;
		s->cmd_after_release = 0;
	} else {
		s->since_low++;
		if (s->since_low >= 2 && cmd) {
			s->cmd_after_release = 1;
		}
	}
	const int should_drive = line_high && s->cmd_after_release;

	/* Safety: a low line always blocks and brakes, in the same step. */
	if (!line_high) {
		CHECK(out.block_commands && out.brake, "low line not braking (%s)", path);
	}
	/* Only a release followed by a fresh command drives, and it always does. */
	CHECK(out.block_commands == !should_drive, "block=%d want %d (%s)",
	      out.block_commands, !should_drive, path);
	/* The gate upstream reads agrees with the step's output. */
	CHECK(arc_estop_blocks_commands(e) == out.block_commands, "gate disagrees (%s)", path);
	/* Blocking and braking go together: never block without holding the motor,
	 * never brake while commands drive. */
	CHECK(out.brake == out.block_commands, "brake=%d block=%d (%s)", out.brake,
	      out.block_commands, path);
	/* The current is finite, never negative, never above the target. */
	CHECK(isfinite(out.brake_current_a) && out.brake_current_a >= 0.0f &&
		      out.brake_current_a <= target() * (1.0f + 1e-5f),
	      "current %g (%s)", (double)out.brake_current_a, path);

	if (out.brake) {
		s->braking_steps++;
		/* The ramp: linear from 0 at ramp_s/dt steps, then held. */
		const float want = fminf((float)s->braking_steps * DT / ARC_ESTOP_RAMP_S_TBC,
					 1.0f) * target();
		CHECK(fabsf(out.brake_current_a - want) <= 1e-3f * target(),
		      "current %g want %g after %d braking steps (%s)",
		      (double)out.brake_current_a, (double)want, s->braking_steps, path);
		/* It never drops while braking, whatever the line does. */
		CHECK(out.brake_current_a >= s->last_current, "current dropped %g -> %g (%s)",
		      (double)s->last_current, (double)out.brake_current_a, path);
		s->last_current = out.brake_current_a;
	} else {
		CHECK(out.brake_current_a == 0.0f, "current while driving (%s)", path);
		s->braking_steps = 0;
		s->last_current = 0.0f;
	}
}

/*
 * Every sequence of (line, command) over `depth` steps, from power on: 4^depth
 * sequences, sharing prefixes. The state is copied at each branch.
 */
static long walked;

static void walk(const struct arc_estop *e, const struct spec *s, int depth,
		 char *path, int at)
{
	if (depth == 0) {
		walked++;
		return;
	}
	for (int in = 0; in < 4; in++) {
		struct arc_estop e2 = *e;
		struct spec s2 = *s;
		const int line_high = in & 1;
		const int cmd = (in >> 1) & 1;

		path[at] = line_high ? (cmd ? 'H' : 'h') : (cmd ? 'L' : 'l');
		path[at + 1] = '\0';
		step_and_check(&e2, &s2, line_high, cmd, path);
		walk(&e2, &s2, depth - 1, path, at + 1);
	}
}

static void test_every_sequence(void)
{
	enum { DEPTH = 11 };
	char path[DEPTH + 2] = "";
	struct arc_estop e;
	struct spec s;

	/* Power on: the state is zeroed, not initialised (it's a static). */
	memset(&e, 0, sizeof(e));
	spec_init(&s);
	walk(&e, &s, DEPTH, path, 0);
	CHECK(walked == 1L << (2 * DEPTH), "walked %ld", walked);

	/* And from arc_estop_init(). */
	walked = 0;
	arc_estop_init(&e);
	spec_init(&s);
	walk(&e, &s, DEPTH, path, 0);
}

/* The global the controller uses: zeroed at boot, so engaged and blocking. */
static void test_global_starts_safe(void)
{
	CHECK(arc_estop_state.mode == ARC_ESTOP_ENGAGED, "mode %d", arc_estop_state.mode);
	CHECK(arc_estop_engaged(), "not engaged at boot");
	CHECK(arc_estop_blocks_commands(&arc_estop_state), "not blocking at boot");
	CHECK(ARC_ESTOP_ENGAGED == 0, "the zero state must be the engaged one");
}

/* Long random runs with random step times, and the global's engaged bit. */
static uint64_t rng = 0x9E3779B97F4A7C15ULL;

static uint32_t rand32(void)
{
	rng ^= rng << 13;
	rng ^= rng >> 7;
	rng ^= rng << 17;
	return (uint32_t)(rng >> 32);
}

static void test_random_runs(void)
{
	struct spec s;
	int prev_drive = 0;

	memset(&arc_estop_state, 0, sizeof(arc_estop_state));
	spec_init(&s);
	for (long i = 0; i < 2000000; i++) {
		/* Mostly long stretches, like a real line, with some bouncing. */
		const int line_high = (rand32() % 100) < 97 ? (int)(i / 5000 % 2) : (int)(rand32() & 1);
		const int cmd = (rand32() % 10) < 3;
		struct arc_estop_output out =
			arc_estop_step(&arc_estop_state, line_high, cmd, DT);

		if (!line_high) {
			CHECK(arc_estop_engaged(), "line low but not engaged at %ld", i);
			CHECK(out.block_commands && out.brake, "line low not braking at %ld", i);
		} else {
			CHECK(!arc_estop_engaged(), "line high but engaged at %ld", i);
		}
		/* From braking, driving needs a high step, then a command after it. */
		if (!prev_drive && !out.block_commands) {
			CHECK(s.since_low >= 1 && cmd, "drove without a release and command at %ld", i);
		}
		s.since_low = line_high ? s.since_low + 1 : 0;
		prev_drive = !out.block_commands;
	}
}

/* Step times the controller could hand over: none may lower or overshoot the
 * brake, or let commands through. */
static void test_odd_step_times(void)
{
	const float dts[] = { 0.0f, -0.001f, -1e30f, 1e-30f, 1e-45f, 1e30f,
			      0.5f, 3600.0f, NAN, -NAN, INFINITY, -INFINITY };

	for (size_t i = 0; i < sizeof(dts) / sizeof(dts[0]); i++) {
		for (int line_high = 0; line_high < 2; line_high++) {
			struct arc_estop e;

			arc_estop_init(&e);
			struct arc_estop_output a = arc_estop_step(&e, false, false, 0.05f);
			struct arc_estop_output b = arc_estop_step(&e, line_high, true, dts[i]);

			CHECK(b.block_commands, "dt %g let commands through", (double)dts[i]);
			CHECK(isfinite(b.brake_current_a), "dt %g: current %g", (double)dts[i],
			      (double)b.brake_current_a);
			CHECK(b.brake_current_a >= a.brake_current_a - 1e-6f,
			      "dt %g lowered the brake %g -> %g", (double)dts[i],
			      (double)a.brake_current_a, (double)b.brake_current_a);
			CHECK(b.brake_current_a <= target() * (1.0f + 1e-5f), "dt %g overshot",
			      (double)dts[i]);
			CHECK(e.ramp >= 0.0f && e.ramp <= 1.0f, "dt %g: ramp %g", (double)dts[i],
			      (double)e.ramp);
			/* estop.h: a negative, NaN or infinite dt counts as 0. */
			if (!(dts[i] > 0.0f) || isinf(dts[i])) {
				CHECK(b.brake_current_a == a.brake_current_a,
				      "dt %g moved the ramp %g -> %g", (double)dts[i],
				      (double)a.brake_current_a, (double)b.brake_current_a);
			}
		}
	}
}

/* Held long enough, the brake reaches the full target and stays there, through
 * releases and re-engages, until a command drives. */
static void test_full_brake_held(void)
{
	struct arc_estop e;
	const int ramp_steps = (int)lroundf(ARC_ESTOP_RAMP_S_TBC / DT);

	arc_estop_init(&e);
	for (int i = 0; i < 100000; i++) {
		/* Low, then released without a command, then bouncing. */
		const int line_high = i < 1000 ? 0 : (i < 50000 ? 1 : (i / 7) % 2);
		struct arc_estop_output out = arc_estop_step(&e, line_high, false, DT);

		CHECK(out.block_commands && out.brake, "step %d", i);
		if (i >= ramp_steps) {
			CHECK(fabsf(out.brake_current_a - target()) <= 1e-5f * target(),
			      "step %d: %g, want the full %g", i, (double)out.brake_current_a,
			      (double)target());
		}
		if (i == ramp_steps - 2) {
			CHECK(out.brake_current_a < target(), "full brake before the ramp time");
		}
	}
}

/* Whatever a memory fault leaves in the state, the next step is safe: a low
 * line brakes, and nothing drives without a release and a fresh command. */
static void test_any_corrupted_state(void)
{
	const float ramps[] = { -1.0f, -0.0f, 0.0f, 0.5f, 1.0f, 1.0000001f, 2.0f,
				1e30f, -1e30f, NAN, INFINITY, -INFINITY };

	for (int mode = -3; mode < 300; mode++) {
		for (size_t r = 0; r < sizeof(ramps) / sizeof(ramps[0]); r++) {
			for (int in = 0; in < 4; in++) {
				struct arc_estop e;
				const int line_high = in & 1;
				const int cmd = (in >> 1) & 1;

				memset(&e, 0, sizeof(e));
				e.mode = (enum arc_estop_mode)mode;
				e.ramp = ramps[r];
				struct arc_estop_output out = arc_estop_step(&e, line_high, cmd, DT);

				CHECK(isfinite(out.brake_current_a) &&
					      out.brake_current_a >= 0.0f &&
					      out.brake_current_a <= target() * (1.0f + 1e-5f),
				      "mode %d ramp %g: current %g", mode, (double)ramps[r],
				      (double)out.brake_current_a);
				if (!line_high) {
					CHECK(out.block_commands && out.brake,
					      "mode %d ramp %g: low line not braking", mode,
					      (double)ramps[r]);
				}
				/* A corrupted ramp while braking brakes fully, not gently
				 * (DRIVE doesn't use the ramp: a press restarts it). */
				if (!(ramps[r] >= 0.0f && ramps[r] <= 1.0f) &&
				    mode != ARC_ESTOP_DRIVE) {
					CHECK(out.brake_current_a == target() || !out.brake,
					      "mode %d ramp %g: current %g, want the full brake",
					      mode, (double)ramps[r], (double)out.brake_current_a);
				}
				/* Only a state that was already DRIVE may drive straight away. */
				if (mode != ARC_ESTOP_DRIVE && mode != ARC_ESTOP_RELEASED) {
					CHECK(out.block_commands, "corrupt mode %d drove", mode);
				}
				CHECK(e.mode == ARC_ESTOP_ENGAGED || e.mode == ARC_ESTOP_RELEASED ||
					      e.mode == ARC_ESTOP_DRIVE,
				      "mode %d left as %d", mode, e.mode);
			}
		}
	}
}

/* The brake current, recomputed independently in double precision. */
static double current_ref(double a, double m, double d, double g, double kv, int w)
{
	const double kt = 60.0 / (2.0 * 3.14159265358979323846 * kv);
	return m * a / w * (d / 2.0) / g / kt;
}

static void test_brake_current_formula(void)
{
	/* The numbers behind ~8 A: 5 m/s^2, 5 kg, 100 mm, 18:1, 4400 kV, 4 wheels. */
	const double want = current_ref(5.0, 5.0, 0.100, 18.0, 4400.0, 4);

	CHECK(fabs(want - 7.9997) < 1e-3, "reference %g", want);
	CHECK(fabs(target() - want) < 1e-3, "target %g, want %g", (double)target(), want);

	/* Linear in deceleration and mass, in wheel diameter and kV; inverse in
	 * gearing and wheel count. */
	for (int i = 0; i < 20000; i++) {
		const double a = 0.1 + (rand32() % 2000) / 100.0;
		const double m = 0.5 + (rand32() % 2000) / 100.0;
		const double d = 0.02 + (rand32() % 300) / 1000.0;
		const double g = 1.0 + (rand32() % 500) / 10.0;
		const double kv = 100.0 + (rand32() % 10000);
		const int w = 1 + (int)(rand32() % 8);
		const double ref = current_ref(a, m, d, g, kv, w);
		const float got = arc_estop_brake_current_a((float)a, (float)m, (float)d,
							    (float)g, (float)kv, w);

		CHECK(fabs(got - ref) <= 1e-4 * ref + 1e-6, "a %g m %g d %g g %g kv %g w %d: %g vs %g",
		      a, m, d, g, kv, w, (double)got, ref);
	}

	/* Anything non-positive or non-finite gives 0, never a negative or NaN
	 * current that could drive the motor. */
	const float bad[] = { 0.0f, -0.0f, -1.0f, NAN, INFINITY, -INFINITY, -1e-30f };
	for (size_t i = 0; i < sizeof(bad) / sizeof(bad[0]); i++) {
		const float x = bad[i];
		CHECK(arc_estop_brake_current_a(x, 5, 0.1f, 18, 4400, 4) == 0.0f, "decel %g", (double)x);
		CHECK(arc_estop_brake_current_a(5, x, 0.1f, 18, 4400, 4) == 0.0f, "mass %g", (double)x);
		CHECK(arc_estop_brake_current_a(5, 5, x, 18, 4400, 4) == 0.0f, "diam %g", (double)x);
		CHECK(arc_estop_brake_current_a(5, 5, 0.1f, x, 4400, 4) == 0.0f, "gear %g", (double)x);
		CHECK(arc_estop_brake_current_a(5, 5, 0.1f, 18, x, 4) == 0.0f, "kv %g", (double)x);
	}
	for (int w = -5; w <= 0; w++) {
		CHECK(arc_estop_brake_current_a(5, 5, 0.1f, 18, 4400, w) == 0.0f, "wheels %d", w);
	}
	/* Overflow gives 0 too, not infinity. */
	CHECK(arc_estop_brake_current_a(1e30f, 1e30f, 1e30f, 1e-30f, 1e30f, 1) == 0.0f, "overflow");
}

/* The constants behind the current are the ones the ADRs and the car config
 * give (test_systems_values.py checks them against config/cars/nominal.yaml). */
static void test_constants(void)
{
	CHECK(ARC_ESTOP_DECEL_MPS2_TBC == 5.0f, "ADR-0012: ~5 m/s^2");
	CHECK(ARC_ESTOP_RAMP_S_TBC == 0.150f, "ADR-0012: ~150 ms ramp");
	CHECK(ARC_ESTOP_CAR_MASS_KG_TBC >= 5.0f, "the upper end of ADR-0044's 4-5 kg");
	CHECK(ARC_ESTOP_BRAKED_WHEELS == 4, "all four corners brake");
	/* The ramp takes a whole number of 1 ms steps. */
	CHECK(fabsf(ARC_ESTOP_RAMP_S_TBC / DT - roundf(ARC_ESTOP_RAMP_S_TBC / DT)) < 1e-3f,
	      "ramp steps");
}

int main(void)
{
	test_global_starts_safe();
	test_every_sequence();
	test_random_runs();
	test_odd_step_times();
	test_full_brake_held();
	test_any_corrupted_state();
	test_brake_current_formula();
	test_constants();

	if (failures) {
		fprintf(stderr, "%ld check(s) failed\n", failures);
		return 1;
	}
	printf("estop properties: %ld sequences of 11 steps, all checks passed\n", walked);
	return 0;
}
