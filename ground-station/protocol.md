# Ground station protocol (v1, TBC)

The contracts between the ground station, its helper, and the car (or the simulator standing in for it). The car side does not exist yet; the simulator implements it. Everything here is provisional until the car's teleop receiver and packet format are recorded in `systems/` (see ADR-0015 for the link rules this follows).

```
browser page ──ws localhost:8081──▶ helper.py ──UDP :9870──▶ car_link   (car, or sim)
browser page ──ws localhost:47380/ws──▶ server.py ──ws :8765──▶ bridge  (car, or sim)
browser page ──ws localhost:47380/scan─▶ server.py ──ws :8765/scan─▶ bridge   one socket per sensor, while shown
browser page ──ws localhost:47380/imu/<name>─▶ server.py ──ws :8765/imu/<name>─▶ bridge   filtered in the server
```

## Ports

| Port | Owner | Transport | Bound to |
| --- | --- | --- | --- |
| 47380 | `ground-station/server.py` | HTTP (page) + websockets `/ws`, `/scan`, `/imu/<name>` | `127.0.0.1` |
| 8081 | `ground-station/helper.py` | websocket | `127.0.0.1` |
| 8765 | bridge (sim: `simulator/.../bridge.py`) | HTTP + websockets `/ws`, `/scan`, `/imu/<name>` | sim container, published on `127.0.0.1:8765` |
| 9870 | car_link (sim: `simulator/.../car_link.py`) | UDP | sim container, published on `127.0.0.1:9870/udp` |

The sim is reached at `127.0.0.1`. The real car's address is a config value in the ground station, `TBD` until the car runs a bridge and car_link.

## UDP: helper ↔ car_link

Little-endian, 29 bytes, Python `struct` format `<4sBIIdff`, both directions:

| Field | Type | Meaning |
| --- | --- | --- |
| magic | 4 bytes | `b'ARC0'`; anything else is dropped |
| type | u8 | `1` heartbeat, `2` teleop, `3` hello (helper → car_link); `4` ack, `5` reject (car_link → helper) |
| session | u32 | The helper's session ID, random per helper run. Replies carry the session they answer |
| seq | u32 | Per-type counter from the sender, starting at 0, wrapping at 2^32. Reject: the reason, `1` no-session, `2` replaced. Ack: 0 |
| t | f64 | Sender's monotonic clock, seconds (for logging; never compared across machines) |
| speed | f32 | Teleop: commanded speed, m/s, + forward. Others: 0 |
| steer | f32 | Teleop: front steering angle, rad, + left. Others: 0 |

- Heartbeat and teleop are sent at 50 Hz. Socket marked DSCP EF (`IP_TOS = 0xB8`), per ADR-0015 rule 4. car_link replies to the address and port a packet came from.
- **Handshake:** before any heartbeat, the helper sends `hello` every 0.1 s (only while page heartbeats are fresh) until car_link acks its session. A hello with a new session ID replaces car_link's session (in the sim the newest ground station wins; what the real car does with several ground stations is an open owner decision): sequence numbers start over and the old session's teleop is forgotten, so the car stays stopped until the new session heartbeats. A repeated hello for the current session is acked again and changes nothing.
- **Sessions:** car_link accepts heartbeat and teleop only from the session it last acked. Anything else is rejected, and the sender gets a `reject` (at most one per 0.2 s per sender) saying why:
  - `no-session`: car_link has no session (it restarted). The helper says hello again.
  - `replaced`: another session said hello since. The helper stops sending to that target and reports `replaced`; only a different target or a ground station restart starts a new handshake, so two ground stations never take the car back and forth.
- Within the session, the receiver drops a packet whose `seq` is not newer than the last one of the same type it accepted (wrap-aware), so stale or reordered packets never apply. A late packet from a replaced session can't take control back: only a hello switches sessions.
- Known limits: a hello from an old helper that arrives very late could still switch sessions (hellos are only sent while handshaking). Packets are not authenticated.

## car_link behaviour

