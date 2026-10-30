# ros

The ROS 2 workspace for the car's Jetson Orin (the Orin Nano Super developer kit on v1.0, the Orin NX on v1.1; ADR-0046): the platform software (drivers, time sync, logging, teleop, choosing between the operator and the experiment). State estimation and control belong to experiments (ADR-0048). The same workspace runs on a laptop or in CI against a simulator or a log replay (ADR-0028).

Platform: JetPack 7.2.1, Ubuntu 24.04, ROS 2 Jazzy, PREEMPT_RT (ADR-0016). Experiments run in containers with resource limits and must never stop the platform from driving, logging or stopping (SYS-21).

## Targets (ADR-0028)

The platform nodes are the same on every target; a **backend** implements the car interface (`arc_msgs`) for each one:

```bash
ros2 launch arc_bringup platform.launch.py target:=sim sim:=gym   # or gazebo, webots (ADR-0032); sensors:=lidar|stereo|all
ros2 launch arc_bringup platform.launch.py target:=replay bag:=<log> rate:=1.0   # play back an MCAP log (below)
ros2 launch arc_bringup platform.launch.py target:=car      # the sync MCU over the sync link (ICD-sync-link)
```

Defaults: `target:=sim sim:=gym sensors:=lidar car:=twin`, so a bare `ros2 launch arc_bringup platform.launch.py` runs the gym with the twin's car. `sim:=` only matters with `target:=sim`; `sensors:=` picks what a simulator renders, and the replay and car backends ignore it.

## What a backend provides

`arc_bringup/launch/platform.launch.py` finds the backend by name and includes its launch file, so every backend package follows the same shape:

| Item | Convention |
| --- | --- |
| Package name | `arc_backend_<target>` (`replay`, `car`), or `arc_backend_sim_<sim>` for a simulator |
| Launch file | `launch/backend.launch.py`, installed to the package's `share/`. It declares a `sensors` argument, which the platform always passes (a simulator renders that profile; the others ignore it) |
| Executable | `backend` |
| Car interface | Publishes and subscribes to `arc_msgs` only, so no platform node knows which backend is running (ADR-0028) |
| Launch test | `arc_bringup/CMakeLists.txt` starts the platform once per backend and waits for the backend's startup line (`<target> backend: …`, or `sim (<sim>) backend: …`); update `test/test_target.py` when a backend's startup line changes |

A new simulator also needs its name in `SIMS` in `platform.launch.py` and in the test loop in `arc_bringup/CMakeLists.txt`.

## Log replay

