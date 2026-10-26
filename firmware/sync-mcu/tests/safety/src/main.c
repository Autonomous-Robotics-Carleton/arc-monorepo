/*
 * Safety module tests (SYS-04, SYS-22, SYS-25, ADR-0039, ADR-0052), in virtual time:
 * the module takes its time as an argument.
 * Run: west twister -T firmware/sync-mcu/tests -p native_sim
 */

#include <math.h>

#include <zephyr/ztest.h>

#include "safety.h"

#define HB_MS 150
#define CMD_MS 150

/* The car's maximums (TBC; the app takes them from Kconfig). */
static const struct safety_envelope car = {9.0f, 8.0f, 0.436f};

ZTEST_SUITE(safety, NULL, NULL, NULL, NULL, NULL);

static void init(struct safety *s)
{
	safety_init(s, &car, HB_MS, CMD_MS, 0);
}

static struct safety_envelope env(float speed, float accel, float steer)
{
	return (struct safety_envelope){speed, accel, steer};
}

static struct drive_cmd cmd(float speed, float accel, float steer)
{
	return (struct drive_cmd){speed, accel, steer};
}

/* A safety struct with an envelope, a heartbeat, the re-arm and a command at t = 0. */
static void armed(struct safety *s)
{
	init(s);
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);

	zassert_true(safety_on_envelope(s, &e));
	safety_on_heartbeat(s, 0);
	zassert_true(safety_on_rearm(s, 0));
	safety_on_command(s, cmd(1.0f, 1.0f, 0.1f), 0);
}

/* A watchdog with a heartbeat and the re-arm at t. */
static void watchdog_armed(struct safety_watchdog *wd, int64_t t)
{
	safety_watchdog_init(wd, HB_MS, t);
	safety_watchdog_feed(wd, t);
	zassert_true(safety_watchdog_rearm(wd, t));
}

/* ---- the envelope ---- */

ZTEST(safety, test_default_envelope_is_3_mps_5_mps2_and_full_steering)
{
	struct safety_envelope e;

	safety_envelope_default(&e, &car); /* ADR-0052 */
	zassert_equal(e.max_speed_mps, 3.0f); /* SYS-22 */
	zassert_equal(e.max_accel_mps2, 5.0f);
	zassert_equal(e.max_steer_rad, car.max_steer_rad);

	struct drive_cmd out = safety_clamp(&e, cmd(2.0f, 1.0f, 0.2f));

	zassert_equal(out.speed_mps, 2.0f);
	zassert_equal(out.accel_mps2, 1.0f);
	zassert_equal(out.steer_rad, 0.2f);
}

ZTEST(safety, test_default_envelope_never_exceeds_the_car)
{
	const struct safety_envelope slow = env(2.0f, 1.0f, 0.2f);
	struct safety_envelope e;

	safety_envelope_default(&e, &slow);
	zassert_equal(e.max_speed_mps, 2.0f);
	zassert_equal(e.max_accel_mps2, 1.0f);
	zassert_equal(e.max_steer_rad, 0.2f);
}

ZTEST(safety, test_a_new_session_starts_with_the_default_envelope)
{
	struct safety s;

	init(&s);
	zassert_equal(s.envelope.max_speed_mps, 3.0f);
	zassert_equal(s.envelope.max_accel_mps2, 5.0f);
	zassert_equal(s.envelope.max_steer_rad, car.max_steer_rad);
}

ZTEST(safety, test_reverse_has_the_same_speed_limit)
{
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);

	zassert_equal(safety_clamp(&e, cmd(-2.0f, 1.0f, 0.0f)).speed_mps, -2.0f);
	zassert_equal(safety_clamp(&e, cmd(-4.0f, 1.0f, 0.0f)).speed_mps, -3.0f);
}

ZTEST(safety, test_clamp_limits_each_field)
{
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);
	struct drive_cmd out = safety_clamp(&e, cmd(9.0f, 50.0f, 1.0f));

	zassert_equal(out.speed_mps, 3.0f);
	zassert_equal(out.accel_mps2, 5.0f);
	zassert_equal(out.steer_rad, 0.4f);

	out = safety_clamp(&e, cmd(-9.0f, -2.0f, -1.0f));
	zassert_equal(out.speed_mps, -3.0f);
	zassert_equal(out.accel_mps2, 0.0f); /* an acceleration limit isn't negative */
	zassert_equal(out.steer_rad, -0.4f);
}

