# ICD-ros2-msgs

- **Revision:** A (2026-10-26). Messages in [`ros/src/arc_msgs/`](../../ros/src/arc_msgs/).
- **Status:** Draft. Topic names, message types and QoS are proposals (TBC) until both sides sign off (#84). Owners: Shrikar Vempati on both sides until reassigned.
- **Side A:** what publishes the car's measurements and state: the car backend (`arc_backend_car`, the Orin's end of the sync link) and the LiDAR and camera drivers (`arc_drivers`) (owner: Shrikar Vempati, until reassigned)
- **Side B:** what uses them and commands the car: the platform nodes (`arc_safety`, `arc_control`, `arc_logging`, car state and health), experiments, and the ground station through its bridge (owner: Shrikar Vempati, until reassigned)
- **Traces to:** SYS-04, SYS-06, SYS-07, SYS-09, SYS-10, SYS-19, SYS-22, SYS-23, SYS-24, SYS-29, ADR-0021, ADR-0028, ADR-0047, ADR-0048, ADR-0049, ADR-0052

The car interface: the ROS 2 topics and services between the code that talks to the car's devices and everything above it (ADR-0028). No node above it knows which target it's on.

Since ADR-0047 the same car backend and drivers run on every target, with the devices below them simulated, on a bench or real. The two shortcuts publish these same topics directly: replay plays them back from a log, and interface mode has a simulator publish them (ADR-0047). Below this interface, the sync link carries the same data in MAVLink ([sync-link](sync-link.md)).

## Conventions

