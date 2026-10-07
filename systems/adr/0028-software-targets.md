# ADR-0028: Every software component builds for simulation, development hardware and the car, from the same source

- **Status:** Accepted
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-07, SYS-21, SYS-22, SYS-24, SYS-29, SYS-33, ADR-0011, ADR-0016, ADR-0017, ADR-0024, ADR-0030

## Context

Hardware arrives in one order, and the team has no development boards. If firmware and platform software can only run on the car's own boards, nothing can be written or tested until the order lands, and then everything is tested for the first time at once.

The car is also a research platform (mission 4): an engineer's experiment should move between a simulator, recorded data and the car without being rewritten.

Software components:

| Component | Runs on | Decided in |
| --- | --- | --- |
| Sync MCU firmware | STM32H723 on the sync board, Zephyr | ADR-0017 |
| VESC firmware | STM32F405 on each VESC fork, VESC firmware (ChibiOS) | ADR-0011, ADR-0030 |
| Platform software | Orin NX, ROS 2 Jazzy | ADR-0016 |
| Experiments | Orin NX, containers | SYS-21 |

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Car hardware only | Least set-up work | Nothing runs before the order; no CI beyond compiling |
| **B. Three kinds of target per component, one source tree** | Firmware and software written and tested before the hardware; CI runs real tests; experiments portable | Target support must be designed in from the start, not added later |
| C. A separate simulation code base | Simulation can be anything | Two code bases drift; tests prove the simulation, not the car's code |

## Criteria

1. Software work can't wait for the hardware (single order).
2. Tests run the code that goes on the car, not a copy.
3. The same experiment runs in simulation and on the car (mission 4).

## Decision

**Option B.** Each component builds from one source tree for three kinds of target. Only board configuration and drivers differ between targets.

| Component | Simulation | Development hardware | The car |
| --- | --- | --- | --- |
| Sync MCU firmware | Zephyr `native_sim` (runs as a program on a laptop or in CI), with loopback CAN and simulated sensors | NUCLEO-H723ZG, same MCU (when bought; none yet) | `arc_sync` board definition, kept in this repo |
| VESC firmware | Our additions (e-stop routine, FD telemetry) unit-tested on the host | A stock VESC 6 (when available) | ARC VESC fork hardware config |
| Platform software | A simulator backend (simulator to be chosen) and an MCAP log-replay backend | The team's existing F1TENTH car, if useful | The car |

How a target is chosen:

- **Firmware:** Zephyr's board and devicetree mechanism. One application, one board definition or overlay per target, chosen at build time (`west build -b <board>`). The VESC fork uses the VESC firmware's own hardware configs.
- **Platform software:** platform nodes talk only to a **car interface**: ROS 2 topics and services defined in `arc_msgs`. Backends implement it for the simulator, log replay and the car (the sync MCU bridge), chosen at launch (`target:=sim | replay | car`). No platform node knows which backend is running.
- **Firmware in the loop:** the sync MCU's `native_sim` build speaks the same protocol to the Orin as the real board. The full stack, real firmware included, can run on a laptop.

## Consequences

- **CI** builds every component for every target, and runs the simulation tests, on every PR (through `nx affected`).
- **The sync MCU ↔ Orin protocol** must be defined independently of the transport, so `native_sim` and the board share it. It needs its own ICD.
- **A simulator must be chosen** for the platform backend. That's a separate decision.
- **The dev-hardware column waits for hardware.** It stays scaffolded but untested until boards are bought or the order lands.
- **Reopen if:** keeping a component buildable for simulation costs more than it saves, e.g. a driver that can't reasonably be simulated.