ZTEST(safety, test_clamp_passes_a_command_inside_the_limits_unchanged)
{
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);
	struct drive_cmd out = safety_clamp(&e, cmd(2.5f, 4.0f, -0.3f));

	zassert_equal(out.speed_mps, 2.5f);
	zassert_equal(out.accel_mps2, 4.0f);
	zassert_equal(out.steer_rad, -0.3f);
}

ZTEST(safety, test_a_command_at_exactly_the_limit_is_not_clamped)
{
	struct safety s;

	armed(&s);
	safety_on_command(&s, cmd(3.0f, 5.0f, 0.4f), 0);
	zassert_false(safety_decide(&s, 1, false, true).clamped);
	safety_on_command(&s, cmd(3.1f, 5.0f, 0.4f), 1);
	zassert_true(safety_decide(&s, 2, false, true).clamped);
}

ZTEST(safety, test_a_non_finite_command_becomes_zero)
{
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);
	const float bad[] = {NAN, INFINITY, -INFINITY};

	for (int i = 0; i < 3; i++) {
		for (int field = 0; field < 3; field++) {
			struct drive_cmd c = cmd(1.0f, 1.0f, 0.1f);

			((float *)&c)[field] = bad[i];
			struct drive_cmd out = safety_clamp(&e, c);

			zassert_equal(out.speed_mps, 0.0f);
			zassert_equal(out.accel_mps2, 0.0f);
			zassert_equal(out.steer_rad, 0.0f);
		}
	}
}

ZTEST(safety, test_an_invalid_envelope_is_refused_and_the_old_one_kept)
{
	struct safety s;

	armed(&s);

	struct safety_envelope bad[] = {
		env(-1.0f, 5.0f, 0.4f), env(3.0f, -1.0f, 0.4f), env(3.0f, 5.0f, -0.1f),
		env(NAN, 5.0f, 0.4f),   env(3.0f, INFINITY, 0.4f), env(3.0f, 5.0f, NAN),
	};

	for (size_t i = 0; i < ARRAY_SIZE(bad); i++) {
		zassert_false(safety_envelope_valid(&bad[i]), "case %zu", i);
		zassert_false(safety_on_envelope(&s, &bad[i]), "case %zu", i);
		zassert_equal(s.envelope.max_speed_mps, 3.0f, "case %zu", i);
		zassert_equal(s.envelope.max_steer_rad, 0.4f, "case %zu", i);
	}
}

ZTEST(safety, test_an_envelope_above_the_car_is_refused_not_clamped)
{
	struct safety s;

	armed(&s);

	struct safety_envelope above[] = {
		env(9.5f, 5.0f, 0.4f), env(3.0f, 8.5f, 0.4f), env(3.0f, 5.0f, 0.5f),
	};

	for (size_t i = 0; i < ARRAY_SIZE(above); i++) {
		zassert_true(safety_envelope_valid(&above[i]), "case %zu", i);
		zassert_false(safety_on_envelope(&s, &above[i]), "case %zu", i);
		zassert_equal(s.envelope.max_speed_mps, 3.0f, "case %zu", i);
		zassert_equal(s.envelope.max_accel_mps2, 5.0f, "case %zu", i);
		zassert_equal(s.envelope.max_steer_rad, 0.4f, "case %zu", i);
	}
}

ZTEST(safety, test_the_envelope_can_be_raised_to_the_car_maximum)
{
	struct safety s;

	armed(&s);
	zassert_true(safety_on_envelope(&s, &car));
	safety_on_command(&s, cmd(20.0f, 20.0f, 1.0f), 1);
	struct safety_output o = safety_decide(&s, 2, false, true);

	zassert_equal(o.cmd.speed_mps, 9.0f);
	zassert_equal(o.cmd.accel_mps2, 8.0f);
	zassert_equal(o.cmd.steer_rad, 0.436f);
}

ZTEST(safety, test_clamp_with_an_invalid_envelope_is_zero)
{
	struct safety_envelope e = env(NAN, 5.0f, 0.4f);
	struct drive_cmd out = safety_clamp(&e, cmd(1.0f, 1.0f, 0.1f));

	zassert_equal(out.speed_mps, 0.0f);
	zassert_equal(out.accel_mps2, 0.0f);
	zassert_equal(out.steer_rad, 0.0f);
}

/* ---- the watchdog ---- */

