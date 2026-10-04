# ADR-0026: One motor per wheel, mounted transverse, inboard and staggered fore and aft

- **Status:** Accepted (backfilled)
- **Date:** before 2026-09-23 (original spec); recorded 2026-10-03
- **Deciders:** Shrikar Vempati (original spec)
- **Traces to:** SYS-01, SYS-02, SYS-10, SYS-13, SYS-16, ADR-0006, ADR-0019, RSK-04, RSK-16, E-51

> [!NOTE]
> **Backfilled.** The decision was made in the original spec, before this ADR process existed. The context, decision and consequences come from the current docs (`architecture.md`, `mechanical/layout-brief.md`). **The options and criteria below are drafted from those docs, not recorded from the original reasoning: confirm or correct them.**

## Context

The car is 4WD with one Castle 1010-4400kV motor per wheel (Ø28 × 58.4 mm can), each through a two-stage gearbox to its wheel. The four motors and gearboxes have to fit inside the F1TENTH/Roboracer width of 238–341 mm (SYS-02), with heavy parts low and central (SYS-16), and each corner swappable as a unit (SYS-13).

## Options

*Drafted, see the note above.*

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Transverse (parallel to the axle), inboard, two motors per axle staggered fore and aft; CVD to each wheel** | Two motors and their gearboxes fit the width; plain spur gears (no right-angle drive); motors low and central; stage 1 gearing swappable | Front corners are tight with steering at full lock and bump (RSK-04); CVDs needed; a large stagger offset may force a belt in stage 2 |
| B. Longitudinal motors with a right-angle (bevel) drive to each wheel | Narrow | Bevel gears at tens of thousands of rpm; more parts, losses and noise |
| C. Motors in or at the wheel hubs | No CVDs; simplest driveline | A 28 mm × 58 mm motor plus gearing doesn't fit a ~64 mm wheel; unsprung mass; crowds the steering knuckle |
| D. One or two motors with differentials (conventional 1/10) | Proven, off-the-shelf drivetrain | Loses independent per-wheel torque, which four motors exist to provide |

## Criteria

*Drafted, see the note above.*

1. Independent torque at each wheel (see the gap noted below).
2. Fit the four motors and gearboxes inside SYS-02's width.
3. A stiff, aligned drivetrain for ≥ 12 m/s (SYS-01).
4. Heavy parts low and central (SYS-16); corners swappable as units (SYS-13).

## Decision

One Castle 1010-4400kV motor per wheel. Motors transverse, inboard, staggered fore and aft on each axle. Two-stage gearbox per corner: a swappable stage-1 spur pair and a fixed ~4.5:1 stage 2, then a CVD to the wheel (`architecture.md`, Drive).

## Consequences

- **RSK-04:** the front corners must fit the staggered motor, both gear stages, the CVD, the wheel and suspension encoders, the knuckle encoder and the steering actuator (ADR-0019), at full lock and full bump. The CAD block layout checks this first (`mechanical/layout-brief.md`).
- **Stage 2 fallback:** a belt instead of spur gears if the stagger offset gets too large for gears.
- **The stagger sets the wheelbase floor.** It interacts with SYS-02's minimum length (RSK-16).
- **Requirement gap:** no SYS requirement asks for independent torque at each wheel. SYS-10 asks to *measure* per-wheel speed, and the mission says 4WD, but nothing states why four motors rather than one. If per-wheel torque control is the reason (torque vectoring, traction control and per-wheel MPC for the research platform), it should be written as a requirement, so this decision has a parent.
- **Reopen if:** the block layout can't fit the front corners (RSK-04) without moving the motors.
