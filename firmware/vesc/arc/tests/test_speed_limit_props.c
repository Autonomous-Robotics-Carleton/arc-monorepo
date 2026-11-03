// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Speed limit (ADR-0034, RSK-18): properties over every kind of configured
 * pair, written from speed_limit.h. test_speed_limit.c has the worked cases.
 * Checks don't use assert(): a build with NDEBUG would skip them.
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "arc/speed_limit.h"

static long failures;

#define CHECK(cond, ...)                                                    \
	do {                                                                \
		if (!(cond)) {                                              \
			if (failures < 20) {                                \
				fprintf(stderr, "%s:%d: %s: ", __FILE__,    \
					__LINE__, #cond);                   \
				fprintf(stderr, __VA_ARGS__);               \
				fputc('\n', stderr);                        \
			}                                                   \
			failures++;                                         \
		}                                                           \
	} while (0)

static uint64_t rng = 0x2545F4914F6CDD1DULL;

static float any_float(void)
{
	/* Every bit pattern: finite values of every size, infinities, NaNs. */
	rng ^= rng << 13;
	rng ^= rng >> 7;
	rng ^= rng << 17;
	union {
		uint32_t u;
		float f;
	} v = { (uint32_t)(rng >> 32) };
	return v.f;
}

static void check_pair(float min_in, float max_in)
{
	const float limit = arc_speed_limit_erpm();
	float min = min_in;
	float max = max_in;

	arc_speed_limit_apply(&min, &max);

	/* Never outside the limit, never NaN: upstream compares against these. */
	CHECK(!isnan(min) && !isnan(max), "%g,%g -> NaN", (double)min_in, (double)max_in);
	CHECK(fabsf(min) <= limit && fabsf(max) <= limit, "%g,%g -> %g,%g", (double)min_in,
	      (double)max_in, (double)min, (double)max);
	/* Never looser than a finite configured value on its own side (min <= 0,
	 * max >= 0): a tighter setting is kept. */
	if (isfinite(min_in) && min_in <= 0.0f) {
		CHECK(min >= min_in, "min loosened %g -> %g", (double)min_in, (double)min);
	}
	if (isfinite(max_in) && max_in >= 0.0f) {
		CHECK(max <= max_in, "max loosened %g -> %g", (double)max_in, (double)max);
	}
	/* Within the limit, nothing changes. */
	if (fabsf(min_in) <= limit) {
		CHECK(min == min_in, "min %g changed to %g", (double)min_in, (double)min);
	}
	if (fabsf(max_in) <= limit) {
		CHECK(max == max_in, "max %g changed to %g", (double)max_in, (double)max);
	}
	/* NaN means "no limit configured": the limit, on its own side. */
	if (isnan(min_in)) {
		CHECK(min == -limit, "NaN min -> %g", (double)min);
	}
	if (isnan(max_in)) {
		CHECK(max == limit, "NaN max -> %g", (double)max);
	}
	/* Applying it again changes nothing: it runs on every timer tick. */
	float min2 = min;
	float max2 = max;
	arc_speed_limit_apply(&min2, &max2);
	CHECK(min2 == min && max2 == max, "not idempotent");
}

int main(void)
{
	const float limit = arc_speed_limit_erpm();

	CHECK(limit == ARC_SPEED_LIMIT_ERPM_TBC, "limit %g", (double)limit);
	CHECK(limit > 0.0f && isfinite(limit), "limit %g", (double)limit);

	/* Upstream's defaults (l_min_erpm -100000, l_max_erpm 100000) are capped. */
	float min = -100000.0f;
	float max = 100000.0f;
	arc_speed_limit_apply(&min, &max);
	CHECK(min == -limit && max == limit, "defaults -> %g,%g", (double)min, (double)max);

	const float edges[] = { 0.0f, -0.0f, 1.0f, -1.0f, limit, -limit,
				nextafterf(limit, INFINITY), -nextafterf(limit, INFINITY),
				nextafterf(limit, 0.0f), 1e30f, -1e30f, INFINITY, -INFINITY,
				NAN, -NAN, 1e-45f };
	const int n = (int)(sizeof(edges) / sizeof(edges[0]));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			check_pair(edges[i], edges[j]);
		}
	}
	for (long i = 0; i < 5000000; i++) {
		check_pair(any_float(), any_float());
	}

	if (failures) {
		fprintf(stderr, "%ld check(s) failed\n", failures);
		return 1;
	}
	printf("speed limit properties: all checks passed\n");
	return 0;
}
