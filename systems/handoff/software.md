# Software handoff

The starting point for anyone writing the car's software. It collects, per component, what's decided, what exists, and what's still open. Read [`software/architecture.md`](../software/architecture.md) first if you're new: it explains what the software is and how the pieces talk. The work itself, in order, is in [`software/platform-plan.md`](../software/platform-plan.md) and the GitHub issues (#110).

## How this repo works

- `systems/` is the source of truth. Where code, a README or this page disagrees with a requirement, ADR or ICD, those win. Flag the conflict; don't silently pick one.
- Changes go through PRs (`.github/CONTRIBUTING.md`). A change to an ICD needs sign-off from the owners on both sides.
- **Safety functions** need their own tests and a second reviewer: the e-stop routine, the heartbeat watchdog, the safety envelope, the any-corner stop, the latch and re-arm, and the sync MCU's choice of who it takes commands from.
- **Generated code is never edited by hand.** Change the schema in `systems/icd/`, run `tools/gen-interfaces.sh`, commit both.
- **No hardware is in hand,** and it arrives one funded order at a time (ADR-0046). Everything is written and tested in simulation first, and the simulation runs the real code as far down as it can (ADR-0047; [`software/simulation.md`](../software/simulation.md)).

**Builds (ADR-0046):** v1.0 runs on the Orin Nano 8 GB developer kit with a UST-20LX LiDAR, the stereo pair and a downward frame camera. It can't run a learned policy beside the platform (LIM-09). v1.1 moves to the Orin NX 16 GB on the Antmicro carrier and adds the remaining cameras. The software is the same source on both; v1.0's differences are configuration.

## Read first

| What | Where |
| --- | --- |
| What the software is, how it flows | [`software/architecture.md`](../software/architecture.md) |
| The simulation and digital twin | [`software/simulation.md`](../software/simulation.md) |
| What needs to get done, in order | [`software/platform-plan.md`](../software/platform-plan.md) |
| How the repo was scaffolded | [`software/setup-plan.md`](../software/setup-plan.md) |
| System requirements | `requirements/system.md` |
| Decisions | `adr/README.md` |
| Interfaces | `icd/README.md`; for software: `sync-link.md` + `sync-link.xml`, `controller-telemetry.md`, `can-command.dbc` |
| Latency budget | `budgets/latency.csv` |
| Verification plan (stages S0–S5, rigs R1–R8) | `verification/plan.md` |
| Risks | `risks.md` |
| Setting up a laptop | The handbook's Dev setup page (`apps/docs/content/docs/handbook/dev-setup.mdx`) |

## Components

### 1. Sync MCU firmware (`firmware/sync-mcu/`)

- **Decided:**
  - STM32H723 on Zephyr v4.4.2 (ADR-0011, ADR-0017); three board targets: `native_sim`, `nucleo_h723zg`, `arc_sync` (ADR-0028).
  - Owns the car's clock: µs-critical sensors stamped at sampling; the Orin follows in software; PPS and camera triggers out (ADR-0021).
  - Forwards every sample to the Orin within 0.25 ms over UDP, no batching (ADR-0024); MAVLink 2 sync link (ADR-0031).
  - Heartbeat watchdog, 150 ms (TBC) (SYS-04); safety envelope, 3 m/s default (SYS-22); any-corner stop within 20 ms (SYS-25, ADR-0035); steering loss stops the car (ADR-0038); stops latch until an e-stop cycle and a re-arm (ADR-0039).
  - The heartbeat reaches it only through the Orin (ICD-sync-link, open issue 3).
  - Commands the controllers on classic CAN at 1 Mbit/s (ADR-0009); reads one 2 Mbit/s UART per controller (ADR-0034); steering on CAN-FD (ADR-0019).
- **What exists:** an application skeleton with one module per job. Only the sync link does anything: it exchanges `ARC_LINK_STATUS` with the Orin on `native_sim` and drops every other message. The safety module is a fail-safe stub (no motion allowed). Twister tests run on `native_sim`. The sync link isn't enabled on `nucleo_h723zg` (no Ethernet configuration).
- **Requirements:** SYS-04, -07, -19, -22, -24, -25, -29, -33.
- **Open:**
  - Everything in the application: the envelope and watchdog (#75), message handling, sensors (#77), CAN (#78), controller links (#82), time base and triggers (#76), the latch and re-arm, power-monitor input, fault reporting (SYS-19).
  - **Splitting the vehicle command into four wheel speeds** from the car's geometry (ADR-0048): wheelbase, track width, wheel radius and gearing, from the car configuration (ADR-0051).
  - **Keep the logic free of Zephyr calls** (time and I/O passed in), as `safety.c` already is. The same code then runs on the host, in the simulator and on the board.
  - **The re-arm message** is `ARC_REARM` in `sync-link.xml` (version 3); the sync MCU doesn't handle it yet, and the operator link doesn't carry it yet (#144).
  - **Who the sync MCU accepts commands from** (ICD-sync-link, open issue 6). A laptop on the service port could otherwise feed the watchdog. A safety function.
  - Gap counters and the version check (#119); the controllers' clock mapping (#74).
  - Board configuration for the NUCLEO: Ethernet, FD-CAN and SPI, so the board build is real before the boards arrive. `arc_sync`'s pins come from the EE (#98).
  - Drivers: Zephyr has one for the ISM330DHCX IMU; the AS5047P and the ToF (part TBC, ADR-0007) need our own.

### 2. Motor-controller firmware (`firmware/vesc/`)

- **Decided:**
  - Four off-the-shelf A50S V2.3c on the deck, running our build of the VESC firmware (ADR-0034), vendored at `release_7_00` (ADR-0033).
  - Our code lives in `firmware/vesc/arc/` and joins upstream through small hooks (ADR-0037).
  - The additions:
    - e-stop brake routine (ADR-0012)
    - a 150 ms (TBC) command timeout that brakes on the same ramp (ADR-0035)
    - UART telemetry, `ARC_MOTOR_STATUS` at 1 kHz with a `state` field (ADR-0024, ADR-0035)
    - an eRPM limit around 75k (TBC) (ADR-0034)
  - All of `firmware/vesc/` is GPL-3.0; never copy from it into the MIT parts of the repo. Don't touch its USB, CAN or firmware-upload code (RSK-20).
- **What exists:** fail-safe stubs for the e-stop, telemetry and speed limit, with a host test; the upstream firmware builds (`nx build vesc`).
- **Requirements:** SYS-04, -05, -24, -25; RSK-12, -18, -20.
- **Open:**
  - Every module (#80, #81, #83), and the command timeout (#120).
  - **Keep `arc/` free of ChibiOS calls,** with time and I/O passed in, so the same code runs in host tests, in the motor-controller model in the twin, and on the controller. Only the hooks into `bldc/` are then hardware-only.
  - Generate the MAVLink code into `firmware/vesc/` (#81).

### 3. Steering (moteus-c1)

- **Decided:** an off-the-shelf moteus-c1 driving a belt actuator on its own CAN-FD bus (ADR-0019, ADR-0036). Losing it stops the car, and the car won't drive without it (ADR-0038).
- **What exists:** nothing in the repo.
- **Requirements:** SYS-25; the actuator targets in ADR-0019.
- **Open:**
  - **The steering-bus ICD** (`icd/README.md`: steering-bus, TBD): the registers used, rates, the timeout mode.
  - The controller's configuration kept in the repo (an unchecked item in `setup-plan.md`).
  - The sync MCU's steering driver (#99), the steering-loss stop (#121), and a model of it for the twin.

### 4. Orin platform software (`ros/`)

- **Decided:**
  - ROS 2 Jazzy on JetPack 7.2.1 with PREEMPT_RT; platform software on reserved cores at real-time priority (ADR-0016).
  - Platform nodes talk only to the car interface (`arc_msgs`); each target supplies what's under it (ADR-0028; ADR-0047 moves that split down to the devices).
  - Launch with `ros2 launch arc_bringup platform.launch.py target:=… sim:=… sensors:=…`.
- **What exists:**
  - `arc_msgs` with the car interface's messages (ICD-ros2-msgs, rev A, draft); nothing publishes them yet.
  - `arc_bringup` (launch, a launch test per backend). Each test waits for the literal log line "not implemented yet", so it breaks as soon as its backend is implemented.
  - The car backend, which only does the link handshake.
  - Stub backends for Gazebo, Webots, the gym and replay.
- **Requirements:** SYS-06, -08, -09, -21, -22, -23, -24, -29, -30.
- **Open:**
  - **The car interface (ICD-ros2-msgs, #84):** drafted (rev A); its topic names, types and QoS await sign-off, and its open issues list what waits on the car configuration (#143) and the operator link (#144).
  - **The car backend (#85):** every sync-link message to and from topics, with time mapped onto the Orin's clock.
  - **Nodes not started:**

    | Node | Job |
    | --- | --- |
    | `arc_safety` | Operator-link receiver; forwards the heartbeat and re-arm; deadman |
    | `arc_control` | Chooses between the operator's teleop and the experiment's command, with a timeout on each (ADR-0048) |
    | `arc_logging` | The MCAP log of every run |
    | Clock sync | The Orin follows the sync MCU's clock (ADR-0021) |
    | Car state and health | The arming state and the SYS-19 faults the ground station shows |
    | Experiment manager | Starts experiments with their limits and envelope |
    | Shutdown handler | Low-battery clean shutdown (SYS-30, ADR-0022) |
    | `arc_drivers` | The Hokuyo driver, and camera nodes that pair each frame with its trigger |
    | Telemetry bridge | What the ground station reads |
    | Config loader | Loads the car configuration |

  - Launch parameters for the car backend's address and ports (today only reachable on one machine).
  - **Not platform work (ADR-0048):** state estimation, localization, mapping, planning and control (MPC included) belong to experiments.

### 5. Orin runtime: image and deploy (`platform/`)

- **Decided:** a reproducible image, with JetPack, kernel configuration and packages pinned (ADR-0016); experiments in containers with limits (SYS-21).
- **Decided:** one platform build for the Orin and laptops, on a Jetson base and an Ubuntu base, with the real-time settings in configuration; experiments run in Docker (ADR-0050).
- **What exists:** nothing but notes for the developer kit (`platform/dev-kit/`).
- **Requirements:** SYS-08, -09, -21, -30.
- **Open:**
  - Process supervision so the platform stays up (SYS-21); deploying a merged PR to the Orin and rolling back (#89); images for both laptop architectures and for Jetson, where experiments need GPU access.

### 6. Ground station

- **Decided:** the operator's laptop at the track; the network rules (ADR-0015): the operator link is a fixed-rate UDP stream with sequence numbers, marked top priority; the laptop is the car's internet gateway (Linux and macOS only); a button hotspot and a wired service port as fallbacks.
- **What exists:** a working ground station on branch `yufeng/ground-station`: a Python helper and a browser page with gamepad teleop, a 50 Hz heartbeat with a test cutoff, a status panel and a 2D track view. It starts the simulator itself and reads the simulator's own state, including ground truth the car can't know.
- **Requirements:** SYS-04, -11, -26, -27, -28.
- **Open:**
  - What the ground station is, decided and scaffolded as its own project (#71). The working version is the obvious candidate.
  - **The operator-link ICD** (`icd/README.md`: operator-link, TBD), from the branch's `protocol.md`.
  - **Talk only to the car's Orin,** never to the simulator: the operator link plus a telemetry bridge on the Orin. Simulator controls go through a separate sim-only interface ([`software/simulation.md`](../software/simulation.md)).
  - Arm and re-arm, the car's state and faults, envelope settings (#90); the dashboard (#91); log offload and viewing (#93); router configuration and the gateway script (#92); gamepads on every OS the team uses.

### 7. Experiments (`experiments/`)

- **Decided:** each experiment runs in its own container with resource limits, never commands the motors, and is logged by the platform (SYS-21, SYS-22, SYS-24).
- **Decided (ADR-0048):**
  - Experiments own estimation, localization, mapping, planning and control.
  - They command speed, an acceleration limit and a steering angle.
  - Any number may run. How they fit together (which one commands the car, how they share data) is for later, not platform work.
  - They may run threads at real-time priority on cores kept for experiments.
  - No per-wheel commands for now (set 2026-10-25): per-wheel speeds when an experiment needs them; per-wheel torque only with a speed limiter on the sync MCU.
  - They run in Docker (ADR-0050).
- **What exists:** a template that prints a line, and an open-items table in `experiments/README.md` (runtime, limits, interface: all TBD).
- **Requirements:** SYS-21, -22, -24.
- **Open:**
  - The runtime and limits (#88). Limits need measuring on the developer kit.
  - **Real-time scheduling for experiments:** which cores, which priorities, per build. How a container gets real-time scheduling on JetPack's PREEMPT_RT kernel: to check.
  - **The baseline estimator:** an example experiment, off the platform's critical path and the safety path. Once it exists, SYS-06's timing is checked with it.
  - The base image experiments build from: laptop and Jetson variants.
  - The first experiment: a gamepad teleop that drives through the experiment interface, exercising the path end to end.

### 8. Simulation, the digital twin (`ros/`, `firmware/`, new device models)

- **Decided:** every component runs in simulation (SYS-33, ADR-0028); Gazebo Harmonic, Webots and the F1TENTH gym as simulators, with one car description for all (ADR-0032).
- **Decided:** the twin runs the whole stack, firmware included, against models of the devices and the world (ADR-0047). Design, profiles, scenarios, fault injection and fidelity are in [`software/simulation.md`](../software/simulation.md).
- **Decided:** the twin runs in lockstep on the simulator's clock: the firmware, the device models and every ROS node follow it (ADR-0049).
- **What exists:** the firmware-in-the-loop test, and the working Gazebo sim on `yufeng/ground-station`.
- **Requirements:** SYS-33, and the evidence for every requirement a simulation can show.
- **Open:** see the open-questions table in [`software/simulation.md`](../software/simulation.md). The largest pieces of work:
  - the device models (sensors, motor controllers, steering, power, LiDAR, cameras)
  - the parameter generator from `systems/`
  - the scenario runner
  - a simulator image

### 9. Tools, CI and generated code (`tools/`, `.github/`)

- **Decided:** one dev container for every firmware and ROS build, also used by CI and tagged by the hash of its inputs (ADR-0029); every project is an Nx project; generated code is checked in CI.
- **What exists:** CI builds and tests the affected projects in the dev container; code generation for the sync link and command bus; the firmware-in-the-loop test.
- **Open:**
  - Running the twin's scenarios in CI.
  - Building every component for every target in CI.
  - A check that code above the split never names a target (ADR-0047).
  - Generating the twin's parameters and the car configuration from `systems/`.

## Cross-cutting open questions

| Question | Recommendation | Who decides | Blocks |
| --- | --- | --- | --- |
| **Who the sync MCU accepts commands from** | Deferred (2026-10-25) until before the car is assembled (ICD-sync-link, open issue 6): the Orin's address only, message signing, or a network the service port can't reach | Systems lead; a safety reviewer | The car's first drive |
| ICD owners for the interfaces still to be written (operator link, steering bus, sim link); the existing software ICDs are owned by Shrikar Vempati until reassigned | — | Team | Their sign-off |
