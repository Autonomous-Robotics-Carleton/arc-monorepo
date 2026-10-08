/*
 * The generated sync-link code (ADR-0031) builds here and round-trips a
 * message: packed by the sync MCU's encoder, parsed back byte by byte.
 */

#include <zephyr/ztest.h>

#include "arc_sync_link/mavlink.h"

ZTEST(sync_link, test_link_status_round_trip)
{
	mavlink_message_t tx;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];

	mavlink_msg_arc_link_status_pack(1, 1, &tx, 123456789ULL,
					 MAVLINK_VERSION, 2, 3);
	uint16_t len = mavlink_msg_to_send_buffer(buf, &tx);

	mavlink_message_t rx;
	mavlink_status_t status = {0};
	bool parsed = false;

	for (uint16_t i = 0; i < len; i++) {
		if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &rx, &status)) {
			parsed = true;
		}
	}
	zassert_true(parsed, "frame not parsed");
	zassert_equal(rx.msgid, MAVLINK_MSG_ID_ARC_LINK_STATUS);

	mavlink_arc_link_status_t out;

	mavlink_msg_arc_link_status_decode(&rx, &out);
	zassert_equal(out.time_ns, 123456789ULL);
	zassert_equal(out.protocol_version, MAVLINK_VERSION);
	zassert_equal(out.rx_gaps, 2);
	zassert_equal(out.rx_bad, 3);
}

ZTEST_SUITE(sync_link, NULL, NULL, NULL, NULL, NULL);
