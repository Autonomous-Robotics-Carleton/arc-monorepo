# Platform plan (v1)

- **Status:** Draft (2026-10-25). Follows [`setup-plan.md`](setup-plan.md), which scaffolded every component.
- **Goal:** one platform for every target (SYS-33). The car's software, firmware included, is complete and tested against a digital twin of the car. When hardware arrives, moving to it means writing board files, measuring the car's configuration and calibrating models, not changing code. After this plan, what's left is experiments.
- **Builds on:** ADR-0016, ADR-0021, ADR-0024, ADR-0028, ADR-0031, ADR-0032, ADR-0034, ADR-0035, ADR-0038, ADR-0039; ADR-0047 for where the targets split; ADR-0049 for the twin's clock; ADR-0050 for the images; ADR-0051 for the car configuration; ADR-0048 for what experiments own.
- **Read with:** [`architecture.md`](architecture.md) (what the software is), [`simulation.md`](simulation.md) (the twin), [`handoff/software.md`](../handoff/software.md) (what's decided and open per component).

The work is in four phases, in order. Each phase ends with something that runs. There are no dates: a phase is done when its exit check passes in CI. Issue numbers are given where an issue exists; "new" marks work that still needs one.

## Phase A: decide

Most of the work below depends on these. Each ends as an ADR or ICD change, written as Proposed first ([`handoff/software.md`](../handoff/software.md), cross-cutting questions).

| Decision | Issue |
| --- | --- |
| Where the targets split (ADR-0047) | Decided: at the devices |
| Who keeps time in the twin (ADR-0049) | Decided: lockstep |
| One platform image on the Orin and on laptops (ADR-0050) | Decided: one build, two bases |
| Experiments own estimation and control, command speed and steering, may run in real time (ADR-0048) | Decided |
| Real-time cores and priorities for experiments, per build | new, with #88 |
| The re-arm message (ADR-0039) | Decided: the operator re-arms; message to add |
| Who the sync MCU accepts commands from (ICD-sync-link, open issue 6) | Deferred until before the car is assembled |
| The car configuration (ADR-0051) | Decided: a file per car, compiled into the sync MCU |
| A simulator image beside the dev container (ADR-0050) | Decided: amd64 and arm64 |
| The ground station: what it is and where it lives | #71, later |
| Sync-link addresses and rates (ICD-network) | Decided: ports and rates; addresses TBC |
| Review of the message sets | #73 |
| The controllers' clock mapping | #74 |
| Experiment runtime and limits | #88: Docker (ADR-0050); limits measured on the dev kit |

**Exit:** each decision recorded in `systems/`.

## Phase B: the spine

The thinnest version of the car's whole code path, running in the twin end to end. From here on, everything runs through the real path, so every later piece lands on something that already works.

| Work | Issue |
| --- | --- |
| **Contracts:** the car interface (ICD ros2-msgs, `arc_msgs`) | #84 |
| **Contracts:** the operator-link ICD, from `ground-station/protocol.md` on `yufeng/ground-station` | new |
| **Contracts:** the sim-link ICD (simulator ↔ `native_sim`) | new |
| **Contracts:** the re-arm message on the sync link | new |
| **Sync MCU:** the envelope, watchdog and brake ramp in `safety.c`, free of Zephyr calls | #75 |
| **Sync MCU:** handling every sync-link message (drive command through the clamp, heartbeat to the watchdog, envelope set, safety state out) | new |
| **Sync MCU:** splitting the vehicle command into four wheel speeds (ADR-0048) | new |
| **Sync MCU:** the latch and re-arm (ADR-0039) | new |
| **Sync MCU:** its end of the sim link | new |
| **Orin:** the car backend, part 1 (commands, heartbeat, envelope, safety state) | #85 |
| **Orin:** `arc_safety`, the operator-link receiver | new |
| **Orin:** `arc_control`, the minimal command switch | new |
| **Orin:** `arc_logging` | new |
| **Twin:** the Gazebo world, from the sim on `yufeng/ground-station`, with a simple motor-controller model that answers commands and reports status | #106 |
| **Twin:** a spike: can `native_sim` follow an outside clock (ADR-0049) | new |
| **Twin:** lockstep: the `native_sim` clock component, time grants on the sim link, the models stepping | new |
| **Twin:** the simulator image | new |
| **Twin:** the sim-only control interface | new |
| **Twin:** profiles and the scenario runner | new |
| **Ground station:** moved into the repo; talks only to the Orin; drives and arms the twin | #71, #90 |
| **CI:** runs the scenarios | new |
| **CI:** platform code reads time only through the ROS clock (ADR-0049) | new |
| **CI:** formatters and linters for C, C++ and Python (clang-format, clang-tidy, ruff, `ament_lint`); today only the websites are linted | new |
| **CI:** the `native_sim` tests built with the address and undefined-behaviour sanitizers | new |
| **CI:** coverage on the safety functions, with a minimum the build enforces | new |
| **CI:** `main` protected: CI and a code owner's approval before merging (check the GitHub settings) | new |
| **Versions:** every binary carries its git hash and whether the tree was clean, reports it at startup, and the sync MCU sends it over the sync link | new |
| **Twin:** measure one `twin` run on a typical team laptop (CPU, memory, real-time factor); that sets the laptop spec in the handbook | new |

**Exit:** the ground station arms and drives the twin through the whole stack (operator link → `arc_safety` → `arc_control` → car backend → sync link → `native_sim` firmware → model → Gazebo), and the SYS-04 heartbeat-cut and SYS-22 envelope scenarios pass in CI.

## Phase C: the whole car in the twin

Every device modelled, every driver real, every safety function tested.

| Area | Work | Issue |
| --- | --- | --- |
| Sensors | Emulators for the ISM330DHCX, AS5047P and ToF; the drivers and sensor layer | #77 |
| Command bus | The CAN driver on Linux virtual CAN | #78 |
| Motor controllers | The full model, running our `arc/` modules | new |
| Motor controllers | The e-stop routine, telemetry, speed limit | #80, #81, #83 |
| Motor controllers | The command timeout and the any-corner stop | #120 |
| Motor controllers | The telemetry links on virtual serial ports | #82 |
| Motor controllers | Gap counters and the version check | #119 |
| Steering | The steering-bus ICD; the moteus-c1 model; the driver's sim side; the steering-loss stop | new, #99, #121 |
| Power | The power model; the battery and rail monitors; the low-battery shutdown, end to end | new (with ADR-0022) |
| Time | The Orin following the sync MCU's clock; trigger and PPS scheduling | new, #76 (sim side) |
| LiDAR, cameras | Models; `arc_drivers`; pairing frames with triggers (ADR-0021) | new |
| Orin | The car backend, part 2: every sample to a topic | #85 |
| Orin | Car state and health (SYS-19) | new |
| Orin | The experiment manager | new |
| Experiments | The runtime, limits and real-time scheduling; the template drives the twin | #88 |
| Experiments | The first experiment: a gamepad teleop through the experiment interface | new |
| Ground station | The dashboard; log offload and viewing | #91, #93 |
| Twin | Per-wheel control in Gazebo | new |
| Twin | The car's networks in Docker (ICD-network), with the operator-link impairment proxy (ADR-0049) | new |
| Twin | The fault catalogue as scenarios | new |
| Twin | Replay | #86 |
| Twin | Which sensors are rendered, per profile and scenario (`sensors:=`); `twin` renders no cameras unless asked | new |
| Testing | Replay regression: recorded runs become tests that new code must handle | new |
| Testing | Fuzzing the parsers that read bytes from a link (sync link, controller telemetry) on the host | new |
| Logging | Run records: every log names the versions, car configuration, envelope, profile and driver | new |
| CI | Automatic dependency updates (Renovate or Dependabot): Zephyr, ROS packages, actions, npm | new |
| Configuration | The parameter generator from `systems/` | new |
| Configuration | The car configuration | new |

**Exit:** the scenarios for SYS-04, SYS-05, SYS-19, SYS-21, SYS-22, SYS-25 and SYS-30 pass in the twin, and the first experiment runs.

## Phase D: ready for hardware

| Work | Issue |
| --- | --- |
| The platform image for Jetson; deploy and roll back | #89 |
| The NUCLEO board configuration (Ethernet, FD-CAN, SPI) and the `hil` profile | new |
| Every component built for every target in CI; the check that code above the split never names a target | new |
| Sync MCU sender filtering: decide (deferred from Phase A) and implement | new |
| The fidelity list: each model's calibration test, written in `systems/tests/` | new |
| Running the real ARM binaries in an emulator such as Renode: investigate | new |
| The handbook page "Run the platform", followed and timed on Linux, macOS and Windows | new |
| Releases: one tag builds every artifact (platform image, sync MCU and VESC firmware, ground station) together, with release notes | new |
| One deploy command for a release, checked on the car afterwards (versions and protocol versions match, power-on checks pass) | #89 |
| A procedure for flashing the motor controllers safely; a failed flash is recoverable only over SWD (RSK-20) | new |
| Timing budgets as tests on the dev kit: latency and CPU against `budgets/latency.csv`, failing on regressions (SYS-06, SYS-29) | new |
| CI runners on hardware: the dev kit, and a bench rig (NUCLEO, stock A50S with an unloaded, clamped motor, breakout sensors, its own e-stop) for nightly safety scenarios. They run only `main` or PRs a member has approved, never a fork's code automatically. The car itself is never a CI runner | new |
| Operations: a pre-drive checklist in the ground station; a health view (CPU, temperatures, link quality, dropped messages); where logs go after offload and how long they're kept; who holds the car's keys and passwords | new |
| Every incident (failure-report template) ends in a scenario or replay test | new |

**Exit:** a new member runs the twin from a fresh clone on any OS; the car-specific work left is listed and short (the `arc_sync` board file, the measured car configuration, model calibration).

## Later, or waiting on hardware

- The baseline estimator, an example experiment (ADR-0048). The platform doesn't wait on it.
- Webots (#107), benchmarks (#108); the gym (#87) when faster-than-real-time training needs it.
- `arc_description` from the CAD layout (#103, #105). Until then the twin's car model comes from `systems/`.
- Hardware bring-up: Ethernet on the board (#79), PPS and triggers on the board (#76), the real router (#92), the real steering bus (#99).

## Where to start

Set up with the handbook's **Dev setup** page. Then pick by what you like working on:

| If you like | Start with | Then |
| --- | --- | --- |
| C and embedded (Zephyr) | #75: the envelope and watchdog, tested on `native_sim`, no hardware needed | The sync MCU's message handling, then the latch and re-arm |
| C, motor control | The motor-controller model | #80, #120 |
| ROS 2 (C++ or Python) | #84: the messages everything uses | `arc_control`, `arc_logging`, `arc_safety` |
| Simulation, Gazebo | #106: the working sim, moved into the repo | Per-wheel control, the device models |
| Web, the operator's laptop | #71 and #90: the ground station | #91 |
| Docker, CI | The simulator image | The scenarios in CI |
| Writing, teaching | The handbook page, tested on a fresh laptop | — |

Issues labelled `safety` need their own tests and a second reviewer; `needs-decision` issues end in an ADR or ICD change written as Proposed first.
