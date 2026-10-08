// SPDX-License-Identifier: GPL-3.0-or-later
/*
 * UART telemetry (ADR-0024, ADR-0034, ICD-controller-telemetry): full
 * status at 1 kHz as ARC_MOTOR_STATUS, stamped at sampling, from a
 * high-priority thread with DMA. Stub.
 */

#ifndef ARC_TELEMETRY_H
#define ARC_TELEMETRY_H

int arc_telemetry_init(void);

#endif /* ARC_TELEMETRY_H */