- **Heartbeat gate:** if no heartbeat for more than 0.15 s (SYS-04 timeout), every command source is overridden and the car stops on the ramp.
- **Teleop timeout:** if heartbeat is alive but no teleop for more than 0.15 s, the teleop command is treated as zero speed and the car stops on the same ramp.
- **Ramp:** commanded speed moves toward the target at no more than 5 m/s² (TBC, SYS-04 / ADR-0012). Steering holds its last value while braking.
- **Conversion:** sim input is a Twist (speed, yaw rate): `yaw_rate = speed * tan(steer) / wheelbase`, wheelbase 0.3302 m (placeholder, `racecar.urdf.xacro`). Steering is clamped to ±0.4189 rad.
- **Sources:** UDP teleop, and `/sim/auto/cmd_vel` (`geometry_msgs/Twist`) for algorithms. Teleop wins while fresh; both are gated by the heartbeat.
- **Status:** publishes `/sim/link/status` (`std_msgs/String`, JSON) at 10 Hz: `{"heartbeat_age": s|null, "teleop_age": s|null, "stopping": bool, "reason": "ok"|"heartbeat"|"teleop"|"idle", "session": "<hex>"|null, "rejected": n, "rejected_reason": "no-session"|"replaced"|null}`. Ages are null before the first packet of the current session. `idle` means nothing has been received in it yet. `rejected` counts rejected packets since car_link started; `rejected_reason` is the latest one's.
- Output `/sim/cmd_vel` at 50 Hz (wall clock).

## Bridge websocket (car or sim → server), port 8765

`GET /track.json`, `GET /track.png`: same as the old `viewer_2d` (track placement, or `null`; alpha-mask PNG). The sim starts with no track (`null`); they change when a track is loaded.

`ws://…:8765/ws` sends JSON at 30 Hz:

```json
{"type": "state", "source": "sim",
 "estimate": {"x": 0, "y": 0, "yaw": 0},
 "truth": {"x": 0, "y": 0, "yaw": 0},
 "link": {"heartbeat_age": 0.02, "teleop_age": 0.02, "stopping": false, "reason": "ok"},
 "rtf": 0.98}
```

- `estimate`: the car's own odometry in the world frame (sim: `/sim/odom` shifted by the spawn pose). Null until known.
- `truth`: sim only (`/sim/ground_truth`). A real car's bridge omits it.
- `source`: `"sim"` or `"car"`.
- `link`: the latest car_link status, or null.
- `rtf`: sim only. Gazebo's real-time factor (sim seconds per wall second, from `/world/<name>/stats`): below 1 when the computer can't keep up. Null while paused or not yet known. A real car's bridge omits it.

The sim's bridge also sends, once on connect and on every change:

```json
{"type": "track_status", "name": "oval", "status": "loading", "error": null}
```

- `status`: `"none"` (no track loaded), `"loading"`, `"ready"`, or `"failed"` (with `error`, a short reason). `name` is the track being loaded or loaded, or null.

The bridge also sends, once on connect and whenever the list changes, the sensors it can stream besides `/scan`:

```json
{"type": "sensors", "imu": [{"name": "cg", "accel_range": 156.9, "gyro_range": 34.91}]}
```

- `imu`: every `/imu/<name>` topic of type `sensor_msgs/Imu` (found by name, so a second IMU in the car model appears here without code changes). `accel_range` (m/s²) and `gyro_range` (rad/s) are the part's configured full scale; the server flags samples at 98 % of it as saturated. Sim: `IMU_ACCEL_RANGE_G`, `IMU_GYRO_RANGE_DPS` in `simulator/.env`.

Server → bridge (sim only):

- `{"type": "load_track", "name": "<name>"|null}`: load bundled track `<name>` (`simulator/src/arc_sim/tracks/<name>.yaml`) into the running sim, or clear it with null. The bridge replaces the walls and moves the car to the track's start pose; the car's `estimate` restarts there. A repeat of the loaded track is ignored. Requests arriving during a load wait for it, and only the newest is loaded. Load time is about 1 s (TBC).

