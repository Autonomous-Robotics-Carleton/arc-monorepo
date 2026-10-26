# ADR-0052: The envelope's defaults, reverse, and a heartbeat stop held until the operator re-arms

- **Status:** Accepted (2026-10-25)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-22, ADR-0012, ADR-0019, ADR-0039, ADR-0048
- **Amends:** ADR-0039 (the operator's re-arm also ends a heartbeat stop, without an e-stop cycle)

## Context

The sync MCU's envelope and watchdog (#75) need three answers that nothing recorded:

1. **The envelope's defaults.** SYS-22 sets the default speed (3 m/s) but not the acceleration or steering limits. Since ADR-0048, an experiment's command is a speed, an acceleration limit and a steering angle, so each needs a default.
2. **Reverse.** Whether a negative speed is allowed, and under which limit.
3. **What happens after a heartbeat stop.** SYS-04 says how the car stops when the operator's heartbeat is lost, not how it starts again. A Wi-Fi blip is common. Experiments keep streaming commands, so if the car resumed as soon as heartbeats returned, it would drive off by itself after every blip. ADR-0039 already rejects that for fault stops: a stop must never restart motion by itself.

## Options

For the heartbeat stop:

| Option | Pros | Cons |
| --- | --- | --- |
| A. Resume when heartbeats and commands return | No action after a blip | The car moves off by itself, possibly with someone walking to it |
| **B. Stay stopped until the heartbeat is back and the operator re-arms from the laptop** | A person decides every restart; no walk to the car | One press after every blip |
| C. Latch like a SYS-25 fault: e-stop cycle on the car, then re-arm | The car is inspected every time | A walk to the car after every blip, with nothing to inspect |

## Decision

1. **Default envelope for a new experiment:**
   - speed 3 m/s (SYS-22)
   - acceleration 5 m/s² (TBC), the deceleration of the brake ramp (ADR-0012), so speeding up and stopping feel alike
   - steering: the car's full mechanical range (about ±25° at the wheels, TBC, ADR-0019), from the car's configuration (ADR-0051)
2. **Reverse is allowed,** under the same speed limit as forward: the envelope bounds the speed's magnitude.
3. **A heartbeat stop is held (option B).**
   - Once the watchdog trips, the car brakes on the e-stop ramp (SYS-04) and stays stopped.
   - Driving resumes only when heartbeats are arriving again **and** the operator re-arms from the laptop (the re-arm message, ICD-sync-link).
   - No e-stop cycle is needed: a heartbeat stop isn't a fault (SYS-25), and there is nothing on the car to inspect.
   - The sync MCU starts up in this state too, so the first drive after power-on also needs a heartbeat and a re-arm.

## Consequences

- **ADR-0039:** its re-arm still needs an e-stop cycle after a fault stop. After a heartbeat stop alone, the re-arm is accepted once the heartbeat is back. So a re-arm forged from the service port could restart a car stopped by a heartbeat loss; that weighs on who the sync MCU accepts messages from (ICD-sync-link, open issue 6).
- **The watchdog's state** reports a held stop as braking until the operator re-arms; whether the sync link needs a separate "waiting for re-arm" state is part of the re-arm message (ICD-sync-link).
- **The acceleration limit's upper bound** (the car's maximum, the cap for raising it) is TBD until measured; until then the envelope can't be raised above the 5 m/s² default.
- **The defaults and the car's maximums** live in the car's configuration once it exists (ADR-0051).
- **Reopen if:** heartbeat blips turn out frequent enough that a re-arm after each one gets in the way of runs (then fix the link first, ADR-0015).
