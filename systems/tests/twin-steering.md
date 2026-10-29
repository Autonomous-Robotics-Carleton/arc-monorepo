# Twin fidelity: steering model

- **Owner:** TBD
- **Status:** Not started; the steering model doesn't exist yet ([`simulation.md`](../software/simulation.md))
- **Calibrates:** the steering row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for SYS-25 (ADR-0038, ADR-0039)
- **Rig:** R2 motor bench with the moteus-c1 (E-31) on the steering bus, stage S2; whether R2 carries the belt, knuckles and knuckle encoders (E-24) or only the actuator is TBD. Steps 2 and 3 need the real mechanism, so on R2 only if it does, otherwise on the assembled car on a stand (S3)
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), ADR-0019, ADR-0038, [`tests/rsk-03-canfd-bench.md`](rsk-03-canfd-bench.md)

## Objective

The twin claims to show the steering-loss stop and the re-arm checks, not belt compliance or the real lock-to-lock time. This procedure checks that the model's fault behaviour matches the moteus-c1's, and sets the model's range, centre and rate from the real mechanism so that runs in the twin steer like the car.

## Steps

Run each step on the rig, then the same commands in the twin.

1. **Fault behaviour.** With the steering at rest and then moving: silence the moteus-c1, fault it, take its bus off, disconnect a knuckle encoder (the R2 injections in SYS-25's row). Record what the sync MCU detects, when, and the stop and re-arm sequence (ADR-0039).
2. **Range and centre.** Command full lock each way and centre. Record both knuckle encoders' angles (E-24) and the moteus position.
3. **Step response.** Command steps of TBD sizes, including lock to lock. Record angle against time: rise time, overshoot, settling, and the tracking error the steering-loss check sees (ADR-0038's ~3° / 20 ms threshold).
4. **E-stop return to centre.** Press the e-stop from full lock. Record the return time (~0.3–0.5 s, TBC, ADR-0019) and the hold.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Fault detection, stop and re-arm sequence | Same events, same order; detection time in simulated time | Order exact; time TBD |
| Range and centre | Model against measured | TBD |
| Step response: rise time, overshoot | Twin against rig | TBD |
| Tracking error during normal steps | The twin doesn't trip the steering-loss check where the rig doesn't, and the reverse | Exact |
| Return-to-centre time | Twin against rig | TBD |

## Parameters it updates

| Parameter | Where | Value today | Set from |
| --- | --- | --- | --- |
| `steering.max_angle` | Car configuration | 0.4363 rad, TBC (ADR-0019) | Step 2, in the measured car's file |
| `steering.centre_offset` | Car configuration | 0.0, measured on each car | Step 2, in the measured car's file |
| Rate limit and response of the modelled actuator | TBD | TBD | Step 3 |
| Backlash and compliance | TBD | TBD | Steps 2–3, if the model has them (TBD) |

A measured range and centre go in the physical car's file, not in `twin.yaml`, which follows the design (ADR-0054). Whether the twin should also run with a measured car's file is an open question in the [verification plan](../verification/plan.md#twin-fidelity).

## Results

| Step | Rig | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Fault behaviour | TBD | TBD | Not run | |
| 2. Range and centre | TBD | TBD | Not run | |
| 3. Step response | TBD | TBD | Not run | |
| 4. E-stop return to centre | TBD | TBD | Not run | |
