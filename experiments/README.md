# experiments

Experiments are whatever a team engineer is trying on the car: a new planner, a learned policy, a perception idea. They're separate from the platform software in `ros/`, which the team maintains and which must stay up.

## Rules (from the requirements)

- **An experiment can't take the platform down** (SYS-21). It runs in its own container with resource limits; if it crashes, hangs, or uses up CPU, GPU, memory or disk, the platform still drives, logs and stops.
- **An experiment commands the classical control layer, never the motors** (SYS-22). Every command passes through the safety envelope: speed, acceleration and steering limits set per session, 3 m/s by default for new experiments.
- **Runs are logged by the platform** (SYS-24), not by the experiment, so a crashed experiment still leaves a complete log.

## Layout

One directory per experiment, copied from [`_template/`](_template/):

```
experiments/<name>/
  README.md     what it tries, who owns it, how to run it
  Dockerfile    its container, built on the platform's ROS image
  src/          its code
```

## Open items

Scaffolding only: nothing here runs yet. Still to decide, by the platform owners:

| Item | Value | Traces to |
| --- | --- | --- |
| Container runtime on the Orin (Docker, Podman, ...) | TBD | SYS-21 |
| CPU limit per experiment | TBD | SYS-21 |
| GPU share per experiment | TBD | SYS-21 |
| Memory limit per experiment | TBD | SYS-21 |
| Disk quota per experiment | TBD | SYS-21 |
| The classical-layer command interface experiments publish to | TBD (ICD ros2-msgs, not written yet) | SYS-22 |
| The platform base image experiments build on | TBD (ADR-0016's platform image) | |
