# ADR-0009: One classic CAN bus to the four corner VESCs

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-06, RSK-03
- **Supersedes:** ADR-0002

## Context

ADR-0002 split the car into a classic CAN bus (motor control) and a CAN-FD bus (corner sensor nodes). ADR-0006 removed the sensor nodes and ADR-0008 wired the corner sensors to the sync board, so the CAN-FD bus has no users.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Keep a CAN-FD bus, unpopulated | Room for future CAN-FD devices | A second pair to every corner, and a second transceiver, for nothing |
| One classic CAN bus | One pair, one bitrate, VESC-native | Future CAN-FD devices need their own bus |

## Criteria

Don't build interfaces with no user. The sync MCU's second FD-capable controller remains, so a CAN-FD bus can be added later without a new MCU.

## Decision

One classic CAN bus between the sync MCU and the four VESCs.

| Parameter | Value |
| --- | --- |
| Bitrate | 1 Mbit/s (TBC; VESC supports 125 kbit/s–1 Mbit/s) |
| Topology | Linear trunk in the harness, stubs to each VESC ≤ 0.3 m |
| Termination | 120 Ω at the sync board and 120 Ω at the far end of the trunk, both off the corner boards |
| Frames | VESC protocol, extended IDs. Message set and rates live in `icd/can.dbc` |

## Consequences

- RSK-03 (CAN-FD ringing) is retired.
- No corner board carries termination, so all four corner boards are identical and any one can go in any corner.
- Motor commands and VESC status frames share the bus. VESC command IDs (e.g. SET_CURRENT = 1) win arbitration over status frames (STATUS = 9 and up), so a command waits at most one frame time (about 130 µs at 1 Mbit/s). This holds only while bus load stays under about 70%, so status rates are capped by the bus-load budget.
- The second FD-capable controller on the sync MCU stays as a spare (architecture rule: every channel has a spare).
