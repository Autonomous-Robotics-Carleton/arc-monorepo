# Verification plan

- **Status:** Draft
- **Rule:** every system requirement maps to at least one verification activity with a pass criterion. A requirement is "met" only when its activity has passed and the result is recorded (link the log, data or report in the Status column).
- **Methods:** **A** analysis · **I** inspection · **D** demonstration · **T** test
- **Funding constraint:** hardware arrives in one order, so everything that can be verified by analysis or simulation (stage S0) is done **before** the order. Bench and vehicle stages follow delivery.

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
| **R1 Sync-board HIL** | Sync board + power board on the stacking header; real sensors where cheap (IMUs, AS5047s on hand-turned shafts, ToF over a moving target); a second MCU acting as fake CAN/CAN-FD nodes; signal generator; scope; fault injection (pull connectors, short a bus, drop the Ethernet link) | SYS-04, -07, -19, -22, -24, -25, -29, -31, -32 |
| **R2 Motor bench** | One corner (VESC fork + 1010 + gearbox) driving a flywheel or brake load; second corner for bus tests; current probe; e-stop button; bus clamp | SYS-05, -25, RSK-02, RSK-11, RSK-12 |
| **R3 CAN-FD harness** | `tests/rsk-03-canfd-bench.md` | RSK-03, SYS-24 bus load |
| **R4 Power** | Electronic load, bench supply standing in for the pack, 19 V brick, scope with current probe | SYS-14, -30, -31, power scenarios |
| **R5 Timing and latency** | Scope; LED + photodiode in front of each camera; GPIO toggles timestamped by both the sync MCU and the Orin | SYS-06, -07, -29 |
| **R6 Network** | Flint 3 router, laptops on Linux/macOS, traffic generator for bulk transfers, packet capture | SYS-11, -26, -27, -28 |
| **R7 Track instrumentation** | Taped distance markers, high-speed phone video, logged heartbeat gaps | SYS-01, -04, -05, -10, -11 |
| **R8 Platform software** | Orin with platform stack; stress tools (CPU, GPU, memory, disk hogs); a deliberately broken experiment container | SYS-08, -21, -23 |

## Verification matrix

Values marked *(proposed)* are the ones proposed for SYS-07, -08, -11, -22 and -25 but not yet written into the requirements.

