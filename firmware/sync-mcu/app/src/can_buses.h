/*
 * The CAN buses: classic command bus to the four VESCs (ADR-0009), two
 * CAN-FD telemetry buses (ADR-0011, ADR-0013) and the CAN-FD steering
 * bus to the moteus-c1 (ADR-0019). Frame layouts come from the .dbc
 * files (ICD-corner-connector). Stub.
 */

#ifndef ARC_CAN_BUSES_H
#define ARC_CAN_BUSES_H

int can_buses_init(void);

#endif /* ARC_CAN_BUSES_H */
