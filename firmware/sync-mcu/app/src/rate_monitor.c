#include "rate_monitor.h"

void rate_monitor_init(struct rate_monitor *m, int64_t window_ns, int64_t now_ns)
{
	*m = (struct rate_monitor){.window_ns = window_ns, .window_start_ns = now_ns};
}

void rate_monitor_arrival(struct rate_monitor *m, int64_t now_ns)
{
	if (!m->have_last) {
		m->window_start_ns = now_ns; /* the first window starts with the stream */
	}
	if (m->have_last && now_ns - m->last_ns > m->max_gap_ns) {
		m->max_gap_ns = now_ns - m->last_ns;
	}
	m->last_ns = now_ns;
	m->have_last = true;
	m->count++;
}

bool rate_monitor_poll(struct rate_monitor *m, int64_t now_ns, struct rate_report *r)
{
	int64_t elapsed = now_ns - m->window_start_ns;

	if (!m->have_last || elapsed < m->window_ns) {
		return false;
	}

	int64_t gap = m->max_gap_ns;

	/* a stream that has gone quiet shows as a growing gap, not only once it resumes */
	if (m->have_last && now_ns - m->last_ns > gap) {
		gap = now_ns - m->last_ns;
	}
	r->count = m->count;
	r->rate_hz = (uint32_t)(((int64_t)m->count * 1000000000LL + elapsed / 2) / elapsed);
	r->max_gap_ns = gap;

	m->window_start_ns = now_ns;
	m->count = 0;
	m->max_gap_ns = 0;
	return true;
}

bool rate_within(uint32_t rate_hz, uint32_t expected_hz, uint32_t tolerance_pct)
{
	uint64_t slack = (uint64_t)expected_hz * tolerance_pct / 100;

	return rate_hz + slack >= expected_hz && rate_hz <= expected_hz + slack;
}