`target:=replay` plays a log through the car interface in place of the car (`arc_backend_replay`, #86). The platform nodes run live on top of it.

```bash
ros2 launch arc_bringup platform.launch.py target:=replay bag:=run.mcap             # as recorded
ros2 launch arc_bringup platform.launch.py target:=replay bag:=run.mcap rate:=0.5   # half speed
```

- **Logs** (`bag:=`) are plain rosbag2 MCAP, a `.mcap` file or a bag directory, with the topic names of ICD-ros2-msgs as recorded with no namespace. Nothing else is needed: no run record, no `arc_logging` code.
- **What's played:** only what the car publishes (ICD-ros2-msgs, "Measurements" and "The car's state"): `/sensors/…`, `/car/motor/…`, `/car/steering`, `/car/power`, `/car/safety_state`, `/car/faults`, `/car/link` and `/diagnostics`, with the log's stamps and timing. The platform's own topics in the log (`/car/cmd/…`, `/control/…`, `/estimate/…`, `/tf`) aren't: the live nodes publish them.
- **Topics the log doesn't have are named at the start,** in one warning: the car's topics (ICD-ros2-msgs' Side A) with no messages in the log. Nothing is published on them, so a node waiting on one gets nothing. A log leaves topics out on purpose (camera images in a log recorded without them, #152; `log_exclude:=`), or a sensor was down when it was recorded.
- **`/clock`** comes from the player, on the log's time (40 Hz, rosbag2's default for `ros2 bag play --clock`), and stops at the end of the log. The platform nodes don't run with `use_sim_time` yet, so they still read the system clock.
- **Commands to the car are ignored:** the backend subscribes to `/car/cmd/…` as the car backend does, logs the first command on each topic and the totals when it stops, and drops them. A log can't be steered.
- **`start_paused:=true`** waits for `/replay_player/resume` (rosbag2's player services), so subscribers can join before the first message.
- **The sample log,** `arc_backend_replay/test/data/sample.mcap` (29 KB, most of it the message definitions MCAP carries), is one second of made-up data written by [`make_sample_log.py`](src/arc_backend_replay/scripts/make_sample_log.py). The same script and rosbag2 give the same bytes; rerun it, with the workspace sourced, after changing it or the messages.

## Packages

Scaffolding: every package builds and starts; apart from the car backend's sync-link handshake and `arc_control`, nothing is implemented yet.

| Package | Job | State | Traces to |
| --- | --- | --- | --- |
| `arc_msgs` | The car interface: the messages and services in ICD-ros2-msgs that have no standard type | Messages defined (ICD rev B, reviewed, #84); `arc_control` uses the drive command and the control mode, nothing else publishes them yet. A test in `arc_backend_car` checks their constants against the sync link's | ADR-0028, ADR-0047 |
| `arc_bringup` | Launch files and parameters; `target:=`, `sim:=`, `sensors:=`, `car:=` | Picks the backend and the car configuration; a launch test per backend | ADR-0028, ADR-0032 |
| `arc_control` | Chooses who drives: the operator's teleop or the experiment's vehicle command, by its mode (`/control/mode`, switched with `/control/set_mode`); a silent source is replaced by the zero command. The envelope itself is on the sync MCU | The rules in ICD-ros2-msgs ("Choosing who drives"), with their TBC values as parameters; started by the platform launch; unit tests on made-up times (with the sanitizers) and a test on a ROS graph | SYS-21, SYS-22, ADR-0048 |
| `arc_car_config` | Loads a car's configuration ([`config/cars/`](../config/cars/README.md)), checks it, hashes it and works out the top speed | Loader and checks; nothing reads the values yet | ADR-0047, ADR-0051 |
| `arc_backend_sim_gazebo` | Gazebo Harmonic: full sensor suite, event cameras; Linux with a GPU | Stub | ADR-0032 |
| `arc_backend_sim_webots` | Webots on the host's GPU (macOS, Windows, Linux), ROS in the container | Stub | ADR-0032 |
| `arc_backend_sim_gym` | F1TENTH gym: 2D and fast; planning, control, CI | Stub | ADR-0032 |
| `arc_backend_replay` | Log-replay backend: plays a rosbag2 MCAP log back as the car, as recorded or faster or slower | The car's topics from the log, with `/clock`; commands ignored; the car's topics a log doesn't have are named in a warning. A launch test replays a sample log at three rates (see [Log replay](#log-replay)) | ADR-0028, SYS-24 |
| `arc_backend_car` | The sync bridge: the Orin's end of the sync link to the sync MCU, and the driver process SYS-29 is measured at. Motor-controller and steering data reach the Orin only through it (ADR-0034) | Exchanges `ARC_LINK_STATUS` with the sync MCU over UDP (version logged, not yet checked; gap counters not implemented); a round-trip test of the generated code; the car interface itself is a stub | ADR-0021, ADR-0024, ADR-0031, SYS-29 |

Still to come:

| Package | Job | Traces to |
| --- | --- | --- |
| `arc_drivers` | LiDAR and camera drivers (the Orin has no CAN: motor-controller and steering data come through `arc_backend_car`) | ADR-0001, ADR-0021 |
| `arc_safety` | Forwards the operator's heartbeat to the sync MCU; the watchdog and the safety envelope run on the sync MCU, not here | SYS-04, SYS-22 |
| `arc_logging` | Time-aligned logs of every sensor, command and operator input | SYS-09, SYS-23, SYS-24 |
| Teleop (package TBD) | Operator driving from the laptop | ADR-0015 |
| `arc_description` | One URDF of the car, the source for every simulator's model | ADR-0032 |

## Build and test

In the dev container (ROS 2 Jazzy is in the image):

```bash
npx nx build ros     # colcon build, into ros/build and ros/install
npx nx test ros      # colcon test: a launch test per backend, the car backend's sync-link round trip and arc_control's rules (gtest)
npx nx check ros     # the generated sync-link code matches systems/icd/sync-link.xml
npx nx test firmware-in-loop   # the native_sim sync MCU firmware and this car backend link up over UDP
source ros/install/setup.bash
```

`arc_backend_car/include/arc_mavlink/` is generated from `systems/icd/sync-link.xml` (ADR-0031) by `tools/gen-interfaces.sh`: never edit it by hand. The car backend's parameters `sync_mcu_address` (default `127.0.0.1`, the `native_sim` firmware) and `port_rx`/`port_tx` (52001/52000, ICD-sync-link) aren't exposed through `platform.launch.py` yet, so `target:=car` only reaches a sync MCU on the same machine.
