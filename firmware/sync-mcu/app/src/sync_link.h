/*
 * The link to the Orin over Ethernet: every sample forwarded within
 * 0.25 ms over UDP (ADR-0024, SYS-29); commands and heartbeats back.
 * Protocol in ICD-sync-link (not written yet). Stub.
 */

#ifndef ARC_SYNC_LINK_H
#define ARC_SYNC_LINK_H

int sync_link_init(void);

#endif /* ARC_SYNC_LINK_H */
