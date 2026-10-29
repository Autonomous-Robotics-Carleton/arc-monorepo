# Simulation: the car as a digital twin

- **Status:** Draft (2026-10-25). Design decided in ADR-0047; nothing on this page is built yet except what "What exists today" lists.
- **Requirements:** SYS-33 (every component runs in simulation), and every requirement verified by test that a simulation can give evidence for (see Fidelity).
- **Read first:** [`architecture.md`](architecture.md) (what the software is), ADR-0028 and ADR-0047 (targets).

## Why simulate this much

None of the car's hardware is in hand, and it arrives one funded order at a time (ADR-0046). The goal is that when it arrives, the software already works. Moving to the car should mean writing the car's board file, measuring its configuration and calibrating the models, not rewriting or debugging code that has never run.

That only works if the simulation runs **the code that will run on the car**, as far down as it goes. So the simulator here isn't a stand-in for the car's software. It's a stand-in for the car's **devices and world**, underneath the real software. That combination is the **digital twin**.

## What's real and what's modelled

| Layer | Real code that runs in the twin | Modelled | Model parameters from |
| --- | --- | --- | --- |
| Operator | The ground station | The Wi-Fi link: latency, loss, dropouts | ADR-0015, SYS-26 to SYS-28 |
| Orin | Every platform node, the car backend, the drivers that can run without hardware, experiments: the same image as the car | The Orin's limited CPU and memory (container limits) | ADR-0016, LIM-09 |
| Sync MCU | The whole application on Zephyr's `native_sim` board, including its sensor, CAN and UART drivers | The devices behind those drivers (below) | `bom/electrical.md` (E-20 to E-26) |
| Motor controllers | Our modules from `firmware/vesc/arc/` (e-stop ramp, command timeout, speed limit, telemetry) | The rest of each controller: current limits, the motor, temperatures, faults | ADR-0034, ADR-0035 |
| Steering | — | The moteus-c1, belt and motor | E-31, ADR-0019 |
| Drivetrain and chassis | — | Motor torque and speed, gearbox, tyres on tile, mass and centre of gravity, suspension | `budgets/mass.csv`, `architecture.md` (gearing), ADR-0027, ADR-0040 |
| Power | — | Pack and cell voltages, rails, monitor data, low-battery shutdown, the e-stop circuit and motor-bus switch timing | ICD-power-rails, ADR-0022, `budgets/power.csv` |
| Sensors on the Orin | The LiDAR driver, if it can talk to an emulated LiDAR; the frame-to-trigger pairing for cameras | LiDAR scans; camera frames, numbered, taken at the firmware's trigger times | E-10, E-11, E-25, ADR-0021 |
| World | — | The school floor, track walls, the floor texture the ground-speed camera sees | SYS-01 |

## How each model attaches

Every model attaches where the real device would, so the driver above it is the real driver.

