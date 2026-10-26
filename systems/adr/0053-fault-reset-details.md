# ADR-0053: The fault reset's details: a 3 s check window, a stopped-wheel threshold, the steering check deferred

- **Status:** Accepted (2026-10-25)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-25, ADR-0034, ADR-0038, ADR-0039
- **Amends:** ADR-0039 (the steering check is skipped until a steering driver exists)

## Context

ADR-0039 sets the reset after a fault stop: hold the e-stop button through the motor-bus cut at T, release it, pass the power-on checks, then the operator re-arms. Implementing it on the sync MCU (#148) needs three answers ADR-0039 leaves open:

1. **How long the checks may take.** After the cut the motor controllers restart and need a moment to report again, so the checks can't all pass the instant the button is released.
2. **What "all wheels stopped" means.** The controllers report electrical speed (eRPM), whose estimate is noisy at standstill; the conversion to m/s isn't known yet.
3. **The steering check.** ADR-0038 requires the steering up and its angle checked, but no steering driver exists yet, so the check can't run.

## Options

For how long the checks may take:

| Option | Pros | Cons |
| --- | --- | --- |
| **A. A fixed window after release; if the checks haven't passed by then, the car stays stopped and needs another cycle** | A fault that clears itself later never clears the stop silently | A slow controller start needs a second cycle |
| B. Keep checking until everything passes | No second cycle | An intermittent fault can clear the stop by itself |

## Decision

1. **The check window is 3 s (TBC)** from the release, option A. If the checks haven't all passed by then, the car stays stopped, reports which check failed, and needs another e-stop cycle.
2. **A wheel counts as stopped at |eRPM| ≤ 100 (TBC)**, until the threshold is measured on the motor bench.
3. **The steering check is skipped on every build** until a steering driver exists, with a warning each time the checks pass. Re-enable it with the driver (ADR-0038).

## Consequences

- **The press must last at least T,** measured on the sync MCU's e-stop status input. A shorter press doesn't reset the stop.
- **Until the steering driver exists,** a car can be re-armed without its steering being checked. The steering driver's issue carries re-enabling the check.
- **For the safety reviewer:** the implementation doesn't count a corner reporting the e-stop toward ADR-0039's "three stops since power-on", because an operator's press isn't a fault. A corner still reporting the e-stop after the release is an e-stop wiring fault (ADR-0035) and needs a restart.
- **The twin** starts every run latched, like the car, so each scenario begins with the reset.
- **Reopen if:** the controllers take longer than the window to restart (then lengthen it), or the stopped threshold proves too loose or too tight on the bench.
