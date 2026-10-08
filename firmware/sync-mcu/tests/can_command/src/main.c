/*
 * The generated command-bus code (systems/icd/can-command.dbc) matches
 * the stock VESC packing in comm/comm_can.c: extended ID
 * (packet << 8) | controller ID, big-endian int32 current x 1000, and
 * status 1 as int32 eRPM, int16 current x 10, int16 duty x 1000.
 */

#include <zephyr/ztest.h>

#include "can_command.h"

ZTEST(can_command, test_set_current_matches_vesc)
{
	struct can_command_set_current_fl_t msg = {
		.current = can_command_set_current_fl_current_encode(12.5),
	};
	uint8_t frame[CAN_COMMAND_SET_CURRENT_FL_LENGTH];

	zassert_equal(can_command_set_current_fl_pack(frame, &msg, sizeof(frame)), 4);
	zassert_equal(CAN_COMMAND_SET_CURRENT_FL_FRAME_ID, (1u << 8) | 1u);
	zassert_true(CAN_COMMAND_SET_CURRENT_FL_IS_EXTENDED);

	/* VESC: buffer_append_int32(current * 1000) = 12500 = 0x000030D4 */
	const uint8_t expected[] = {0x00, 0x00, 0x30, 0xD4};

	zassert_mem_equal(frame, expected, sizeof(expected));
}

ZTEST(can_command, test_status_1_decodes_vesc_frame)
{
	/* 80000 eRPM, 12.0 A (120), duty 0.8 (800), as VESC sends it */
	const uint8_t frame[] = {0x00, 0x01, 0x38, 0x80, 0x00, 0x78, 0x03, 0x20};
	struct can_command_status_1_rr_t msg;

	zassert_equal(can_command_status_1_rr_unpack(&msg, frame, sizeof(frame)), 0);
	zassert_equal(CAN_COMMAND_STATUS_1_RR_FRAME_ID, (9u << 8) | 4u);
	zassert_equal(msg.erpm, 80000);
	zassert_within(can_command_status_1_rr_current_decode(msg.current), 12.0, 1e-6);
	zassert_within(can_command_status_1_rr_duty_decode(msg.duty), 0.8, 1e-6);
}

ZTEST_SUITE(can_command, NULL, NULL, NULL, NULL, NULL);