| Device (count) | On the car | In the twin | Real code exercised | Not exercised |
| --- | --- | --- | --- | --- |
| IMU, ISM330DHCX (2) | SPI + data-ready line to the sync MCU | An emulator on the emulated SPI bus, fed with acceleration and rotation from the physics, plus noise and bias | Zephyr's ISM330DHCX driver and our sensor layer | The STM32 SPI peripheral and DMA |
| Angle sensor, AS5047P (10) | SPI | Emulators fed with wheel, suspension and knuckle angles | Our AS5047P driver (Zephyr has none) and the sensor layer | The same |
| Ride-height ToF (5) | I2C through a mux | Emulators fed with ride height | Our ToF driver (part TBC, ADR-0007) | The same |
| Battery and rail monitors | I2C plus fault interrupts (ADR-0022, Proposed) | Emulators fed by the power model | The monitor drivers, the low-battery logic | The same |
| E-stop status input | GPIO | Zephyr's emulated GPIO, driven by the scenario or the sim controls | The latch and reset logic (ADR-0039) | — |
| Motor controllers (4) | Command CAN + one UART each | A motor-controller model on Linux virtual CAN and virtual serial ports, linking our `arc/` modules | Our CAN driver, UART links, any-corner stop (SYS-25) | The FD-CAN and UART peripherals, the VESC's own control loop |
| Steering, moteus-c1 | CAN-FD | A model on virtual CAN-FD | Our steering driver, the steering-loss stop (ADR-0038) | The FD-CAN peripheral |
| Camera triggers, PPS | Timer outputs | Trigger times reported to the simulator over the sim link | The trigger scheduling | The timer peripheral |
| Sync link | Ethernet, Zephyr's IP stack | The laptop's own network stack (Zephyr's offloaded sockets) | Every message on the link | Zephyr's IP stack and the STM32 Ethernet driver |
| LiDAR (Hokuyo UST-20LX on v1.0) | Ethernet, Hokuyo's SCIP protocol | An emulator that speaks SCIP, fed with scans from the world (TBC) | The real ROS driver | — |
| Cameras (stereo, ground-speed) | CSI, V4L2 | A node that publishes numbered raw frames from the world | Pairing frames with triggers (ADR-0021), everything after | The camera drivers |
| Operator Wi-Fi | Router, 6 GHz | A Docker network, with delay and loss from a proxy in simulated time (ADR-0049) | The operator link, the heartbeat path | The radio |

The left-hand side of this table is the target-specific code ADR-0047 allows: board files and device models. Everything in the "real code" column is shared with the car.

## Choosing what to run

Three choices, set at launch (ADR-0047):

| Choice | Values | Means |
| --- | --- | --- |
| `target` | `sim`, `devhw`, `car` | Where the devices are |
| `world` | `gazebo`, `gym` | Which simulator supplies physics and sensor data (`sim` only) |
| `mode` | `full` (default), `interface`, `replay` | `full` runs the whole stack. The two shortcuts skip everything below the car interface: `interface` lets the world drive it directly, with the firmware's `safety.c` compiled in; `replay` plays a log back |

Named presets for everyday work:

| Profile | Choices | Use it for | Runs |
| --- | --- | --- | --- |
| `twin` (default) | `sim`, `gazebo`, `full` | Anything that will run on the car: platform work, the ground station, safety, experiments | Lockstep, up to real time |
| `fast` | `sim`, `gym` or `gazebo` without rendering, `interface` | Control and learning work that needs many runs | Faster than real time |
| `perception` | `sim`, `gazebo`, `full` with a scripted drive; or `replay` | Perception against rendered or recorded sensors | Lockstep, up to real time; faster on replay |
| `hil` | `devhw` | Bench tests with the NUCLEO and real boards, rig R1 | Real time |
| `car` | `car` | The car | Real time |

Shortcuts are labelled in every log and result. A requirement is never verified on a shortcut alone.

## Scenarios

A **scenario** is one file that describes a run: the profile, the world, who drives, the events to inject and the pass criteria. Limits come from `systems/`, never from the test. The same file runs on a laptop, in CI, and later on the rig or the car, so a scenario is both a test and a rehearsal of a verification procedure.

```yaml
verifies: SYS-04
profile: twin
world: {track: oval}
envelope: {max_speed: 3.0}
driver: {scripted: {speed: 3.0}}
events:
  - {at: 5.0, cut: heartbeat}
pass:
  - watchdog reaches NO_HEARTBEAT within CONFIG_ARC_HEARTBEAT_TIMEOUT_MS
  - rolled before braking <= 2 m
```

The format is a proposal; see [`platform-plan.md`](platform-plan.md).

## Faults the twin can inject

Fault injection copies the verification plan's rig procedures, so each becomes a scenario before the rig exists.

| Fault | From | Injected at | Checks |
| --- | --- | --- | --- |
| Heartbeat lost; Wi-Fi drops or degrades | R7, R6 | The network model | SYS-04, SYS-26 to SYS-28 |
| E-stop pressed, at speed | R2, R7 | The emulated e-stop input and the motor-controller model | SYS-05 |
| Command beyond the envelope | R1 | The scenario's driver | SYS-22 |
| Command bus dead, a controller's UART dead, a controller fault | R1, R2 | The motor-controller model | SYS-25, ADR-0035 |
| Steering silent, faulted or bus-off; knuckle encoder lost | R2 | The steering model and an angle-sensor emulator | SYS-25, ADR-0038 |
| Sensor dropout, over-temperature, rail fault, brownout | R1, R4 | Sensor emulators, the power model | SYS-19 |
| Low battery | R4 | The power model | SYS-30 |
| Sync link packets dropped or reordered; version mismatch | R1 | The network model, the firmware build | ICD-sync-link, #119 |
| Experiment crashes, hangs, hogs CPU or memory | R8 | The experiment container | SYS-21 |

## Time

On the car the sync MCU is the clock, and the Orin follows it (ADR-0021). In the twin there would be a second clock, the simulator's, which slows down whenever physics or rendering can't keep up. If the firmware ran on the wall clock meanwhile, it would see a world in slow motion, and its watchdog, ramps and telemetry rates would stop meaning anything.

So the twin runs in **lockstep** (ADR-0049):

- **The simulator is the only clock.** Each step (1 ms, TBC) it advances the world, grants the same time to the firmware and the device models, and waits for them before the next step.
- **The firmware follows it.** `native_sim`'s clock advances only when the simulator grants time. That's a component of the `native_sim` board; the application doesn't change. A spike checks this first.
- **ROS nodes run on the simulator's clock** (`use_sim_time`). Platform code reads time only through the ROS clock, never the system clock, so the same code runs on the car, where ROS time follows the sync MCU. CI checks it.
- **Runs are paced to at most real time.** A slow machine slows the run down instead of breaking it. Every run records its real-time factor and the age of sync-link messages in simulated time, and is flagged if the ROS side fell behind.
- **Faster than real time** with the Orin in the loop would need the car backend to join the lockstep: later, if wanted. The `fast` profile covers fast runs until then.

## The twin's network

The twin builds the car's networks in Docker, at the car's addresses ([ICD-network](../icd/network.md)): the laptop, the Orin and the sync MCU are separate machines, as they are on the car. That way address configuration and the sync MCU's choice of who it accepts commands from (ICD-sync-link, open issue 6) are exercised. Delay and loss on the operator link come from a proxy that counts simulated time, not from `tc netem`, which counts wall-clock time (ADR-0049).

## Simulator controls

Loading a track, resetting the car, pausing and showing ground truth are things only a simulator can do. They go through a separate **sim-only interface**, never through the car interface or the ground station's link to the car. That way nothing in the platform depends on them, and the ground station behaves the same against the twin and the car.

## Where the parameters come from

- **The car's numbers** (wheelbase and track, tyre, gearing, steering) come from the twin's car configuration, [`config/cars/twin.yaml`](../../config/cars/README.md). It extends `nominal.yaml`, the car as designed, unchanged. Each design value names its source in `systems/` or is marked a placeholder, and a design change is an edit to both (ADR-0054). A physical car's file overrides only what was measured on it.
- **Rates, limits and timeouts** come from the ICDs, the same on every car.
- **Sensor positions and mass properties** come from the car's URDF (#105) once the block layout (#103) places them.

## Fidelity: what the twin can and can't prove

| Model | Good enough to show | Can't show | Calibrated by |
| --- | --- | --- | --- |
| Sensor emulators | The drivers parse and stamp correctly; dropouts are detected | Electrical noise, real SPI timing, mounting vibration | Bench tests with breakouts (rig R1); TBD |
| Motor-controller model | The command path, the e-stop and timeout behaviour of our modules, the any-corner stop | The VESC's motor control, current regulation, heating | [`tests/a50s-arrival.md`](../tests/a50s-arrival.md), rig R2 |
| Steering model | The steering-loss stop, re-arm checks | Belt compliance, real lock-to-lock time | Rig R2; TBD |
| Drivetrain and tyres | Plausible acceleration and stopping distance, roughly | Real grip on tile, slip (SYS-10) | A system-identification run on the floor (TBD) |
| Power model | The low-battery sequence and fault handling | Real voltage sag, inrush, transients (those stay as LTspice analysis) | Rig R4 |
| Network model | The heartbeat and teleop behaviour under loss and delay | Real 6 GHz behaviour at the venue | Rig R6 |
| Timing | The order of events and the logic | Real interrupt latency, SYS-29's latencies, Orin load (SYS-08) | Rig R5; the dev kit |

The twin's timing is simulated (ADR-0049), so timing results show the logic, not the car's real latencies. Those are measured on the rigs (`verification/plan.md`).

## What exists today

- **Firmware in the loop:** `nx test firmware-in-loop` runs the `native_sim` firmware and the ROS car backend together; they exchange link status over UDP.
- **A working Gazebo sim and ground station,** unmerged, on branch `yufeng/ground-station`:
  - Gazebo Harmonic with a track loader and a placeholder car model, in its own image that also builds for Apple Silicon
  - a browser ground station with gamepad teleop and a heartbeat test cutoff

  Its `car_link.py` re-implements the watchdog and brake ramp in Python. Under ADR-0047 that behaviour comes from the real firmware instead, so the file splits into the operator-link receiver on the Orin, the firmware's `safety.c`, and the Gazebo-side model.
- **The gym backend** (`arc_backend_sim_gym`), in interface mode without `safety.c` yet (a stub): the F1TENTH gym's 2D car behind the car interface, with the car configuration's geometry and gearing and its other vehicle values TBC; a CI test drives it closed loop through `arc_control`.
- **Stubs** for the Gazebo, Webots and replay backends in `ros/src/`.

## Open questions

| Question | Blocks |
| --- | --- |
| Whether `native_sim` can follow an outside clock without changes to Zephyr (the ADR-0049 spike) | Lockstep |
| How the sim link carries data into `native_sim`: simulator-only MAVLink messages, Zephyr's emulators, or both | The sensor emulators, the motor-controller model |
| Whether Linux virtual CAN works on Docker Desktop (macOS, Windows) | The `twin` profile on those laptops |
| Whether a SCIP emulator for the Hokuyo is worth it, or a LiDAR model at the topic level | The LiDAR in the twin |
| Whether to run the real ARM firmware binaries in an emulator such as Renode (support for the STM32H7's FD-CAN and Ethernet is unconfirmed) | Testing the peripheral drivers before the boards arrive |
