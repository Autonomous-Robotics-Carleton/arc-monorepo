// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Speed limit (ADR-0034, RSK-18): the configured eRPM limits never exceed
 * the cap, whatever VESC Tool or a CAN configuration message writes.
 */

/* The checks are asserts: never let a release build (NDEBUG) drop them. */
#undef NDEBUG
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "arc/speed_limit.h"

static void apply(float min_in, float max_in, float min_out, float max_out)
{
	float min_erpm = min_in;
	float max_erpm = max_in;

	arc_speed_limit_apply(&min_erpm, &max_erpm);
	assert(min_erpm == min_out);
	assert(max_erpm == max_out);
}

int main(void)
{
	const float cap = ARC_SPEED_LIMIT_ERPM_TBC;

	assert(arc_speed_limit_erpm() == cap);
	assert(cap > 0.0f);

	/* Upstream's defaults (+-100k) and the A50S's hardware limit (+-200k). */
	apply(-100e3f, 100e3f, -cap, cap);
	apply(-200e3f, 200e3f, -cap, cap);

	/* A tighter configuration is kept, either side. */
	apply(-30e3f, 50e3f, -30e3f, 50e3f);
	apply(-cap, cap, -cap, cap);
	apply(0.0f, 0.0f, 0.0f, 0.0f);

	/* Asymmetric: only the side over the cap moves. */
	apply(-90e3f, 40e3f, -cap, 40e3f);
	apply(-10e3f, 90e3f, -10e3f, cap);

	/* Limits on the wrong side of zero stay within the cap too. */
	apply(90e3f, -90e3f, cap, -cap);

	/* Infinities and NaNs never lift the cap. */
	apply(-INFINITY, INFINITY, -cap, cap);
	apply(NAN, NAN, -cap, cap);

	/* Applying twice changes nothing more. */
	{
		float min_erpm = -150e3f;
		float max_erpm = 150e3f;

		arc_speed_limit_apply(&min_erpm, &max_erpm);
		arc_speed_limit_apply(&min_erpm, &max_erpm);
		assert(min_erpm == -cap && max_erpm == cap);
	}

	puts("arc speed limit: ok");
	return 0;
}
