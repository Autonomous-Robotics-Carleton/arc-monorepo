# Decision Records

Copy `0000-template.md` to the next number. Never edit an accepted ADR's decision; write a new one that supersedes it.

The ADRs marked *backfilled* record decisions made before this process existed. Their rationale is taken from `architecture.md` only. The options and criteria sections still need the reasoning from whoever made the call.

| ADR | Decision | Status |
| --- | --- | --- |
| [0001](0001-lidar-hokuyo.md) | Hokuyo UST-10LX over RPLidar A3 | Accepted (backfilled) |
| [0002](0002-separate-can-buses.md) | Classic CAN for motor control, separate CAN-FD bus for sensors | Superseded by 0009 |
| [0003](0003-corner-sensor-node.md) | Separate sensor MCU per corner, not on the motor controller | Superseded by 0006 |
| [0004](0004-corner-connectors.md) | Separate power and signal connectors at each corner; XT30(2+2) rejected | Accepted (pinout in ICD rev B) |
| [0005](0005-wifi-only-link.md) | Wi-Fi to the laptop is the only wireless link; no RC radio | Accepted (backfilled) |
| [0006](0006-no-corner-module.md) | No corner module in v1; VESC is the only corner board | Accepted |
| [0007](0007-ride-height-sensors.md) | Four corner ride-height sensors in v1, plus one at the optical flow sensor | Accepted |
| [0008](0008-corner-sensors-to-sync-board.md) | Wheel encoders and suspension pots wire directly to the sync board | Accepted |
| [0009](0009-single-classic-can-bus.md) | One classic CAN bus to the four corner VESCs | Accepted, amended by 0011 |
| [0010](0010-compute-orin-nx.md) | Orin NX 16GB for v1, all inference onboard | Accepted |
| [0011](0011-vesc-telemetry-link.md) | VESC 6.4 fork; telemetry on two CAN-FD buses, commands on the classic bus | Accepted, amended by 0013 and 0024 |
| [0012](0012-estop-controlled-braking.md) | E-stop: controlled braked stop, then hardware torque cut | Accepted, amended by 0019 |
| [0013](0013-canfd-physical-layer.md) | CAN-FD physical layer: daisy-chain, discrete transceivers, rework-only fallbacks | Accepted |
| [0014](0014-ground-speed-event-camera.md) | Ground speed: downward event camera with three lighting modes; dead-wheel pod as fallback | Accepted |
| [0015](0015-ground-link.md) | Ground link: team router (Flint 3), laptop gateway, button hotspot, wired service port | Accepted |
| [0016](0016-orin-software-platform.md) | Orin: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT, experiments in containers | Accepted |
| [0017](0017-sync-mcu-rtos.md) | Sync MCU firmware on Zephyr | Accepted |
| [0018](0018-steering-actuator.md) | Steering: VESC-driven brushless actuator, belt reduction, return-to-centre on e-stop | Superseded by 0019 |
| [0019](0019-steering-off-the-shelf-controller.md) | Steering: moteus-c1 + belt actuator on its own CAN-FD bus; integrated actuator (CubeMars AK class) as fallback | Accepted |
| [0020](0020-time-sync-ptp.md) | Time sync: PTP with hardware timestamping through a PTP-aware switch | Superseded by 0021 |
| [0021](0021-time-sync-sync-mcu-domain.md) | Time sync: µs-critical sensors timestamped in the sync MCU's clock; Orin synced in software; optional i210/i226 | Accepted |
| [0022](0022-power-board-telemetry-and-shutdown.md) | Power-board data to the sync MCU over I2C with fault interrupts; two-tier low-battery shutdown | Proposed (recommendation) |
| [0023](0023-no-orin-i210-nic.md) | No i210/i226 network card on the Orin carrier in v1; sync MCU stays on the switch | Proposed (recommendation) |
| [0024](0024-telemetry-latency.md) | Telemetry stamped at sampling, forwarded within 0.25 ms; SYS-29 measured at the driver process, p99 | Accepted |
| [0025](0025-orin-carrier-antmicro-fork.md) | Orin carrier is a fork of Antmicro's open Jetson baseboard | Accepted (backfilled) |
| [0026](0026-staggered-transverse-motors.md) | One motor per wheel; transverse, inboard, staggered fore and aft | Accepted (backfilled) |
| [0027](0027-suspension-family.md) | Suspension, steering and wheel parts from a competition touring car (XRAY X4 family) | Proposed (recommendation) |
| [0028](0028-software-targets.md) | Every software component builds for simulation, development hardware and the car, from one source | Accepted |
| [0029](0029-dev-environment.md) | One dev container for every toolchain; boards flashed from the host | Accepted |
| [0030](0030-vesc-firmware-in-repo.md) | VESC firmware fork in this repo as a git subtree of upstream (GPL-3.0 subdirectory) | Accepted |
| [0031](0031-sync-link-encoding.md) | Sync link encoded as MAVLink 2 with our own message set | Proposed (recommendation) |
| [0032](0032-simulator.md) | Gazebo Harmonic as the simulator for the platform software | Proposed (recommendation) |

