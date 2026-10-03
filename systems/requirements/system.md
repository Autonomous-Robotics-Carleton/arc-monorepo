# System Requirements

Status: **Draft**. Every value here was inferred from `architecture.md` or left `TBD`. None are baselined.
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
| SYS-01 | Top speed on tile ≥ `TBD` m/s (spec gear table spans 8–12.5 m/s) | Racing (known user) | T | Draft |
| SYS-02 | Wheelbase ≤ 324 mm and track ≤ 296 mm | Fair racing vs the Slash 4x4 (known user) | I | Draft |
| SYS-03 | Run time ≥ 10 min of hard driving per pack | One session per pack | T | Draft |
| SYS-04 | After loss of operator heartbeat, the car rolls ≤ 2 m before braking starts (≤ ~150 ms watchdog timeout at 12 m/s), then brakes to a stop using the same controlled brake ramp as the e-stop. Expected total from 12 m/s at ~5 m/s²: ~17 m | Safety; sets watchdog timeout and VESC brake config | T | Draft |
| SYS-05 | A physical e-stop brings the car to a controlled, braked stop (ramped, not instantaneous; never free rolling), then removes drive torque in hardware within `TBC` 3 s, without depending on Orin or sync-board software (IEC 60204-1 stop category 1, ADR-0012) | Safety; covers a hung Orin or sync MCU | T + I | Draft |
| SYS-06 | Vehicle-state estimate available to the controller at ≥ 200 Hz, age ≤ 5 ms p99 (sensor sample to estimate available); output rate configurable upward | Classical control and MPC (mission 1). Sets encoder/pot sample rates and VESC status rates (ADR-0008, ADR-0009). | T | Draft |
| SYS-07 | All sensor samples carry timestamps on one time base, aligned to within `TBD` µs | Sensor fusion and MPC (mission 1); aligned training data (mission 2) | T | Draft |
| SYS-08 | Onboard compute runs the classical stack at ≥ `TBD` Hz and a learned policy that fits in 16 GB alongside the platform software (ADR-0010) at ≥ `TBD` Hz | Missions 1 and 2; the parent for the compute module choice | T | Draft |
| SYS-09 | All sensor streams are logged onboard for a full run (≥ SYS-03) | Training data (mission 2) and fault attribution (mission 3) | D | Draft |
| SYS-10 | Measure ground velocity directly, forward and sideways (ADR-0014), and per-wheel speed, well enough to estimate slip ratio and sideslip to within ±2% (TBC) | MPC on a low-grip surface (mission 1) | T | Draft |
| SYS-11 | Manual teleop end-to-end latency ≤ `TBD` ms (p95) | Demonstration data quality (mission 2) | T | Draft |
| SYS-12 | LiDAR and cameras survive a frontal impact at `TBD` m/s with no damage | Testing will crash the car | T | Draft |
| SYS-13 | Swapping one corner (motor, gearbox, VESC) takes ≤ `TBD` min, with no code changes (config only) | Modularity, serviceability | D | Draft |
| SYS-14 | Low-voltage systems run from wall power or battery, switching over without reboot | Bench work without cycling LiPos | T | Draft |
| SYS-15 | Reserve underfloor volume `TBD` and power `TBD` W for active aero | Active-aero capstone (known user) | I + A | Draft |
| SYS-16 | Total mass ≤ `TBD` kg, CG height ≤ `TBD` mm | Handling, aero, top speed | A then I | Draft |
| SYS-17 | v1 build cost ≤ `TBD` CAD | Team budget | A | Draft |
| SYS-18 | Goal: zero hardware- or firmware-caused failures. Every such failure is logged, root-caused and fixed (FRACAS). Acceptance for v1: ≥ 10 h of test runs with zero hardware- or firmware-caused failures | Mission 3 | T | Draft |
| SYS-19 | Every hardware fault (brownout, rail overcurrent, bus error, sensor dropout, over-temperature, watchdog trip) is detected, timestamped and logged | Mission 3: a failed run can be attributed to hardware or ruled out | T + I | Draft |
| SYS-20 | *Deleted:* payload ports. Hardware modularity moved to v2 | — | — | Deleted |
| SYS-21 | An experiment process that crashes, hangs, or exhausts CPU, GPU, memory or disk cannot stop platform software from driving, logging or stopping | Platform vs experiment (missions 3, 4) | T | Draft |
| SYS-22 | No policy, learned or classical, can command beyond the safety envelope (speed, acceleration, steering limits `TBD`); the envelope is enforced outside the policy's process | Layered control (mission 2) | T | Draft |
| SYS-23 | Logs record operator commands and policy outputs on the same time base as the sensors | Training data (mission 2) | T | Draft |
| SYS-24 | Every sensor is sampled at the fastest rate its data is useful at, timestamped and logged; no data path (link, storage, offload) exceeds 50% of capacity, and no CAN bus exceeds 70%, with all sensors at full rate (`budgets/bus-load.csv`) | Hardware never limits what software can try (mission 4) | A + T | Draft |
| SYS-25 | Loss of commands or telemetry from any one corner (missing CAN acknowledgements, health status or telemetry frames, or bus-off) triggers a controlled stop of all four corners within `TBD` ms, using the SYS-04 brake ramp | A single-corner fault must not leave one corner braking while the others drive (ADR-0011) | T | Draft |
| SYS-26 | A laptop connected to the car keeps its own internet connection; the team network gives a default gateway only to the car (ADR-0015) | Operators need internet while working on the car | D | Draft |
| SYS-27 | The car is reachable with no outside network present: over the team router network, through a button-activated car hotspot, or through a wired service Ethernet port (ADR-0015) | The car must never be unreachable because of building Wi-Fi | D | Draft |
| SYS-28 | The car gets internet through the team network when a ground-station laptop or the router's phone link provides it; control traffic (teleop, heartbeat) has priority, and bulk transfers run only while the car is parked | Updates and package installs without risking the driving link | T | Draft |
| SYS-29 | Sync-board sensors and VESC telemetry reach the Orin, timestamped, within ≤ 2 ms of sampling; frame cameras within ≤ 1 frame + 5 ms; every message carries its sample time (`budgets/latency.csv`) | Hardware latency must not limit what software can do (mission 4) | T | Draft |

## Spec items with no parent yet

Mission 4 (sensors already there) gives the sensor suite a parent. The rule still applies: each payload sensor names at least one experiment someone wants to run with it, so the suite is chosen, not just accumulated.

| Spec item | Parent | Question to answer |
| --- | --- | --- |
| Four CSI ports, event camera, quad side/rear cameras | SYS-08, mission 4 | Which first experiment uses side/rear views and the event camera? |
| Second IMU | SYS-06, mission 4 | Which experiment uses it? |
| Steering servo current as a grip proxy | SYS-10, mission 4 | Used in v1 or just logged? |
| Regulators rated ≥ 20 V input | SYS-14 | 20 V leaves almost no margin over a 19 V brick (see RSK-06) |
