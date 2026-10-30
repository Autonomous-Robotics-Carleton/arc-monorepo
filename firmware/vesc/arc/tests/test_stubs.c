// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Placeholder: the stubs are fail-safe. Replace with real tests as each
 * module is implemented; the e-stop routine needs its own suite.
 */

/* The checks are asserts: never let a release build (NDEBUG) drop them. */
#undef NDEBUG
#include <assert.h>
#include <stdio.h>

#include "arc/estop.h"
#include "arc/telemetry.h"

int main(void)
{
	assert(arc_estop_engaged());
	assert(arc_telemetry_init() == 0);
	puts("arc stubs: fail-safe");
	return 0;
}
