/*
 * The Zephyr side of the safety module (safety.h): holds its state under a
 * lock, decides every millisecond from the heartbeat, the re-arm, the command,
 * the envelope and the controller links, logs each change and reports it to the
 * Orin as ARC_SAFETY_STATE.
 *
 * The decision doesn't reach the brakes yet: the command CAN bus is a stub,
 * so safety_task_output() has no consumer.
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_SAFETY_TASK_H
#define ARC_SAFETY_TASK_H

#include <stdbool.h>

#include "safety.h"

int safety_task_init(void);

/* From the sync link (ARC_OPERATOR_HEARTBEAT, ARC_DRIVE_COMMAND, ARC_ENVELOPE_SET). */
void safety_task_heartbeat(void);
void safety_task_command(struct drive_cmd cmd);
/* False if the envelope was invalid and refused. */
bool safety_task_envelope(const struct safety_envelope *env);
/* ARC_REARM. False if refused (no heartbeat arriving). */
bool safety_task_rearm(void);

/* The latest decision. */
struct safety_output safety_task_output(void);

#endif /* ARC_SAFETY_TASK_H */
