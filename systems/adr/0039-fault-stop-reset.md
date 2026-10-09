# ADR-0039: A fault stop latches, and only an e-stop cycle on the car resets it

- **Status:** Accepted
- **Date:** 2026-10-08
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-18, SYS-19, SYS-22, SYS-25, ADR-0012, ADR-0035, ADR-0038
- **Amends:** ADR-0035 (how a run resumes after a SYS-25 stop)

## Context

SYS-25 stops the car when a corner, the command bus or the steering fails (ADR-0035, ADR-0038). Nothing said how driving resumes. Resuming on its own after a fault is unsafe: the cause may still be there, and nobody has looked at the car.

A full restart (power off and on) gets the car to a known state, but:

- **It's slow:** the Orin takes a minute or more to boot, so one corrupted burst on a link costs the session.
- **It can lose the record of the fault:** a log still being written, or the fault held in the sync MCU's memory (SYS-18).
- **It doesn't prove the cause is gone:** a loose connector passes the boot checks and fails again on the next run.

Industrial machines use a latched stop with a deliberate manual reset, where the reset itself doesn't restart motion (IEC 60204-1).

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Full restart: power off and on | Simple; a known state | Slow; can lose the fault record; doesn't check the cause |
| **B. Latched stop; reset by cycling the e-stop button on the car, then power-on checks, then the operator re-arms from the laptop** | Physical, so no software can reset the car; restarts only the drive side; checks the cause before driving; keeps the logs | A reset sequence to implement on the sync MCU |
| C. Latched stop; the operator re-arms from the laptop | No walk to the car | Software alone can reset the car (a policy on the Orin, a stuck ground-station control) |

## Criteria

1. No software alone can let the car drive again after a fault.
2. The cause is checked before driving resumes.
3. The fault record survives.

## Decision

**Option B.**

1. **The stop latches.** On a SYS-25 stop the sync MCU saves the cause, refuses drive commands and stays stopped. It also boots into this state, so a power cycle can't skip the reset.
2. **Reset:** press the e-stop button on the car (E-32, latching), hold it at least until the motor-bus cut at T, then release it. The sync MCU sees the button on its e-stop status input. The cut restarts the drive side (the motor controllers); the Orin, the logs and the sync MCU keep running.
3. **Power-on checks on release:** all four corners sending telemetry and CAN status with no state bits or faults (ADR-0035); the steering up and its angle checked (ADR-0038); all wheels stopped. If a check fails, the car stays stopped and reports which one.
4. **The operator re-arms from the laptop.** The first commands after it are zero, and the safety envelope returns to its 3 m/s default (SYS-22).
5. **Faults that need a full restart and an inspection** (a failure record, SYS-18) before the car drives again: three SYS-25 stops since power-on (TBC); a controller reporting overcurrent or overvoltage; an ESTOP wiring fault; a steering-angle disagreement.

## Consequences

- Resetting means walking to the car, so someone looks at it after every fault.
- The sync link needs a re-arm message from the operator, forwarded by the Orin like the heartbeat (ICD-sync-link, TBD). It works only after the e-stop cycle, so a forged re-arm can't drive the car.
- If the controllers' Aux input isn't on an always-on rail, they restart during the cut; the checks run once they're back.
- The latch, the reset sequence and the checks are safety functions: their own tests and a second reviewer.
- **Reopen if:** walking to the car becomes impractical (larger venues), which would mean a remote e-stop with its own reset.
