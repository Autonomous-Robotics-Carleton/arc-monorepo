# Twin fidelity: sensor emulators

- **Owner:** TBD
- **Status:** Not started; the emulators don't exist yet ([`simulation.md`](../software/simulation.md), "How each model attaches")
- **Calibrates:** the sensor emulators row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for SYS-06, SYS-07 and SYS-19 (ADR-0047)
- **Rig:** R1 sync-board HIL (real IMUs, AS5047Ps on hand-turned shafts, ToF over a moving target), stage S1/S2; the NUCLEO `hil` profile until the motherboard exists
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), E-20 to E-26 in [`bom/electrical.md`](../bom/electrical.md)

## Objective

Show that each emulator gives the real driver what the real sensor gives it: the same axes, signs, units and range, comparable noise and bias, and the same behaviour when the sensor drops out. Then set the emulators' noise and bias parameters from the measurement.

The drivers and the sensor layer are the same code in both runs. A difference in what they do with the same register values is a defect in that code or in the emulator's register map, not something to calibrate.

Out of scope: electrical noise, real SPI timing and mounting vibration, which the twin can't show; the battery and rail monitors, which are in the [power procedure](twin-power.md). Whether the LiDAR emulator and the camera frame node get a procedure here, and on which rig, is TBD.

## Steps

Record every rig run with the sensor layer's output, as the twin would see it. Then run the same scenario in the twin (`twin` profile) with the emulator fed the same physical input: the orientation, angle or distance the rig held.

1. **IMU (E-20, E-21), static.** With the board still and level, record TBD minutes at the configured output data rate. Repeat with the board on each of its six faces.
   - From the level run: per-axis noise (standard deviation) and bias of the accelerometer and gyro, and the Allan deviation of the gyro (bias instability; the datasheet gives 3 °/h typical).
   - From the six faces: the sign and axis of gravity on each face, and the accelerometer's scale and offset per axis.
2. **IMU, rotation.** Turn the board by hand through a known angle about each axis, against a stop (fixture TBD). Check that the integrated gyro rate has the sign and axis the emulator gives for the same rotation.
3. **Angle sensor (E-22 to E-24).** Turn the shaft to known angles across a full turn (angle reference TBD). Record the reported angle at each: sign, zero offset, noise at rest, and the reading as the shaft passes through zero.
4. **Ride-height ToF (E-26).** Put the target at known distances across the sensor's range, on the floor material the car will see, if available (RSK-07). Record noise, bias against distance, the measurement period, and how invalid or out-of-range readings are reported.
5. **Dropout.** Pull one sensor's connector during a run (as SYS-19's fault injection), then the same fault in the emulator. Record what the sensor layer reports, when, and what's logged.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Axes, signs, units, frame names | Same for the same physical input | Exact |
| Noise (standard deviation), per sensor and axis | Twin against rig | TBD |
| Bias and gyro bias instability | Twin against rig | TBD |
| AS5047P zero-crossing and range handling | Same readings across zero | Exact |
| ToF bias against distance; invalid-reading reporting | Twin against rig | TBD; reporting exact |
| Dropout: detection, timestamp, log entry | Same event, same fields; detection time in simulated time | Fields exact; time TBD |

## Parameters it updates

Where these live is TBD: they're not in the car configuration's schema ([`config/cars/README.md`](../../config/cars/README.md)), and sensor mounts wait for the URDF (#105).

| Parameter | Value today | Set from |
| --- | --- | --- |
| IMU noise and bias per axis (accelerometer, gyro); gyro bias instability | TBD | Steps 1–2 |
| AS5047P noise at rest | TBD | Step 3 |
| ToF noise, bias against distance, invalid-reading behaviour | TBD | Step 4 |

A per-car mounting offset (an encoder's zero, an IMU's alignment) is a measurement of that car, not of the model; it belongs in that car's file once the schema has a place for it (TBD).

## Results

| Sensor | Quantity | Rig | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- | --- |
| IMU (E-20) | Noise, bias, axes | TBD | TBD | Not run | |
| IMU (E-21) | Noise, bias, axes | TBD | TBD | Not run | |
| AS5047P | Noise, zero crossing | TBD | TBD | Not run | |
| ToF | Noise, bias, invalid readings | TBD | TBD | Not run | |
| Each | Dropout | TBD | TBD | Not run | |
