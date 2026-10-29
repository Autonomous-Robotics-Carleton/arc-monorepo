# Twin fidelity: network model

- **Owner:** TBD
- **Status:** Not started; the impairment proxy doesn't exist yet (ADR-0049)
- **Calibrates:** the network row of the [fidelity table](../software/simulation.md#fidelity-what-the-twin-can-and-cant-prove)
- **Supports:** SYS-33; the twin's evidence for SYS-04, SYS-11 and SYS-26 to SYS-28. Shares its track measurement with RSK-09's verification (log heartbeat gaps while driving)
- **Rig:** R6 network (Flint 3 router, laptops, traffic generator, packet capture), stage S1; then R6 with R7 at the venue, stage S4
- **Read first:** [verification plan, "Twin fidelity"](../verification/plan.md#twin-fidelity), [ICD-network](../icd/network.md), ADR-0015

## Objective

The twin applies delay and loss to the operator link with a proxy in simulated time (ADR-0049). It claims to show the heartbeat and teleop behaviour under loss and delay, not real 6 GHz behaviour at the venue. This procedure measures the link the car actually has and sets the proxy's delay and loss from it, so the twin's runs see the same gaps.

## Steps

Capture the operator UDP stream (teleop and heartbeat, 50–100 Hz, sequence numbers, ADR-0015) at both ends. One-way delay needs both ends on one clock; the method is TBD (otherwise half the round trip).

1. **Bench baseline.** On R6, with the car's Wi-Fi card (or the dev kit's) near the router: delay, jitter and loss with no other traffic.
2. **At the venue, parked.** Router mounted as at the track. The car parked at TBD positions, including the farthest and any behind obstacles. Same measurements.
3. **At the venue, driving.** Drive the track for TBD minutes. Record delay, loss and the length of every gap in the heartbeat; record where on the track each long gap happened.
4. **Bulk transfer while parked.** Run a bulk download (SYS-28) and repeat step 2's measurement of the control stream.
5. **In the twin.** Run the step 3 scenario with the proxy set from the measurement. Record the same quantities and the watchdog stops (SYS-04).

## What "matches" means

| Quantity | Comparison | Tolerance |
| --- | --- | --- |
| Delay percentiles (p50, p95, p99) | Twin against venue | TBD |
| Loss rate and the distribution of gap lengths | Twin against venue | TBD |
| Watchdog stops per hour of driving | Twin against venue | TBD |
| Control-stream delay during bulk transfer | Twin against venue | TBD |

## Parameters it updates

| Parameter | Where | Value today | Set from |
| --- | --- | --- | --- |
| Delay distribution on the operator link | Impairment proxy; TBD | TBD | Steps 1–4 |
| Loss model (independent losses or bursts) and its parameters | Impairment proxy; TBD | TBD | Step 3 |
| One profile per venue, or one for all | TBD | TBD | Steps 2–3 at each venue |

## Results

| Step | Measured | Twin | Match? | Parameter change (PR) |
| --- | --- | --- | --- | --- |
| 1. Bench baseline | TBD | TBD | Not run | |
| 2. Venue, parked | TBD | TBD | Not run | |
| 3. Venue, driving | TBD | TBD | Not run | |
| 4. Bulk transfer | TBD | TBD | Not run | |
