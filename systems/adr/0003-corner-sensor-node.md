# ADR-0003: A separate sensor MCU on each corner

- **Status:** Superseded by ADR-0006
- **Date:** before 2026-09-23
- **Deciders:** TBD
- **Traces to:** SYS-06, SYS-07, SYS-10

## Context

Each corner has a wheel encoder, suspension pot, ride-height sensor and tire temperature sensor to read.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Read them on the corner motor controller | One board per corner | Sensor firmware bugs can affect motor control; modifies VESC firmware |
| Separate STM32G4-class node per corner | Isolates sensor firmware from motor control; short local wiring | Extra board per corner; second CAN pair |
| Wire everything back to the central sync MCU | One MCU | Long analog and encoder runs through a moving suspension |

## Criteria

Sensor firmware must never be able to affect motor control.

## Decision

A separate sensor node per corner, sending on the CAN-FD bus (ADR-0002).

## Consequences

- The node is powered from the corner controller board, so it goes dark when motor power is cut (loop key or e-stop relay). Wheel data is lost exactly when the car is coasting to a stop. Confirm that's acceptable or power the node from the low-voltage side.