ZTEST(safety, test_the_watchdog_needs_a_heartbeat_and_a_rearm)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	zassert_true(safety_watchdog_expired(&wd, 0));
	zassert_true(safety_watchdog_expired(&wd, 10000));
	zassert_false(safety_watchdog_rearm(&wd, 10000)); /* no heartbeat yet */
	safety_watchdog_feed(&wd, 10000);
	zassert_true(safety_watchdog_expired(&wd, 10000)); /* a heartbeat alone doesn't arm */
	zassert_true(safety_watchdog_rearm(&wd, 10010));
	zassert_false(safety_watchdog_expired(&wd, 10010));
}

ZTEST(safety, test_rearm_refused_once_the_heartbeat_has_timed_out)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	safety_watchdog_feed(&wd, 0);
	zassert_false(safety_watchdog_rearm(&wd, HB_MS));
	zassert_true(safety_watchdog_expired(&wd, HB_MS));
}

ZTEST(safety, test_exactly_the_timeout_has_expired)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	safety_watchdog_feed(&wd, 1000);
	zassert_false(safety_watchdog_expired(&wd, 1000 + HB_MS - 1));
	zassert_true(safety_watchdog_expired(&wd, 1000 + HB_MS)); /* SYS-04: <= 150 ms */
	zassert_true(wd.tripped);
}

ZTEST(safety, test_regular_heartbeats_keep_it_alive)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	for (int64_t t = 0; t < 10000; t += 20) { /* 50 Hz */
		safety_watchdog_feed(&wd, t);
		zassert_false(safety_watchdog_expired(&wd, t + 19));
	}
}

ZTEST(safety, test_a_trip_latches_and_a_late_heartbeat_does_not_clear_it)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	zassert_true(safety_watchdog_expired(&wd, 500));
	safety_watchdog_feed(&wd, 501);
	zassert_true(safety_watchdog_expired(&wd, 501));
	zassert_true(safety_watchdog_expired(&wd, 502));
	zassert_true(wd.tripped);
}

ZTEST(safety, test_rearm_after_a_trip_needs_the_heartbeat_back)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	zassert_true(safety_watchdog_expired(&wd, 500));
	zassert_false(safety_watchdog_rearm(&wd, 600)); /* still no heartbeat */
	zassert_true(wd.tripped);
	safety_watchdog_feed(&wd, 610);
	zassert_true(safety_watchdog_rearm(&wd, 620));
	zassert_false(wd.tripped);
	zassert_false(safety_watchdog_expired(&wd, 620));
}

ZTEST(safety, test_rearm_refused_if_the_heartbeat_was_lost_again)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	zassert_true(safety_watchdog_expired(&wd, 200));
	safety_watchdog_feed(&wd, 300);
	/* no check between the second loss and the re-arm */
	zassert_false(safety_watchdog_rearm(&wd, 300 + HB_MS));
	zassert_true(safety_watchdog_expired(&wd, 300 + HB_MS));
}

ZTEST(safety, test_a_clock_going_backwards_trips_it)
{
	struct safety_watchdog wd;

	watchdog_armed(&wd, 0);
	safety_watchdog_feed(&wd, 1000);
	zassert_true(safety_watchdog_expired(&wd, 999));
	zassert_true(wd.tripped);

	watchdog_armed(&wd, 0);
	safety_watchdog_feed(&wd, 1000);
	safety_watchdog_feed(&wd, 500);
	zassert_true(safety_watchdog_expired(&wd, 1000));
	zassert_true(wd.tripped);
}

ZTEST(safety, test_large_times_do_not_overflow)
{
	struct safety_watchdog wd;
	const int64_t t0 = INT64_MAX - 1000;

	watchdog_armed(&wd, t0);
	zassert_false(safety_watchdog_expired(&wd, t0 + HB_MS - 1));
	zassert_true(safety_watchdog_expired(&wd, t0 + HB_MS));
}

/* ---- the decision ---- */

ZTEST(safety, test_boot_state_is_stopped_but_not_braking)
{
	struct safety s;

	init(&s);
	struct safety_output o = safety_decide(&s, 0, false, true);

	zassert_equal(o.reason, SAFETY_NOT_ARMED);
	zassert_false(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);
}

ZTEST(safety, test_commands_before_a_heartbeat_do_nothing)
{
	struct safety s;

	init(&s);
	safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), 0);
	zassert_equal(safety_decide(&s, 1, false, true).reason, SAFETY_NOT_ARMED);
}

