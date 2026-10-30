/*
 * The fault-stop latch and its reset (SYS-25, ADR-0039), in virtual time.
 * Run: west twister -T firmware/sync-mcu/tests -p native_sim
 */

#include <zephyr/ztest.h>

#include "fault_latch.h"

#define T_MS 2400      /* the motor-bus cut */
#define WINDOW_MS 3000 /* the power-on checks */

static const struct fl_config cfg = {.cut_ms = T_MS, .check_window_ms = WINDOW_MS, .max_stops = 3};

static struct fault_latch fl;

static void before(void *f)
{
	ARG_UNUSED(f);
	fl_init(&fl, &cfg, 0);
}

ZTEST_SUITE(fault_latch, NULL, NULL, before, NULL, NULL);

/* The car as the checks want it: button up, corners fine, wheels stopped. */
static struct fl_inputs healthy(void)
{
	return (struct fl_inputs){.corners = CL_OK, .corner = -1, .wheels_stopped = true};
}

static struct fl_inputs pressed(void)
{
	struct fl_inputs in = healthy();

	in.estop_pressed = true;
	in.corners = CL_ESTOP; /* every controller sees the line too */
	in.corner = 0;
	return in;
}

static struct fl_inputs corner_stop(enum cl_reason r, int corner, uint8_t code)
{
	struct fl_inputs in = healthy();

	in.corners = r;
	in.corner = corner;
	in.fault_code = code;
	return in;
}

/* Press at t, hold to t + T, release; returns the release time. */
static int64_t cycle(int64_t t)
{
	struct fl_inputs p = pressed();
	struct fl_inputs r = healthy();

	r.corners = CL_SILENT; /* the controllers are restarting after the cut */
	zassert_equal(fl_update(&fl, t, &p), FL_EV_PRESSED);
	zassert_equal(fl_update(&fl, t + T_MS - 1, &p), FL_EV_NONE);
	zassert_equal(fl_update(&fl, t + T_MS, &r), FL_EV_CHECKING);
	return t + T_MS;
}

/* A full reset from the latched state, ending clear at the returned time. */
static int64_t reset(int64_t t)
{
	int64_t at = cycle(t) + 200;
	struct fl_inputs in = healthy();

	zassert_equal(fl_update(&fl, at, &in), FL_EV_CLEARED);
	zassert_true(fl_clear(&fl));
	return at;
}

/* ---- power-on ---- */

ZTEST(fault_latch, test_it_boots_latched_and_not_braking)
{
	struct fl_inputs in = healthy();

	zassert_equal(fl.state, FL_LATCHED);
	zassert_equal(fl.cause, FL_CAUSE_BOOT);
	zassert_false(fl_clear(&fl));
	zassert_false(fl_brake(&fl)); /* nothing to stop at power-on */
	/* healthy corners alone never reset it */
	for (int64_t t = 0; t < 10000; t += 100) {
		zassert_equal(fl_update(&fl, t, &in), FL_EV_NONE);
	}
	zassert_false(fl_clear(&fl));
}

ZTEST(fault_latch, test_a_full_cycle_resets_it)
{
	reset(1000);
	zassert_equal(fl.cause, FL_CAUSE_NONE);
	zassert_false(fl_brake(&fl));
}

/* ---- the press ---- */

ZTEST(fault_latch, test_a_press_shorter_than_the_cut_does_not_reset)
{
	struct fl_inputs p = pressed();
	struct fl_inputs r = healthy();

	zassert_equal(fl_update(&fl, 0, &p), FL_EV_PRESSED);
	zassert_equal(fl_update(&fl, T_MS - 1, &r), FL_EV_RELEASED_EARLY);
	zassert_equal(fl.state, FL_LATCHED);
	zassert_equal(fl_update(&fl, T_MS + 5000, &r), FL_EV_NONE);
	zassert_false(fl_clear(&fl));
}

ZTEST(fault_latch, test_a_press_of_exactly_the_cut_counts)
{
	struct fl_inputs p = pressed();
	struct fl_inputs r = healthy();

	fl_update(&fl, 0, &p);
	zassert_equal(fl_update(&fl, T_MS, &r), FL_EV_CHECKING);
	zassert_equal(fl_update(&fl, T_MS + 1, &r), FL_EV_CLEARED);
}

ZTEST(fault_latch, test_a_second_press_while_checking_starts_over)
{
	int64_t t = cycle(0);
	struct fl_inputs p = pressed();
	struct fl_inputs r = healthy();

	zassert_equal(fl_update(&fl, t + 100, &p), FL_EV_PRESSED);
	/* released again too soon: latched, not checking */
	zassert_equal(fl_update(&fl, t + 200, &r), FL_EV_RELEASED_EARLY);
	zassert_false(fl_clear(&fl));
}

