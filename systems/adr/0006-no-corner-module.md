# ADR-0006: No corner module in v1; the VESC is the only board at each corner

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-06, SYS-10, SYS-13, SYS-17
- **Supersedes:** ADR-0003. Reopens ADR-0002 and ADR-0004.

## Context

`architecture.md` builds each corner as a self-contained module: motor, gearbox, a custom VESC-derived controller and a separate STM32G4 sensor node, with a fixed connector set. That puts a custom sensor board on every corner and a second CAN bus on the car before anything drives.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Self-contained corner module with a sensor node (as in architecture.md) | Corner swaps are plug-and-play; sensor firmware isolated from motor control | Four extra custom boards, a CAN-FD bus, and the connector problems in RSK-05 |
| VESC only at each corner | Far fewer boards and buses; one CAN bus | Corner sensors need another home; corners aren't a single swappable unit |

## Decision

No corner module and no corner sensor node in v1. Each corner has a motor, a gearbox and a VESC. Revisit as a later upgrade.

## Consequences

- The CAN-FD sensor bus has no user, so ADR-0002 is reopened. The likely result is one classic CAN bus to the four VESCs.
- The corner signal connector shrinks to CAN + GND (+ e-stop enable / spare), so ICD-corner-connector is redone.
- RSK-03 (CAN-FD ringing) goes away. RSK-05 shrinks to the e-stop enable line and the spare conductor.
- The corner controller stays the custom VESC 6 derived board (decided 2026-10-02). RSK-02 stays open. The board gets a hardware enable input so the e-stop can reach it without a contactor.
- Tire temperature sensors are dropped (decided 2026-10-02). No requirement used them.
- Ride height is settled in ADR-0007.
- The wheel encoders and suspension pots are settled in ADR-0008.
