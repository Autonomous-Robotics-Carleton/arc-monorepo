# Verification plan

- **Status:** Draft
- **Rule:** every system requirement maps to at least one verification activity with a pass criterion. A requirement is "met" only when its activity has passed and the result is recorded (link the log, data or report in the Status column).
- **Methods:** **A** analysis · **I** inspection · **D** demonstration · **T** test
- **Funding constraint:** hardware arrives with each funded order (v1.0, then v1.1; ADR-0046), so everything that can be verified by analysis or simulation (stage S0) is done **before** each order. Bench and vehicle stages follow delivery.
- **Builds:** v1.0 verifies every requirement except SYS-08's learned-policy part (LIM-09) and SYS-10 at full speed (LIM-11), which are verified on v1.1.

## Stages

| Stage | When | What |
| --- | --- | --- |
| **S0** Analysis and simulation | Before the order | Budgets close, LTspice, datasheet checks, fab gate (`reviews/fab-gate.md`) |
| **S1** Board bring-up | Each board alone | Power-up, rails, interfaces, ICD pin-by-pin checks |
| **S2** Hardware-in-the-loop (HIL) | Boards together on the bench, motors on stands | Timing, safety paths, fault injection, bus behaviour |
| **S3** Vehicle, static | Assembled car on a stand, wheels off the ground | Full system with nothing moving on the floor |
| **S4** Vehicle, track | School floor | Dynamic performance |
| **S5** Reliability campaign | After S4 passes | The 10 h run and FRACAS (SYS-18) |

## Rigs

| Rig | Contents | Used for |
| --- | --- | --- |
| **R1 Sync-board HIL** | Combined power/sync motherboard (ADR-0042); real sensors where cheap (IMUs, AS5047s on hand-turned shafts, ToF over a moving target); a second MCU acting as fake CAN nodes and four controller UART links; signal generator; scope; fault injection (pull connectors, short a bus, drop the Ethernet link) | SYS-04, -07, -19, -22, -24, -25, -29, -31, -32; twin fidelity (sensors, timing) |
| **R2 Motor bench** | One corner (A50S + 1010 + gearbox, ADR-0034) driving a flywheel or brake load; second corner for bus tests; a moteus-c1 on the steering bus (RSK-03); current probe; e-stop button; power board's motor-bus cut; bus clamp | SYS-05, -25, RSK-03, RSK-11, RSK-12, RSK-18; twin fidelity (motor controller, steering, motor-bus cut) |
| **R3 CAN-FD harness** | Original standalone rig retired by ADR-0034. Steering checks remain on R2; drive UART qualification follows ADR-0034 and `tests/a50s-arrival.md`; see `tests/rsk-03-canfd-bench.md` | SYS-24, RSK-03 |
| **R4 Power** | Electronic load, bench supply standing in for the pack, 19 V brick, scope with current probe | SYS-14, -30, -31, power scenarios; twin fidelity (power) |
| **R5 Timing and latency** | Scope; LED + photodiode in front of each camera; GPIO toggles timestamped by both the sync MCU and the Orin | SYS-06, -07, -29; twin fidelity (timing) |
| **R6 Network** | Flint 3 router, laptops on Linux/macOS, traffic generator for bulk transfers, packet capture | SYS-11, -26, -27, -28; twin fidelity (network) |
| **R7 Track instrumentation** | Taped distance markers, high-speed phone video, logged heartbeat gaps | SYS-01, -04, -05, -10, -11; twin fidelity (drivetrain and tyres, network) |
| **R8 Platform software** | Orin with platform stack; stress tools (CPU, GPU, memory, disk hogs); a deliberately broken experiment container | SYS-08, -21, -23 |

## Verification matrix

