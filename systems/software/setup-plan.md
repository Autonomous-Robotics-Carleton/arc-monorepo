# Software setup plan (v1)

- **Status:** Draft (2026-10-07). Phase 0 done; phase 1 in progress.
- **Goal:** the repo ready for software work: every component scaffolded, building and tested in CI for its targets, before the hardware arrives. Writing the drivers, estimator and controllers comes after this plan.
- **Order:** firmware first (current focus), then interfaces, then platform software.
- **How it's built:** [`architecture.md`](architecture.md), ADR-0028, ADR-0029, ADR-0030.

Each phase ends with something that builds and passes CI. Tick items as they land.

## Phase 0: Decisions and docs

- [x] SYS-33: every component runs on simulation, development hardware and the car
- [x] ADR-0028 targets, ADR-0029 dev environment, ADR-0030 VESC firmware in this repo
- [x] [`architecture.md`](architecture.md) and this plan

## Phase 1: Dev environment

- [x] `.devcontainer/` image: Zephyr SDK 1.0.1 (Arm toolchain only), west, Zephyr v4.4.2 and its STM32/CMSIS modules baked in from `firmware/sync-mcu/west.yml`, Node 22 and pnpm. ROS 2 Jazzy comes in phase 5 and the VESC toolchain in phase 4, to keep the first image small
- [x] `smoke-test.sh`: builds and runs hello_world on `native_sim`, builds it for `nucleo_h723zg`
- [ ] CI builds the image, runs the smoke test in it, and publishes it to GHCR from main
- [ ] Make the GHCR package public, so teammates pull its layers without logging in
- [ ] Verify on Linux, macOS (Intel and Apple Silicon) and Windows (WSL 2): open the repo in the container and build
- [x] Handbook: a firmware section in Dev setup (Docker, opening the container, `probe-rs` on the host)

## Phase 2: Sync MCU firmware scaffold

- [x] West manifest pinning Zephyr and its modules (`firmware/sync-mcu/west.yml`); the dev container bakes the workspace in at `/opt/zephyr-ws`, so nothing is committed or fetched by hand
- [ ] Application skeleton, one module per job, as stubs:
  - time base
  - sensor sampling
  - CAN buses (command, telemetry, steering)
  - watchdog and safety envelope
  - sync link to the Orin
- [ ] Three targets building:
  - `native_sim`
  - `nucleo_h723zg`
  - `arc_sync`: a board definition skeleton in this repo, pins TBD from the EE
- [ ] Tests on `native_sim` with Zephyr's test runner (twister), using loopback CAN
- [ ] Nx targets: build per board, test; CI runs them on every affected PR

## Phase 3: Interfaces

- [ ] ICD-sync-link: what the sync MCU sends the Orin and back, framing, versioning, timing (ADR-0024)
- [ ] Decision on the sync-link encoding, written as a recommendation ADR first
- [ ] `can-command.dbc` and `can-telemetry.dbc` skeletons, plus code generation (e.g. `cantools`) into the sync MCU and VESC firmware
- [ ] CI fails if generated code doesn't match its source

## Phase 4: VESC firmware fork

- [ ] `vedderb/bldc` as a squashed subtree at `firmware/vesc/bldc/`, on a chosen release branch
- [ ] The VESC's Arm toolchain in the dev container: GCC 7 (2018-q2) for the upstream release branches, Arm GNU 14.3 for upstream master; follows SQ-3
- [ ] Licence boundary written into the root README and `LICENSE` (GPL-3.0 for `firmware/vesc/`)
- [ ] ARC hardware config skeleton (pins TBD; VESC 6 MK5 vs 6.4 base is open in the electrical handoff)
- [ ] Stubs for the e-stop brake routine and FD telemetry, with host unit tests
- [ ] Nx targets; CI builds the stock VESC 6 config and ours

## Phase 5: Platform software scaffold

- [ ] ROS 2 Jazzy added to the dev container
- [ ] Colcon workspace in `ros/`: `arc_msgs` (car interface), `arc_bringup` (launch with `target:=sim | replay | car` and `sim:=gazebo | webots | gym`), backend skeletons:
  - simulators: one package each for Gazebo Harmonic, Webots and the F1TENTH gym (ADR-0032, Proposed)
  - replay: MCAP
  - car: sync MCU bridge
- [ ] `arc_description`: one URDF of the car, the source for every simulator's model
- [ ] Sensor profiles per launch (`sensors:=lidar | stereo | all`)
- [ ] Benchmark one scene (LiDAR + stereo) per backend, per OS, before accepting ADR-0032
- [ ] `colcon build` and `colcon test` as Nx targets in CI

## Phase 6: Firmware in the loop

- [ ] The sync MCU's `native_sim` build and the ROS sync bridge talk over UDP on one laptop
- [ ] A CI smoke test: firmware in simulation → bridge → a topic with timestamped samples

## Phase 7: Experiments scaffold

- [ ] `experiments/` with a template experiment, its container definition and resource limits (SYS-21), commanding only the classical layer (SYS-22)

## Later (not in this plan)

- The platform image and the path from a merged PR to the Orin (ADR-0016)
- The real drivers, estimator, controllers and logging

## Open questions

| # | Question | Blocks |
| --- | --- | --- |
| SQ-1 | ~~Zephyr version to pin~~ **Answered:** v4.4.2 with SDK 1.0.1 (newest stable; supports the NUCLEO-H723ZG). The 3.7 LTS is two years older and its 4.4 release notes list fixes not backported to it. Revisit when a 4.x LTS appears | Phase 1 |
| SQ-2 | Sync-link encoding. **Recommendation in ADR-0031 (Proposed):** MAVLink 2 with our own message set | Phase 3 |
| SQ-3 | VESC firmware release branch to start from. **Recommendation:** `release_7_00` (firmware 7.00, the current stable release, maintained; `master` is 7.01 test builds). It builds with GCC 7 (2018-q2) | Phase 4 |
| SQ-4 | Simulators. **Recommendation in ADR-0032 (Proposed):** pluggable backends: Gazebo Harmonic (full sensors, event cameras), Webots (native GPU on macOS and Windows), F1TENTH gym (fast, CI) | Phase 5 |
| SQ-5 | Development boards: buy a NUCLEO-H723ZG (and a stock VESC 6) before the order, or wait for it? | Testing the development-hardware targets |
