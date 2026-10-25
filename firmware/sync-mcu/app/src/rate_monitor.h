/*
 * Arrival rate of a message stream, measured over fixed windows. Pure logic:
 * times are passed in (nanoseconds), so the tests run it in virtual time.
 *
 * Used for the Orin's ARC_DRIVE_COMMAND stream (ICD-sync-link: 500 Hz, TBC).
 * It only measures. Whether a slow stream should stop the car is a
 * safety-envelope decision nobody has made yet; today the command timeout
 * (safety.c) is the only enforcement.
 */

#ifndef ARC_RATE_MONITOR_H
#define ARC_RATE_MONITOR_H

#include <stdbool.h>
#include <stdint.h>

struct rate_report {
	uint32_t count;      /* arrivals in the window */
	uint32_t rate_hz;    /* count over the window's real length, rounded */
	int64_t max_gap_ns;  /* longest time between arrivals, including time since the last one */
};

struct rate_monitor {
	int64_t window_ns;
	int64_t window_start_ns;
	int64_t last_ns;
	int64_t max_gap_ns;
	uint32_t count;
	bool have_last;
};

void rate_monitor_init(struct rate_monitor *m, int64_t window_ns, int64_t now_ns);

/* One message arrived. */
void rate_monitor_arrival(struct rate_monitor *m, int64_t now_ns);

/*
 * True, with the report filled in, when a window has ended; then the next
 * window starts. Call often (every task period). Nothing is reported until
 * the first arrival, which starts the first window; after that a window with
 * no arrivals reports count 0.
 */
bool rate_monitor_poll(struct rate_monitor *m, int64_t now_ns, struct rate_report *r);

/* Whether rate_hz is within tolerance_pct of expected_hz (inclusive). */
bool rate_within(uint32_t rate_hz, uint32_t expected_hz, uint32_t tolerance_pct);

#endif /* ARC_RATE_MONITOR_H */
