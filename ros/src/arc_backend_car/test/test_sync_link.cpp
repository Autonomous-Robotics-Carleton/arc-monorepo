// The generated sync-link code (ADR-0031) builds on the Orin side and
// round-trips a command: packed by the bridge, parsed back byte by byte.

#include <gtest/gtest.h>

#include "arc_sync_link/mavlink.h"

TEST(SyncLink, DriveCommandRoundTrip)
{
  mavlink_message_t tx;
  uint8_t buf[MAVLINK_MAX_PACKET_LEN];
  mavlink_msg_arc_drive_command_pack(1, 1, &tx, 42ULL, 1.5f, 2.0f, 0.1f);
  const uint16_t len = mavlink_msg_to_send_buffer(buf, &tx);

  mavlink_message_t rx;
  mavlink_status_t status{};
  bool parsed = false;
  for (uint16_t i = 0; i < len; i++) {
    parsed = mavlink_parse_char(MAVLINK_COMM_0, buf[i], &rx, &status) || parsed;
  }
  ASSERT_TRUE(parsed);
  ASSERT_EQ(rx.msgid, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND);

  mavlink_arc_drive_command_t out;
  mavlink_msg_arc_drive_command_decode(&rx, &out);
  EXPECT_EQ(out.time_ns, 42ULL);
  EXPECT_FLOAT_EQ(out.speed, 1.5f);
  EXPECT_FLOAT_EQ(out.accel, 2.0f);
  EXPECT_FLOAT_EQ(out.steer, 0.1f);
}
