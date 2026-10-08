/*
 * The link to the Orin over UDP (ICD-sync-link, MAVLink 2, ADR-0031):
 * every sample forwarded within 0.25 ms (ADR-0024, SYS-29); commands and
 * heartbeats back. So far it exchanges ARC_LINK_STATUS once a second on
 * native_sim only; elsewhere it's a stub until the Ethernet stack is set up.
 */

#ifndef ARC_SYNC_LINK_H
#define ARC_SYNC_LINK_H

int sync_link_init(void);

#endif /* ARC_SYNC_LINK_H */
