// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * The e-stop routine's hooks into upstream VESC firmware (ADR-0037),
 * implemented in port/estop_vesc.c; the logic is in arc/estop.h. Firmware
 * only: not built on the host.
 */

#ifndef ARC_ESTOP_PORT_H
#define ARC_ESTOP_PORT_H

#include <stdbool.h>

/* Configures the ESTOP pin and starts the 1 ms routine thread. Called once
 * from main(), right after mc_interface_init(). Until the thread's first
 * step, commands are ignored (arc_estop_state starts engaged). */
void arc_estop_start(void);

/* Called from upstream's timeout_reset(), which every motor command calls
 * (CAN, USB, apps): a command arrived. */
void arc_estop_note_command(void);

/* Called from mc_interface_try_input(): true if motor commands must be
 * ignored now. */
bool arc_estop_input_blocked(void);

#endif /* ARC_ESTOP_PORT_H */
