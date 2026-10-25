/*
 * The motor-controller link logic (SYS-25, ADR-0035, ICD-controller-telemetry):
 * parsing, and when the car must stop. Time is injected, so the 5 ms window
 * is checked to the nanosecond, not against a clock.
 *
 * golden.h holds frames from the fake controllers (tools/sim/make_golden.py),
 * made by pymavlink: a second MAVLink implementation checking this parser
 * and the schema.
 */

#include <string.h>

#include <zephyr/ztest.h>

#include "controller_link_core.h"
#include "golden.h"

#define MS 1000000LL
#define SILENCE (5 * MS)

static struct cl_links links;

static void before(void *f)
{
	ARG_UNUSED(f);
	cl_init(&links, SILENCE, MAVLINK_VERSION);
}

ZTEST_SUITE(controller_link, NULL, NULL, before, NULL, NULL);

/* One ARC_MOTOR_STATUS frame for a corner, packed by the C encoder. */
static size_t status_frame(uint8_t *buf, int corner, uint8_t fault, uint8_t state)
{
	mavlink_message_t m;

	mavlink_msg_arc_motor_status_pack(1, corner + 1, &m, 123456789ULL, 1000, 10, 1.0f, 0.0f,
					  0.5f, 0.3f, 16.0f, 30.0f, 28.0f, corner, fault, state);
	return mavlink_msg_to_send_buffer(buf, &m);
}

static size_t link_frame(uint8_t *buf, int corner, uint16_t version)
{
	mavlink_message_t m;

	mavlink_msg_arc_link_status_pack(1, corner + 1, &m, 1ULL, version, 0, 0);
	return mavlink_msg_to_send_buffer(buf, &m);
}

static void feed_status(int corner, int64_t now, uint8_t fault, uint8_t state)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	size_t n = status_frame(buf, corner, fault, state);

	cl_feed(&links, corner, buf, n, now);
}

static void feed_all(int64_t now)
{
	for (int i = 0; i < CL_CORNERS; i++) {
		feed_status(i, now, 0, 0);
	}
}

/* --- parsing ----------------------------------------------------------- */

ZTEST(controller_link, test_golden_frames_from_pymavlink_decode)
{
	mavlink_message_t msg;
	mavlink_status_t st = {0};
	mavlink_arc_motor_status_t s;
	bool got = false;

	for (size_t i = 0; i < sizeof(golden_fl_status) && !got; i++) {
		got = mavlink_parse_char(MAVLINK_COMM_0, golden_fl_status[i], &msg, &st);
	}
	zassert_true(got);
	zassert_equal(msg.msgid, MAVLINK_MSG_ID_ARC_MOTOR_STATUS);
	mavlink_msg_arc_motor_status_decode(&msg, &s);
	zassert_equal(s.erpm, GOLDEN_FL_FIRST_ERPM);
	zassert_equal(s.time_ns, GOLDEN_FL_FIRST_TIME_NS);
	zassert_equal(s.corner, ARC_CORNER_FL);
	zassert_equal(msg.seq, GOLDEN_FL_FIRST_SEQ);
}

ZTEST(controller_link, test_golden_stream_is_accepted_without_gaps)
{
	cl_feed(&links, 0, golden_fl_status, sizeof(golden_fl_status), 100 * MS);
	zassert_equal(links.c[0].frames, GOLDEN_FL_STATUS_FRAMES);
	zassert_equal(links.c[0].gaps, 0);
	zassert_equal(links.c[0].bad, 0);
	zassert_true(links.c[0].seen);
	zassert_equal(links.c[0].last_valid_ns, 100 * MS);
}

ZTEST(controller_link, test_frames_arriving_a_byte_at_a_time)
{
	for (size_t i = 0; i < sizeof(golden_fl_status); i++) {
		cl_feed(&links, 0, &golden_fl_status[i], 1, 10 * MS);
	}
	zassert_equal(links.c[0].frames, GOLDEN_FL_STATUS_FRAMES);
}

ZTEST(controller_link, test_resyncs_after_garbage)
{
	static const uint8_t junk[] = {0x00, 0x13, 0x37, 0xff, 0x02, 0x55};

	cl_feed(&links, 0, junk, sizeof(junk), 1 * MS);
	cl_feed(&links, 0, golden_fl_status, sizeof(golden_fl_status), 2 * MS);
	zassert_equal(links.c[0].frames, GOLDEN_FL_STATUS_FRAMES);
	zassert_true(links.c[0].seen);
}

ZTEST(controller_link, test_a_stray_start_byte_costs_at_most_one_frame)
{
	/* MAVLink framing: a start byte (0xfd) in the noise swallows the next
	 * frame's first bytes as its header; the parser recovers by the next frame */
	static const uint8_t junk[] = {0x00, 0xfd};

	cl_feed(&links, 0, junk, sizeof(junk), 1 * MS);
	cl_feed(&links, 0, golden_fl_status, sizeof(golden_fl_status), 2 * MS);
	zassert_true(links.c[0].frames >= GOLDEN_FL_STATUS_FRAMES - 1);
	zassert_true(links.c[0].seen);
}

