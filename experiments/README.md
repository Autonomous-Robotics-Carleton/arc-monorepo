# experiments

Experiments are whatever a team engineer is trying on the car: a state estimator, localization, a planner, a controller, a learned policy, a perception idea. They're separate from the platform software in `ros/`, which the team maintains and which must stay up. The platform provides measurements, not interpretations: estimating, deciding and controlling are the experiment's job (ADR-0048).

## Rules (from the requirements)

- **An experiment can't take the platform down** (SYS-21). It runs in its own container with resource limits; if it crashes, hangs, or uses up CPU, GPU, memory or disk, the platform still drives, logs and stops.
- **An experiment commands speed and steering, never the motors** (SYS-22, ADR-0048). It sends one vehicle command: speed, an acceleration limit and a steering angle. Every command passes through the safety envelope (speed, acceleration and steering limits set per session, 3 m/s by default for new experiments). The sync MCU then splits it into the four wheels' speeds.
- **The experiment drives only while it's given control** (ICD-ros2-msgs, "Choosing who drives"). The ground station hands it control and the operator can take it back at any time. It keeps publishing its command, or the car stops; when `/control/mode` gives it control, it resets its controller, since its commands were dropped until then.
- **Any number of experiments may run** (ADR-0048). How they fit together (which one commands the car, how they share data) is for later.
- **An experiment may run in real time** (ADR-0048): threads at real-time priority on cores kept for experiments, below the platform's priorities.
- **Runs are logged by the platform** (SYS-24), not by the experiment, so a crashed experiment still leaves a complete log.

## Layout

One directory per experiment, copied from [`_template/`](_template/):

```
experiments/<name>/
  README.md     what it tries, who owns it, how to run it
  Dockerfile    its container, on ros:jazzy-ros-base until the platform
                image exists (TBD)
  src/          its code
```

## Open items

Scaffolding only: nothing here runs yet. Still to decide, by the platform owners:

| Item | Value | Traces to |
| --- | --- | --- |
| Container runtime on the Orin | Docker, with NVIDIA's container runtime for the GPU (ADR-0050) | SYS-21 |
| CPU limit per experiment | TBD | SYS-21 |
| GPU share per experiment | TBD | SYS-21 |
| Memory limit per experiment | TBD | SYS-21 |
| Disk quota per experiment | TBD | SYS-21 |
| The vehicle command's topic and message (speed, acceleration limit, steering angle) | `/control/experiment`, `arc_msgs/DriveCommand` (draft, [ICD-ros2-msgs](../systems/icd/ros2-msgs.md)) | SYS-22, ADR-0048 |
| How an experiment gets and keeps control | The ground station hands it control; it publishes at least every 100 ms (TBC) or the car gets the zero command, and loses control after 1 s (TBC); it resets when `/control/mode` gives it control (ICD-ros2-msgs, "Choosing who drives") | SYS-21, ADR-0048 |
| Cores and real-time priorities for experiments, per build | TBD | SYS-21, ADR-0048 |
| The baseline estimator (an example experiment, off the platform's critical path) | TBD | SYS-06, ADR-0048 |
| The platform base image experiments build on | The platform build (ADR-0050); not built yet | |
