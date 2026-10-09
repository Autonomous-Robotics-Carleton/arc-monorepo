# ros

The ROS 2 workspace for the car's Jetson Orin NX: the platform software (drivers, time sync, logging, state estimation, low-level control, teleop). The same workspace runs on a laptop or in CI against a simulator or a log replay (ADR-0028).

Platform: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT (ADR-0016). Experiments run in containers with resource limits and must never stop the platform from driving, logging or stopping (SYS-21).

## Targets (ADR-0028)

The platform nodes are the same on every target; a **backend** implements the car interface (`arc_msgs`) for each one:

```bash
ros2 launch arc_bringup platform.launch.py target:=sim sim:=gym   # or gazebo, webots (ADR-0032); sensors:=lidar|stereo|all
ros2 launch arc_bringup platform.launch.py target:=replay   # play back an MCAP log (stub)
ros2 launch arc_bringup platform.launch.py target:=car      # the sync MCU over the sync link (ICD-sync-link)
```

Defaults: `target:=sim sim:=gym sensors:=lidar`, so a bare `ros2 launch arc_bringup platform.launch.py` runs the gym. `sim:=` only matters with `target:=sim`; `sensors:=` picks what a simulator renders, and the replay and car backends ignore it.

## Packages

Scaffolding: every package builds and starts; apart from the car backend's sync-link handshake, nothing is implemented yet.

| Package | Job | State | Traces to |
| --- | --- | --- | --- |
| `arc_msgs` | The car interface: messages and services, implemented by every backend | Placeholder message; ICD ros2-msgs isn't written yet | ADR-0028 |
| `arc_bringup` | Launch files and parameters; `target:=`, `sim:=`, `sensors:=` | Picks the backend; a launch test per backend | ADR-0028, ADR-0032 |
| `arc_backend_sim_gazebo` | Gazebo Harmonic: full sensor suite, event cameras; Linux with a GPU | Stub | ADR-0032 |
| `arc_backend_sim_webots` | Webots on the host's GPU (macOS, Windows, Linux), ROS in the container | Stub | ADR-0032 |
| `arc_backend_sim_gym` | F1TENTH gym: 2D and fast; planning, control, CI | Stub | ADR-0032 |
| `arc_backend_replay` | Log-replay backend | Stub | ADR-0028, SYS-24 |
| `arc_backend_car` | The sync bridge: the Orin's end of the sync link to the sync MCU, and the driver process SYS-29 is measured at. Motor-controller and steering data reach the Orin only through it (ADR-0034) | Exchanges `ARC_LINK_STATUS` with the sync MCU over UDP (version logged, not yet checked; gap counters not implemented); a round-trip test of the generated code; the car interface itself is a stub | ADR-0021, ADR-0024, ADR-0031, SYS-29 |

Still to come:

| Package | Job | Traces to |
| --- | --- | --- |
| `arc_drivers` | LiDAR and camera drivers (the Orin has no CAN: motor-controller and steering data come through `arc_backend_car`) | ADR-0001, ADR-0021 |
| `arc_state_estimation` | Vehicle-state estimate | SYS-06 |
| `arc_control` | Low-level control inside the safety envelope (the envelope itself is on the sync MCU) | SYS-06, SYS-22 |
| `arc_safety` | Forwards the operator's heartbeat to the sync MCU; the watchdog and the safety envelope run on the sync MCU, not here | SYS-04, SYS-22 |
| `arc_logging` | Time-aligned logs of every sensor, command and operator input | SYS-09, SYS-23, SYS-24 |
| Teleop (package TBD) | Operator driving from the laptop | ADR-0015 |
| `arc_description` | One URDF of the car, the source for every simulator's model | ADR-0032 |

## Build and test

In the dev container (ROS 2 Jazzy is in the image):

```bash
npx nx build ros     # colcon build, into ros/build and ros/install
npx nx test ros      # colcon test: a launch test per backend, and the car backend's sync-link round trip (gtest)
npx nx check ros     # the generated sync-link code matches systems/icd/sync-link.xml
npx nx test firmware-in-loop   # the native_sim sync MCU firmware and this car backend link up over UDP
source ros/install/setup.bash
```

`arc_backend_car/include/arc_mavlink/` is generated from `systems/icd/sync-link.xml` (ADR-0031) by `tools/gen-interfaces.sh`: never edit it by hand. The car backend's parameters `sync_mcu_address` (default `127.0.0.1`, the `native_sim` firmware) and `port_rx`/`port_tx` (52001/52000, ICD-sync-link, TBC) aren't exposed through `platform.launch.py` yet, so `target:=car` only reaches a sync MCU on the same machine.
