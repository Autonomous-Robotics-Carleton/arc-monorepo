# System Requirements

Status: **Draft**. Values were first inferred from `architecture.md`; later ones come from the ADRs cited in each row, or are `TBD`. None are baselined.
Values marked `TBC` are a guess that the owner must confirm.

## Mission

The car is a 1/10-scale 4WD **autonomy research platform**: a sandbox where the team's engineers can try an idea without building hardware first.

1. **Classical autonomy first.** Get conventional pipelines (state estimation, localization, planning, MPC) running and dependable.
2. **Then learned and embodied AI.** Move on to vision-language-action (VLA) models and other embodied-AI architectures running on the car.
3. **Hardware is never the reason an experiment fails.** When something doesn't work, the cause must be the software under test. The hardware has to be reliable, and any fault it does have must be detected and logged so it can be ruled out.
4. **Sensors are already there.** The car carries a broad sensor suite, so a new idea needs code, not a hardware build. The sandbox is the software. The hardware is tightly integrated and doesn't need to be easy to change; modular hardware is a v2 goal.

Known first users (from `architecture.md`, TBC): MPC research, head-to-head racing against a Traxxas Slash 4x4, and the active-aero capstone.

### What this means for the architecture

- **Platform vs experiment software.** *Platform* software (drivers, time sync, logging, state estimation, low-level control, safety) is maintained by the team and must stay up. *Experiment* software is whatever an engineer is trying today. An experiment that crashes, hangs or hogs CPU, GPU, memory or disk cannot stop the platform from driving, logging or stopping. On the hardware side, this only needs per-rail fusing, which the power board already has.
- **Layered control.** Learned policies run slowly and can be wrong, so they never command the motors directly. They command a classical control layer, which runs inside a safety envelope that neither layer can override.
- **Data is a product.** Learned methods need demonstration and run data, so time-aligned logs of every sensor, every command and every operator input are a requirement, not a debugging aid.

## Operating concept (draft)

- **Environment:** indoor, glossy/uniform tile, dedicated Wi-Fi access point at the track.
- **Session:** a battery pack lasts one run of ≥ 10 min hard driving. Packs are removed and charged at the team's charging station, a separate system outside this spec. The car does no charging.
- **Bench:** subsystems run on 19 V wall power without a LiPo.
- **Operators:** one laptop operator with a gamepad, plus a person at the physical e-stop.
- **Users:** team engineers running their own experiments, often on code that is new and untested.

## System requirements

