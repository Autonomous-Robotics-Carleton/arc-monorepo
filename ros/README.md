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
| `arc_logging` | Records a run's topics to MCAP (rosbag2's MCAP storage): every topic but camera images, or every topic with `log_profile:=full` (ADR-0056), in chunks, from the platform's start to its stop, in its own process so an experiment can't stop it and it can't hold up the platform; reports on `/diagnostics` when a run isn't being logged. See [Logs](#logs) | On by default on `target:=car` only (`log:=` on the platform launch). Warns, without stopping anything, when a car run isn't logged, a `full` run falls back to `minimal`, free space is low, the recorder stops or rosbag2 reports lost messages. `log_exclude:=` leaves topics out (default: none). The run record so far holds the target, simulator, sensors, car, car configuration hash, profile and exclude pattern. Launch tests: a short run in chunks with a crashing experiment, read back; each warning | SYS-09, SYS-21, SYS-23, SYS-24 |
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
| Teleop (package TBD) | Operator driving from the laptop | ADR-0015 |
| `arc_description` | One URDF of the car, the source for every simulator's model | ADR-0032 |

## Logs

`arc_logging` runs `ros2 bag record --all-topics --storage mcap`, minus the run's profile's message types, for the length of the run. Each run gets its own directory, `<log_dir>/<UTC start>_<target>` (e.g. `20261029T140501Z_car`): a standard rosbag2 log, `metadata.yaml` and one `.mcap` per chunk, that `ros2 bag info`, `ros2 bag play` and the replay backend read with nothing of ours.

- **When:** `log:=true|false` on the platform launch. The default is `true` on `target:=car` and `false` on every other target; the platform launch chooses it (ADR-0047), and arc_logging never looks at the target.
- **Where:** `log_dir:=`, else `$ARC_LOG_DIR`. On the car one of them must be set (the Orin's image points `$ARC_LOG_DIR` at the NVMe, #89); elsewhere the fallback is `~/arc_logs`.
- **What:** the run's profile, `log_profile:=` on the platform launch (ADR-0056):
  - `minimal`, the default on every target: every topic except camera images, left out by message type (`sensor_msgs/Image`, `sensor_msgs/CompressedImage`, and image_transport's theora and ffmpeg packets). Camera info and triggers, the LiDAR, every sync-link measurement, commands, the car's state and faults, and an experiment's topics are kept. About ≤ 10 MB/s (TBC).
  - `full`: every topic, images too: about 296 MB/s on v1.0, 178 GB per 10-minute run. On `full` runs a camera driver's image_transport compressed topics are recorded too, and the driver encodes every frame on the CPU for them.

  Profiles choose topics, not rates: images at a lower rate need a decimated topic from the camera driver.
- **How:** every topic with a publisher, found as it appears (an experiment's topics too), with the publisher's QoS: reliable where it's offered (ICD-ros2-msgs), transient local for state. Each message is stamped with when the recorder received it; `header.stamp` still says when it was sampled.
- **Leaving topics out:** `log_exclude:=<regex>` on the platform launch skips every topic whose name contains a match (rosbag2's `--exclude-regex`), on top of the profile, e.g. `log_exclude:='^/car/motor/'`. The default is empty: every topic is recorded. The pattern goes into the run record, so a log says what it left out. A pattern that isn't a valid regex, or that contains `=` (which rosbag2's custom data can't hold), is ignored: the whole run is recorded and the node warns.
- **Chunks:** the log is split every `chunk_s` seconds (TBC: 15), so a crash or power loss loses at most the last chunk.
- **Free space:** a run starts recording only with at least `min_free_gb` free: by default its profile's (TBC: `minimal` 20 GB, three 10-minute runs at its estimated rate; `full` 270 GB, a 10-minute run at v1.0's ~296 MB/s × 1.5). A `full` run without room for it records `minimal` instead, with a warning; a run without room for `minimal` isn't recorded. Logs are never deleted automatically.
- **The run record** (#152): target, simulator, sensors, car, car configuration hash, the profile recorded (`log_profile`) and `log_exclude` (each left out when empty), written as the log's custom data, in `metadata.yaml` and in each `.mcap` file's own `rosbag2` metadata record. `ros2 bag info` doesn't show it yet; `rosbag2_py.Info().read_metadata(run, 'mcap').custom_data` does.
- **Starting:** the run counts as recorded only once the recorder's own output shows rosbag2 recording (its `Recording...` line, with the run's directory created). Until then the status is "starting the recorder: not recording yet", a warning on the car. If the recorder exits first, or hasn't started within `start_timeout_s` (TBC: 5 s), the operator is told the run isn't recorded.
- **Warnings:** the `arc_logging` node publishes its status on `/diagnostics` (name `arc_logging`, 1 Hz) and logs it to the console. It's WARN, and the platform keeps running, when a car run isn't logged (`log:=false`, no log directory, too little free space, or the recorder still starting, failed to start or not started in time), when a `full` run records `minimal` for lack of space, when free space drops below `warn_free_gb` while recording (by default the recorded profile's, TBC: `minimal` 10 GB, `full` 60 GB, about 3 minutes of v1.0 logging), when the recorder stops mid-run, when rosbag2 reports lost messages (it only reports them in its log, which the node reads), or when `log_exclude` was ignored. The ground station and car health (#167) will show it.
- **Stopping:** at shutdown the node forwards SIGINT to the recorder, which writes out its cache and closes the file. The recorder runs in its own session and gets SIGINT from the kernel if the node itself dies.

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