/* ---- the power-on checks ---- */

ZTEST(fault_latch, test_checks_wait_for_the_controllers_to_come_back)
{
	int64_t t = cycle(0);
	struct fl_inputs in = healthy();

	in.corners = CL_SILENT;
	zassert_equal(fl_update(&fl, t + WINDOW_MS - 1, &in), FL_EV_NONE);
	in = healthy();
	zassert_equal(fl_update(&fl, t + WINDOW_MS - 1, &in), FL_EV_CLEARED);
}

ZTEST(fault_latch, test_checks_fail_at_the_end_of_the_window)
{
	const struct fl_inputs bad[] = {
		corner_stop(CL_SILENT, 1, 0),        corner_stop(CL_NOT_READY, 2, 0),
		corner_stop(CL_FAULT, 3, 7),         corner_stop(CL_COMMAND_TIMEOUT, 0, 0),
		corner_stop(CL_VERSION, 1, 0),       corner_stop(CL_UNKNOWN_STATE, 2, 0),
	};

	for (size_t i = 0; i < ARRAY_SIZE(bad); i++) {
		fl_init(&fl, &cfg, 0);
		int64_t t = cycle(0);

		zassert_equal(fl_update(&fl, t + WINDOW_MS - 1, &bad[i]), FL_EV_NONE, "case %zu", i);
		zassert_equal(fl_update(&fl, t + WINDOW_MS, &bad[i]), FL_EV_CHECKS_FAILED,
			      "case %zu", i);
		zassert_equal(fl.state, FL_LATCHED, "case %zu", i);
		zassert_equal(fl.cause, FL_CAUSE_CHECKS, "case %zu", i);
		zassert_equal(fl.corner_reason, bad[i].corners, "case %zu", i);
		zassert_equal(fl.corner, bad[i].corner, "case %zu", i);
		zassert_true(fl_brake(&fl), "case %zu", i);

		/* a later recovery doesn't clear it by itself: it needs another cycle */
		struct fl_inputs ok = healthy();

		zassert_equal(fl_update(&fl, t + WINDOW_MS + 5000, &ok), FL_EV_NONE, "case %zu", i);
		zassert_false(fl_clear(&fl), "case %zu", i);
		reset(t + WINDOW_MS + 6000);
	}
}

ZTEST(fault_latch, test_checks_need_the_wheels_stopped)
{
	int64_t t = cycle(0);
	struct fl_inputs in = healthy();

	in.wheels_stopped = false;
	zassert_equal(fl_update(&fl, t + 10, &in), FL_EV_NONE);
	zassert_equal(fl_update(&fl, t + WINDOW_MS, &in), FL_EV_CHECKS_FAILED);
	zassert_true(fl.wheels_moving);
}

ZTEST(fault_latch, test_not_braking_while_checking)
{
	struct fl_inputs in = corner_stop(CL_SILENT, 0, 0);

	reset(0);
	fl_update(&fl, 10000, &in); /* a stop */
	zassert_true(fl_brake(&fl));
	int64_t t = cycle(11000);

	zassert_equal(fl.state, FL_CHECKING);
	zassert_false(fl_brake(&fl), "the restarting controllers need a live bus");
	(void)t;
}

/* ---- stops ---- */

ZTEST(fault_latch, test_a_corner_stop_latches_and_brakes)
{
	int64_t t = reset(0);
	struct fl_inputs in = corner_stop(CL_FAULT, 2, 9);

	zassert_equal(fl_update(&fl, t + 1, &in), FL_EV_LATCHED);
	zassert_equal(fl.state, FL_LATCHED);
	zassert_equal(fl.cause, FL_CAUSE_CORNER);
	zassert_equal(fl.corner_reason, CL_FAULT);
	zassert_equal(fl.corner, 2);
	zassert_true(fl_brake(&fl));
	zassert_equal(fl.stops, 1);

	/* the corner coming back doesn't clear it */
	struct fl_inputs ok = healthy();

	zassert_equal(fl_update(&fl, t + 1000, &ok), FL_EV_NONE);
	zassert_false(fl_clear(&fl));
}

ZTEST(fault_latch, test_the_button_latches_without_counting_as_a_fault)
{
	int64_t t = reset(0);
	struct fl_inputs p = pressed();

	p.corners = CL_OK; /* the input before the controllers report it */
	zassert_equal(fl_update(&fl, t + 1, &p), FL_EV_LATCHED);
	zassert_equal(fl.cause, FL_CAUSE_ESTOP);
	zassert_equal(fl.stops, 0);
	zassert_equal(fl_update(&fl, t + 2, &p), FL_EV_PRESSED);
}

