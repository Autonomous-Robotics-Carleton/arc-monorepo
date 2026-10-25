#include <zephyr/ztest.h>

#include "rate_monitor.h"

#define MS 1000000LL
#define SEC 1000000000LL

/* arrivals every period_ns from start (inclusive) to end (exclusive) */
static void stream(struct rate_monitor *m, int64_t start, int64_t end, int64_t period)
{
	for (int64_t t = start; t < end; t += period) {
		rate_monitor_arrival(m, t);
	}
}

ZTEST(rate_monitor, test_no_report_before_the_window_ends)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, 900 * MS, 2 * MS);
	zassert_false(rate_monitor_poll(&m, 999 * MS, &r));
}

ZTEST(rate_monitor, test_steady_500_hz)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, SEC, 2 * MS);
	zassert_true(rate_monitor_poll(&m, SEC, &r));
	zassert_equal(r.count, 500);
	zassert_equal(r.rate_hz, 500);
	zassert_equal(r.max_gap_ns, 2 * MS);
}

ZTEST(rate_monitor, test_windows_follow_each_other)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, SEC, 2 * MS);
	zassert_true(rate_monitor_poll(&m, SEC, &r));
	/* the next stream starts 22 ms after the last arrival, across the window edge */
	stream(&m, SEC + 20 * MS, 2 * SEC, 10 * MS);
	zassert_true(rate_monitor_poll(&m, 2 * SEC, &r));
	zassert_equal(r.count, 98);
	zassert_equal(r.max_gap_ns, 22 * MS);
}

ZTEST(rate_monitor, test_one_lost_datagram_shows_as_a_double_gap)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, 400 * MS, 2 * MS);
	stream(&m, 402 * MS, SEC, 2 * MS);
	zassert_true(rate_monitor_poll(&m, SEC, &r));
	zassert_equal(r.count, 499);
	zassert_equal(r.max_gap_ns, 4 * MS); /* 398 -> 402: the 400 ms one never came */
}

ZTEST(rate_monitor, test_a_stream_that_stops_shows_a_growing_gap)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, 200 * MS, 2 * MS);
	zassert_true(rate_monitor_poll(&m, SEC, &r));
	zassert_equal(r.count, 100);
	zassert_equal(r.max_gap_ns, SEC - 198 * MS);
}

ZTEST(rate_monitor, test_silence_after_a_stream_reports_zero)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, SEC, 2 * MS);
	zassert_true(rate_monitor_poll(&m, SEC, &r));
	zassert_true(rate_monitor_poll(&m, 2 * SEC, &r));
	zassert_equal(r.count, 0);
	zassert_equal(r.rate_hz, 0);
	zassert_true(r.max_gap_ns >= SEC);
}

ZTEST(rate_monitor, test_nothing_is_reported_before_the_first_arrival)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	zassert_false(rate_monitor_poll(&m, 5 * SEC, &r));
}

ZTEST(rate_monitor, test_the_first_window_starts_with_the_stream)
{
	struct rate_monitor m;
	struct rate_report r;

	/* the monitor was set up at 0, the stream starts 130 ms later: not a slow stream */
	rate_monitor_init(&m, SEC, 0);
	stream(&m, 130 * MS, 1130 * MS, 2 * MS);
	zassert_false(rate_monitor_poll(&m, 1129 * MS, &r));
	zassert_true(rate_monitor_poll(&m, 1130 * MS, &r));
	zassert_equal(r.count, 500);
	zassert_equal(r.rate_hz, 500);
}

ZTEST(rate_monitor, test_a_late_poll_uses_the_real_window_length)
{
	struct rate_monitor m;
	struct rate_report r;

	rate_monitor_init(&m, SEC, 0);
	stream(&m, 0, 2 * SEC, 2 * MS);
	zassert_true(rate_monitor_poll(&m, 2 * SEC, &r));
	zassert_equal(r.count, 1000);
	zassert_equal(r.rate_hz, 500);
}

ZTEST(rate_monitor, test_tolerance_is_inclusive_and_two_sided)
{
	zassert_true(rate_within(500, 500, 20));
	zassert_true(rate_within(400, 500, 20));
	zassert_true(rate_within(600, 500, 20));
	zassert_false(rate_within(399, 500, 20));
	zassert_false(rate_within(601, 500, 20));
	zassert_false(rate_within(0, 500, 20));
	zassert_true(rate_within(500, 500, 0));
	zassert_false(rate_within(501, 500, 0));
}

ZTEST_SUITE(rate_monitor, NULL, NULL, NULL, NULL, NULL);
