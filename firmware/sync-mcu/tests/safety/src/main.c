/*
 * Safety module tests (SYS-04, SYS-22, SYS-25, ADR-0039), in virtual time:
 * the module takes its time as an argument.
 * Run: west twister -T firmware/sync-mcu/tests -p native_sim
 */

#include <math.h>

#include <zephyr/ztest.h>

#include "safety.h"

#define HB_MS 150
#define CMD_MS 150

ZTEST_SUITE(safety, NULL, NULL, NULL, NULL, NULL);

static struct safety_envelope env(float speed, float accel, float steer)
{
	return (struct safety_envelope){speed, accel, steer};
}

static struct drive_cmd cmd(float speed, float accel, float steer)
{
	return (struct drive_cmd){speed, accel, steer};
}

/* A safety struct that has a heartbeat, an envelope and a command at t = 0. */
static void armed(struct safety *s)
{
	safety_init(s, HB_MS, CMD_MS, 0);
	struct safety_envelope e = env(3.0f, 5.0f, 0.4f);

	zassert_true(safety_on_envelope(s, &e));
	safety_on_heartbeat(s, 0);
	safety_on_command(s, cmd(1.0f, 1.0f, 0.1f), 0);
}

/* ---- the envelope ---- */

ZTEST(safety, test_default_envelope_is_3_mps_and_no_acceleration_or_steering)
{
	struct safety_envelope e;

	safety_envelope_default(&e);
	zassert_equal(e.max_speed_mps, 3.0f); /* SYS-22 */
	zassert_equal(e.max_accel_mps2, 0.0f);
	zassert_equal(e.max_steer_rad, 0.0f);

	struct drive_cmd out = safety_clamp(&e, cmd(2.0f, 1.0f, 0.2f));

	zassert_equal(out.speed_mps, 2.0f);
	zassert_equal(out.accel_mps2, 0.0f);
	zassert_equal(out.steer_rad, 0.0f);
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

ZTEST(safety, test_clamp_with_an_invalid_envelope_is_zero)
{
	struct safety_envelope e = env(NAN, 5.0f, 0.4f);
	struct drive_cmd out = safety_clamp(&e, cmd(1.0f, 1.0f, 0.1f));

	zassert_equal(out.speed_mps, 0.0f);
	zassert_equal(out.accel_mps2, 0.0f);
	zassert_equal(out.steer_rad, 0.0f);
}

/* ---- the watchdog ---- */

ZTEST(safety, test_the_watchdog_is_expired_until_the_first_heartbeat)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	zassert_true(safety_watchdog_expired(&wd, 0));
	zassert_true(safety_watchdog_expired(&wd, 10000));
	safety_watchdog_feed(&wd, 10000);
	zassert_false(safety_watchdog_expired(&wd, 10000));
}

ZTEST(safety, test_exactly_the_timeout_is_still_in_time)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	safety_watchdog_feed(&wd, 1000);
	zassert_false(safety_watchdog_expired(&wd, 1000 + HB_MS));
	zassert_true(safety_watchdog_expired(&wd, 1000 + HB_MS + 1));
}

ZTEST(safety, test_regular_heartbeats_keep_it_alive)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	for (int64_t t = 0; t < 10000; t += 20) { /* 50 Hz */
		safety_watchdog_feed(&wd, t);
		zassert_false(safety_watchdog_expired(&wd, t));
	}
}

ZTEST(safety, test_a_trip_latches_and_a_late_heartbeat_does_not_clear_it)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	safety_watchdog_feed(&wd, 0);
	zassert_true(safety_watchdog_expired(&wd, 500));
	safety_watchdog_feed(&wd, 501);
	zassert_true(safety_watchdog_expired(&wd, 501));
	zassert_true(safety_watchdog_expired(&wd, 502));
}

ZTEST(safety, test_rearm_clears_a_trip_but_needs_a_new_heartbeat)
{
	struct safety_watchdog wd;

	safety_watchdog_init(&wd, HB_MS, 0);
	safety_watchdog_feed(&wd, 0);
	zassert_true(safety_watchdog_expired(&wd, 500));
	safety_watchdog_rearm(&wd, 600);
	zassert_true(safety_watchdog_expired(&wd, 600)); /* not armed until a heartbeat */
	safety_watchdog_feed(&wd, 610);
	zassert_false(safety_watchdog_expired(&wd, 610));
}

/* ---- the decision ---- */

ZTEST(safety, test_boot_state_is_stopped_but_not_braking)
{
	struct safety s;

	safety_init(&s, HB_MS, CMD_MS, 0);
	struct safety_output o = safety_decide(&s, 0, false, true);

	zassert_equal(o.reason, SAFETY_NOT_ARMED);
	zassert_false(o.brake);
	zassert_equal(o.cmd.speed_mps, 0.0f);
}

ZTEST(safety, test_commands_before_a_heartbeat_do_nothing)
{
	struct safety s;

	safety_init(&s, HB_MS, CMD_MS, 0);
	safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), 0);
	zassert_equal(safety_decide(&s, 1, false, true).reason, SAFETY_NOT_ARMED);
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
	for (int64_t t = 1001; t <= 1000 + HB_MS; t++) {
		safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), t);
		struct safety_output o = safety_decide(&s, t, false, true);

		zassert_equal(o.reason, SAFETY_DRIVING, "t=%lld", t);
		zassert_false(o.brake);
	}
	safety_on_command(&s, cmd(2.0f, 1.0f, 0.0f), 1000 + HB_MS + 1);
	struct safety_output o = safety_decide(&s, 1000 + HB_MS + 1, false, true);

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

	safety_init(&s, HB_MS, CMD_MS, 0);
	safety_on_heartbeat(&s, 0);
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

	safety_init(&s, HB_MS, CMD_MS, 0);
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
