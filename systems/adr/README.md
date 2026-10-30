# Decision Records

Copy `0000-template.md` to the next number. Never edit an accepted ADR's decision; write a new one that supersedes it.

The ADRs marked *backfilled* record decisions made before this process existed. Their rationale is taken from `architecture.md` only. The options and criteria sections still need the reasoning from whoever made the call.

| ADR | Decision | Status |
| --- | --- | --- |
| [0001](0001-lidar-hokuyo.md) | Hokuyo UST-10LX over RPLidar A3 | Accepted (backfilled), amended by 0046 |
| [0002](0002-separate-can-buses.md) | Classic CAN for motor control, separate CAN-FD bus for sensors | Superseded by 0009 |
| [0003](0003-corner-sensor-node.md) | Separate sensor MCU per corner, not on the motor controller | Superseded by 0006 |
| [0004](0004-corner-connectors.md) | Separate power and signal connectors at each corner; XT30(2+2) rejected | Superseded by 0034 |
| [0005](0005-wifi-only-link.md) | Wi-Fi to the laptop is the only wireless link; no RC radio | Accepted (backfilled) |
| [0006](0006-no-corner-module.md) | No corner module in v1; VESC is the only corner board | Accepted, amended by 0034 |
| [0007](0007-ride-height-sensors.md) | Four corner ride-height sensors in v1, plus one at the optical flow sensor | Accepted |
| [0008](0008-corner-sensors-to-sync-board.md) | Wheel encoders and suspension pots wire directly to the sync board | Accepted |
| [0009](0009-single-classic-can-bus.md) | One classic CAN bus to the four corner VESCs | Accepted, amended by 0011 and 0034 |
| [0010](0010-compute-orin-nx.md) | Orin NX 16GB for v1, all inference onboard | Accepted, amended by 0046 |
| [0011](0011-vesc-telemetry-link.md) | VESC 6.4 fork; telemetry on two CAN-FD buses, commands on the classic bus | Superseded by 0034 |
| [0012](0012-estop-controlled-braking.md) | E-stop: controlled braked stop, then hardware torque cut | Accepted, amended by 0019, 0034, 0055 and 0057 |
| [0013](0013-canfd-physical-layer.md) | CAN-FD physical layer: daisy-chain, discrete transceivers, rework-only fallbacks | Superseded by 0036 |
| [0014](0014-ground-speed-event-camera.md) | Ground speed: downward event camera with three lighting modes; dead-wheel pod as fallback | Accepted, amended by 0046 |
| [0015](0015-ground-link.md) | Ground link: team router (Flint 3), laptop gateway, button hotspot, wired service port | Accepted |
| [0016](0016-orin-software-platform.md) | Orin: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT, experiments in containers | Accepted, amended by 0048 and 0050 |
| [0017](0017-sync-mcu-rtos.md) | Sync MCU firmware on Zephyr | Accepted |
| [0018](0018-steering-actuator.md) | Steering: VESC-driven brushless actuator, belt reduction, return-to-centre on e-stop | Superseded by 0019 |
| [0019](0019-steering-off-the-shelf-controller.md) | Steering: moteus-c1 + belt actuator on its own CAN-FD bus; integrated actuator (CubeMars AK class) as fallback | Accepted, amended by 0034 and 0038 |
| [0020](0020-time-sync-ptp.md) | Time sync: PTP with hardware timestamping through a PTP-aware switch | Superseded by 0021 |
| [0021](0021-time-sync-sync-mcu-domain.md) | Time sync: µs-critical sensors timestamped in the sync MCU's clock; Orin synced in software; optional i210/i226 | Accepted |
| [0022](0022-power-board-telemetry-and-shutdown.md) | Power-board data to the sync MCU over I2C with fault interrupts; two-tier low-battery shutdown | Proposed (recommendation) |
| [0023](0023-no-orin-i210-nic.md) | No i210/i226 network card on the Orin carrier in v1; sync MCU stays on the switch | Proposed (recommendation) |
| [0024](0024-telemetry-latency.md) | Telemetry stamped at sampling, forwarded within 0.25 ms; SYS-29 measured at the driver process, p99 | Accepted, amended by 0034 |
| [0025](0025-orin-carrier-antmicro-fork.md) | Orin carrier is a fork of Antmicro's open Jetson baseboard | Accepted (backfilled), amended by 0046 |
| [0026](0026-staggered-transverse-motors.md) | One motor per wheel; transverse, inboard, staggered fore and aft | Accepted (backfilled) |
| [0027](0027-suspension-family.md) | Suspension, steering and wheel parts from a competition touring car (XRAY X4 family) | Superseded by 0040 (never accepted) |
| [0028](0028-software-targets.md) | Every software component builds for simulation, development hardware and the car, from one source | Accepted, amended by 0034 and 0047 |
| [0029](0029-dev-environment.md) | One dev container for every toolchain; boards flashed from the host | Accepted |
| [0030](0030-vesc-firmware-in-repo.md) | VESC firmware fork in this repo as a git subtree of upstream (GPL-3.0 subdirectory) | Superseded by 0033 |
| [0031](0031-sync-link-encoding.md) | Sync link encoded as MAVLink 2 with our own message set | Accepted |
| [0032](0032-simulator.md) | Simulators as pluggable backends: Gazebo Harmonic, Webots, F1TENTH gym | Accepted, amended by 0047 |
| [0033](0033-vesc-firmware-vendored.md) | VESC firmware vendored as an upstream snapshot, updated by script (GPL-3.0 subdirectory) | Accepted, amended by 0034 and 0037 |
| [0034](0034-off-the-shelf-motor-controllers.md) | Off-the-shelf motor controllers (A50S) on the deck; telemetry over UART; top speed 9 m/s | Accepted, amended by 0035 and 0057; sizing review proposed in 0044 |
| [0035](0035-corner-state-and-command-timeout.md) | Each corner reports its state in its telemetry; the controllers brake on command loss | Accepted, amended by 0039 |
| [0036](0036-steering-bus-physical-layer.md) | Steering CAN-FD bus physical layer: discrete transceiver, rework-only fallbacks | Accepted |
| [0037](0037-vesc-modules-in-arc.md) | Our VESC firmware code lives in `firmware/vesc/arc/`, joined to upstream by small hooks | Accepted |
| [0038](0038-steering-loss-stops-the-car.md) | Losing steering stops the car, and the car doesn't drive without it | Accepted |
| [0039](0039-fault-stop-reset.md) | A fault stop latches, and only an e-stop cycle on the car resets it | Accepted, amended by 0052, 0053 and 0055 |
| [0040](0040-custom-corners.md) | We design the corners; only generic parts are bought (Arrma 6S drivetrain and wheel parts, industrial bearings and rod ends) | Accepted |
| [0041](0041-grounding-and-isolation.md) | Controlled return star / non-isolated review baseline; complete isolation alternative | Proposed |
| [0042](0042-combined-power-sync-motherboard.md) | Power and sync MCU share one motherboard PCB; Jetson carrier stays separate | Accepted |
| [0043](0043-board-size-estimates.md) | Preliminary motherboard and Jetson PCB / installed-envelope size estimates | Proposed (TBC; placement and thermal review pending) |
| [0044](0044-ee-sizing-after-custom-corners.md) | Align EE review with 100 mm wheels, 18:1 gearing and the revised current/regen estimates; resistor sizing reopened | Proposed (ratings and parts review pending) |
| [0045](0045-vehicle-state-sensor-boards.md) | Vehicle-state sensors on our own small boards (AS5047P, ISM330DHCXTR, five ToF), assembled in house | Accepted |
| [0046](0046-v1-builds.md) | The v1 car is built twice: v1.0 on borrowed compute and LiDAR with fewer cameras, then v1.1, the full design | Accepted |
| [0047](0047-one-platform-targets-differ-at-devices.md) | One platform for every target: the same stack everywhere, and targets differ only at the devices (simulated, development or the car's) | Accepted, amended by 0049 |
| [0048](0048-experiments-own-estimation-and-control.md) | Experiments own estimation and control, command speed and steering, and may run in real time | Accepted |
| [0049](0049-twin-runs-in-lockstep.md) | The twin runs in lockstep: the simulator's clock drives the firmware, the device models and the platform | Accepted |
| [0050](0050-one-platform-build-and-images.md) | One platform build for the Orin and laptops; images for amd64 and arm64; experiments in Docker | Accepted |
| [0051](0051-car-configuration.md) | Car configuration: one file per car in the repo, compiled into the sync MCU's build | Accepted, amended by 0054 |
| [0052](0052-envelope-defaults-and-heartbeat-stop.md) | The envelope's defaults, reverse, and a heartbeat stop held until the operator re-arms | Accepted |
| [0053](0053-fault-reset-details.md) | The fault reset's details: a 3 s check window, a stopped-wheel threshold, the steering check deferred | Accepted |
| [0054](0054-car-configuration-values.md) | The car configuration's values: written by hand in nominal.yaml, and only what code needs now | Accepted |
| [0055](0055-every-estop-press-latches.md) | Every e-stop press latches the stop, fault or not; driving resumes after the release, the power-on checks and the operator's re-arm | Accepted |
| [0056](0056-logging-profiles.md) | Each run logs a profile: everything but camera images by default, images when a run asks | Accepted |
| [0057](0057-estop-delay-and-mass.md) | E-stop delay T of 2.8 s, sized from the speed cap; 4.2 kg nominal mass, 5 kg where it's the safe side | Accepted |
