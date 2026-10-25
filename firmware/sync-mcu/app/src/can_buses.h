/*
 * The CAN buses: classic command bus to the four motor controllers
 * (ADR-0009) and the CAN-FD steering bus to the moteus-c1 (ADR-0019).
 * Command frame layouts come from can-command.dbc.
 *
 * The command bus sends every 2 ms, to all four controllers: the safety
 * decision as frames (can_command_core), never silent, so the controllers'
 * own command timeout trips only on real loss (ADR-0035). It receives
 * STATUS_1 from each controller. The steering bus is a stub.
 *
 * Only native_sim has the command-bus device so far (a loopback device, with
 * the simulator bridge behind it: sim_can_bridge.c); the other boards build a
 * stub until their CAN controllers are in the board files.
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_CAN_BUSES_H
#define ARC_CAN_BUSES_H

#include <stdint.h>

#include <zephyr/device.h>

int can_buses_init(void);

/* The command bus device, or NULL where there isn't one. */
const struct device *can_buses_command_device(void);

/* STATUS_1 frames received from a corner, and the sync-MCU time of the last (0 if none). */
uint32_t can_buses_status_count(int corner);
int64_t can_buses_status_last_ns(int corner);

#endif /* ARC_CAN_BUSES_H */
