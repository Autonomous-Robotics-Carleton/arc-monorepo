# Twin fidelity: power model

- **Owner:** TBD
- **Status:** Not started; the power model and the monitor emulators don't exist yet ([`simulation.md`](../software/simulation.md))
- **Calibrates:** the power row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for SYS-19, SYS-30 and SYS-32 (ADR-0022)
- **Rig:** R4 power (electronic load, bench supply standing in for the pack, 19 V brick, cell simulator, scope with current probe), stage S1/S2; step 4 on R2
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), [ICD-power-rails](../icd/power-rails.md), [`budgets/power-scenarios.md`](../budgets/power-scenarios.md)

## Objective

The twin claims to show the low-battery sequence and the fault handling. It doesn't show real voltage sag, inrush or transients; those stay LTspice analysis and R4 tests. This procedure checks that the model and the monitor emulators give the sync MCU the same data and the same sequence as the power board, and sets the model's pack and timing parameters from measurement.

The low-battery policy (thresholds W and S, ADR-0022) is firmware that runs in both. A different sequence for the same cell voltages is a defect, not a calibration.

## Steps

Run each step on the rig, then the same voltages, loads and events in the twin.

1. **Monitor data.** Set known cell voltages and rail currents. Record what the sync MCU reads: rate, resolution and noise per channel (SYS-32: ±10 mV at ≥ 100 Hz).
2. **Low-battery sequence.** Ramp the lowest cell down through W, S and the hardware cutoff (values TBD, ADR-0022). Record the time of each step: warning, shutdown request, Orin halted, compute rail removed; and the backstop if the halt is withheld.
3. **Rail faults.** Inject the R4 faults in SYS-19's row (rail short, fuse or e-fuse trip, power-good loss). Record the flag, the interrupt, and what's detected and logged.
4. **Motor-bus cut after e-stop.** On R2, press the e-stop and record when the motor-bus power goes off (T = 2.4 s, TBC, [ICD-corner-connector](../icd/corner-connector.md)).
5. **Pack under load.** With the real pack (E-40, choice TBD), record voltage against current and charge drawn over a discharge. Rig and load profile TBD: R4's bench supply can't stand in for this step.
6. **Wall and battery handover.** Unplug and replug the wall supply (SYS-14). Record the monitor data the sync MCU sees.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Monitor rate, resolution and fault flags | Twin against rig | Rate and flags exact; noise TBD |
| Low-battery sequence | Same steps in the same order for the same cell voltages; times in simulated time | Order exact; times TBD |
| Rail-fault detection and log entries | Same fields | Exact |
| Motor-bus cut delay | Model against measured | TBD |
| Pack voltage against current and charge drawn | Model against measured | TBD |

## Parameters it updates

Where the power model's parameters live is TBD.

| Parameter | Value today | Set from |
| --- | --- | --- |
| Monitor noise and resolution per channel | TBD | Step 1 |
| Shutdown handshake times (Orin halt, rail removal) | TBD | Step 2 |
| Motor-bus cut delay T | 2.4 s, TBC (ICD-corner-connector) | Step 4 |
| Pack open-circuit voltage against charge, internal resistance | TBD (20 mΩ is the power-scenarios estimate) | Step 5 |
| Rail currents per operating state | TBD (`budgets/power.csv` until measured) | Steps 1, 6 |

## Results

| Step | Rig | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Monitor data | TBD | TBD | Not run | |
| 2. Low-battery sequence | TBD | TBD | Not run | |
| 3. Rail faults | TBD | TBD | Not run | |
| 4. Motor-bus cut | TBD | TBD | Not run | |
| 5. Pack under load | TBD | TBD | Not run | |
| 6. Wall and battery handover | TBD | TBD | Not run | |
