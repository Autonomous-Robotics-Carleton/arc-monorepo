# arc_backend_sim_gym

The [F1TENTH gym](https://github.com/f1tenth/f1tenth_gym) behind the car interface ([ICD-ros2-msgs](../../../systems/icd/ros2-msgs.md)): a 2D single-track car on an occupancy-grid track, for fast control experiments and CI (ADR-0032).

```bash
ros2 launch arc_bringup platform.launch.py target:=sim sim:=gym   # the platform on the gym (the default)
ros2 launch arc_backend_sim_gym backend.launch.py real_time_factor:=0   # the gym alone, as fast as it runs
```

It runs in **interface mode** (ADR-0047), a shortcut: the gym drives the car interface directly, with no car backend, sync MCU firmware or motor-controller models below it. So nothing enforces the safety envelope or the heartbeat watchdog here. Interface mode is meant to compile the firmware's `safety.c` in, and that waits until `safety.c` is implemented: today it's a fail-safe stub that allows no motion. Its startup line says it's a shortcut. A requirement is never verified on it alone.

## The car interface

| Direction | Topic | What the gym supplies |
| --- | --- | --- |
| In | `car/cmd/drive` | `arc_control`'s command, as on the car. The latest wins and is held, as on the sync link. `speed` is reached at no more than `accel` (capped at `accel_max`; zero or less holds the speed), `steer` at the steering's rate limit. A command with a NaN or infinite field is dropped |
| Out | `sensors/lidar/scan` | E-10's geometry: 1081 beams over 270°, 40 Hz, 10 m; beyond range is `+inf` (REP-117) |
| Out | `car/motor/{fl,fr,rl,rr}` | `erpm` from each wheel's rolling speed through the axle's gearing and the pole pairs (`config/cars/`); `tachometer` at 6 steps per electrical revolution. The rest isn't modelled: NaN, `fault` and `state` 0 |
| Out | `car/steering` | `angle` and `velocity` of the single-track model's steering; `mode` 10 (position). Torque and temperature NaN |
| Out | `sensors/imu/cg` | Yaw rate, and acceleration at the centre of gravity with gravity (+z). No orientation |
| Out | `sensors/angle/wheel_{fl,fr,rl,rr}` | Each wheel's angle, integrated from its rolling speed, in [0, 2π) |
| Out | `/clock` | The simulator's time. It starts at the wall clock's time when the backend starts, so nodes not on `use_sim_time` agree with it at a real-time factor of 1 |

Every stamp is the simulator's time at the sample. Not published, because the gym doesn't model them: suspension, knuckle and ride-height sensors, the front IMU, cameras and camera triggers, power, `car/safety_state`, `car/faults`, `car/link`. No `/tf`: the frames' positions come from the car's URDF (#105).

**Sim only**, never part of the car interface (simulation.md, Simulator controls):

| Name | Type | What |
| --- | --- | --- |
| `sim/ground_truth` | `nav_msgs/Odometry` | `base_link` in the track's `map` frame, and its velocity |
| `sim/collision` | `std_msgs/Bool` (transient local) | Whether the car is against a wall; the gym stops it there |
| `sim/reset` | `std_srvs/Trigger` | Back to the start pose, stopped, and the held command cleared |

## Where the numbers come from

From the car configuration (`car:=`, default `twin`, read with `arc_car_config`): the wheelbase and tracks, the tyre diameter, the gearing and pole pairs, the steering range, and the top speed (`top_speed()`), which is the gym's speed limit both ways.

From `systems/`: the LiDAR (E-10), and the steering rate: lock to lock in 0.1 s (ADR-0019), the slowest the requirement allows.

**TBC**, all node parameters: nothing records them yet, so they're the gym's defaults for its F1TENTH Traxxas car.

| Parameter | Value | Replace when |
| --- | --- | --- |
| `mass` | 3.74 kg | `budgets/mass.csv` has a total |
| `yaw_inertia` | 0.04712 kg·m² | The CAD has the mass properties (#105) |
| `cg_height` | 0.074 m | The same |
| `cg_from_rear_axle` | 0.5192 of the wheelbase | The same |
| `friction` | 1.0489 | A system-identification run on the floor (simulation.md, Fidelity) |
| `cornering_stiffness_front`, `_rear` | 4.718, 5.4562 /rad | The same |
| `accel_max` | 9.51 m/s² | The drivetrain is characterised (`tests/a50s-arrival.md`) |
| `v_switch` | 7.319 m/s | The same |
| `length`, `width` | 0.58 m, 0.31 m (collision outline, inside SYS-02) | The CAD block layout (#103) |

Simulator settings, also TBC:

| Parameter | Value | Why |
| --- | --- | --- |
| `timestep` | 1 ms | simulation.md's step |
| `sample_period` | 5 ms | 200 Hz for the sample topics, not the car's ≥ 1 kHz, so Python keeps up with real time |
| `lidar_noise` | 0.01 m | The gym's default |
| `lidar_x` | 0 m (at `base_link`) | TBD: the block layout (#103) places the LiDAR |
| `model` | `st` | The gym's single-track model (`ks` is its kinematic one) |

Also: `map` (`oval`, or a map_server YAML file), the oval's `oval_straight`, `oval_radius` and `oval_width` (10 m, 3 m, 2 m: a made-up test track, not the venue), `start_pose`, and `real_time_factor` (1; 0 for as fast as it runs).

**Simplifications:** both front wheels take the single-track steering angle (the linkage isn't designed); no longitudinal slip; every corner's eRPM is positive going forward (each controller's own sign isn't recorded).

## The gym

Installed in the dev container from git at a fixed commit of its `v1.0.0` branch. What it needs beyond the image's own Python packages is pinned in [`.devcontainer/requirements-gym.txt`](../../../.devcontainer/requirements-gym.txt), all with wheels for amd64 and arm64 (ADR-0050). numpy, OpenCV and the rest come from the image and are never changed by the gym's install. This package uses the gym's `Simulator` rather than its gymnasium environment, which adds lap counting, raceline-based start poses and a renderer. The renderer is the only part that imports OpenCV, so the environment doesn't start in the dev container while the image's OpenCV can't load (`libGL.so.1` is missing); this backend doesn't need it. The first start compiles the gym's numba functions, which takes about 10 s without a cache.

## Tests

`npx nx test ros` runs them:

- `test_vehicle`: the parameters and wheel kinematics, without the gym
- `test_sim`: the car in the gym, both models: speed, acceleration limit, steering, the LiDAR, a wall
- `test_drive`: closed loop. An experiment in the test reads the LiDAR, steers between the walls and asks `arc_control` for control. The car starts off the centreline heading for a wall, and must drive 3 m, centred, without touching one
