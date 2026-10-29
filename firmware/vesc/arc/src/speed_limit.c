// SPDX-License-Identifier: GPL-3.0-or-later
#include "arc/speed_limit.h"

float arc_speed_limit_erpm(void)
{
	return ARC_SPEED_LIMIT_ERPM_TBC;
}

/* x clamped into [-limit, limit]; NaN becomes nan_value. */
static float clamp(float x, float limit, float nan_value)
{
	if (x != x) {
		return nan_value;
	}
	if (x > limit) {
		return limit;
	}
	if (x < -limit) {
		return -limit;
	}
	return x;
}

void arc_speed_limit_apply(float *min_erpm, float *max_erpm)
{
	const float limit = arc_speed_limit_erpm();

	*min_erpm = clamp(*min_erpm, limit, -limit);
	*max_erpm = clamp(*max_erpm, limit, limit);
}
