# Twin fidelity: drivetrain and tyres

- **Owner:** TBD
- **Status:** Not started; needs a driving car (v1.0) and the twin's vehicle model
- **Calibrates:** the drivetrain and tyres row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove): the system-identification run on the floor
- **Supports:** SYS-33; the twin's evidence for SYS-04 and SYS-05 (stopping distance), and for control work in the `fast` profile
- **Rig:** R7 track instrumentation on the school floor, stage S4, plus a scale for mass and CG (SYS-16)
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), [`config/cars/README.md`](../../config/cars/README.md), ADR-0054

## Objective

Measure how the car accelerates, coasts, brakes and turns on the real floor, and set the vehicle model's parameters so that the same commands give the same motion in the twin. The twin claims plausible acceleration and stopping distance, roughly; it is not evidence for grip or slip (SYS-10), which are verified on the car.

Every run stays inside the safety envelope in force (default 3 m/s, ADR-0052). Runs above it need the envelope raised, which is the owner's call (TBD).

## Steps

Log every run (commands, wheel speeds, motor telemetry, IMU, steering angle, ground-speed camera) with the car's configuration hash. Then replay the logged commands into the twin, open loop, from the same starting state.

1. **Mass and CG.** Weigh the car, and each axle and side, as run. Record the configuration (battery, sensors fitted).
2. **Rolling diameter and gearing.** Drive slowly a measured distance between markers; compare wheel-encoder turns, motor eRPM and distance. This gives the rolling diameter, and checks the gear ratio against the car file.
3. **Straight-line acceleration.** From rest, step to each of TBD speeds. Record speed against time.
4. **Coast-down.** From each of TBD speeds, command zero current (if the command path allows it; otherwise TBD). Record speed against time.
5. **Braking.** From each of TBD speeds, press the e-stop and then cut the heartbeat. Record the stopping distance against the markers and on video.
6. **Steady cornering.** At each of TBD speeds and steering angles, drive steady circles. Record yaw rate, lateral acceleration and the ground-speed camera's sideslip.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Speed against time in steps 3–4 | Twin against car, same commands | TBD |
| Stopping distance in step 5 | Twin against car, same speed | TBD |
| Yaw rate and lateral acceleration in step 6 | Twin against car, same speed and steering angle | TBD |
| Distance per wheel turn | Car file against step 2 | TBD |

Each world that models the drivetrain (Gazebo, the gym) has its own parameters. Whether both must match, or only the one the `twin` profile uses, is TBD.

## Parameters it updates

| Parameter | Where | Value today | Set from |
| --- | --- | --- | --- |
| `wheels.rolling_diameter` | Car configuration | 0.100 m (ADR-0040; real tyre is MQ-6, #117) | Step 2, in the measured car's file |
| `drivetrain.gear_ratio_front`, `gear_ratio_rear` | Car configuration | 18.0 each (`architecture.md`) | Step 2 checks; a change is the car's gearing, not a calibration |
| Mass, CG and inertia | The car's URDF (#105), ADR-0054 | TBD (`budgets/mass.csv` until measured) | Step 1; inertia TBD |
| Rolling resistance and drivetrain losses | Vehicle model; TBD | TBD | Step 4 |
| Tyre longitudinal and lateral grip on tile | Vehicle model; TBD | TBD | Steps 3, 5, 6 |
| Suspension stiffness and damping | Vehicle model; TBD | TBD | TBD (no step yet) |

## Results

| Step | Car | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Mass and CG | TBD | — | Not run | |
| 2. Rolling diameter and gearing | TBD | — | Not run | |
| 3. Straight-line acceleration | TBD | TBD | Not run | |
| 4. Coast-down | TBD | TBD | Not run | |
| 5. Braking | TBD | TBD | Not run | |
| 6. Steady cornering | TBD | TBD | Not run | |
