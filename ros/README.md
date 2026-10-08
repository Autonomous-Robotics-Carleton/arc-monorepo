# ros

The ROS 2 workspace that runs on the car's Jetson Orin NX: the platform software (drivers, time sync, logging, state estimation, low-level control, safety).

Platform: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT (ADR-0016). Experiments run in containers with resource limits and must never stop the platform from driving, logging or stopping (SYS-21).

## Targets (ADR-0028)

The platform nodes are the same on every target; a **backend** implements the car interface (`arc_msgs`) for each one:

```bash
ros2 launch arc_bringup platform.launch.py target:=sim sim:=gym   # or gazebo, webots (ADR-0032); sensors:=lidar|stereo|all
ros2 launch arc_bringup platform.launch.py target:=replay   # play back an MCAP log
ros2 launch arc_bringup platform.launch.py target:=car      # the sync MCU over the sync link (ICD-sync-link)
```

## Packages

Scaffolding: every package builds, but nothing is implemented yet.

| Package | Job | State | Traces to |
| --- | --- | --- | --- |
| `arc_msgs` | The car interface: messages and services | Placeholder message; the ICD isn't written yet | `systems/icd/` |
| `arc_bringup` | Launch files and parameters; `target:=`, `sim:=`, `sensors:=` | Picks the backend; a launch test per backend | ADR-0028, ADR-0032 |
| `arc_backend_sim_gazebo` | Gazebo Harmonic: full sensor suite, event cameras; Linux with a GPU | Stub | ADR-0032 |
| `arc_backend_sim_webots` | Webots on the host's GPU (macOS, Windows, Linux), ROS in the container | Stub | ADR-0032 |
| `arc_backend_sim_gym` | F1TENTH gym: 2D and fast; planning, control, CI | Stub | ADR-0032 |
| `arc_backend_replay` | Log-replay backend | Stub | SYS-24 |
| `arc_backend_car` | Bridge to the sync MCU | Exchanges `ARC_LINK_STATUS` with the sync MCU over UDP (the generated sync-link code, a round-trip test); the car interface itself is a stub | ADR-0021, ADR-0024, ADR-0031 |

Still to come:

| Package | Job | Traces to |
| --- | --- | --- |
| `arc_drivers` | LiDAR, cameras, VESC/CAN bridge | ADR-0001, ADR-0011 |
| `arc_state_estimation` | Vehicle-state estimate | SYS-06 |
| `arc_control` | Low-level control inside the safety envelope | |
| `arc_safety` | Operator heartbeat and watchdog | SYS-04 |
| `arc_logging` | Time-aligned logs of every sensor, command and operator input | SYS-24 |

## Build and test

In the dev container (ROS 2 Jazzy is in the image):

```bash
npx nx build ros     # colcon build, into ros/build and ros/install
npx nx test ros      # colcon test: the launch tests
source ros/install/setup.bash
```
