# ADR-0049: The twin runs in lockstep: the simulator's clock drives the firmware, the device models and the platform

- **Status:** Accepted (2026-10-25)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-22, SYS-25, SYS-33, ADR-0021, ADR-0047
- **Amends:** ADR-0047 (the full stack no longer has to run in real time)

## Context

On the car there is one clock: the sync MCU's. The Orin follows it (ADR-0021).

In the twin (ADR-0047) there are two clocks:
- **The simulator's:** it slows down when physics or rendering can't keep up.
- **The firmware's:** by default, `native_sim` ties it to the wall clock.

If the simulator falls behind, the firmware sees a world in slow motion. Its timers then run against the wrong world, and the safety functions are what get tested wrongly:
- the heartbeat watchdog (SYS-04)
- the brake ramp
- the any-corner stop (SYS-25)
- the telemetry rates

Rendered cameras, a busy laptop or a CI runner without a GPU are enough to cause it.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Everything on the wall clock; every run checks the world kept up, and fails if it didn't | Simplest; nothing to build | Fails on slow machines; runs can't be repeated exactly; heavy runs are unusable |
| B. ROS nodes on the simulator's clock, the firmware on the wall clock | Little work | Two clocks again; the firmware still breaks when the world slows |
| **C. Lockstep: the simulator's clock is the only clock in the twin; the firmware, the device models and the platform all follow it** | Correct on any machine at any speed; the world, firmware and models repeat exactly; a slow machine slows the run instead of breaking it | `native_sim` needs a clock driven from outside; every model steps on command; platform code must read time only through the ROS clock; network delay must be simulated in simulated time |

## Criteria

1. Safety timing tested in the twin means the same as on the car.
2. A run's result doesn't depend on how fast the machine is.
3. The code above the split stays the same on every target (ADR-0047).

## Decision

**Option C.**

1. **The simulator is the time master in the twin.** Each step (1 ms, TBC), it:
   - advances the world
   - grants the same time to the firmware and the device models
   - waits for them to finish before the next step

   PX4's simulator works this way with Gazebo.
2. **The firmware follows that clock.** `native_sim`'s clock advances only when the simulator grants time.
   - This is a component of the `native_sim` board: target-specific code that ADR-0047 allows. The application doesn't change.
   - Zephyr's BabbleSim boards already run this way. Whether `native_sim` can take an outside clock as cleanly is checked with a short spike first (TBC).
3. **Device models step with the simulator:** motor controllers, steering, power and sensors.
4. **ROS nodes run on the simulator's clock.**
   - This covers platform nodes, experiments and the ground station. In the twin, ROS time is the simulator's clock (`use_sim_time`).
   - On the car, ROS time is the Orin's clock, which follows the sync MCU (ADR-0021).
   - Platform code reads time only through the ROS clock, never the system clock, so the same code runs on both. A CI check enforces it.
5. **The ROS side isn't held step by step.** It runs on the simulator's clock, but the simulator doesn't wait for it.
   - **Pacing:** runs are paced to at most real time (real-time factor 1.0, TBC), so the ROS side has at least as much time as on the car.
   - **What's checked:** every run records its real-time factor and the age of sync-link messages in simulated time. A run where the ROS side fell behind is flagged.
   - **Faster runs:** running faster than real time with the Orin in the loop would need the car backend to join the lockstep. That's for later, if wanted. Until then, the `fast` profile covers fast runs.
6. **Network delay and loss are applied in simulated time,** by an impairment proxy on the operator link, not by `tc netem`. `tc netem` counts wall-clock time.
7. **A person driving** runs paced at real time. If the machine can't keep up, they see slow motion, and the run says so.
8. **Repeatability:** the world, the firmware and the models repeat exactly from the same inputs. The ROS side doesn't, because of thread scheduling. So scenario pass criteria don't rely on bit-exact repeats.

## Consequences

- **ADR-0047:**
  - Its consequence "the full stack runs in real time, because the firmware runs on the wall clock" no longer holds.
  - The twin runs at whatever speed the machine allows, up to real time.
  - Its "reopen if the full stack is too slow" becomes less likely.
- **New work** (in [`software/platform-plan.md`](../software/platform-plan.md)):
  - the spike, then the `native_sim` clock component
  - time grants on the sim link
  - stepping in every device model
  - the CI check on time sources
  - the network impairment proxy
- **Heavy runs** (rendered cameras, CI without a GPU) get slower, not invalid.
- **The car is unchanged:** the sync MCU's clock, with the Orin following it (ADR-0021).
- **Reopen if** the spike shows `native_sim` can't follow an outside clock without changes to Zephyr that upstream wouldn't take.