| Req | Method | Stage | Rig / procedure | Pass criterion | Status |
| --- | --- | --- | --- | --- | --- |
| SYS-01 Top speed | A, T | S0, S4 | Gearing/power analysis; then timed runs between markers | ≥ 9 m/s on tile, within the controllers' eRPM limit (ADR-0034) | Not started |
| SYS-02 Footprint | I | S0 (CAD), S3 | Measure | Width 238–341 mm, length 454–654 mm | Not started |
| SYS-03 Run time | A, T | S0, S4 | `budgets/power-scenarios.md`; then a hard-driving run to the low-battery warning | ≥ 10 min | Not started |
| SYS-04 Heartbeat stop | T | S2, S4 | R1: cut the heartbeat, measure watchdog reaction. R2: halt the sync MCU, measure each controller's command timeout. R7: cut heartbeat at speed, measure roll before braking and stop distance | Watchdog ≤ 150 ms (TBC, RSK-09); controller command timeout ≤ 150 ms (TBC, ADR-0035); ≤ 2 m rolled before braking at top speed; same brake ramp as e-stop | Not started |
| SYS-05 E-stop | T, I | S2, S3, S4 | R2: press e-stop; scope the brake current and the motor-bus voltage; repeat with the controller firmware halted. R7 at speed | Ramped brake, no free rolling; motor-bus power off by T (TBC 2.4 s, after the ~1.9 s stop; ICD-corner-connector) whatever the firmware does; steering returns to centre, its power cut ~1 s after T (TBC, ADR-0019) | Not started |
| SYS-06 State estimate | T | S2, S3 | R5 + R1: timestamp a sensor event and the baseline estimator's estimate that reflects it (ADR-0048) | ≥ 200 Hz; age ≤ 5 ms p99 | Not started |
| SYS-07 Time alignment | T | S2 | R5: one physical event seen by several sensors (light flash for cameras, tap for IMUs, edge for encoders); compare timestamps | ≤ 10 µs between sensors; LiDAR ≤ 1 ms | Not started |
| SYS-08 Compute rates | T | S3 | R8: classical stack + learned policy at full load for 30 min | Localization 40 Hz, MPC ≥ 100 Hz, perception ≥ 30 Hz (≤ 50 ms), policy ≥ 5 Hz; MPC deadline misses < 0.1% | Not started |
| SYS-09 Logging | D | S3, S4 | Log a full run; check every stream is present and complete | All streams, full run, no gaps | Not started |
| SYS-10 Slip and sideslip | T | S4 | Ground-speed camera against fusion, plus a measured-distance run; motion capture if ever available | Slip ratio and sideslip within ±2% (TBC) | Not started |
| SYS-11 Teleop latency | T | S4 | R6 + R7: timestamp the gamepad event on the laptop and the wheel response on the car | ≤ 20 ms p95, ≤ 50 ms p99 | Not started |
| SYS-12 Crash survival | T | S4 | Controlled frontal impacts into a padded barrier at increasing speed | No damage at SYS-12 speed | Not started |
| SYS-13 Corner swap | D | S3 | Time a swap, config change only | ≤ SYS-13 minutes; no code change | Not started |
| SYS-14 Wall/battery handover | T | S1, S3 | R4: unplug and replug the wall supply under full LV load | No reset anywhere | Not started |
| SYS-15 Aero reserve | I, A | S0 | CAD volume check; motor-bus power reserve | Volume and power reserved | Not started |
| SYS-16 Mass and CG | A, I | S0, S3 | Mass budget, then weigh and balance | Tracked and recorded (no limit) | Not started |
| SYS-17 Cost | A | S0 | `budgets/cost.csv` | Every purchase request justified from the budget file | Not started |
| SYS-18 Reliability | T | S5 | 10 h of runs; every hardware/firmware failure goes through FRACAS | 10 h with zero hardware/firmware-caused failures | Not started |
| SYS-19 Fault detection | T, I | S2 | R1/R4 fault injection: brownout, rail short, bus short, pulled sensor, over-temperature (heat gun), watchdog trip | Each injected fault detected, timestamped and logged | Not started |
| SYS-21 Platform isolation | T | S3 | R8: run the broken experiment container (crash, hang, CPU/GPU/memory/disk hogs) while driving on the stand | Platform keeps driving, logging and can stop | Not started |
| SYS-22 Safety envelope | T | S2, S3 | R1: send commands beyond each limit, including from a "malicious" experiment | Nothing beyond the limits reaches the actuators; default 3 m/s; limits raisable to the physical maximum | Not started |
| SYS-23 Command logging | D | S3 | Check logs for operator commands and policy outputs on the sensor time base | Present, same time base | Not started |
| SYS-24 Full-rate data | A, T | S0, S2 | Bus / storage budgets, R1/R2 final interfaces at full rate; analyzer / scope / sequence logs | Four independent 2 Mbit/s 8N1 drive UARTs with full ≥1 kHz status each, measured UART/data paths ≤50%; CAN ≤70%; separate steering baseline 1 / 5 Mbit/s BRS (ADR-0034). Correct sample-time alignment and completeness | Not started |
| SYS-25 Any-corner stop | T | S2, S3 | R1/R2: unplug one controller's command bus, telemetry link, ESTOP wire, or power mid-run; inject a controller fault; unplug the steering bus, fault the moteus-c1, disconnect a knuckle encoder; try to drive with the steering down | All four corners begin a controlled stop ≤ 20 ms after detection (5 ms of telemetry silence or ~60 ms without CAN status, TBC; or a corner reporting e-stop, command timeout or a fault; or the steering lost); a single dropped frame never triggers; no drive while the steering is down; the stop stays latched until the e-stop cycle, the checks and a re-arm (ADR-0039) | Not started |
| SYS-26 Laptop internet | D | S1 | R6: laptop wired to the router keeps internet; other devices get no gateway | Pass on Linux and macOS | Not started |
| SYS-27 Reachability | D | S3 | R6: reach the car via router, button hotspot, and service port, each with no outside network | All three paths work | Not started |
| SYS-28 Car internet | T | S3 | R6: bulk download during teleop; check control-traffic priority and the parked-only rule | No teleop degradation; bulk only while parked | Not started |
| SYS-29 Latency | T | S2 | R5: event → timestamped in the sync-MCU driver process on the Orin, per sensor, all sensors at full rate (`budgets/latency.csv`) | p99 ≤ 2 ms sync-board/VESC/steering; p99 ≤ 1 frame + 5 ms cameras | Not started |
| SYS-30 Low battery | T | S2 | R4: ramp the "pack" down; lower one cell via a cell simulator | Warning, then clean Orin shutdown before cutoff; triggered by the lowest cell | Not started |
| SYS-31 Brownout immunity | T | S2 | R4: step the pack voltage down by the peak-current sag and beyond | No resets at the expected sag | Not started |
| SYS-32 Cell monitoring | T | S1, S2 | Cell simulator: known voltages; measure the off-state drain | ±10 mV at ≥ 100 Hz; negligible drain when off | Not started |
| SYS-33 Software targets | D, T | S0, S3 | CI builds every component for every target and runs the simulation tests; the platform stack runs unchanged against the simulator, log replay and the car (R8) | All targets build; simulation tests pass; same launch on every target except the target argument | Not started |