ZTEST(controller_link, test_a_frame_that_fails_its_crc_is_not_a_sign_of_life)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	size_t n = status_frame(buf, 0, 0, 0);

	buf[n / 2] ^= 0x5a;
	cl_feed(&links, 0, buf, n, 1 * MS);
	zassert_false(links.c[0].seen);
	zassert_equal(links.c[0].bad, 1);
	zassert_equal(links.c[0].frames, 0);
}

ZTEST(controller_link, test_dropped_frames_are_counted_from_the_sequence_numbers)
{
	uint8_t a[MAVLINK_MAX_PACKET_LEN], b[MAVLINK_MAX_PACKET_LEN], c[MAVLINK_MAX_PACKET_LEN];
	size_t na = status_frame(a, 0, 0, 0);

	status_frame(b, 0, 0, 0); /* lost in transit */
	size_t nc = status_frame(c, 0, 0, 0);

	cl_feed(&links, 0, a, na, 1 * MS);
	cl_feed(&links, 0, c, nc, 3 * MS);
	zassert_equal(links.c[0].gaps, 1);
}

ZTEST(controller_link, test_a_frame_for_another_corner_is_rejected)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	size_t n = status_frame(buf, 2, 0, 0);

	cl_feed(&links, 0, buf, n, 1 * MS);
	zassert_false(links.c[0].seen, "a crossed UART must not look alive");
	zassert_equal(links.c[0].bad, 1);
}

/* --- when to stop ------------------------------------------------------ */

ZTEST(controller_link, test_all_four_corners_alive_is_ok)
{
	for (int64_t t = 0; t <= 100 * MS; t += MS) {
		feed_all(t);
		zassert_equal(cl_check(&links, t, NULL), CL_OK, "t=%lld", t / MS);
	}
}

ZTEST(controller_link, test_a_corner_that_never_reported_is_not_ready_not_a_stop)
{
	int corner = -1;

	for (int i = 0; i < 3; i++) {
		feed_status(i, 1 * MS, 0, 0);
	}
	zassert_equal(cl_check(&links, 2 * MS, &corner), CL_NOT_READY);
	zassert_equal(corner, 3);
	zassert_false(cl_is_stop(CL_NOT_READY));
}

ZTEST(controller_link, test_silent_corner_trips_after_exactly_more_than_5_ms)
{
	int corner = -1;

	for (int64_t t = 0; t <= 100 * MS; t += MS) {
		feed_all(t);
	}
	/* corner 1 goes quiet after t = 100 ms; the others carry on */
	for (int64_t t = 101 * MS; t <= 120 * MS; t += MS) {
		feed_status(0, t, 0, 0);
		feed_status(2, t, 0, 0);
		feed_status(3, t, 0, 0);
	}
	zassert_equal(cl_check(&links, 100 * MS + SILENCE, &corner), CL_OK, "exactly 5 ms");
	zassert_equal(cl_check(&links, 100 * MS + SILENCE + 1, &corner), CL_SILENT);
	zassert_equal(corner, 1);
}

ZTEST(controller_link, test_one_dropped_frame_does_not_stop_the_car)
{
	for (int64_t t = 0; t <= 50 * MS; t += MS) {
		feed_all(t);
	}
	/* corner 3 misses the frames at 51 and 52 ms; the others don't */
	for (int64_t t = 51 * MS; t <= 60 * MS; t += MS) {
		for (int i = 0; i < 3; i++) {
			feed_status(i, t, 0, 0);
		}
		if (t > 52 * MS) {
			feed_status(3, t, 0, 0);
		}
		zassert_equal(cl_check(&links, t, NULL), CL_OK, "t=%lld", t / MS);
	}
}

ZTEST(controller_link, test_a_silent_corner_recovers_when_frames_return)
{
	int corner;

	feed_all(0);
	zassert_equal(cl_check(&links, 20 * MS, &corner), CL_SILENT);
	feed_all(21 * MS);
	zassert_equal(cl_check(&links, 21 * MS, NULL), CL_OK);
}

ZTEST(controller_link, test_the_lowest_silent_corner_is_reported_first)
{
	int corner = -1;

	feed_all(0);
	feed_status(0, 10 * MS, 0, 0);
	feed_status(1, 10 * MS, 0, 0);
	zassert_equal(cl_check(&links, 11 * MS, &corner), CL_SILENT);
	zassert_equal(corner, 2);
}

ZTEST(controller_link, test_link_status_is_not_a_sign_of_life_for_the_motor_status)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	int corner = -1;

	feed_all(0);
	for (int64_t t = MS; t <= 20 * MS; t += MS) {
		for (int i = 0; i < 4; i++) {
			if (i != 2) {
				feed_status(i, t, 0, 0);
			}
		}
		cl_feed(&links, 2, buf, link_frame(buf, 2, MAVLINK_VERSION), t);
	}
	zassert_equal(cl_check(&links, 20 * MS, &corner), CL_SILENT);
	zassert_equal(corner, 2);
}

