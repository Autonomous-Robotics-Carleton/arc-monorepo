# ADR-0038: Losing steering stops the car, and the car doesn't drive without it

- **Status:** Accepted
- **Date:** 2026-10-08
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-19, SYS-25, ADR-0019, ADR-0035, ADR-0036, RSK-03
- **Amends:** ADR-0019 (what the sync MCU does when steering fails)

## Context

ADR-0019 says what the steering does when the sync MCU fails: the moteus-c1 holds its position. It doesn't say what the car does when the steering fails: the steering bus goes down (RSK-03), the moteus-c1 faults, or the steering angle can't be trusted. SYS-25 stops the car when one drive corner fails (ADR-0035) but doesn't cover the steering. A car that keeps driving with its steering frozen or unknown goes wherever it was pointed.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Steering failure triggers the SYS-25 stop, and drive is refused until steering is healthy** | One stop path for every actuator failure; uses telemetry the sync MCU already receives | One more safety function on the sync MCU |
| B. Log it and leave stopping to the operator | Nothing to build | Depends on the operator noticing; fails at the heartbeat's limits too |

## Criteria

1. A car that can't steer must not drive.
2. Every stop uses the same ramp (ADR-0012).

## Decision

**Option A.**

| Item | Decision |
| --- | --- |
| Triggers | The sync MCU stops all four corners (SYS-25) when: no valid steering reply has arrived for 5 ms (TBC); the moteus-c1 reports a fault; the steering bus goes bus-off; or the steering angle can't be trusted (a knuckle encoder fails, or the knuckle encoders and the moteus-c1 disagree by more than ~3° for ≥ 20 ms, both TBC) |
| Angle check | Each knuckle encoder is compared with the angle the moteus-c1's output angle predicts for that wheel, from a linkage table measured once on the car (lock to lock; the two wheels differ by Ackermann geometry and toe change). Samples are matched by their sync-MCU timestamps. An encoder hardware error (magnet field out of range, parity, a failed read) trips at once |
| Interlock | The sync MCU refuses drive commands until the steering bus is up, the moteus-c1 has no fault, and its output angle has been set from the knuckle encoders and checked (ADR-0019) |
| Steering during the stop | As ADR-0019: return to centre if the moteus-c1 is still responding, otherwise it holds position (or does nothing, if it has failed) |

## Consequences

- SYS-25 covers the steering as well as the four corners.
- RSK-03's worst case becomes a controlled stop, not a car driving with frozen steering.
- The steering triggers and the interlock are safety functions: their own tests and a second reviewer.
- **The ~3° / 20 ms threshold needs a second look** once the steering is built (E-24, E-31). Normal error is estimated at ~1–2° (encoder accuracy, ≤ 0.1° belt backlash, ~0.5–1° linkage slop, ~0.5–1° bump steer); a belt skipping one tooth shifts the output by 360° ÷ the output pulley's teeth (~6° with 60). Set it from the differences logged on the bench and the track: ~1.5–2× the worst normal case, and below half a tooth skip.
- **Reopen if:** false steering-loss stops show up on the track.
