/*
 * Safety module tests. Run: west twister -T firmware/sync-mcu/tests -p native_sim
 *
 * Placeholder: the module is a stub (safety.h), so this only checks the
 * stubs are fail-safe, and that the test harness runs. Replace with real
 * tests (SYS-04, SYS-22) when the module is implemented.
 */

#include <zephyr/ztest.h>

#include "safety.h"

ZTEST_SUITE(safety, NULL, NULL, NULL, NULL, NULL);

ZTEST(safety, test_stubs_never_allow_motion)
{
	struct safety_envelope env;
	struct safety_watchdog wd;

	safety_envelope_default(&env);
	struct drive_cmd out = safety_clamp(&env, (struct drive_cmd){1.0f, 1.0f, 0.1f});

	zassert_equal(out.speed_mps, 0.0f);
	zassert_equal(out.accel_mps2, 0.0f);
	zassert_equal(out.steer_rad, 0.0f);

	safety_watchdog_init(&wd, 150, 0);
	zassert_true(safety_watchdog_expired(&wd, 0));
}