ZTEST(safety, test_heartbeats_without_a_rearm_do_not_drive)
{
	struct safety s;

	init(&s);
	for (int64_t t = 0; t < 1000; t += 20) {
		safety_on_heartbeat(&s, t);
		safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), t);
		struct safety_output o = safety_decide(&s, t, false, true);

		zassert_equal(o.reason, SAFETY_NOT_ARMED);
		zassert_false(o.brake);
		zassert_equal(o.cmd.speed_mps, 0.0f);
	}
}

ZTEST(safety, test_driving_passes_the_clamped_command)
{
	struct safety s;

	armed(&s);
	safety_on_command(&s, cmd(8.0f, 1.0f, 0.1f), 0);
	struct safety_output o = safety_decide(&s, 1, false, true);

	zassert_equal(o.reason, SAFETY_DRIVING);
	zassert_false(o.brake);
	zassert_true(o.clamped);
	zassert_equal(o.cmd.speed_mps, 3.0f);
	zassert_equal(o.cmd.accel_mps2, 1.0f);
}

ZTEST(safety, test_lost_heartbeat_brakes_after_150_ms_and_the_command_is_zero)
{
	struct safety s;

	armed(&s);
	for (int64_t t = 0; t <= 1000; t += 20) { /* heartbeat and commands arrive */
		safety_on_heartbeat(&s, t);
		safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), t);
		zassert_equal(safety_decide(&s, t, false, true).reason, SAFETY_DRIVING);
	}
	/* the heartbeat stops after t = 1000; commands carry on (ICD: the 500 Hz stream) */
	for (int64_t t = 1001; t < 1000 + HB_MS; t++) {
		safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), t);
		struct safety_output o = safety_decide(&s, t, false, true);

		zassert_equal(o.reason, SAFETY_DRIVING, "t=%lld", t);
		zassert_false(o.brake);
	}
	safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), 1000 + HB_MS);
	struct safety_output o = safety_decide(&s, 1000 + HB_MS, false, true);

	zassert_equal(o.reason, SAFETY_HEARTBEAT_LOST);
	zassert_true(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);
}

ZTEST(safety, test_a_returning_heartbeat_does_not_restart_the_car)
{
	struct safety s;

	armed(&s);
	zassert_equal(safety_decide(&s, 400, false, true).reason, SAFETY_HEARTBEAT_LOST);
	for (int64_t t = 401; t < 2000; t += 20) {
		safety_on_heartbeat(&s, t);
		safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), t);
		struct safety_output o = safety_decide(&s, t, false, true);

		zassert_equal(o.reason, SAFETY_HEARTBEAT_LOST);
		zassert_true(o.brake);
	}
}

ZTEST(safety, test_rearm_after_a_heartbeat_stop_drives_only_on_new_commands)
{
	struct safety s;

	armed(&s);
	zassert_equal(safety_decide(&s, 400, false, true).reason, SAFETY_HEARTBEAT_LOST);
	safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), 410);
	zassert_false(safety_on_rearm(&s, 415)); /* the heartbeat isn't back */
	safety_on_heartbeat(&s, 420);
	zassert_true(safety_on_rearm(&s, 425)); /* ADR-0052: no e-stop cycle */

	/* the command from before the re-arm is dropped */
	struct safety_output o = safety_decide(&s, 426, false, true);

	zassert_equal(o.reason, SAFETY_NO_COMMAND);
	zassert_false(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);

	safety_on_command(&s, cmd(0.5f, 1.0f, 0.0f), 430);
	o = safety_decide(&s, 431, false, true);
	zassert_equal(o.reason, SAFETY_DRIVING);
	zassert_equal(o.cmd.speed_mps, 0.5f);
}

ZTEST(safety, test_rearm_does_not_clear_a_controller_stop)
{
	struct safety s;

	armed(&s);
	zassert_true(safety_on_rearm(&s, 1));
	zassert_equal(safety_decide(&s, 2, true, true).reason, SAFETY_CONTROLLER_STOP);
}

