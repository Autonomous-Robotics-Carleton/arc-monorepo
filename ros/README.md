# ros

The ROS 2 workspace that runs on the car's Jetson Orin NX: the platform software (drivers, time sync, logging, state estimation, low-level control, safety).

Platform: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT (ADR-0016). Experiments run in containers with resource limits and must never stop the platform from driving, logging or stopping (SYS-21).

Nothing is here yet. Packages go in `src/` as `arc_<name>`. Expected first packages:

| Package | Job | Traces to |
| --- | --- | --- |
| `arc_msgs` | Message and service definitions; the ROS-side ICDs point here | `systems/icd/` |
| `arc_bringup` | Launch files and parameters | |
| `arc_drivers` | LiDAR, cameras, VESC/CAN bridge, sync MCU link | ADR-0001, ADR-0011, ADR-0021 |
| `arc_state_estimation` | Vehicle-state estimate | SYS-06 |
| `arc_control` | Low-level control inside the safety envelope | |
| `arc_safety` | Operator heartbeat and watchdog | SYS-04 |
| `arc_logging` | Time-aligned logs of every sensor, command and operator input | SYS-24 |

Build with colcon. Nx targets in `project.json` wrap it once the first package lands.
