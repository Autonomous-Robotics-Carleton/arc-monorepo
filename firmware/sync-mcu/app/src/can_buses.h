/*
 * The CAN buses: classic command bus to the four motor controllers
 * (ADR-0009) and the CAN-FD steering bus to the moteus-c1 (ADR-0019).
 * Command frame layouts come from can-command.dbc. Stub.
 */

#ifndef ARC_CAN_BUSES_H
#define ARC_CAN_BUSES_H

int can_buses_init(void);

#endif /* ARC_CAN_BUSES_H */
