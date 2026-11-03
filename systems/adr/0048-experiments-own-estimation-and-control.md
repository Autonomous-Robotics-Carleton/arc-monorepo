# ADR-0048: Experiments own estimation and control, command speed and steering, and may run in real time

- **Status:** Accepted (2026-10-25). Set on 2026-10-25: no per-wheel commands for now; experiments send the vehicle command only. Per-wheel speeds come when an experiment needs them; per-wheel torque only with a speed limiter on the sync MCU and its own safety review
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-06, SYS-08, SYS-21, SYS-22, SYS-24, SYS-29, ADR-0016, ADR-0021, ADR-0034
- **Amends:** ADR-0016 (what runs on the platform's reserved real-time cores)

## Context

ADR-0016 puts "drivers, logging, state estimation, MPC" on reserved CPU cores at real-time priority, as platform software. `experiments/README.md` and the software architecture say experiments command "the classical control layer": a platform controller between the experiment and the motors.

But the car exists so the team can try estimation, localization, planning and control (`architecture.md`: "classical pipelines come first (state estimation, localization, planning, MPC), then learned policies"). Putting a team-maintained estimator and controller in the platform means:

- The platform takes on the research problems it's meant to host, and every experiment inherits one estimator's and one controller's choices.
- An experiment that wants its own estimator or controller has to work around the platform's.

Meanwhile nothing on the safety path needs an estimate:
- The envelope clamps commands (SYS-22).
- The watchdog watches the heartbeat (SYS-04).
- The any-corner stop watches telemetry (SYS-25).
- Each motor controller closes its own wheel-speed loop.

The platform can drive, log and stop with no estimator running.

Estimators and controllers do need timing guarantees. SYS-06 asks for an estimate at ≥ 200 Hz, ≤ 5 ms old, and ADR-0016 gives real-time scheduling only to the platform.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. The platform owns estimation and control (ADR-0016 as is); experiments hand it paths or setpoints | Experiments are simpler; one tuned estimator | The platform owns research problems; experiments are boxed into one design; the platform grows |
| **B. Experiments own estimation, localization, planning and control; they command speed and steering; the platform provides measurements, safety, logging and real-time scheduling** | Small, stable platform; nothing a researcher wants to try is locked away; safety doesn't depend on experiment code | Every experiment needs an estimator: a baseline one is written as an example; experiments need real-time access without endangering the platform |
| C. The platform provides a fixed estimator; experiments own control | Experiments get a pose for free | Estimation is itself research; the platform still owns one design |

## Criteria

1. The platform keeps the car safe and recorded whatever an experiment does (SYS-21, SYS-22, SYS-24).
2. Nothing a team member might want to research is fixed inside the platform.
3. An experiment can meet SYS-06-class timing.
4. The platform stays small enough to finish and maintain.

## Decision

**Option B.**

1. **The platform provides measurements, not interpretations:**
   - every sensor, timestamped on the sync MCU's clock (ADR-0021), calibrated, at full rate (SYS-24, SYS-29)
   - the car's state, faults and safety state
   - logging
   - the operator link and the ground station
   - the digital twin
2. **Experiments own estimation, localization, mapping, planning and control**, MPC included.
3. **The experiment command** is the vehicle command `ARC_DRIVE_COMMAND` already carries: speed (m/s), an acceleration limit and a steering angle.
   - It passes through `arc_control`, which only chooses between the operator and the experiment, and then through the safety envelope on the sync MCU.
   - The sync MCU splits it into four wheel speeds from the car's geometry (wheelbase, track width, wheel radius, gearing; from the car configuration). In a turn each wheel follows a different arc.
   - Each motor controller closes its own wheel-speed loop.
   - **Per-wheel commands are open:** four wheel speeds can be clamped like the vehicle command; per-wheel torque needs a speed limiter on the sync MCU, which is a new safety function.
4. **No limit on how many experiments run.** How they fit together is left for later:
   - which of them commands the car
   - how they share data
   - how one person's estimator feeds another's controller

   The platform's job is done once it publishes the measurements and accepts the vehicle command. Whatever runs:
   - every command still passes the envelope (SYS-22)
   - every experiment's container keeps its limits (SYS-21)
5. **Experiments may run in real time.**
   - An experiment's threads may run at real-time priority on cores reserved for experiments, below the platform's priorities and within the experiment's limits.
   - The platform's own reserved cores stay out of an experiment's reach.
   - **If the Orin is starved anyway,** `arc_safety` stops forwarding the heartbeat and the sync MCU stops the car (SYS-04). Safety never depends on the scheduler.
   - Core counts and priorities per build: TBD (v1.0's Orin Nano and v1.1's Orin NX have different core counts).
   - How a container gets real-time scheduling on JetPack's PREEMPT_RT kernel: to check. Docker's real-time options depend on the kernel's configuration.
6. **A baseline estimator is written as an example experiment** in `experiments/`: the first estimation experiment, which others can start from or compare against.
   - It isn't platform software and isn't on the safety path.
   - The platform doesn't wait on it.
   - Once it exists, it's what SYS-06's timing is checked with.

## Consequences

- **ADR-0016:** its reserved real-time cores hold the platform (drivers, the car backend, the operator link, logging), not estimation and MPC; experiments get real-time scheduling on their own cores.
- **`requirements/system.md`, "What this means for the architecture"**, needs rewording if this is accepted. Two bullets conflict with it:
  - "Platform vs experiment software" lists state estimation and low-level control as platform software.
  - "Layered control" has learned policies command "a classical control layer".

  The safety argument behind layered control holds: no policy commands the motors directly, and the safety envelope is the layer no one can override (SYS-22). Whether an experiment puts a classical controller under its learned policy becomes that experiment's choice.
- **SYS-06** now describes what the car supports for an estimator rather than a platform output. Its rationale names the platform's controller, so its wording needs revisiting: systems lead.
- **SYS-08's localization rate** becomes a compute check, run with experiment code.
- **SYS-22 is unchanged.** `experiments/README.md` replaces "the classical control layer" with the vehicle command above.
- **`arc_control` shrinks** to choosing between the operator and the experiment, with a timeout on each. `arc_state_estimation` leaves the platform; the baseline estimator in `experiments/` takes its place as an example.
- **New sync MCU logic:** splitting the vehicle command into wheel speeds. It's plain C, the same on every target, and it reads the geometry from the car configuration.
- **The ground station** shows the raw measurements always, and whatever estimate the running experiment publishes, if any.
- **How experiments fit together** (several running at once, combining parts) is an open question for later, not platform work.
- **Reopen if** most experiments end up needing the same estimator: it could then move into the platform.
