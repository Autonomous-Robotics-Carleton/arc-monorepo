/*
 * The link to the Orin over UDP (ICD-sync-link, MAVLink 2, ADR-0031):
 * every sample forwarded within 0.25 ms (ADR-0024, SYS-29); commands and
 * heartbeats back. On native_sim only; elsewhere it's a stub until the
 * Ethernet stack is set up. It exchanges ARC_LINK_STATUS once a second,
 * hands ARC_OPERATOR_HEARTBEAT, ARC_DRIVE_COMMAND and ARC_ENVELOPE_SET to
 * the safety task, and sends ARC_SAFETY_STATE on request.
 */

#ifndef ARC_SYNC_LINK_H
#define ARC_SYNC_LINK_H

int sync_link_init(void);

/* ARC_SAFETY_STATE to the Orin (SYS-22 confirmation, watchdog state). watchdog is an
 * ARC_WATCHDOG_STATE. A no-op where the link isn't up. */
void sync_link_send_safety_state(float max_speed, float max_accel, float max_steer,
				 int watchdog, int estop);

#endif /* ARC_SYNC_LINK_H */
