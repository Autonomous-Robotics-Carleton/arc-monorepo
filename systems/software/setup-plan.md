# Software setup plan (v1)

- **Status:** Draft (2026-10-07). Phase 0 done; phase 1 in progress.
- **Goal:** the repo ready for software work: every component scaffolded, building and tested in CI for its targets, before the hardware arrives. Writing the drivers, estimator and controllers comes after this plan.
- **Order:** firmware first (current focus), then interfaces, then platform software.
- **How it's built:** [`architecture.md`](architecture.md), ADR-0028, ADR-0029, ADR-0033.

Each phase ends with something that builds and passes CI. Tick items as they land.

## Phase 0: Decisions and docs

- [x] SYS-33: every component runs on simulation, development hardware and the car
- [x] ADR-0028 targets, ADR-0029 dev environment, ADR-0030 VESC firmware in this repo
- [x] [`architecture.md`](architecture.md) and this plan

## Phase 1: Dev environment

- [x] `.devcontainer/` image: Zephyr SDK 1.0.1 (Arm toolchain only), west, Zephyr v4.4.2 and its STM32/CMSIS modules baked in from `firmware/sync-mcu/west.yml`, Node 22 and pnpm. ROS 2 Jazzy comes in phase 5 and the VESC toolchain in phase 4, to keep the first image small
- [x] `smoke-test.sh`: builds and runs hello_world on `native_sim`, builds it for `nucleo_h723zg`
- [x] CI builds the image, runs the smoke test in it, and publishes it to GHCR. The image is tagged by the hash of its inputs, so a PR that changes it is tested in its own image; `main` also publishes it as `:latest`
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

- [x] ICD-sync-link: what the sync MCU sends the Orin and back, framing, versioning, timing (ADR-0024); message set drafted in `systems/icd/sync-link.xml`
- [x] Sync-link code generation (`tools/gen-sync-link.sh`, mavgen) into the sync MCU firmware and the ROS car backend, with a round-trip test on each side
- [x] Decision on the sync-link encoding: MAVLink 2 (ADR-0031)
- [ ] `can-command.dbc` skeleton, plus code generation (e.g. `cantools`) into the sync MCU and VESC firmware
- [x] ICD controller-telemetry: the UART link, drafted (reuses `ARC_MOTOR_STATUS` from the sync-link message set; clock alignment open)
- [x] CI fails if generated code doesn't match its source (`nx check sync-mcu`, in the dev-container job)

## Phase 4: VESC firmware

The motor controllers are off-the-shelf A50S boards running our build of the VESC firmware (ADR-0034), so this phase is firmware only: no hardware config of our own.

- [x] ADR-0033: vendored snapshot plus an update script, replacing ADR-0030's subtree (a subtree needs merge commits; this repo rebase-merges)
- [x] `tools/vesc-upstream.sh` (import and three-way update), tested in a scratch repo: 6.06 → 7.00 applies cleanly with a local change kept; a conflicting change is reported and marked. Skips lispBM's test reports, REPL, docs and examples (153 MB → 22 MB)
- [ ] Upstream `vedderb/bldc` imported into `firmware/vesc/bldc/` at `release_7_00` (SQ-3), recorded in `firmware/vesc/UPSTREAM`
- [ ] The VESC's Arm toolchain in the dev container: GCC 7 (2018-q2), which `release_7_00` builds with (SQ-3)
- [x] Licence boundary written into the root README and `LICENSE` (GPL-3.0 for `firmware/vesc/`)
- [x] ~~ARC hardware config skeleton~~ Not needed: the A50S's hardware config is upstream (`hwconf/teamtriforceuk/a50s_v23c/`, in `release_6_06` and `release_7_00`)
- [ ] Stubs for the e-stop brake routine, UART telemetry and the eRPM limit, with host unit tests
- [ ] Nx targets; CI builds the stock `a50s_v23c_12s` target and ours

## Phase 5: Platform software scaffold

- [x] ROS 2 Jazzy added to the dev container (ros-base, colcon, rosdep, MCAP storage, launch_testing)
- [x] Colcon workspace in `ros/`: `arc_msgs` (car interface, placeholder message), `arc_bringup` (launch with `target:=sim | replay | car`, a launch test per target), stub backends `arc_backend_sim`, `arc_backend_replay`, `arc_backend_car`
- [ ] `sim:=gazebo | webots | gym`, with one backend package each for Gazebo Harmonic, Webots and the F1TENTH gym (ADR-0032)
- [ ] `arc_description`: one URDF of the car, the source for every simulator's model
- [ ] Sensor profiles per launch (`sensors:=lidar | stereo | all`)
- [ ] Benchmark one scene (LiDAR + stereo) per backend, per OS, once they run (ADR-0032)
- [x] `colcon build` and `colcon test` as Nx targets (`nx build ros`, `nx test ros`), run by CI's dev-container job (tag `env:dev-container`)

## Phase 6: Firmware in the loop

- [ ] The sync MCU's `native_sim` build and the ROS sync bridge talk over UDP on one laptop
- [ ] A CI smoke test: firmware in simulation → bridge → a topic with timestamped samples

## Phase 7: Experiments scaffold

- [x] `experiments/` with a template experiment and its container definition
- [ ] Resource limits (SYS-21), container runtime and the classical-layer interface (SYS-22): TBD, listed in `experiments/README.md`

## Later (not in this plan)

- The platform image and the path from a merged PR to the Orin (ADR-0016)
- The real drivers, estimator, controllers and logging

## Open questions

| # | Question | Blocks |
| --- | --- | --- |
| SQ-1 | ~~Zephyr version to pin~~ **Answered:** v4.4.2 with SDK 1.0.1 (newest stable; supports the NUCLEO-H723ZG). The 3.7 LTS is two years older and its 4.4 release notes list fixes not backported to it. Revisit when a 4.x LTS appears | Phase 1 |
| SQ-2 | ~~Sync-link encoding~~ **Answered:** MAVLink 2 with our own message set (ADR-0031) | Phase 3 |
| SQ-3 | ~~VESC firmware release branch~~ **Answered (2026-10-07):** `release_7_00` (firmware 7.00, the current stable release; includes the A50S v2.3c hardware config). Builds with GCC 7 (2018-q2) | Phase 4 |
| SQ-4 | ~~Simulators~~ **Answered (ADR-0032):** pluggable backends: Gazebo Harmonic (full sensors, event cameras), Webots (native GPU on macOS and Windows), F1TENTH gym (fast, CI) | Phase 5 |
| SQ-5 | Development boards: put a NUCLEO-H723ZG (ST's board with the sync MCU's chip, Ethernet and CAN-FD) in the single order? It runs the sync MCU firmware on real hardware while the custom sync board is still being made. **Recommendation:** yes, it's inexpensive. A spare A50S from the order serves as the VESC development hardware (ADR-0034) | Testing the development-hardware targets |
| SQ-6 | ~~CAN `.dbc` file split~~ **Answered by ADR-0034:** telemetry left CAN for a UART per controller, so only `can-command.dbc` remains | — |
