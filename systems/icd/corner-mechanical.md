# ICD-corner-mechanical

- **Revision:** draft outline (2026-10-03). Values come from the block layout (`mechanical/cad-workflow.md`, phase 2).
- **Status:** Draft. Needs an owner on each side and their sign-off.
- **Side A:** corner assembly: motor (E-51), gearbox and bearing plates, CVD, wheel encoder (E-22), suspension pivot sensor (E-23); front corners also the knuckle encoder (E-24) and steering linkage (owner TBD)
- **Side B:** chassis: lower tub, splice plates, suspension mounts (owner TBD)
- **Traces to:** SYS-01, SYS-02, SYS-13, ADR-0006, ADR-0008, ADR-0019, ADR-0026, ADR-0034, RSK-04, RSK-17

A corner is swapped as a unit (SYS-13). This document fixes everything the two sides must agree on so each can be designed separately: where the corner sits, how it's held, and what crosses the boundary.

## Datums

| Item | Value |
| --- | --- |
| Corner datum | TBD: e.g. the gearbox plate's mounting face and one dowel hole |
| Position of each corner's datum in car coordinates | TBD from `wheelbase`, `track_front`, `track_rear`, `motor_stagger` (CAD parameters) |
| Front and rear corners identical or mirrored | TBD |

## Mounting

| Item | Value |
| --- | --- |
| Fasteners: size, count, pattern | TBD |
| Location features (dowels or shoulder bolts) | TBD: precision is held in aluminium, never in the print (printed-deck rules) |
| Into the tub: heat-set inserts or through-bolts to a splice plate | TBD (RSK-17) |
| Loads at the joint: drive torque reaction, braking, crash | TBD, written down per the fab gate |

## Envelope and clearances

| Item | Value |
| --- | --- |
| Corner envelope (box) | TBD |
| Clearance at full steering lock and full bump and droop | TBD (RSK-04) |
| Space kept for the steering actuator (front) | TBD (ADR-0019) |
| Encoder pocket behind each motor (future rear-shaft encoder) | Reserved (`architecture.md`) |

## What crosses the boundary

| Item | Value |
| --- | --- |
| Motor phase and Hall cable connectors at the corner boundary (ADR-0034): position and access | TBD; electrical side in ICD-corner-connector rev E |
| Wheel and suspension sensor cables to the sync board (ADR-0008) | TBD routing and strain relief |
| Motor phase leads and Hall sensor cable to the controller on the deck (ADR-0034) | TBD routing and strain relief across the suspension; Hall cable away from the phase leads |

## Swap procedure (SYS-13)

TBD: tools, steps and time, demonstrated per the verification plan. Known step since ADR-0034: the controller stays on the deck, so after a swap it loads the new motor's saved configuration (resistance, inductance, flux, Hall table) with VESC Tool, or re-runs motor detection with the wheel off the ground (~1 min). Each motor is labelled and its configuration kept in the repo.