## Sensor websockets (bridge → server → page)

Each sensor has its own websocket, separate from `/ws`, so a large frame never holds up `state`. Opening one is the subscription: the bridge subscribes to the sensor's topic while at least one of its sockets is open and unsubscribes when the last closes. In the sim that is passed down the chain (noise node, then `ros_gz_bridge`, both lazy), so Gazebo renders a sensor only while some page shows it. Sensors are off until the page opens their socket.

- The server relays each page socket to its own bridge socket on the same path, one to one. With no bridge connected it closes the page socket at once; the page retries every 1 s while the sensor is switched on.
- Frames are binary websocket messages. The bridge and the server keep only the newest frame per client: a slow client skips frames, it doesn't fall behind.
- Nothing is sent toward the bridge on these sockets.

`/scan`: the LiDAR's `/scan` (`sensor_msgs/LaserScan`), every scan, ranges unfiltered. Little-endian:

| Offset | Type | Field |
| --- | --- | --- |
| 0 | float64 | stamp, s (the message's header stamp; sim time in the sim) |
| 8 | float32 | angle_min, rad (0 = car's forward, counter-clockwise positive) |
| 12 | float32 | angle_increment, rad |
| 16 | float32 | range_min, m |
| 20 | float32 | range_max, m |
| 24 | uint32 | count |
| 28 | float32 | mount x, m (LiDAR origin in `base_link`) |
| 32 | float32 | mount y, m |
| 36 | float32 | mount yaw, rad |
| 40 | float32 × count | ranges, m; outside [range_min, range_max] (including `inf`) means no return |

The mount is the fixed `base_link` → scan `frame_id` transform from `/tf_static` (published by `robot_state_publisher` from the URDF), reduced to the ground plane. The bridge sends no frames until that lookup succeeds. The page uses it to draw the scan on the main view: car pose, then mount, then each ray.

At the sim's defaults (1081 rays, 40 Hz) that is about 4.4 kB per scan, 175 kB/s.

`/imu/<name>`: the IMU's raw samples (`sensor_msgs/Imu`: angular velocity and linear acceleration; the car's IMU has no orientation), in batches. **Unlike `/scan`, every sample is sent, in order**: the server integrates the gyro, so a dropped batch would be a lasting yaw error. A client that falls 200 frames behind is disconnected (close code 1013) and reconnects fresh. A frame holds up to 50 samples and is sent when full or every 0.05 s. Little-endian:

| Offset | Type | Field |
| --- | --- | --- |
| 0 | uint32 | count |
| 4 + 32·i | float64 | stamp, s (the message's header stamp; sim time in the sim) |
| 12 + 32·i | float32 × 3 | angular velocity x, y, z, rad/s, in the IMU's frame (x forward, y left, z up) |
| 24 + 32·i | float32 × 3 | linear acceleration x, y, z, m/s², including gravity's reaction (+9.81 on z at rest) |

The relay is not one to one for IMUs: the server reads every frame itself (see below) and sends the page summaries, not samples. At 1 kHz that is 32 kB/s per IMU.

## Server websocket (server → page), `ws://localhost:47380/ws`

Server → page:

- Every bridge `state` message, relayed unchanged.
- `{"type": "link", "mode": "sim"|"real", "connected": bool, "car_found": bool, "sim": "stopped"|"starting"|"running", "udp": {"host": "127.0.0.1", "port": 9870}|null}`, sent on every change and once on connect. `udp` is where the helper should send, null when not connected.
- `{"type": "tracks", "names": ["oval", ...], "selected": "oval"}`, once on connect: the sim tracks the page may pick, from `simulator/src/arc_sim/tracks/*.yaml`. `selected` starts as config `sim.track`.
- The bridge's `track_status`, relayed unchanged, and to pages that connect later while connected.
- `{"type": "sensors", "imu": ["cg", ...]}`: the IMU names the bridge announced, on connect and on every change; empty with no bridge. The page shows one IMU panel per name.
- `{"type": "track", "meta": {...}|null, "png": "<base64>"|null}`: the bridge's `/track.json` and `/track.png` after each (re)connect and after each finished track load, and to pages that connect later. The image is embedded so the page needs no separate request (Firefox failed to load it from `/track.png`). `/track.png` is still proxied for other tools.

Page → server:

- `{"type": "switch", "mode": "sim"|"real"}`. `sim` starts the simulator if it is not running; `real` stops it and tries the car.
- `{"type": "sim", "track": "<name>"}`: the picked sim track. The server asks the bridge to load it now if connected to the sim, and on every later connect. Must be one of the `tracks` names; anything else is ignored.

## Server IMU websocket (server ↔ page), `ws://localhost:47380/imu/<name>`

Opened by the page while that IMU's panel is shown; the server opens `/imu/<name>` on the bridge for it, so the IMU streams only then. Closed at once with no bridge or an unknown name; the page retries every 1 s. Each page socket has its own filter (`ground-station/imu.py`), fed every sample.

Server → page, one JSON message per step (0.1 s unless the page asks for another), even if no samples arrived:

```json
{"type": "imu", "rate": 1000, "age": 0.03, "n": 100,
 "roll": 0.01, "pitch": -0.02, "yaw": 0.4,
 "gyro": [0, 0, 0.5], "accel": [0.2, 1.1, 9.8],
 "peak_g": 1.13, "sat": false, "bias": [0.0, 0.0, 0.001], "stopped": false}
```

- `rate`: samples received in the last second (wall clock). `age`: seconds since the last batch arrived, null before the first. `n`: samples in this step.
- `roll`, `pitch`, `yaw` (rad): complementary filter, time constant 1 s (TBC). Roll and pitch are pulled toward the accelerometer's tilt only while |a| is within 10 % of 1 g. Yaw is relative: zero when the socket opened or was last reset, drifting with the gyro bias. Null before the first sample.
- `gyro`, `accel`: means over the step, null if `n` is 0. `peak_g`: largest |a| in the step, in g. `sat`: any sample at 98 % of the range.
- `bias`: mean gyro reading while the car is still (every axis under 0.05 rad/s and |a| within 5 % of 1 g), null until it has been. `stopped`: whether the latest sample counted as still.

Page → server:

- `{"type": "step", "s": 0.1|0.05|0.02|0.01}`: the step between summaries. Other values are ignored.
- `{"type": "reset_yaw"}`: zero the relative yaw.

## Helper websocket (page ↔ helper), `ws://localhost:8081`

Page → helper:

- `{"type": "hb", "seq": n}` every 50 ms. The helper sends UDP heartbeats only while page heartbeats are fresh: more than 0.15 s without one (3 intervals) and it stops.
- `{"type": "teleop", "speed": m/s, "steer": rad}` at 50 Hz while driving. The helper sends the latest at 50 Hz, and only while page heartbeats are fresh.
- `{"type": "target", "host": "…", "port": n}` or `{"type": "target", "host": null}`: where to send UDP (from the server's `link.udp`). Null stops sending.
- `{"type": "cutoff", "on": true|false}`: Test heartbeat cutoff. While on, the helper sends no UDP heartbeats (teleop continues, so the car's heartbeat gate is what stops it).

Helper → page:

- `{"type": "sent", "count": n, "last_seq": n, "t": s, "sending": bool, "cutoff": bool, "reason": "ok"|"no-page-hb"|"no-target"|"cutoff"|"handshaking"|"replaced", "session": "<hex>", "link": "handshaking"|"linked"|"replaced", "rejected": "no-session"|"replaced"|null}` at 5 Hz: the sent-only heartbeat log. `count` is heartbeats sent since the previous report. `link` is the handshake with the current target. `rejected` is the latest reject from car_link if it arrived in the last second. The page's Link tile shows `Rejecting packets sent` while `rejected` is set, `Different session detected` when `link` is `replaced`, and `handshaking` until the ack.
