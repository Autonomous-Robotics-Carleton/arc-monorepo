# System Requirements

Status: **Draft**. Every value here was inferred from `architecture.md` or left `TBD`. None are baselined.
Values marked `TBC` are a guess that the owner must confirm.

## Mission (draft, confirm)

The car is a 1/10-scale 4WD research platform that:

1. Drives autonomously on an indoor tile track, using onboard compute ("onboard AI from day one").
2. Serves as the plant for model-predictive-control (MPC) research, so it must give a high-rate, low-latency, time-aligned estimate of vehicle state (wheel speeds, slip, suspension, IMU).
3. Races head-to-head against a Traxxas Slash 4x4 under the same footprint.
4. Hosts the active-aero capstone later without a chassis redesign.
5. Is driven manually over Wi-Fi to collect human-demonstration data.

## Operating concept (draft)

- **Environment:** indoor, glossy/uniform tile, dedicated Wi-Fi access point at the track.
- **Session:** a battery pack lasts one run of ≥ 10 min hard driving. Packs are removed and charged at the team's charging station, a separate system outside this spec. The car does no charging.
- **Bench:** subsystems run on 19 V wall power without a LiPo.
- **Operators:** one laptop operator with a gamepad, plus a person at the physical e-stop.

## System requirements

| ID | Requirement | Rationale | Verif. | Status |
| --- | --- | --- | --- | --- |
| SYS-01 | Top speed on tile ≥ `TBD` m/s (spec gear table spans 8–12.5 m/s) | Racing (mission 3) | T | Draft |
| SYS-02 | Wheelbase ≤ 324 mm and track ≤ 296 mm | Fair racing vs the Slash 4x4 (mission 3) | I | Draft |
| SYS-03 | Run time ≥ 10 min of hard driving per pack | One session per pack | T | Draft |
| SYS-04 | After loss of operator heartbeat, stop from top speed within `TBD` m | Safety; sets watchdog timeout and VESC brake config | T | Draft |
| SYS-05 | A physical e-stop removes drive torque without depending on any software | Safety; covers a hung sync MCU | T + I | Draft |
| SYS-06 | Vehicle-state estimate available to the controller at ≥ `TBD` Hz with age ≤ `TBD` ms | MPC (mission 2). **This is the parent for the 1 kHz corner rate and the CAN-FD bus; neither is justified until this number exists.** | T | Draft |
| SYS-07 | All sensor samples carry timestamps on one time base, aligned to within `TBD` µs | Sensor fusion and MPC (mission 2) | T | Draft |
| SYS-08 | Perception + planning run onboard at ≥ `TBD` Hz | Autonomy (mission 1); the parent for the Orin NX choice | T | Draft |
| SYS-09 | All sensor streams are logged onboard for a full run (≥ SYS-03) | Research data and debugging | D | Draft |
| SYS-10 | Measure ground speed and per-wheel speed well enough to estimate slip to within `TBD` % | MPC on a low-grip surface (mission 2) | T | Draft |
| SYS-11 | Manual teleop end-to-end latency ≤ `TBD` ms (p95) | Demonstration data quality (mission 5) | T | Draft |
| SYS-12 | LiDAR and cameras survive a frontal impact at `TBD` m/s with no damage | Testing will crash the car | T | Draft |
| SYS-13 | Swapping one corner module takes ≤ `TBD` min, with no code changes (config only) | Modularity, serviceability | D | Draft |
| SYS-14 | Low-voltage systems run from wall power or battery, switching over without reboot | Bench work without cycling LiPos | T | Draft |
| SYS-15 | Reserve underfloor volume `TBD` and power `TBD` W for active aero | Capstone (mission 4) | I + A | Draft |
| SYS-16 | Total mass ≤ `TBD` kg, CG height ≤ `TBD` mm | Handling, aero, top speed | A then I | Draft |
| SYS-17 | v1 build cost ≤ `TBD` CAD | Team budget | A | Draft |

## Spec items with no parent yet

These values in `architecture.md` don't trace to any requirement above. Each one needs a parent requirement, or it is gold-plating for v1.

| Spec item | Candidate parent | Question to answer |
| --- | --- | --- |
| Corner encoder/pot sample rate, VESC status rates | SYS-06 | What rate and latency does the MPC actually need? (Status rates are also capped by CAN bus load, ADR-0009.) |
| Four CSI ports, event camera, quad side/rear cameras | SYS-08 | Which perception task needs side and rear views in v1? |
| Second IMU | SYS-06 | Is it needed for v1? (Ride height is now traced in ADR-0007.) |
| Steering servo current as a grip proxy | SYS-10? | Is it used in v1 or just logged? |
| Orin NX 16GB ($999+) over Orin Nano | SYS-08 | What workload does the Nano fail on? |
| Regulators rated ≥ 20 V input | SYS-14 | 20 V leaves almost no margin over a 19 V brick (see RSK-06) |
