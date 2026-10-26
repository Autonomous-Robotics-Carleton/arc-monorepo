# ADR-0051: Car configuration: one file per car in the repo, compiled into the sync MCU's build

- **Status:** Accepted (2026-10-25)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-22, SYS-33, ADR-0047, ADR-0048

## Context

The code is the same on every target (ADR-0047), but every physical car differs slightly. Examples:
- the motor controllers' CAN IDs
- wheel radius, gearing, wheelbase and track width
- steering centre and limits
- sensor positions and calibration
- the envelope's upper limits

These numbers have to live outside the code. The twin needs a set too.

Several components read them:
- **The sync MCU:** splitting the vehicle command into wheel speeds (ADR-0048), the CAN IDs, the envelope's limits.
- **The Orin:** the car backend, the drivers.
- **The twin:** the device models.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. One file per car in the repo; the sync MCU's values compiled into its firmware build for that car** | Simple; nothing to get out of sync at run time; every value is reviewed in a PR | One firmware build per car; the Orin and the sync MCU must be built from the same file |
| B. The same file, sent to the sync MCU over the sync link at start-up | One firmware binary for every car | A new message and a check that both sides agree; safety limits arrive over a network |

## Decision

**Option A.**

1. **One file per car** in the repo: the twin's car, each physical car. The location and format are part of the implementation.
2. **Default values** are generated from `systems/`: the budgets, the BOM, the ICDs. **Measured values** are written over them when the car is built. The twin's file is all defaults.
3. **The sync MCU's values are compiled into its build** for that car, as a generated header or devicetree overlay. Code reads them through one interface, never as literal numbers.
4. **The Orin's nodes and the device models** read the same file at start-up.
5. **Both sides report which configuration they were built from** (a hash), and a mismatch is a fault. How the hash travels, and whether a mismatch refuses drive commands like a protocol mismatch does (ICD-sync-link): TBC, with the implementation.

## Consequences

- **The firmware build takes a car name.** CI builds the twin's car on every PR, and each physical car's configuration once it exists.
- **The generator** for the defaults is the twin's parameter generator (`software/simulation.md`, "Where the parameters come from").
- **Changing a car's configuration is a PR,** so calibration changes are reviewed and logged.
- **Reopen if** the team runs enough cars that one build per car becomes a burden.