## Twin fidelity

The digital twin ([`software/simulation.md`](../software/simulation.md)) gives evidence for a requirement only as far as its models are calibrated. Each row of its fidelity table has a procedure here that runs the same input on a rig and in the twin, says what "matches" means, and names the model parameters the measurement sets.

- **Code is checked, models are calibrated.** The firmware, drivers and platform code run unchanged in the twin (ADR-0047). Where they behave differently on the rig and in the twin for the same input, that's a defect to fix, not a parameter to tune. Only the modelled parts get calibrated.
- **A calibration is a PR.** It changes the parameter, names the procedure and its result as the value's source, and records the run in the procedure's results table.
- **A fidelity test verifies no requirement by itself.** A requirement verified by test still needs its row in the matrix above to pass on hardware. The twin's results stand in for a rig only for what the fidelity table says the model is good enough to show.
- **Stage:** each procedure runs when its rig first exists, and again when the hardware it measured changes (rerun triggers TBD).
- **Open:** where the calibrated parameters of each model live (the car configuration holds only geometry, tyre, gearing and steering, ADR-0054); whether the twin runs with a measured car's file or only `twin.yaml`; the tolerances, all TBD.

| Model | Procedure | Rig | Stage | Parameters it sets |
| --- | --- | --- | --- | --- |
| Sensor emulators | [`tests/twin-sensors.md`](../tests/twin-sensors.md) | R1 | S1, S2 | IMU, angle-sensor and ToF noise and bias |
| Motor-controller model | [`tests/twin-motor-controller.md`](../tests/twin-motor-controller.md), alongside [`tests/a50s-arrival.md`](../tests/a50s-arrival.md) | R2 | S2 | Motor constants, current limits |
| Steering model | [`tests/twin-steering.md`](../tests/twin-steering.md) | R2; the car on a stand (TBD) | S2, S3 | Steering range and centre, actuator response |
| Drivetrain and tyres | [`tests/twin-drivetrain.md`](../tests/twin-drivetrain.md) | R7 | S4 | Rolling diameter, mass and CG, losses, tyre grip |
| Power model | [`tests/twin-power.md`](../tests/twin-power.md) | R4, R2 | S1, S2 | Monitor noise, handshake times, motor-bus cut delay, pack model |
| Network model | [`tests/twin-network.md`](../tests/twin-network.md) | R6, R7 | S1, S4 | Operator-link delay and loss |
| Timing | [`tests/twin-timing.md`](../tests/twin-timing.md) | R5, R1, dev kit + R8 | S2 | The twin's Orin limits; order of events checked |

## Before the order (S0 checklist)

- [ ] Power, bus-load, latency, cost and mass budgets close (`budgets/`)
- ~~RSK-03 LTspice simulation (`tests/rsk-03-canfd-bench.md`, step 0)~~ Not needed: retired with the drive CAN-FD buses (ADR-0034)
- [ ] Camera drivers confirmed for JetPack 7.2 (RSK-01)
- [x] Sync MCU pin-mux fits (desk check 2026-10-07, `handoff/electrical.md` §2: steering on an internal FDCAN, before the tally's one spare per interface type; the EE confirms with spares and the MCP2518FD option)
- [ ] Every custom board through the fab gate, with extra bare PCBs ordered
- [ ] Optional: ground-speed phone test (`tests/ground-speed-phone-test.md`)
