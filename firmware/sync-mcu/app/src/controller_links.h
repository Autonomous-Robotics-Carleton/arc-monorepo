/*
 * Telemetry from the four motor controllers: one UART each, point to
 * point, full status at 1 kHz stamped at sampling (ADR-0024, ADR-0034).
 * Message set: ICD-controller-telemetry (draft): MAVLink 2, ARC_MOTOR_STATUS
 * from systems/icd/sync-link.xml. Stub.
 */

#ifndef ARC_CONTROLLER_LINKS_H
#define ARC_CONTROLLER_LINKS_H

int controller_links_init(void);

#endif /* ARC_CONTROLLER_LINKS_H */