ZTEST(controller_link, test_estop_state_stops_at_once)
{
	int corner = -1;

	feed_all(1 * MS);
	feed_status(3, 2 * MS, 0, ARC_MOTOR_STATE_ESTOP);
	zassert_equal(cl_check(&links, 2 * MS, &corner), CL_ESTOP);
	zassert_equal(corner, 3);
}

ZTEST(controller_link, test_command_timeout_state_stops_at_once)
{
	int corner = -1;

	feed_all(1 * MS);
	feed_status(0, 2 * MS, 0, ARC_MOTOR_STATE_COMMAND_TIMEOUT);
	zassert_equal(cl_check(&links, 2 * MS, &corner), CL_COMMAND_TIMEOUT);
	zassert_equal(corner, 0);
}

ZTEST(controller_link, test_any_state_bit_or_fault_code_stops)
{
	int corner = -1;

	feed_all(1 * MS);
	feed_status(1, 2 * MS, 7, 0);
	zassert_equal(cl_check(&links, 2 * MS, &corner), CL_FAULT);
	zassert_equal(corner, 1);
}

ZTEST(controller_link, test_golden_estop_and_fault_frames_from_pymavlink)
{
	int corner = -1;

	cl_feed(&links, 2, golden_rl_estop, sizeof(golden_rl_estop), 1 * MS);
	zassert_equal(cl_check(&links, 1 * MS, &corner), CL_ESTOP);
	zassert_equal(corner, 2);

	cl_init(&links, SILENCE, MAVLINK_VERSION);
	cl_feed(&links, 2, golden_rl_fault, sizeof(golden_rl_fault), 1 * MS);
	zassert_equal(cl_check(&links, 1 * MS, &corner), CL_FAULT);
	zassert_equal(links.c[2].fault, GOLDEN_RL_FAULT_CODE);
}

ZTEST(controller_link, test_stop_reasons_outrank_a_missing_corner)
{
	int corner = -1;

	feed_status(2, 1 * MS, 0, ARC_MOTOR_STATE_ESTOP); /* others never reported */
	zassert_equal(cl_check(&links, 1 * MS, &corner), CL_ESTOP);
	zassert_equal(corner, 2);
}

ZTEST(controller_link, test_a_state_bit_the_firmware_does_not_know_stops_the_car)
{
	int corner = -1;

	feed_all(1 * MS);
	feed_status(1, 2 * MS, 0, 0x04); /* a bit a newer controller might set */
	zassert_equal(cl_check(&links, 2 * MS, &corner), CL_UNKNOWN_STATE);
	zassert_equal(corner, 1);
	zassert_true(cl_is_stop(CL_UNKNOWN_STATE));
}

/* --- robustness ---------------------------------------------------------- */

ZTEST(controller_link, test_chunking_does_not_change_what_is_parsed)
{
	uint8_t stream[3 * MAVLINK_MAX_PACKET_LEN];
	size_t len = 0;

	for (int i = 0; i < 3; i++) {
		len += status_frame(stream + len, 0, 0, 0);
	}
	for (size_t chunk = 1; chunk <= 17; chunk++) {
		cl_init(&links, SILENCE, MAVLINK_VERSION);
		for (size_t off = 0; off < len; off += chunk) {
			cl_feed(&links, 0, stream + off, MIN(chunk, len - off), 1 * MS);
		}
		zassert_equal(links.c[0].frames, 3, "chunk %zu", chunk);
		zassert_equal(links.c[0].gaps, 0, "chunk %zu", chunk);
		zassert_equal(links.c[0].bad, 0, "chunk %zu", chunk);
	}
}

ZTEST(controller_link, test_noise_never_looks_alive)
{
	/* 256 KiB of a fixed pseudo-random sequence: the parser must neither
	 * crash nor accept a frame, and the corner must stay unseen */
	uint32_t x = 12345;
	uint8_t buf[256];

	for (int block = 0; block < 1024; block++) {
		for (size_t i = 0; i < sizeof(buf); i++) {
			x = x * 1664525u + 1013904223u;
			buf[i] = x >> 24;
		}
		cl_feed(&links, 0, buf, sizeof(buf), block * MS);
	}
	zassert_false(links.c[0].seen);
	zassert_equal(cl_check(&links, 2000 * MS, NULL), CL_NOT_READY);
}

/* --- protocol version -------------------------------------------------- */

ZTEST(controller_link, test_matching_version_from_pymavlink_is_accepted)
{
	zassert_equal(GOLDEN_LINK_VERSION, MAVLINK_VERSION, "the two builds read different schemas");
	cl_feed(&links, 0, golden_fl_link_status, sizeof(golden_fl_link_status), 1 * MS);
	zassert_true(links.c[0].version_known);
	feed_all(1 * MS);
	zassert_equal(cl_check(&links, 1 * MS, NULL), CL_OK);
}

ZTEST(controller_link, test_version_mismatch_refuses_drive_without_a_stop)
{
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	int corner = -1;

	feed_all(1 * MS);
	cl_feed(&links, 3, buf, link_frame(buf, 3, MAVLINK_VERSION + 1), 1 * MS);
	zassert_equal(cl_check(&links, 1 * MS, &corner), CL_VERSION);
	zassert_equal(corner, 3);
	zassert_false(cl_is_stop(CL_VERSION));
}
