/*
 * What the firmware puts on the command bus for each safety decision
 * (can_command_core, ICD can-command, ADR-0035), checked byte by byte against
 * the stock VESC packing: extended ID (packet << 8) | controller ID,
 * big-endian int32.
 */

#include <math.h>

#include <zephyr/ztest.h>

#include "can_command_core.h"

ZTEST_SUITE(can_command_core, NULL, NULL, NULL, NULL, NULL);

static const struct cc_config configured = {.erpm_per_mps = 1000.0f, .brake_current_a = 10.0f};
static const struct cc_config unconfigured = {0};

static struct safety_output output(enum safety_reason reason, bool brake, float speed)
{
	return (struct safety_output){.reason = reason, .brake = brake,
				      .cmd = {.speed_mps = speed}};
}

static void expect_frame(const struct cc_frame *f, uint32_t packet, int corner, int32_t value)
{
	zassert_equal(f->id, (packet << 8) | (uint32_t)(corner + 1));
	zassert_equal(f->len, 4);
	zassert_equal(f->data[0], (uint8_t)((uint32_t)value >> 24));
	zassert_equal(f->data[1], (uint8_t)((uint32_t)value >> 16));
	zassert_equal(f->data[2], (uint8_t)((uint32_t)value >> 8));
	zassert_equal(f->data[3], (uint8_t)value);
}

ZTEST(can_command_core, test_driving_sends_rpm_to_all_four)
{
	struct safety_output o = output(SAFETY_DRIVING, false, 2.5f);
	struct cc_plan p = cc_make_plan(&configured, &o);

	zassert_equal(p.kind, CC_RPM);
	for (int c = 0; c < 4; c++) {
		expect_frame(&p.frame[c], 3, c, 2500); /* CAN_PACKET_SET_RPM = 3 */
	}
}

ZTEST(can_command_core, test_reverse_is_a_negative_rpm)
{
	struct safety_output o = output(SAFETY_DRIVING, false, -1.0f);
	struct cc_plan p = cc_make_plan(&configured, &o);

	expect_frame(&p.frame[0], 3, 0, -1000);
}

ZTEST(can_command_core, test_rpm_is_limited_to_the_dbc_range)
{
	struct safety_output o = output(SAFETY_DRIVING, false, 9000.0f);
	struct cc_plan p = cc_make_plan(&configured, &o);

	expect_frame(&p.frame[0], 3, 0, 200000);
	o.cmd.speed_mps = -9000.0f;
	p = cc_make_plan(&configured, &o);
	expect_frame(&p.frame[0], 3, 0, -200000);
}

ZTEST(can_command_core, test_driving_without_a_conversion_is_zero_rpm_current)
{
	struct safety_output o = output(SAFETY_DRIVING, false, 2.5f);
	struct cc_plan p = cc_make_plan(&unconfigured, &o);

	zassert_equal(p.kind, CC_ZERO_CURRENT);
	for (int c = 0; c < 4; c++) {
		expect_frame(&p.frame[c], 1, c, 0); /* CAN_PACKET_SET_CURRENT = 1 */
	}
}

ZTEST(can_command_core, test_not_driving_keeps_the_bus_alive_with_zero_current)
{
	const enum safety_reason reasons[] = {SAFETY_NOT_ARMED, SAFETY_NO_COMMAND,
					      SAFETY_CONTROLLERS_NOT_READY};

	for (size_t i = 0; i < ARRAY_SIZE(reasons); i++) {
		struct safety_output o = output(reasons[i], false, 0.0f);
		struct cc_plan p = cc_make_plan(&configured, &o);

		zassert_equal(p.kind, CC_ZERO_CURRENT, "reason %zu", i);
		for (int c = 0; c < 4; c++) {
			expect_frame(&p.frame[c], 1, c, 0);
		}
	}
}

ZTEST(can_command_core, test_braking_sends_brake_current_to_all_four)
{
	const enum safety_reason reasons[] = {SAFETY_HEARTBEAT_LOST, SAFETY_CONTROLLER_STOP};

	for (size_t i = 0; i < ARRAY_SIZE(reasons); i++) {
		struct safety_output o = output(reasons[i], true, 0.0f);
		struct cc_plan p = cc_make_plan(&configured, &o);

		zassert_equal(p.kind, CC_BRAKE, "reason %zu", i);
		for (int c = 0; c < 4; c++) {
			expect_frame(&p.frame[c], 2, c, 10000); /* SET_CURRENT_BRAKE = 2, 10 A */
		}
	}
}

ZTEST(can_command_core, test_braking_ignores_the_speed_command)
{
	struct safety_output o = output(SAFETY_CONTROLLER_STOP, true, 3.0f);
	struct cc_plan p = cc_make_plan(&configured, &o);

	zassert_equal(p.kind, CC_BRAKE);
}

ZTEST(can_command_core, test_braking_with_no_brake_current_sends_nothing)
{
	/* the controllers' own command timeout brakes (ADR-0035), later than SYS-25's 20 ms */
	struct safety_output o = output(SAFETY_HEARTBEAT_LOST, true, 0.0f);
	struct cc_plan p = cc_make_plan(&unconfigured, &o);

	zassert_equal(p.kind, CC_NONE);
}

ZTEST(can_command_core, test_a_nonsense_brake_current_is_not_a_brake)
{
	struct cc_config bad[] = {{.brake_current_a = -5.0f}, {.brake_current_a = NAN}};

	for (size_t i = 0; i < ARRAY_SIZE(bad); i++) {
		struct safety_output o = output(SAFETY_HEARTBEAT_LOST, true, 0.0f);

		zassert_equal(cc_make_plan(&bad[i], &o).kind, CC_NONE, "case %zu", i);
	}
}

ZTEST(can_command_core, test_brake_current_is_limited_to_the_dbc_range)
{
	struct cc_config big = {.brake_current_a = 5000.0f};
	struct safety_output o = output(SAFETY_HEARTBEAT_LOST, true, 0.0f);
	struct cc_plan p = cc_make_plan(&big, &o);

	expect_frame(&p.frame[0], 2, 0, 200000);
}

ZTEST(can_command_core, test_status_1_ids)
{
	zassert_equal(cc_status_1_id(0), (9u << 8) | 1u);
	zassert_equal(cc_status_1_id(3), (9u << 8) | 4u);
	zassert_equal(cc_status_1_corner((9u << 8) | 3u), 2);
	zassert_equal(cc_status_1_corner((1u << 8) | 3u), -1); /* a command, not a status */
	zassert_equal(cc_status_1_corner((9u << 8) | 5u), -1); /* not one of ours */
}
