# System Requirements

Status: **Draft**. Every value here was inferred from `architecture.md` or left `TBD`. None are baselined.
Values marked `TBC` are a guess that the owner must confirm.

## Mission

The car is a 1/10-scale 4WD **autonomy research platform**: a sandbox where the team's engineers can try an idea without building hardware first.

1. **Classical autonomy first.** Get conventional pipelines (state estimation, localization, planning, MPC) running and dependable.
2. **Then learned and embodied AI.** Move on to vision-language-action (VLA) models and other embodied-AI architectures running on the car.
3. **Hardware is never the reason an experiment fails.** When something doesn't work, the cause must be the software under test. The hardware has to be reliable, and any fault it does have must be detected and logged so it can be ruled out.
4. **Sensors are already there.** The car carries a broad sensor suite on standard ports, so a new idea needs code, not a hardware build.

Known first users (from `architecture.md`, TBC): MPC research, head-to-head racing against a Traxxas Slash 4x4, and the active-aero capstone.

### What this means for the architecture

- **Core vs payload.** The *core* (drive, power, safety, compute, primary state sensors) must be reliable. The *payload* (sandbox sensors and experiments) must be unable to take the core down: a shorted, hung or babbling payload device is isolated, and the car still drives and stops.
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
| SYS-04 | After loss of operator heartbeat, stop from top speed within `TBD` m | Safety; sets watchdog timeout and VESC brake config | T | Draft |
| SYS-05 | A physical e-stop removes drive torque without depending on any software | Safety; covers a hung sync MCU | T + I | Draft |
| SYS-06 | Vehicle-state estimate available to the controller at ≥ `TBD` Hz with age ≤ `TBD` ms | Classical control and MPC (mission 1). Sets encoder/pot sample rates and VESC status rates (ADR-0008, ADR-0009). | T | Draft |
| SYS-07 | All sensor samples carry timestamps on one time base, aligned to within `TBD` µs | Sensor fusion and MPC (mission 1); aligned training data (mission 2) | T | Draft |
| SYS-08 | Onboard compute runs the classical stack at ≥ `TBD` Hz and a learned policy of class `TBD` (model size, precision) at ≥ `TBD` Hz | Missions 1 and 2; the parent for the compute module choice | T | Draft |
| SYS-09 | All sensor streams are logged onboard for a full run (≥ SYS-03) | Training data (mission 2) and fault attribution (mission 3) | D | Draft |
| SYS-10 | Measure ground speed and per-wheel speed well enough to estimate slip to within `TBD` % | MPC on a low-grip surface (mission 1) | T | Draft |
| SYS-11 | Manual teleop end-to-end latency ≤ `TBD` ms (p95) | Demonstration data quality (mission 2) | T | Draft |
| SYS-12 | LiDAR and cameras survive a frontal impact at `TBD` m/s with no damage | Testing will crash the car | T | Draft |
| SYS-13 | Swapping one corner (motor, gearbox, VESC) takes ≤ `TBD` min, with no code changes (config only) | Modularity, serviceability | D | Draft |
| SYS-14 | Low-voltage systems run from wall power or battery, switching over without reboot | Bench work without cycling LiPos | T | Draft |
| SYS-15 | Reserve underfloor volume `TBD` and power `TBD` W for active aero | Active-aero capstone (known user) | I + A | Draft |
| SYS-16 | Total mass ≤ `TBD` kg, CG height ≤ `TBD` mm | Handling, aero, top speed | A then I | Draft |
| SYS-17 | v1 build cost ≤ `TBD` CAD | Team budget | A | Draft |
| SYS-18 | Complete ≥ `TBD` hours of test runs with zero hardware-caused failures | Mission 3 | T | Draft |
| SYS-19 | Every hardware fault (brownout, rail overcurrent, bus error, sensor dropout, over-temperature, watchdog trip) is detected, timestamped and logged | Mission 3: a failed run can be attributed to hardware or ruled out | T + I | Draft |
| SYS-20 | Provide ≥ `TBD` payload ports, each with fused, switchable power, a data link and access to the time base, on the deck mounting grid | Mission 4 | I | Draft |
| SYS-21 | A payload fault (short, hang, bus flooding) cannot stop the core from driving, logging or stopping | Core vs payload (mission 3) | T | Draft |
| SYS-22 | No policy, learned or classical, can command beyond the safety envelope (speed, acceleration, steering limits `TBD`); the envelope is enforced outside the policy's process | Layered control (mission 2) | T | Draft |
| SYS-23 | Logs record operator commands and policy outputs on the same time base as the sensors | Training data (mission 2) | T | Draft |

## Spec items with no parent yet

Mission 4 (sensors already there) gives the sandbox sensors a parent. The rule still applies: each payload sensor names at least one experiment someone wants to run with it, so the suite is chosen, not just accumulated.

| Spec item | Parent | Question to answer |
| --- | --- | --- |
| Encoder/pot sample rate, VESC status rates | SYS-06 | What rate and latency does the MPC need? (Status rates are also capped by CAN bus load, ADR-0009.) |
| Four CSI ports, event camera, quad side/rear cameras | SYS-08, SYS-20 | Core or payload? Which first experiment uses side/rear views and the event camera? |
| Second IMU | SYS-06 or SYS-20 | Core or payload? |
| Steering servo current as a grip proxy | SYS-10 or SYS-20 | Used in v1 or just logged? |
| Orin NX 16GB | SYS-08 | Does 16 GB fit the learned-policy class you want to run onboard? (See ADR to write.) |
| Regulators rated ≥ 20 V input | SYS-14 | 20 V leaves almost no margin over a 19 V brick (see RSK-06) |