| Item | Rule |
| --- | --- |
| Timestamps | `header.stamp` is when the data was **sampled**, never when it arrived (SYS-07). The car backend maps each sync-link `time_ns` onto ROS time: on the car, the Orin's clock follows the sync MCU's (ADR-0021, #165); in the twin, ROS time is the simulator's clock (ADR-0049). LiDAR scans are stamped on arrival on the Orin's clock (≤ 1 ms, ADR-0021); camera frames take the trigger time of the same frame number (ADR-0021). Commands and operator inputs are stamped when they were computed (SYS-23) |
| Time source | Every node reads time from the ROS clock, never the system clock (ADR-0049) |
| Units and axes | SI units and REP-103 axes: x forward, y left, z up; positive steering turns left. One exception: `MotorStatus.erpm` is the controller's own unit, electrical rpm. It stays; wheel speed in m/s comes as a field of its own once the car configuration supplies the conversion (#143) |
| Frames | REP-105. `base_link` is fixed to the chassis, at the centre of the rear axle at ground level (decided 2026-10-26: the rear wheels don't steer, so this point moves along the car's heading, as path-following controllers assume). Each sensor has its own frame, named in the tables below; their positions come from the car description and the car configuration (#143), published as static transforms |
| Enum values | The constants in `arc_msgs` have the same names and values as the sync link's enums in [`sync-link.xml`](sync-link.xml), so the car backend copies them across. A compile-time test in `arc_backend_car` fails if they drift |
| Standard types | Used wherever one fits (`sensor_msgs`, `nav_msgs`, `diagnostic_msgs`); `arc_msgs` holds only what has no standard type |
| One topic per sensor | A sensor's topic name says which sensor it is, and its frame, so messages don't carry a sensor index |
| Topic names | Nodes use relative names (`sensors/imu/cg`), so the whole car can run under a namespace; the tables write them as they appear with none. `/clock`, `/tf`, `/tf_static` and `/diagnostics` are the ROS-wide ones |

### QoS profiles (TBC)

Publishers offer `reliable`, so a subscriber can ask for either reliable or best effort; a best-effort publisher can't serve a reliable subscriber. `arc_logging` subscribes reliably, so every stream reaches the log (SYS-09).

- **History sets how long a subscriber can stall.** With keep last, reliable delivery resends only what is still in the publisher's history, so a subscriber that falls further behind loses samples. Samples keep 100 ms of their stream.
- **State needs it on both sides.** A late subscriber gets the last value only if it also asks for transient local.
- **Drivers are set to publish reliable.** Some LiDAR and camera drivers default to best effort, and a reliable subscriber then receives nothing, with only a warning.

| Profile | Reliability | Durability | History | For |
| --- | --- | --- | --- | --- |
| Samples | reliable | volatile | keep last 100 ms of samples (TBC): 100 at 1 kHz, 4 for the LiDAR; at least 1 | Sensor streams and controller status |
| State | reliable | transient local | keep last 1 | Values a late subscriber needs at once: safety state, control mode, envelope |
| Events | reliable | volatile | keep last 100 (TBC) | Faults: none may be dropped |
| Commands | reliable | volatile | keep last 1 | Latest wins, as on the sync link: an old command is never replayed |

## Measurements: Side A → Side B

### From the sync link (`arc_backend_car`)

| Topic | Type | Frame | Sync-link source | Rate |
| --- | --- | --- | --- | --- |
| `/sensors/imu/cg`, `/sensors/imu/front` | `sensor_msgs/Imu` | `imu_cg`, `imu_front` | `ARC_IMU_SAMPLE` | Every sample (SYS-24; E-20, E-21) |
| `/sensors/angle/wheel_{fl,fr,rl,rr}` | `arc_msgs/AngleSample` | `wheel_{fl,…}` | `ARC_ANGLE_SAMPLE` | Every sample (SYS-24, E-22) |
| `/sensors/angle/suspension_{fl,fr,rl,rr}` | `arc_msgs/AngleSample` | `suspension_{fl,…}` | `ARC_ANGLE_SAMPLE` | Every sample (SYS-24, E-23) |
| `/sensors/angle/knuckle_{fl,fr}` | `arc_msgs/AngleSample` | `knuckle_{fl,fr}` | `ARC_ANGLE_SAMPLE` | Every sample (SYS-24, E-24) |
| `/sensors/ride_height/{fl,fr,rl,rr}`, `/sensors/ride_height/ground_camera` | `sensor_msgs/Range` | `ride_height_{fl,…}` | `ARC_RIDE_HEIGHT_SAMPLE` | Every sample (SYS-24, E-26) |
| `/sensors/camera/triggers` | `arc_msgs/CameraTrigger` | — | `ARC_CAMERA_TRIGGER` | Every frame (ADR-0021) |
| `/car/motor/{fl,fr,rl,rr}` | `arc_msgs/MotorStatus` | — | `ARC_MOTOR_STATUS` | ≥ 1 kHz per corner (ADR-0034) |
| `/car/steering` | `arc_msgs/SteeringStatus` | — | `ARC_STEERING_STATUS` | ≥ 1 kHz (ADR-0019) |
| `/car/power` | `sensor_msgs/BatteryState` | — | `ARC_POWER_SAMPLE` | ≥ 100 Hz (SYS-32) |

- **IMU:** no orientation, so `orientation_covariance[0]` is -1 (the `sensor_msgs/Imu` convention). Covariances come from calibration in the car configuration (#143); until then they're all zero, which means unknown.
- **Angles:** `status` carries the AS5047P's error flags; any bit set means the angle can't be trusted (ADR-0038).
- **Ride height:** a sample with a non-zero sensor status is published with `range` = NaN (REP-117). The code itself isn't carried: a failing sensor also raises an `ARC_FAULT`.
- **Power:** `current` follows `BatteryState`'s sign, negative while discharging, the opposite of `ARC_POWER_SAMPLE`. Rail currents come with ADR-0022.

### From the Orin's drivers (`arc_drivers`)

| Topic | Type | Frame | Rate |
| --- | --- | --- | --- |
| `/sensors/lidar/scan` | `sensor_msgs/LaserScan` | `lidar` | Each scan, 40 Hz (E-10) |
| `/sensors/camera/stereo/{left,right}/image_raw`, `…/camera_info` | `sensor_msgs/Image`, `sensor_msgs/CameraInfo` | `camera_stereo_{left,right}_optical` | Each frame (E-11) |
| `/sensors/camera/ground/image_raw`, `…/camera_info` | `sensor_msgs/Image`, `sensor_msgs/CameraInfo` | `camera_ground_optical` | Each frame (E-25) |

The side and rear cameras (E-12) and the event cameras (E-13) aren't fitted on v1.0 (ADR-0046); their topics come with them.

### Ground velocity

| Topic | Type | Frame | Rate |
| --- | --- | --- | --- |
| `/sensors/ground_velocity` | `geometry_msgs/TwistWithCovarianceStamped` | `base_link` (TBC) | Each ground-camera frame, up to 240 fps on v1.0 (LIM-11) |

SYS-10 asks for ground velocity measured directly, forward and sideways, so it's a measurement, not an experiment's estimate (ADR-0048). It's computed on the Orin from the ground camera (ADR-0014): optical flow, a planar motion fit, the IMU for yaw rate and the ride height for scale. Who writes that node is open (open issue 8).

### The car's state

| Topic | Type | Profile | Source | Rate |
| --- | --- | --- | --- | --- |
| `/car/safety_state` | `arc_msgs/SafetyState` | State | `ARC_SAFETY_STATE` | On change, plus 50 Hz (ICD-sync-link) |
| `/car/faults` | `arc_msgs/Fault` | Events | `ARC_FAULT`, and the car backend's own (`SOURCE_CAR_BACKEND`: protocol mismatch, receive gaps, a command topic's publishers) | On occurrence (SYS-19) |
| `/car/link` | `arc_msgs/LinkStatus` | Samples | `ARC_LINK_STATUS` each way | 1 Hz |
| `/diagnostics` | `diagnostic_msgs/DiagnosticArray` | Samples | Car state and health (#167): one status per corner, the steering, the sync link, the Orin's clock against the sync MCU's (#165), each sensor and each platform node | 1 Hz (TBC) |

### Time and transforms

| Topic | Type | Published by | Notes |
| --- | --- | --- | --- |
| `/clock` | `rosgraph_msgs/Clock` | The simulator in the twin (ADR-0049); `ros2 bag play --clock` in replay | Nodes run with `use_sim_time` there. On the car nothing publishes it: ROS time follows the sync MCU (ADR-0021) |
| `/robot_description` | `std_msgs/String` (State) | `robot_state_publisher`, from `arc_description` | The car's URDF (ADR-0032) |
| `/tf_static` | `tf2_msgs/TFMessage` | `robot_state_publisher` | Where each sensor sits relative to `base_link` |
| `/joint_states` | `sensor_msgs/JointState` | TBD (open issue 10) | Steering, suspension and wheel angles, once they're calibrated (#143) |
| `/tf` | `tf2_msgs/TFMessage` | `robot_state_publisher` for the moving parts, from `/joint_states`; the running experiment for `odom` → `base_link`, if it estimates | |

## Commands: Side B → Side A

The car backend subscribes to these and sends each over the sync link. Nothing else commands the car, and each topic has one publisher, which the car backend enforces (below).

| Topic | Type | Profile | Published by | Sync-link message | Rate |
| --- | --- | --- | --- | --- | --- |
| `/car/cmd/drive` | `arc_msgs/DriveCommand` | Commands | `arc_control` (#151) only | `ARC_DRIVE_COMMAND` | Up to 500 Hz (ICD-sync-link) |
| `/car/cmd/heartbeat` | `arc_msgs/Heartbeat` | Commands | `arc_safety` (#150) only | `ARC_OPERATOR_HEARTBEAT` | 50–100 Hz (ADR-0015) |
| `/car/cmd/rearm` | `arc_msgs/Rearm` | Commands | `arc_safety` only | `ARC_REARM` | On the operator's action |
| `/car/cmd/envelope` | `arc_msgs/Envelope` | State | The session's owner (TBC: the ground station through `arc_safety`) | `ARC_ENVELOPE_SET` | Per session |

The sync MCU confirms the envelope in force, the watchdog and the latch in `/car/safety_state`. Whether a command was applied is read there, not from these topics.

### One publisher per command

ROS lets any process publish on any topic, so the *Published by* column is checked, the way `nx check systems` checks `systems/`.

- The car backend forwards a command topic only while its publisher is the one node named above: never two, never a different one.
- While that's broken it raises a fault (`SOURCE_CAR_BACKEND`, `CODE_COMMAND_PUBLISHERS`, the topic in `detail`) and handles the topic as below, until the topic is back to its one publisher.

| Topic | While it has the wrong publishers |
| --- | --- |
| `/car/cmd/drive` | Sends the zero command `arc_control` sends for a silent source (#151) instead, so the car stops |
| `/car/cmd/heartbeat` | Stops forwarding: the sync MCU's watchdog stops the car (SYS-04), and it stays stopped until the operator re-arms (ADR-0052) |
| `/car/cmd/rearm` | Drops them |
| `/car/cmd/envelope` | Drops them: the envelope in force stays. Until the envelope's publisher is named (open issue 5), only "one" is checked |

- **It catches mistakes, not an attacker:** a stray `ros2 topic pub`, a test node left running, an experiment publishing here instead of on `/control/experiment`. Node names aren't authenticated (open issue 7).
- **A restart stops the car.** While a restarting `arc_control` or `arc_safety` overlaps its old instance, the topic has two publishers.
- **On the bench**, drive commands go through `/control/teleop`, not `ros2 topic pub /car/cmd/drive`.

## Platform topics experiments use

| Topic | Type | Profile | Published by | Read by |
| --- | --- | --- | --- | --- |
| `/control/experiment` | `arc_msgs/DriveCommand` | Commands | The experiment in control (ADR-0048) | `arc_control` |
| `/control/teleop` | `arc_msgs/DriveCommand` | Commands | `arc_safety`, from the operator link (#144) | `arc_control` |
| `/control/mode` | `arc_msgs/ControlMode` | State | `arc_control` | The ground station, logging |
| `/estimate/odom` | `nav_msgs/Odometry` | Samples | The running experiment, if it estimates (optional; `odom` → `base_link`) | The ground station's map (#91), logging |

| Service | Type | Served by | Called by |
| --- | --- | --- | --- |
| `/control/set_mode` | `arc_msgs/SetControlMode` | `arc_control` | The ground station, through `arc_safety` (TBC, #144) |

An experiment reads the measurements above and publishes one `DriveCommand` (ADR-0048). `arc_control` forwards it or the teleop to `/car/cmd/drive`, and replaces a silent source with a zero command (#151). The envelope stays on the sync MCU (SYS-22).

## The messages (`arc_msgs`)

| Message | Fields, beyond `std_msgs/Header header` | Mirrors |
| --- | --- | --- |
| `AngleSample` | `angle` (rad), `status` (bitmask: `STATUS_MAGNET_LOW`, `_MAGNET_HIGH`, `_CORDIC_OVERFLOW`, `_READ_ERROR`, `_NOT_READY`) | `ARC_ANGLE_SAMPLE`, `ARC_ANGLE_STATUS` |
| `MotorStatus` | `erpm`, `tachometer`, `current_q`, `current_d`, `current_in` (A), `duty`, `voltage_in` (V), `temp_fet`, `temp_motor` (°C), `fault`, `state` (bitmask: `STATE_ESTOP`, `STATE_COMMAND_TIMEOUT`) | `ARC_MOTOR_STATUS`, `ARC_MOTOR_STATE` |
| `SteeringStatus` | `angle` (rad), `velocity` (rad/s), `torque` (N·m), `temp` (°C), `fault`, `mode` | `ARC_STEERING_STATUS` |
| `CameraTrigger` | `frame`, `camera` | `ARC_CAMERA_TRIGGER` |
| `Envelope` | `max_speed` (m/s), `max_accel` (m/s²), `max_steer` (rad) | `ARC_ENVELOPE_SET` |
| `SafetyState` | `envelope` (`Envelope`), `watchdog` (`WATCHDOG_*`), `estop`, `latch` (`LATCH_*`), `latch_cause` (`CAUSE_*`) | `ARC_SAFETY_STATE` and its enums |
| `Fault` | `source` (`SOURCE_*`), `code`, `detail`; the car backend's own source, codes and command topics (`SOURCE_CAR_BACKEND`, `CODE_*`, `COMMAND_*`) | `ARC_FAULT`, `ARC_FAULT_SOURCE` |
| `LinkStatus` | `peer_protocol_version`, `local_protocol_version`, `peer_rx_gaps`, `peer_rx_bad` (as the sync MCU reports them), `rx_gaps`, `rx_bad` (the car backend's own) | `ARC_LINK_STATUS`, both directions |
| `DriveCommand` | `speed` (m/s), `accel` (m/s², the limit for reaching it), `steer` (rad) | `ARC_DRIVE_COMMAND` |
| `Heartbeat` | `counter` | `ARC_OPERATOR_HEARTBEAT` |
| `Rearm` | `counter` | `ARC_REARM` |
| `ControlMode` | `mode` (`MODE_OPERATOR`, `MODE_EXPERIMENT`) | — |
| `SetControlMode` (service) | request `mode`; response `accepted`, `reason` | — |

## From the simulator branch

`yufeng/simulator` and `yufeng/ground-station` already drive a simulated car on their own topics. Nothing there has to change now. When that work moves onto the platform, these are its counterparts:

| There | Here |
| --- | --- |
| `/sim/auto/cmd_vel` (`geometry_msgs/Twist`), what an algorithm sends | `/control/experiment` (`DriveCommand`): the same speed, and a steering angle instead of a yaw rate (yaw rate = speed × tan(steer) / wheelbase) |
| UDP teleop (speed, steer) into `car_link` | `/control/teleop`, from `arc_safety` (#150, #144) |
| `car_link`'s heartbeat gate and teleop timeout | `arc_safety` (#150), `arc_control` (#151) and the sync MCU's watchdog |
| `/sim/link/status` (JSON) | `/car/safety_state` and `/car/link` |
| `/sim/odom` | `/estimate/odom` from an experiment; wheel and IMU data come through the measurement topics |
| `/sim/ground_truth` | Twin-only, on the simulator's own control interface (ADR-0047); never part of the car interface |

## Open issues

1. Sign-off of the topic names, types and QoS profiles by both sides (#84).
2. High-rate streams: two IMUs, ten angle sensors and five controllers, one message per sample, could cost the Orin a lot of CPU (SYS-24's 50% margin). Measure on the dev kit (rig R5); if it's too much, the options are batching samples into one message per period, or best-effort delivery for subscribers that can skip samples.
3. Every sensor frame's position relative to `base_link`: from the car description and the car configuration (#143).
4. Wheel speed in m/s, as a field beside `erpm`, and wheel angles in the car's frame, once the car configuration supplies the gearing, pole pairs and calibration (#143).
5. Who sets the session's envelope, and how the ground station calls `/control/set_mode`: with the operator-link ICD (#144).
6. What the dashboard shows beyond the estimate, such as a map or a planned path (#91). Those are experiment outputs (ADR-0048); a shared convention for them is for later.
7. Node names aren't authenticated, and any laptop on the team network running ROS in the car's domain can see and publish the car's topics. Limiting discovery to the Orin (`ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST`) would close that, at the cost of running `ros2` tools from a laptop; SROS2 would be real access control.
8. Ground velocity (SYS-10, ADR-0014): who writes the node, its frame, and whether the ground camera's raw frames, up to 240 fps, go on the ROS graph at all or only into that node (SYS-24's 50% margin).
9. When drive commands stop: ICD-sync-link doesn't say what the sync MCU does when `ARC_DRIVE_COMMAND` stops arriving while heartbeats still do. The latest command wins, so if `arc_control` dies the car could hold its last speed. Either the sync MCU gets a timeout for it (ICD-sync-link), or the car backend sends a zero command after a silence (TBC).
10. Who publishes `/joint_states`, once calibrated angles exist (#143).
11. Every log records the car configuration it ran with (#143), so a run can be read later.
