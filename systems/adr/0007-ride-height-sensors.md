# ADR-0007: Four corner ride-height sensors in v1, plus one paired with the optical flow sensor

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-10, SYS-15, RSK-07, ADR-0014 (the fifth sensor scales the downward event camera)

## Context

ADR-0006 removed the corner sensor nodes, so the ride-height sensors need a new home. The suspension pots and IMUs already give heave, pitch and roll on flat tile, so the question was whether ride height adds anything in v1.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| None | Simplest | Optical flow can't be scaled to m/s; no aero baseline |
| One sensor, paired with the optical flow sensor | Gives the height needed to scale optical flow | No aero baseline; no pot cross-check |
| One at the flow sensor + three reserved near the center | Short runs | Smaller spacing; adding later loses aero-off data |
| One at the flow sensor + four at the corners | Aero-off baseline from day one; widest spacing for pitch/roll; measures the gap at the floor edges where a suction-fan seal matters; cross-checks the pots and tire squash | Four more sync-board inputs; I2C address conflicts; low-mounted parts exposed to crashes |

## Criteria

1. Data the aero capstone needs, including aero-off control data that can't be collected after the fact.
2. Measuring what the pots and IMUs can't (floor gap, tire squash).
3. Harness complexity (not cost; budget isn't the constraint here).

## Decision

Fit four ride-height sensors at the corners, mounted on the chassis (sprung side), and one more paired with the optical ground-speed sensor.

## Consequences

- The sync board needs five ride-height inputs. Most time-of-flight parts share one I2C address, so it also needs an I2C mux or a shutdown (XSHUT) line per sensor.
- Mounts need underside clearance at full suspension compression, and crash protection.
- The sensors face the same glossy-tile risk as optical flow. RSK-07's spike test covers both.
