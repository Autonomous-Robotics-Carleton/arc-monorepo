# simulator

Gazebo simulator for the car. The same ROS 2 stack runs against it or the real car: the sim stands in for the car's drivers behind the same topics. It runs on a laptop in Docker, never on the car.

ROS 2 Jazzy + Gazebo Harmonic, headless by default, software rendering (Mesa llvmpipe, since Docker on macOS has no GPU).

## Run

```bash
./run.sh            # headless
./run.sh --bridge   # for the ground station: bridge on :8765, car_link on :9870/udp
./run.sh --gui3d    # Gazebo's 3D GUI in your browser (noVNC); CPU-heavy
```

The sim starts with no track. You pick one in the ground station, which loads it into the running sim through the bridge (see Tracks).

`run.sh` builds the image (cached after the first time) and runs the container in the foreground. Ctrl+C (or a TERM signal) stops it. Docker Desktop must be running. Published ports listen on this machine only and have no password.

The 2D top-down view and the driving controls are the ground station's ([`../ground-station/`](../ground-station/)); it starts the sim with `run.sh --bridge` itself when you switch it to sim mode. The physics and sensors are 3D either way.

- **`--bridge`**: publishes the bridge (`127.0.0.1:8765`: track files and a websocket streaming the car's estimate, Gazebo's true pose and car_link's status) and car_link (`127.0.0.1:9870/udp`: heartbeat and teleop). Both follow [`../ground-station/protocol.md`](../ground-station/protocol.md). Costs almost nothing: the sim stays at real time.
- **`--gui3d`** (`http://localhost:6080`): Gazebo's own GUI on a virtual screen streamed through noVNC; the camera follows the car. It renders 3D on the CPU (llvmpipe), which slowed the blank-plane sim to about 37% of real time on an 8-core M-series Mac. Use it to inspect the model, not to drive.

## Tracks

A track is a ROS occupancy map: a `.yaml` plus the image it names, the format `map_server`, slam_toolbox and [f1tenth_gym](https://github.com/f1tenth/f1tenth_gym/blob/main/examples/example_map.yaml) use, so their maps drop straight in.

```yaml
image: track.png          # next to the yaml
resolution: 0.05          # metres per pixel
origin: [-10.0, -5.0, 0.0] # world pose (x, y, yaw) of the image's bottom-left corner
negate: 0
occupied_thresh: 0.65
free_thresh: 0.196
start: [0.0, -3.5, 0.0]   # optional, ours: car spawn pose (x, y, yaw); default 0, 0, 0
```

Occupied pixels become static 0.3 m walls (`wall_height:=` changes it), merged into as few boxes as possible; free and unknown pixels stay open floor, so a map must enclose its track with occupied pixels. Walls have low friction (0.1): at the default, the tyres climb a wall and flip the car. The bridge serves the same map to the ground station, which draws it under the car, walls solid and unknown space faint.

Tracks are loaded while the sim runs, not at launch. The ground station sends the bridge `load_track` with a bundled track's name (`src/arc_sim/tracks/<name>.yaml`); the bridge removes the old walls, spawns the new ones and moves the car to the track's `start` through Gazebo's UserCommands services (`/world/empty/remove`, `create`, `set_pose`), which takes about a second, and reports `track_status` (`loading`, `ready`, `failed`) for the ground station's status panel. Gazebo and car_link keep running, so the heartbeat chain is never interrupted. Only bundled tracks can be loaded: to use your own map, add it to `src/arc_sim/tracks/` (the image is rebuilt on the next `run.sh`). Bundled:

| Name | What |
| --- | --- |
| `oval` | Synthetic stadium oval: 2 m lane, 10 m straights, 0.15 m walls. Spawns on the bottom straight, facing counter-clockwise |

To see the walls a map produces: `docker exec arc-sim /entrypoint.sh python3 -m arc_sim.track <map.yaml>`.

## Drive

Drive from the ground station. The car only moves while it receives heartbeats: `car_link` (the sim's stand-in for the car's receiving end) stops the car on a 5 m/s² ramp if heartbeats stop for 0.15 s, and treats teleop as zero speed if teleop stops for 0.15 s.

For scripted testing without the ground station, send the same UDP packets yourself (`./run.sh --bridge` publishes the port). This drives at 1 m/s, steering 0.2 rad left, for 2 s:

```python
import socket, struct, time
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
for seq in range(100):  # 50 Hz; seq must keep rising for as long as the sim runs
    for kind, speed, steer in ((1, 0, 0), (2, 1.0, 0.2)):  # heartbeat, teleop
        s.sendto(struct.pack('<4sBIdff', b'ARC0', kind, seq, time.monotonic(), speed, steer), ('127.0.0.1', 9870))
    time.sleep(0.02)
```

car_link ignores a packet whose seq isn't newer than the last it accepted of that type, so a second run must start above the first's last seq, or restart the sim. Algorithms in the container publish `geometry_msgs/Twist` on `/sim/auto/cmd_vel` instead; teleop wins while fresh, and both stop without heartbeats.

Watch the link and the position from another shell (`/sim/odom` is the car's estimate; swap in `/sim/ground_truth` for the actual pose):

```bash
docker exec arc-sim /entrypoint.sh ros2 topic echo /sim/link/status
docker exec arc-sim /entrypoint.sh ros2 topic echo /sim/odom --field pose.pose.position
```

## Layout

`run.sh` starts everything on your machine; `start-sim.sh` is the container's main process (`BRIDGE=1` adds the bridge, `GUI=3d` the noVNC stack); `entrypoint.sh` sources ROS for every command, including `docker exec`.

`src/arc_sim/` is one ROS 2 package:

| Path | What |
| --- | --- |
| `urdf/racecar.urdf.xacro` | Car model: joints, inertia, collision, Ackermann plugin, ground-truth publisher. Swap it to swap the car |
| `meshes/chassis.stl` | Visual only. OpenRC F1 body, reduced to 20k triangles, metres, nose on +x |
| `worlds/empty.sdf` | Blank plane with a sun (the light is only used by the 3D viewer); the bridge spawns a track's walls into it |
| `tracks/` | Bundled tracks: map yaml + image |
| `arc_sim/track.py` | Map yaml + image → occupied pixels → merged wall boxes → walls model SDF. Used by the bridge |
| `config/bridge.yaml` | Gazebo ↔ ROS 2 topics. Add one entry per new topic |
| `config/gui.config` | Gazebo 3D GUI layout for `--gui3d`: window size, starting camera |
| `launch/sim.launch.py` | Server, spawn, topic bridge, car_link. `bridge:=true` adds the ground-station bridge; `gui:=true` the 3D GUI (needs a display) |
| `scripts/car_link.py` | The car's receiving end: UDP heartbeat + teleop on port 9870 and `/sim/auto/cmd_vel` in, heartbeat gate and speed ramp, `/sim/cmd_vel` and `/sim/link/status` out |
| `scripts/bridge.py` | Loads tracks into the running sim on request, serves the track files and streams the car's estimate, true pose and link status over a websocket (port 8765), for the ground station |

## Topics

Internal names only. Noise nodes (outside the sim) own the public names such as `/drive` and `/odom`.

The sim runs on `ROS_DOMAIN_ID=42` (set in the `Dockerfile`), so its graph stays separate from the car's (default domain 0) even if the container shares the host network. A node outside the container must set the same ID to see these topics.

| Topic | Direction | Type |
| --- | --- | --- |
| `/sim/auto/cmd_vel` | in (algorithm → car_link) | `geometry_msgs/Twist` (speed + yaw rate) |
| `/sim/cmd_vel` | in (car_link → car) | `geometry_msgs/Twist` (speed + yaw rate) |
| `/sim/link/status` | out (car_link → bridge) | `std_msgs/String`, JSON: heartbeat and teleop ages, `stopping`, `reason` |
| `/sim/odom` | out | `nav_msgs/Odometry`, the car's estimate from wheel motion |
| `/sim/ground_truth` | out | `nav_msgs/Odometry`, Gazebo's actual pose. For viewers and scoring; algorithms must never use it |
| `/clock` | out | sim time |

## Placeholders and assumptions

The car is the F1TENTH (Traxxas) geometry from `f1tenth_simulator`'s `racecar.xacro` until the ARC CAD exists; the chassis mesh is a different car and doesn't match it. Values for the ARC car come from `systems/` (wheelbase is open, `systems/mechanical/layout-brief.md` M2). Until then:

- Mass (3 kg chassis, 0.1 kg per wheel) and inertia are TBC; see the comments in the xacro.
- Steering limit 0.4189 rad is the f1tenth gym's value, TBC.
- All four wheels are driven, assumed from the 4WD layout (ADR-0026); not checked against the real drivetrain.
- Tire friction is Gazebo's default. car_link limits speed changes to 5 m/s² (TBC, SYS-04 / ADR-0012); there is no steering-rate limit yet, so steering changes faster than a real car's.
- The bridge's `estimate` is odometry shifted by the spawn pose, which assumes odometry starts at zero there. If they disagree, the estimate is offset by the same amount.

## Known issues

- The car turns less than commanded: about 0.66 rad/s for 1.0 rad/s requested at 1 m/s. Not yet investigated (steering lag, tire slip or plugin limits).
- `/sim/odom` drifts from `/sim/ground_truth` during turns (about 0.3 m after a 4 s turn), likely the same cause: the plugin's odometry assumes the commanded turn. No noise is added anywhere yet.