ZTEST(fault_latch, test_corners_reporting_the_estop_do_not_count)
{
	int64_t t = reset(0);
	struct fl_inputs in = corner_stop(CL_ESTOP, 0, 0);

	zassert_equal(fl_update(&fl, t + 1, &in), FL_EV_LATCHED);
	zassert_equal(fl.stops, 0);
}

ZTEST(fault_latch, test_stops_are_ignored_while_latched)
{
	struct fl_inputs in = corner_stop(CL_SILENT, 1, 0);

	zassert_equal(fl_update(&fl, 10, &in), FL_EV_NONE);
	zassert_equal(fl.stops, 0);
	zassert_equal(fl.cause, FL_CAUSE_BOOT);
}

/* ---- restart required (ADR-0039 item 5) ---- */

ZTEST(fault_latch, test_the_third_stop_needs_a_restart)
{
	int64_t t = 0;
	struct fl_inputs in = corner_stop(CL_SILENT, 0, 0);

	for (int i = 1; i <= 2; i++) {
		t = reset(t + 1000);
		zassert_equal(fl_update(&fl, t + 1, &in), FL_EV_LATCHED);
		zassert_equal(fl.stops, i);
	}
	t = reset(t + 1000);
	zassert_equal(fl_update(&fl, t + 1, &in), FL_EV_RESTART);
	zassert_equal(fl.state, FL_RESTART);
	zassert_equal(fl.cause, FL_CAUSE_TOO_MANY);
}

ZTEST(fault_latch, test_over_current_and_over_voltage_need_a_restart)
{
	const struct {
		uint8_t code;
		enum fl_cause cause;
	} cases[] = {
		{FL_VESC_FAULT_ABS_OVER_CURRENT, FL_CAUSE_OVERCURRENT},
		{FL_VESC_FAULT_OVER_VOLTAGE, FL_CAUSE_OVERVOLTAGE},
	};

	for (size_t i = 0; i < ARRAY_SIZE(cases); i++) {
		fl_init(&fl, &cfg, 0);
		int64_t t = reset(0);
		struct fl_inputs in = corner_stop(CL_FAULT, 1, cases[i].code);

		zassert_equal(fl_update(&fl, t + 1, &in), FL_EV_RESTART, "case %zu", i);
		zassert_equal(fl.cause, cases[i].cause, "case %zu", i);
	}
}

ZTEST(fault_latch, test_over_current_found_by_the_checks_needs_a_restart)
{
	int64_t t = cycle(0);
	struct fl_inputs in = corner_stop(CL_FAULT, 3, FL_VESC_FAULT_ABS_OVER_CURRENT);

	zassert_equal(fl_update(&fl, t + WINDOW_MS, &in), FL_EV_RESTART);
	zassert_equal(fl.cause, FL_CAUSE_OVERCURRENT);
}

ZTEST(fault_latch, test_a_corner_still_reporting_the_estop_after_release_is_a_wiring_fault)
{
	int64_t t = cycle(0);
	struct fl_inputs in = corner_stop(CL_ESTOP, 2, 0); /* button released */

	zassert_equal(fl_update(&fl, t + 100, &in), FL_EV_NONE);
	zassert_equal(fl_update(&fl, t + WINDOW_MS, &in), FL_EV_RESTART);
	zassert_equal(fl.cause, FL_CAUSE_ESTOP_WIRING);
	zassert_equal(fl.corner, 2);
}

ZTEST(fault_latch, test_restart_required_never_clears)
{
	struct fl_inputs in = corner_stop(CL_FAULT, 0, FL_VESC_FAULT_OVER_VOLTAGE);
	int64_t t = reset(0);

	fl_update(&fl, t + 1, &in);
	zassert_equal(fl.state, FL_RESTART);
	zassert_true(fl_brake(&fl));

	/* not even a full e-stop cycle */
	struct fl_inputs p = pressed();
	struct fl_inputs ok = healthy();

	zassert_equal(fl_update(&fl, t + 100, &p), FL_EV_NONE);
	zassert_equal(fl_update(&fl, t + 100 + T_MS, &ok), FL_EV_NONE);
	zassert_equal(fl_update(&fl, t + 200 + T_MS, &ok), FL_EV_NONE);
	zassert_equal(fl.state, FL_RESTART);
	zassert_false(fl_clear(&fl));
}
