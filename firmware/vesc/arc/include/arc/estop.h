// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * E-stop brake routine (ADR-0012, ADR-0034): reads ESTOP on the Servo/PPM
 * pin with the MCU's pull-down, ignores CAN commands while it's low, and
 * ramps brake current to the target deceleration. Safety function: its own
 * tests, extra review on every change. Stub.
 */

#ifndef ARC_ESTOP_H
#define ARC_ESTOP_H

#include <stdbool.h>

/* True while the car must brake. Stub: always true, so nothing moves. */
bool arc_estop_engaged(void);

#endif /* ARC_ESTOP_H */
