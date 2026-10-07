# Software setup plan (v1)

- **Status:** Draft (2026-10-07). Phase 0 in progress.
- **Goal:** the repo ready for software work: every component scaffolded, building and tested in CI for its targets, before the hardware arrives. Writing the drivers, estimator and controllers comes after this plan.
- **Order:** firmware first (current focus), then interfaces, then platform software.
- **How it's built:** [`architecture.md`](architecture.md), ADR-0028, ADR-0029, ADR-0030.

Each phase ends with something that builds and passes CI. Tick items as they land.

## Phase 0: Decisions and docs

- [ ] SYS-33: every component runs on simulation, development hardware and the car
- [ ] ADR-0028 targets, ADR-0029 dev environment, ADR-0030 VESC firmware in this repo
- [ ] [`architecture.md`](architecture.md) and this plan

## Phase 1: Dev environment

- [ ] `.devcontainer/` image with pinned Zephyr SDK and west, Arm GNU toolchain, Node and pnpm (ROS 2 Jazzy added in phase 5, to keep the first image small)
- [ ] CI builds the image, publishes it to GHCR, and runs firmware jobs in it
- [ ] Verify on Linux, macOS (Intel and Apple Silicon) and Windows (WSL 2): open the repo in the container and build
- [ ] Handbook: a firmware section in Dev setup (Docker, opening the container, `probe-rs` on the host)

## Phase 2: Sync MCU firmware scaffold

- [ ] West workspace: a manifest pinning the Zephyr version; Zephyr pulled by `west update`, not committed
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
- [ ] Licence boundary written into the root README and `LICENSE` (GPL-3.0 for `firmware/vesc/`)
- [ ] ARC hardware config skeleton (pins TBD; VESC 6 MK5 vs 6.4 base is open in the electrical handoff)
- [ ] Stubs for the e-stop brake routine and FD telemetry, with host unit tests
- [ ] Nx targets; CI builds the stock VESC 6 config and ours

## Phase 5: Platform software scaffold

- [ ] ROS 2 Jazzy added to the dev container
- [ ] Colcon workspace in `ros/`: `arc_msgs` (car interface), `arc_bringup` (launch with `target:=sim | replay | car`), backend skeletons:
  - simulator: stub until a simulator is chosen
  - replay: MCAP
  - car: sync MCU bridge
- [ ] Simulator choice, written as a recommendation ADR first
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
| SQ-1 | Zephyr version to pin | Phase 2 |
| SQ-2 | Sync-link encoding (fixed-layout binary generated from a schema, CBOR, protobuf, …) | Phase 3 |
| SQ-3 | VESC firmware release branch to start from | Phase 4 |
| SQ-4 | Simulator for the platform backend | Phase 5 |
| SQ-5 | Development boards: buy a NUCLEO-H723ZG (and a stock VESC 6) before the order, or wait for it? | Testing the development-hardware targets |