| Req | Method | Stage | Rig / procedure | Pass criterion | Status |
| --- | --- | --- | --- | --- | --- |
| SYS-01 Top speed | A, T | S0, S4 | Gearing/power analysis; then timed runs between markers | ≥ SYS-01 value on tile | Not started |
| SYS-02 Footprint | I | S0 (CAD), S3 | Measure | Wheelbase ≤ 324 mm, track ≤ 296 mm | Not started |
| SYS-03 Run time | A, T | S0, S4 | `budgets/power-scenarios.md`; then a hard-driving run to the low-battery warning | ≥ 10 min | Not started |
| SYS-04 Heartbeat stop | T | S2, S4 | R1: cut the heartbeat, measure watchdog reaction. R7: cut heartbeat at speed, measure roll before braking and stop distance | Watchdog ≤ ~150 ms; ≤ 2 m rolled before braking at top speed; same brake ramp as e-stop | Not started |
| SYS-05 E-stop | T, I | S2, S3, S4 | R2: press e-stop; scope the brake current and EN_GATE; repeat with the VESC firmware halted. R7 at speed | Ramped brake, no free rolling; gate drive off by T ≈ 3 s whatever the firmware does; steering returns to centre, power cut ~1 s later | Not started |
| SYS-06 State estimate | T | S2, S3 | R5 + R1: timestamp a sensor event and the estimate that reflects it | ≥ 200 Hz; age ≤ 5 ms p99 | Not started |
| SYS-07 Time alignment | T | S2 | R5: one physical event seen by several sensors (light flash for cameras, tap for IMUs, edge for encoders); compare timestamps | ≤ 100 µs between sensors; LiDAR ≤ 1 ms *(proposed)* | Not started |
| SYS-08 Compute rates | T | S3 | R8: classical stack + learned policy at full load for 30 min | Localization 40 Hz, MPC 50 Hz, perception 30 Hz, policy ≥ 5 Hz; MPC deadline misses < 0.1% *(proposed)* | Not started |
| SYS-09 Logging | D | S3, S4 | Log a full run; check every stream is present and complete | All streams, full run, no gaps | Not started |
| SYS-10 Slip and sideslip | T | S4 | Ground-speed camera against fusion, plus a measured-distance run; motion capture if ever available | Slip ratio and sideslip within ±2% (TBC) | Not started |
| SYS-11 Teleop latency | T | S4 | R6 + R7: timestamp the gamepad event on the laptop and the wheel response on the car | ≤ 50 ms p95 *(proposed)* | Not started |
| SYS-12 Crash survival | T | S4 | Controlled frontal impacts into a padded barrier at increasing speed | No damage at SYS-12 speed | Not started |
| SYS-13 Corner swap | D | S3 | Time a swap, config change only | ≤ SYS-13 minutes; no code change | Not started |
| SYS-14 Wall/battery handover | T | S1, S3 | R4: unplug and replug the wall supply under full LV load | No reset anywhere | Not started |
| SYS-15 Aero reserve | I, A | S0 | CAD volume check; motor-bus power reserve | Volume and power reserved | Not started |
| SYS-16 Mass and CG | A, I | S0, S3 | Mass budget, then weigh and balance | ≤ SYS-16 limits | Not started |
| SYS-17 Cost | A | S0 | `budgets/cost.csv` against the budget | ≤ SYS-17 | Not started |
| SYS-18 Reliability | T | S5 | 10 h of runs; every hardware/firmware failure goes through FRACAS | 10 h with zero hardware/firmware-caused failures | Not started |
| SYS-19 Fault detection | T, I | S2 | R1/R4 fault injection: brownout, rail short, bus short, pulled sensor, over-temperature (heat gun), watchdog trip | Each injected fault detected, timestamped and logged | Not started |
| SYS-21 Platform isolation | T | S3 | R8: run the broken experiment container (crash, hang, CPU/GPU/memory/disk hogs) while driving on the stand | Platform keeps driving, logging and can stop | Not started |
| SYS-22 Safety envelope | T | S2, S3 | R1: send commands beyond each limit, including from a "malicious" experiment | Nothing beyond the limits reaches the actuators; default 3 m/s *(proposed)* | Not started |
| SYS-23 Command logging | D | S3 | Check logs for operator commands and policy outputs on the sensor time base | Present, same time base | Not started |
| SYS-24 Full-rate data | A, T | S0, S2 | Bus-load and storage budgets; R1/R3 measured bus load and dropped-sample counts at full rate | Every sensor at native rate; links ≤ 50%, CAN ≤ 70% | Not started |
| SYS-25 Any-corner stop | T | S2, S3 | R1/R2: unplug one corner's command bus, telemetry bus, or power mid-run | All four corners begin a controlled stop ≤ 20 ms *(proposed)* | Not started |
| SYS-26 Laptop internet | D | S1 | R6: laptop wired to the router keeps internet; other devices get no gateway | Pass on Linux and macOS | Not started |
| SYS-27 Reachability | D | S3 | R6: reach the car via router, button hotspot, and service port, each with no outside network | All three paths work | Not started |
| SYS-28 Car internet | T | S3 | R6: bulk download during teleop; check control-traffic priority and the parked-only rule | No teleop degradation; bulk only while parked | Not started |
| SYS-29 Latency | T | S2 | R5: event → timestamped on the Orin, per sensor (`budgets/latency.csv`) | ≤ 2 ms sync-board/VESC; ≤ 1 frame + 5 ms cameras | Not started |
| SYS-30 Low battery | T | S2 | R4: ramp the "pack" down; lower one cell via a cell simulator | Warning, then clean Orin shutdown before cutoff; triggered by the lowest cell | Not started |
| SYS-31 Brownout immunity | T | S2 | R4: step the pack voltage down by the peak-current sag and beyond | No resets at the expected sag | Not started |
| SYS-32 Cell monitoring | T | S1, S2 | Cell simulator: known voltages; measure the off-state drain | ±10 mV at ≥ 100 Hz; negligible drain when off | Not started |

## Before the order (S0 checklist)

- [ ] Power, bus-load, latency, cost and mass budgets close (`budgets/`)
- [ ] RSK-03 LTspice simulation (`tests/rsk-03-canfd-bench.md`, step 0)
- [ ] Camera drivers confirmed for JetPack 7.2 (RSK-01)
- [ ] Sync MCU pin-mux fits (EE)
- [ ] Every custom board through the fab gate, with extra bare PCBs ordered
- [ ] Optional: ground-speed phone test (`tests/ground-speed-phone-test.md`)
