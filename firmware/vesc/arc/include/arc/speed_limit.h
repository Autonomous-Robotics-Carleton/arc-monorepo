// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * Speed limit (ADR-0034): the maximum eRPM, ~75k (TBC), keeps the motor
 * where the controller's control loop has >= 10 samples per electrical
 * cycle (RSK-18). Stub.
 */

#ifndef ARC_SPEED_LIMIT_H
#define ARC_SPEED_LIMIT_H

/* The largest |eRPM| allowed. Stub: 0, so nothing moves. */
float arc_speed_limit_erpm(void);

#endif /* ARC_SPEED_LIMIT_H */
