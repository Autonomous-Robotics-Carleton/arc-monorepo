# ADR-0002: Separate classic CAN and CAN-FD buses

- **Status:** Superseded by ADR-0009
- **Date:** before 2026-09-23
- **Deciders:** TBD
- **Traces to:** SYS-06, RSK-03

## Context

The VESC-derived corner controllers use classic CAN. The corner sensor nodes want CAN-FD for 1 kHz frames. Classic CAN controllers flag CAN-FD frames as errors, so the two can't share a bus.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| One classic CAN bus for everything | One harness pair | Not enough bandwidth for 4 × 1 kHz sensor frames; sensor traffic could delay motor commands |
| Classic CAN (motors) + CAN-FD (sensors) | Motor commands are never delayed by sensor traffic; sensor bandwidth | Two pairs to every corner; two FD-capable controllers on the sync MCU |
| CAN-FD everywhere | One bus | Needs FD-capable motor controllers (not stock VESC 6) |

## Criteria

Motor command latency must not depend on sensor traffic. Sensor bandwidth comes second.

## Decision

Two buses: classic CAN for motor control, CAN-FD for corner sensors.

## Consequences

- The corner connector carries two differential pairs (see `icd/corner-connector.md`).
- The 1 kHz rate that motivates CAN-FD has no parent requirement yet (SYS-06). If the MPC needs much less, reopen this.
