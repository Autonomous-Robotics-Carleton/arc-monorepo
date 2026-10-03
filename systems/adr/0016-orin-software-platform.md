# ADR-0016: Orin software platform: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-08, SYS-21, ADR-0010, ADR-0015, RSK-01, RSK-14

## Context

The Orin NX needs a JetPack release. JetPack 7.2 (Jetson Linux 39.2) is the first JetPack 7 release with Orin support; JetPack 7.2.1 shipped on 2026-08-11. It moves Orin from Ubuntu 22.04 / kernel 5.15 (JetPack 6.2.x) to Ubuntu 24.04 / kernel 6.8.

## Options

| Option | OS / kernel | ROS 2 | Notes |
| --- | --- | --- | --- |
| JetPack 6.2.x | Ubuntu 22.04 / 5.15 | Humble (end of life May 2027) | Most third-party drivers today; MT7922 needs a backported driver (RSK-14) |
| **JetPack 7.2.1** | **Ubuntu 24.04 / 6.8** | **Jazzy (supported to 2029)** | MT7922 in mainline; PREEMPT_RT packaged for Orin; carrier and camera drivers must be on 7.2 |
| Yocto (meta-tegra) or NixOS (jetpack-nixos) | Custom | Build yourself | Reproducible, locked-down images; much more build engineering |

## Decision

- **JetPack 7.2.1** (Jetson Linux 39.2.1) on Ubuntu 24.04, kernel 6.8.
- **ROS 2 Jazzy.**
- **PREEMPT_RT kernel**, with platform software (drivers, logging, state estimation, MPC) on reserved CPU cores at real-time priority (SYS-21). The Orin is soft real-time only; hard real-time stays on the sync MCU and the VESCs.
- **Experiments run in containers** with CPU, memory, GPU and disk limits (SYS-21).
- **Reproducible image:** the JetPack version, kernel configuration and package set are pinned in version control.
- The Orin's onboard real-time core (SPE, FreeRTOS) is not used; the sync MCU covers that role.

## Consequences

- RSK-14 closes: the MT7922 driver is in mainline kernel 6.8.
- RSK-01 gains a check: Arducam (AR0234 stereo, quad OV9281) and Prophesee (GenX320) drivers must exist for JetPack 7.2. If they don't, the fallback is JetPack 6.2.x, which brings back RSK-14 and ROS 2 Humble.
- The Antmicro carrier fork's device tree and the Orin Nano dev kit bench reference both target JetPack 7.2.
- Yocto is the path for a locked-down image later (v2), if needed.