| ID | Requirement | Rationale | Verif. | Status |
| --- | --- | --- | --- | --- |
| SYS-01 | Top speed on tile ≥ 9 m/s (~32 km/h). Lowered from 12 m/s on 2026-10-07: the team doesn't need more, and the motor controllers' stable range ends there at 13.5:1 gearing (ADR-0034) | Racing and limit-handling research; F1TENTH/Roboracer sets no speed cap. Practical limits are the track and grip | T | Draft |
| SYS-02 | Overall width 238–341 mm and length 454–654 mm (F1TENTH/Roboracer rule 2.1.3: within 15% of Traxxas). **To check in CAD:** a 1/10 touring layout (~310 mm wheelbase) may come in under the 454 mm minimum length (RSK-16). **To check against the rule:** these bounds are +15% and −20% of the Traxxas reference (~297 × 569 mm), not ±15%; confirm rule 2.1.3's wording | Eligible to race F1TENTH/Roboracer. The head-to-head presence rule and the power-equivalence and single-motor rules are deliberately not followed. LiDAR (≤ UST-30LX equivalent) and battery (≤ 4S) already comply | I | Draft |
| SYS-03 | Run time ≥ 10 min of hard driving per pack | One session per pack | T | Draft |
| SYS-04 | After loss of operator heartbeat, the car rolls ≤ 2 m before braking starts (≤ 150 ms watchdog timeout, TBC per RSK-09: ~1.4 m at 9 m/s), then brakes to a stop using the same controlled brake ramp as the e-stop. If the sync MCU itself stops commanding, each motor controller brakes on the same ramp after 150 ms (TBC) without a command (ADR-0035). Expected total from 9 m/s at 5 m/s² (TBC, ADR-0012): ~10.1 m (~1.35 m rolled, then ~8.8 m braking, ramp included) | Safety; sets watchdog timeout and the controllers' brake config | T | Draft |
| SYS-05 | A physical e-stop brings the car to a controlled, braked stop (ramped, not instantaneous; never free rolling), then removes drive torque in hardware within `TBC` 3 s, without depending on Orin or sync-board software (IEC 60204-1 stop category 1, ADR-0012 as amended by ADR-0034) | Safety; covers a hung Orin or sync MCU | T + I | Draft |
| SYS-06 | Vehicle-state estimate available to the controller at ≥ 200 Hz, age ≤ 5 ms p99 (sensor sample to estimate available); output rate configurable upward | Classical control and MPC (mission 1). Sets the encoder and angle-sensor sample rates, the motor controllers' telemetry rate (ADR-0008, ADR-0034), and the drive command rate on the command bus: 500 Hz (TBC), 2.5× the 200 Hz minimum, so a control loop configured faster still reaches the motors. | T | Draft |
| SYS-07 | All sensor samples carry timestamps on one time base, aligned to within ≤ 10 µs between any two sensors (LiDAR ≤ 1 ms); µs-critical sensors are timestamped in the sync MCU's clock (ADR-0021) | Sensor fusion and MPC (mission 1); aligned training data (mission 2) | T | Draft |
| SYS-08 | Onboard compute, while running a learned policy that fits in 16 GB alongside the platform software (ADR-0010) at ≥ 5 Hz, also runs localization at 40 Hz, camera perception at ≥ 30 Hz with ≤ 50 ms camera-to-output latency, and MPC at ≥ 100 Hz with < 0.1% missed deadlines | Missions 1 and 2; the parent for the compute module choice | T | Draft |
| SYS-09 | All sensor streams are logged onboard for a full run (≥ SYS-03) | Training data (mission 2) and fault attribution (mission 3) | D | Draft |
| SYS-10 | Measure ground velocity directly, forward and sideways (ADR-0014), and per-wheel speed, well enough to estimate slip ratio and sideslip to within ±2% (TBC) | MPC on a low-grip surface (mission 1) | T | Draft |
| SYS-11 | Manual teleop latency, gamepad event to wheel response, ≤ 20 ms p95 and ≤ 50 ms p99 | Demonstration data quality (mission 2) | T | Draft |
| SYS-12 | LiDAR and cameras survive a frontal impact at `TBD` m/s with no damage | Testing will crash the car | T | Draft |
| SYS-13 | Swapping one corner (motor, gearbox; the motor controller stays on the deck, ADR-0034) takes ≤ `TBD` min, with no code changes (config only: the controller loads that motor's saved configuration) | Modularity, serviceability | D | Draft |
| SYS-14 | Low-voltage systems run from wall power or battery, switching over without reboot | Bench work without cycling LiPos | T | Draft |
| SYS-15 | Reserve underfloor volume `TBD` and power `TBD` W for active aero | Active-aero capstone (known user) | I + A | Draft |
| SYS-16 | Mass and CG are not limited; they are tracked (mass budget, CG estimate) for handling, power and braking estimates | Mass isn't a design driver for v1 | A then I | Draft |
| SYS-17 | No fixed cost cap: funding is approved per request, so every purchase request is justified from `budgets/cost.csv` | How the team's funding works | A | Draft |
| SYS-18 | Goal: zero hardware- or firmware-caused failures. Every such failure is logged, root-caused and fixed (FRACAS). Acceptance for v1: ≥ 10 h of test runs with zero hardware- or firmware-caused failures | Mission 3 | T | Draft |
| SYS-19 | Every hardware fault (brownout, rail overcurrent, bus error, sensor dropout, over-temperature, watchdog trip) is detected, timestamped and logged | Mission 3: a failed run can be attributed to hardware or ruled out | T + I | Draft |
| SYS-20 | *Deleted:* payload ports. Hardware modularity moved to v2 | — | — | Deleted |
| SYS-21 | An experiment process that crashes, hangs, or exhausts CPU, GPU, memory or disk cannot stop platform software from driving, logging or stopping | Platform vs experiment (missions 3, 4) | T | Draft |
| SYS-22 | No policy, learned or classical, can command beyond the safety envelope (speed, acceleration, steering limits); limits are set per session, default to 3 m/s for new experiments, and can be raised to the car's physical maximum; the envelope is enforced on the sync MCU, outside any policy's process | Layered control (mission 2) | T | Draft |
| SYS-23 | Logs record operator commands and policy outputs on the same time base as the sensors | Training data (mission 2) | T | Draft |
| SYS-24 | Every sensor is sampled at the fastest rate its data is useful at, timestamped and logged; no data path (link, storage, offload) exceeds 50% of capacity, and no CAN bus exceeds 70%, with all sensors at full rate (`budgets/bus-load.csv`) | Hardware never limits what software can try (mission 4) | A + T | Draft |
| SYS-25 | Any one corner going silent (no valid telemetry for 5 ms, TBC: several frames, so one dropped frame never stops a run; or no CAN status for ~60 ms, TBC), braking on its own (reporting e-stop or command timeout) or faulted (reporting a fault code), or the command bus failing (no acknowledgements, or bus-off), or the steering failing (no valid steering reply for 5 ms, TBC; a steering controller fault; steering-bus bus-off; or a steering angle that can't be trusted), triggers a controlled stop of all four corners within ≤ 20 ms of detection, using the SYS-04 brake ramp. Drive commands are refused until the steering is up and its angle checked. The stop latches: driving resumes only after a reset by the e-stop button on the car and the power-on checks (ADR-0039) | A single-corner fault must not leave one corner braking while the others drive, and a car that can't steer must not drive (ADR-0011, ADR-0035, ADR-0038, ADR-0039; telemetry arrives over UART since ADR-0034, ICD-controller-telemetry) | T | Draft |
| SYS-26 | A laptop connected to the car keeps its own internet connection; the team network gives a default gateway only to the car (ADR-0015) | Operators need internet while working on the car | D | Draft |
| SYS-27 | The car is reachable with no outside network present: over the team router network, through a button-activated car hotspot, or through a wired service Ethernet port (ADR-0015) | The car must never be unreachable because of building Wi-Fi | D | Draft |
| SYS-28 | The car gets internet through the team network when a ground-station laptop or the router's phone link provides it; control traffic (teleop, heartbeat) has priority, and bulk transfers run only while the car is parked | Updates and package installs without risking the driving link | T | Draft |
| SYS-29 | Sync-board sensors, VESC telemetry and steering telemetry reach the sync-MCU driver process on the Orin, timestamped, within ≤ 2 ms of sampling (p99); frame cameras within ≤ 1 frame + 5 ms (p99); every message carries its sample time. Delivery to other processes counts against SYS-06 (ADR-0024, `budgets/latency.csv`) | Hardware latency must not limit what software can do (mission 4) | T | Draft |
| SYS-30 | On low battery, judged on the lowest cell (SYS-32), the car warns, then shuts the Orin down cleanly before the pack reaches its low-voltage cutoff; the NVMe never loses power mid-write | Protect logs and the filesystem (SYS-09) | T | Draft |
| SYS-31 | Battery voltage sag at peak current never resets the Orin, the sync board or any sensor | A brownout must not look like a software failure (mission 3) | T | Draft |
| SYS-32 | Every cell voltage in the pack is measured, timestamped and logged at ≥ 100 Hz (TBC) to ±10 mV, without draining the pack when the car is off | Catch a weak or sagging cell before it looks like a random brownout (mission 3) | T | Draft |
| SYS-33 | Every software component (sync MCU firmware, VESC firmware, platform software) builds and runs from the same source on three kinds of target: simulation, off-the-shelf development hardware, and the car's own hardware. Target-specific code is limited to board configuration and drivers (ADR-0028) | Hardware arrives in one order, so software is written and tested before it exists; experiments move between simulation and the car without changes (mission 4) | D + T | Draft |

## Accepted limits

Places where v1 hardware deliberately limits what software can do. Each was chosen knowingly; revisit in v2.

| ID | Limit | Why accepted | Decided in | Revisit |
| --- | --- | --- | --- | --- |
| LIM-01 | Onboard models must fit in the Orin NX's 16 GB alongside the platform software; 7B-class VLAs are out | Carrier, power and cost for v1; the classical stack comes first | ADR-0010 | v2 compute (Thor T4000 class) |
| LIM-02 | 2D LiDAR: one plane, 40 Hz, 10 m, ±40 mm | Standard for 1/10 racing; 3D scanners are slower and bigger | ADR-0001 | v2 if 3D perception becomes a goal |
| LIM-03 | Event cameras are 320 × 320 | Only small, low-power MIPI event sensor | ADR-0014 | Higher-resolution event sensors in v2 |
| LIM-04 | Ride height from ToF: ~10–20 ms per reading, mm-level noise | Laser triangulation rejected on cost | ADR-0007, BOM E-26 | If aero needs µm-level ride height |
| LIM-05 | No room to add hardware without a change: all four CSI ports used. (The CAN controllers were all used too until ADR-0034 freed two) | Tight integration in v1; modularity is a v2 goal | ADR-0014, ADR-0034 | v2 modular car |
| LIM-06 | The sync MCU is a single point of failure: if it dies, data stops, steering holds, and the VESCs time out and brake | Fails safe; redundancy isn't worth the complexity in v1 | ADR-0017, ADR-0019 | If S5 shows sync-MCU failures |
| LIM-07 | Wheel forces and tire temperature aren't measured directly (forces are estimated from suspension angle × spring rate and motor current; tire temperature dropped). The A50S's ±~165 A current full scale makes the ~12 A motor-current signal coarse (ADR-0034) | No requirement needed them in v1 | ADR-0006, ADR-0034 | If vehicle-dynamics research needs them |
| LIM-08 | Raw data from a full run can't be streamed live over Wi-Fi; it comes off on the swappable NVMe | Wi-Fi bandwidth; everything is still logged onboard at full rate | ADR-0015 | — |

## Spec items with no parent yet

Mission 4 (sensors already there) gives the sensor suite a parent. The rule still applies: each payload sensor names at least one experiment someone wants to run with it, so the suite is chosen, not just accumulated.

| Spec item | Parent | Question to answer |
| --- | --- | --- |
| Four CSI ports, event camera, quad side/rear cameras | SYS-08, mission 4 | Which first experiment uses side/rear views and the event camera? |
| Second IMU | SYS-06, mission 4 | Which experiment uses it? |
| Steering torque (moteus-c1 q-axis current, ADR-0019) as a grip proxy | SYS-10, mission 4 | Used in v1 or just logged? |