ZTEST(safety, test_an_invalid_command_brakes)
{
	struct safety s;
	const struct drive_cmd bad[] = {
		cmd(NAN, 1.0f, 0.1f),      cmd(1.0f, NAN, 0.1f),      cmd(1.0f, 1.0f, NAN),
		cmd(INFINITY, 1.0f, 0.1f), cmd(-INFINITY, 1.0f, 0.1f), cmd(1.0f, INFINITY, 0.1f),
		cmd(1.0f, 1.0f, INFINITY), cmd(1.0f, -1.0f, 0.1f),
	};

	for (size_t i = 0; i < ARRAY_SIZE(bad); i++) {
		armed(&s);
		safety_on_command(&s, bad[i], 1);
		struct safety_output o = safety_decide(&s, 2, false, true);

		zassert_equal(o.reason, SAFETY_COMMAND_REJECTED, "case %zu", i);
		zassert_true(o.brake, "case %zu", i);
		zassert_equal(o.cmd.speed_mps, 0.0f, "case %zu", i);

		/* latest wins: the next valid command drives again */
		safety_on_command(&s, cmd(1.0f, 1.0f, 0.0f), 3);
		zassert_equal(safety_decide(&s, 4, false, true).reason, SAFETY_DRIVING,
			      "case %zu", i);
	}
}

ZTEST(safety, test_a_stale_command_gives_zero_without_braking)
{
	struct safety s;

	armed(&s);
	safety_on_heartbeat(&s, 100);
	zassert_equal(safety_decide(&s, CMD_MS, false, true).reason, SAFETY_DRIVING);
	struct safety_output o = safety_decide(&s, CMD_MS + 1, false, true);

	zassert_equal(o.reason, SAFETY_NO_COMMAND);
	zassert_false(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);

	/* a new command resumes it: the watchdog, not the command, is what latches */
	safety_on_command(&s, cmd(1.0f, 1.0f, 0.0f), CMD_MS + 2);
	zassert_equal(safety_decide(&s, CMD_MS + 3, false, true).reason, SAFETY_DRIVING);
}

ZTEST(safety, test_no_command_ever_gives_zero)
{
	struct safety s;

	init(&s);
	safety_on_heartbeat(&s, 0);
	zassert_true(safety_on_rearm(&s, 0));
	zassert_equal(safety_decide(&s, 1, false, true).reason, SAFETY_NO_COMMAND);
}

ZTEST(safety, test_a_controller_stop_brakes_and_comes_first)
{
	struct safety s;

	armed(&s);
	struct safety_output o = safety_decide(&s, 1, true, true);

	zassert_equal(o.reason, SAFETY_CONTROLLER_STOP);
	zassert_true(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);

	/* even with the watchdog also expired, and before any heartbeat */
	o = safety_decide(&s, 5000, true, true);
	zassert_equal(o.reason, SAFETY_CONTROLLER_STOP);

	init(&s);
	zassert_equal(safety_decide(&s, 0, true, true).reason, SAFETY_CONTROLLER_STOP);
}

ZTEST(safety, test_controllers_not_ready_refuses_drive_without_braking)
{
	struct safety s;

	armed(&s);
	struct safety_output o = safety_decide(&s, 1, false, false);

	zassert_equal(o.reason, SAFETY_CONTROLLERS_NOT_READY);
	zassert_false(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);
}

ZTEST(safety, test_the_envelope_applies_to_the_next_decision)
{
	struct safety s;

	armed(&s);
	safety_on_command(&s, cmd(2.5f, 1.0f, 0.0f), 0);
	zassert_equal(safety_decide(&s, 1, false, true).cmd.speed_mps, 2.5f);

	struct safety_envelope tight = env(1.0f, 5.0f, 0.4f);

	zassert_true(safety_on_envelope(&s, &tight));
	struct safety_output o = safety_decide(&s, 2, false, true);

	zassert_equal(o.cmd.speed_mps, 1.0f);
	zassert_true(o.clamped);
}

ZTEST(safety, test_nothing_beyond_the_envelope_ever_comes_out)
{
	struct safety s;

	armed(&s);
	uint32_t x = 12345;

	for (int i = 0; i < 20000; i++) {
		x = x * 1664525u + 1013904223u;
		struct drive_cmd c = cmd(((int32_t)x >> 8) / 1000.0f, ((int32_t)(x * 7u) >> 8) / 1000.0f,
					 ((int32_t)(x * 13u) >> 8) / 1000.0f);

		safety_on_heartbeat(&s, i);
		safety_on_command(&s, c, i);
		struct safety_output o = safety_decide(&s, i, false, true);

		zassert_true(fabsf(o.cmd.speed_mps) <= 3.0f);
		zassert_true(o.cmd.accel_mps2 >= 0.0f && o.cmd.accel_mps2 <= 5.0f);
		zassert_true(fabsf(o.cmd.steer_rad) <= 0.4f);
	}
}
