# Twin fidelity: timing

- **Owner:** TBD
- **Status:** Not started; needs the lockstep (ADR-0049) and, for steps 1–2, rig R5
- **Calibrates:** the timing row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for the order of events in SYS-04, SYS-05 and SYS-25. Not evidence for SYS-06, SYS-07, SYS-08 or SYS-29, which are measured on the rigs
- **Rig:** R5 timing and latency with R1, stage S2 (steps 1–2); the Orin Nano dev kit with R8's stress tools (step 3), whenever it's available
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), ADR-0049, [`budgets/latency.csv`](../budgets/latency.csv)

## Objective

The twin's time is simulated, so it shows the order of events and the logic, not the car's latencies. This procedure checks that the order is right, records the real latencies next to the twin's, and sets the twin's Orin limits from the dev kit.

Whether the device models add the measured latencies, or stay ideal, is TBD. Until that's decided, step 2's measurements are recorded but update nothing.

## Steps

1. **Order of events.** On R5 + R1, run the e-stop, heartbeat-cut and any-corner-stop scenarios, with every event timestamped by the sync MCU and the Orin. Run the same scenarios in the twin. Compare the sequence of events: the trigger, its detection, each state change, each message on the sync link, the first brake command.
2. **Latencies.** On R5, the per-sensor latencies of SYS-29 and `budgets/latency.csv`, and the sample timing of each stream. Record the twin's for the same streams.
3. **Orin load.** On the dev kit, run the platform stack under the classical-stack load of SYS-08 (R8's tools for extra load). Record each node's CPU, memory and rate. Run the same stack in the twin with its container limits (ADR-0016, LIM-09) and record the same.
4. **Lockstep step.** Check that every stream's rate is represented at the twin's step (1 ms, TBC, ADR-0049), including streams faster than the step, such as the 4 kHz IMUs (E-20, E-21); how they're batched is TBD.

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Order of events in each scenario | Twin against rig | Exact |
| Per-sensor latency | Recorded; compared only if the device models add latency (TBD) | TBD |
| Node rates and CPU and memory headroom | Twin's container against the dev kit | TBD |
| Sample counts per stream per second | Twin against configured rate | Exact |

## Parameters it updates

| Parameter | Where | Value today | Set from |
| --- | --- | --- | --- |
| The twin's CPU and memory limits for the Orin | Twin's container configuration; TBD | TBD | Step 3 |
| Per-device latency in the models | TBD (if modelled at all) | None | Step 2 |
| Lockstep step | ADR-0049 | 1 ms, TBC | Step 4 checks it; a change is a decision, not a calibration |

## Results

| Step | Rig | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Order of events: e-stop | TBD | TBD | Not run | |
| 1. Order of events: heartbeat cut | TBD | TBD | Not run | |
| 1. Order of events: any-corner stop | TBD | TBD | Not run | |
| 2. Latencies | TBD | TBD | Not run | |
| 3. Orin load | TBD | TBD | Not run | |
| 4. Lockstep step | — | TBD | Not run | |
