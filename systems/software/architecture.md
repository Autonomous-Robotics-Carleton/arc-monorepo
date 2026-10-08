# Software architecture (v1)

- **Status:** Draft (2026-10-07). The repo is being set up for software: see [`setup-plan.md`](setup-plan.md).
- **Decisions:** ADR-0028 (targets), ADR-0029 (dev environment), ADR-0033 (VESC firmware vendored in this repo; supersedes ADR-0030); also ADR-0016 (Orin platform), ADR-0017 (Zephyr), ADR-0021 (time sync), ADR-0024 (telemetry latency).
- **Requirements:** SYS-33 (targets) and the software-facing requirements SYS-06, SYS-09, SYS-21 to SYS-24, SYS-29.

The car's software, where each part lives in this repo, and how each part runs on more than one target.

## Components

| Component | Runs on | Job | Lives in |
| --- | --- | --- | --- |
| Sync MCU firmware | STM32H723, Zephyr | The car's time base; samples and timestamps the vehicle-state sensors; drives the command and steering CAN buses and reads each motor controller's telemetry UART (ADR-0034); heartbeat watchdog and safety envelope (SYS-04, SYS-22); streams everything to the Orin | `firmware/sync-mcu/` |
| VESC firmware | STM32F4 on each A50S motor controller (on the deck, ADR-0034), VESC firmware | Motor control (upstream); our e-stop brake routine, UART telemetry and speed limit | `firmware/vesc/` |
| Platform software | Orin NX, ROS 2 Jazzy, PREEMPT_RT | Sync MCU bridge, camera and LiDAR drivers, logging, state estimation, classical control, teleop: maintained by the team and always up (SYS-21) | `ros/` |
| Platform image | Orin NX | JetPack, kernel, containers and services, pinned (ADR-0016) | `platform/` |
| Experiments | Orin NX, containers with resource limits | Whatever an engineer is trying; commands the classical control layer, never the motors (SYS-21, SYS-22) | `experiments/` (scaffold) |

## Layers

```
 experiments (containers)          learned or classical policies
        │  targets only, through the car interface
 platform software (ROS 2)         estimation, classical control, logging, teleop
        │  car interface: arc_msgs topics and services
 backend: sim | replay | car       one per target (ADR-0028)
        │  car backend only: sync link (UDP)
 sync MCU firmware (Zephyr)        time base, sensors, safety envelope, CAN
        │  CAN: command (classic), steering (FD); UART: telemetry from each controller
 VESC firmware ×4, moteus-c1       motor control, e-stop braking
```

- **The safety envelope** lives on the sync MCU (SYS-22). Nothing above it can command past it, on any target.
- **Experiments never talk to a backend directly.** They command the classical layer, which runs inside the envelope (requirements: layered control).

## Targets (ADR-0028)

| Component | Simulation | Development hardware | The car |
| --- | --- | --- | --- |
| Sync MCU firmware | `native_sim`: runs on a laptop or in CI, loopback CAN, simulated sensors | NUCLEO-H723ZG (2 in the order, SQ-5) | `arc_sync` board definition |
| VESC firmware | Our additions unit-tested on the host | A spare A50S running stock firmware | The A50S with our firmware (upstream target `a50s_v23c_12s`) |
| Platform software | Simulator backends (Gazebo Harmonic, Webots, F1TENTH gym; ADR-0032); MCAP log replay | The team's F1TENTH car, if useful | The car |

The sync MCU's `native_sim` build speaks the same sync-link protocol as the real board, so the full stack, real firmware included, runs on a laptop.

## Interfaces

| Interface | Defined in | Generated into |
| --- | --- | --- |
| Command CAN bus (classic) | `can-command.dbc` (draft; stock VESC frames) | Sync MCU firmware (the VESC side is stock) |
| Controller telemetry (UART) | ICD controller-telemetry (draft; MAVLink 2, the ARC message set in `sync-link.xml`) | Sync MCU firmware, VESC firmware |
| Sync MCU ↔ Orin (UDP) | ICD-sync-link, `sync-link.xml` (draft; MAVLink 2, ADR-0031) | Sync MCU firmware, the ROS sync bridge |
| Car interface (ROS 2) | `arc_msgs` (ICD ros2-msgs) | Every ROS package and experiment |

Generated code is never edited by hand; CI checks it matches its source.

## Development environment (ADR-0029)

- **One dev container**, `.devcontainer/`, with every toolchain at pinned versions, on Linux, macOS and Windows. CI uses the same image.
- **Boards are flashed from the host** with `probe-rs` (ST-Link), and the motor controllers with VESC Tool over USB (SWD is the recovery path, RSK-20).
- **Every component is an Nx project**, so `npx nx affected -t lint test check build` covers firmware and ROS as well as the docs.

## Licences

The repo is MIT, except `firmware/vesc/`, which follows upstream's GPL-3.0 (ADR-0033).
