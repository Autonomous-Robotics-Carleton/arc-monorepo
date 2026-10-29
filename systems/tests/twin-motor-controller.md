# Twin fidelity: motor-controller model

- **Owner:** TBD
- **Status:** Not started; the motor-controller model doesn't exist yet ([`simulation.md`](../software/simulation.md))
- **Calibrates:** the motor-controller row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for SYS-04, SYS-05 and SYS-25 (ADR-0047, ADR-0035)
- **Rig:** R2 motor bench (one corner, A50S + 1010 + gearbox, on a flywheel or brake load), stage S2. Runs alongside the loaded integration in [`a50s-arrival.md`](a50s-arrival.md), after that procedure's bench checks pass
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), [ICD-controller-telemetry](../icd/controller-telemetry.md), [ICD-corner-connector](../icd/corner-connector.md)

## Objective

The model runs our `firmware/vesc/arc/` modules (e-stop ramp, command timeout, speed limit, telemetry) and models the rest of the controller and the motor. This procedure checks two things:

1. **Our modules behave the same on the A50S as in the model.** They're the same code, so a difference here is a defect in the model's integration or in the module, not a calibration.
2. **The modelled part is close enough for what the twin claims to show:** the command path, the e-stop and timeout behaviour and the any-corner stop. It is not evidence for the VESC's own motor control, current regulation or heating.

## Steps

Run each step on R2 with the load's inertia recorded (value TBD, from the rig), then the same commands in the twin with the model given that inertia.

1. **Speed and current response.** Command steps in speed (and current, if the command path allows it) inside the envelope, and record wheel speed, phase current and the telemetry stream. Command values TBD.
2. **E-stop ramp.** At each of TBD speeds, press the e-stop. Record brake current against time, the time to stop, and the telemetry state sequence (ADR-0035).
3. **Command timeout.** Stop sending commands with telemetry still up (the sync MCU halted, as in SYS-04's R2 step). Record the time to the timeout and the ramp that follows.
4. **Telemetry link loss.** Break the controller's UART during a run. Record what the sync MCU sees and when (SYS-25).
5. **Speed limit.** Command above the eRPM cap (ADR-0034) and record where the speed settles.
6. **Faults.** Each controller fault the bench can inject safely (list TBD, from the readiness review): record the fault code and state the controller reports.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Telemetry fields, rates and state sequence | Same sequence for the same event | Exact |
| E-stop ramp: brake current profile, time to stop | Twin against rig | TBD |
| Command-timeout trip time | Both against the configured timeout (≤ 150 ms, TBC, ADR-0035) | TBD |
| Detection of a dead UART (simulated time) | Twin against rig | TBD |
| Speed at the eRPM cap | Twin against rig | TBD |
| Speed step response (rise time, steady state) | Twin against rig | TBD |
| Fault codes and states | Same code, same state | Exact |

## Parameters it updates

Where the model's parameters live is TBD.

| Parameter | Value today | Set from |
| --- | --- | --- |
| Motor torque and speed constants, winding resistance | TBD (Castle 1010; 2 pole pairs in `config/cars/nominal.yaml`) | Step 1 |
| Current limits the model applies | TBD (the controllers' configuration) | Step 1 |
| Load inertia the model sees on the bench | TBD (rig) | Rig record |
| `drivetrain.erpm_max` (car configuration) | 75,000, TBC (ADR-0034, RSK-18) | Step 5 confirms it; a change is a design change, not a calibration |

The ramp, the timeout and the telemetry are our code's settings, not model parameters; they come from the ICDs and ADRs.

## Results

| Step | Rig | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Speed and current response | TBD | TBD | Not run | |
| 2. E-stop ramp | TBD | TBD | Not run | |
| 3. Command timeout | TBD | TBD | Not run | |
| 4. Telemetry link loss | TBD | TBD | Not run | |
| 5. Speed limit | TBD | TBD | Not run | |
| 6. Faults | TBD | TBD | Not run | |
