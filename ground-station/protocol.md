# Ground station protocol (v1, TBC)

The contracts between the ground station, its helper, and the car (or the simulator standing in for it). The car side does not exist yet; the simulator implements it. Everything here is provisional until the car's teleop receiver and packet format are recorded in `systems/` (see ADR-0015 for the link rules this follows).

```
browser page ──ws localhost:8081──▶ helper.py ──UDP :9870──▶ car_link   (car, or sim)
browser page ──ws localhost:8080/ws──▶ server.py ──ws :8765──▶ bridge  (car, or sim)
```

## Ports

| Port | Owner | Transport | Bound to |
| --- | --- | --- | --- |
| 8080 | `ground-station/server.py` | HTTP (page) + websocket `/ws` | `127.0.0.1` |
| 8081 | `ground-station/helper.py` | websocket | `127.0.0.1` |
| 8765 | bridge (sim: `simulator/.../bridge.py`) | HTTP + websocket `/ws` | sim container, published on `127.0.0.1:8765` |
| 9870 | car_link (sim: `simulator/.../car_link.py`) | UDP | sim container, published on `127.0.0.1:9870/udp` |

The sim is reached at `127.0.0.1`. The real car's address is a config value in the ground station, `TBD` until the car runs a bridge and car_link.

## UDP: helper → car_link

Little-endian, 25 bytes, Python `struct` format `<4sBIdff`:

| Field | Type | Meaning |
| --- | --- | --- |
| magic | 4 bytes | `b'ARC0'`; anything else is dropped |
| type | u8 | `1` heartbeat, `2` teleop |
| seq | u32 | Per-type counter from the sender, starting at 0, wrapping at 2^32 |
| t | f64 | Sender's monotonic clock, seconds (for logging; never compared across machines) |
| speed | f32 | Teleop: commanded speed, m/s, + forward. Heartbeat: 0 |
| steer | f32 | Teleop: front steering angle, rad, + left. Heartbeat: 0 |

- Both types are sent at 50 Hz. Socket marked DSCP EF (`IP_TOS = 0xB8`), per ADR-0015 rule 4.
- The receiver drops a packet whose `seq` is not newer than the last one of the same type it accepted (wrap-aware), so stale or reordered packets never apply.

## car_link behaviour

- **Heartbeat gate:** if no heartbeat for more than 0.15 s (SYS-04 timeout), every command source is overridden and the car stops on the ramp.
- **Teleop timeout:** if heartbeat is alive but no teleop for more than 0.15 s, the teleop command is treated as zero speed and the car stops on the same ramp.
- **Ramp:** commanded speed moves toward the target at no more than 5 m/s² (TBC, SYS-04 / ADR-0012). Steering holds its last value while braking.
- **Conversion:** sim input is a Twist (speed, yaw rate): `yaw_rate = speed * tan(steer) / wheelbase`, wheelbase 0.3302 m (placeholder, `racecar.urdf.xacro`). Steering is clamped to ±0.4189 rad.
- **Sources:** UDP teleop, and `/sim/auto/cmd_vel` (`geometry_msgs/Twist`) for algorithms. Teleop wins while fresh; both are gated by the heartbeat.
- **Status:** publishes `/sim/link/status` (`std_msgs/String`, JSON) at 10 Hz: `{"heartbeat_age": s|null, "teleop_age": s|null, "stopping": bool, "reason": "ok"|"heartbeat"|"teleop"|"idle"}`. Ages are null before the first packet. `idle` means nothing has ever been received.
- Output `/sim/cmd_vel` at 50 Hz (wall clock).

## Bridge websocket (car or sim → server), port 8765

`GET /track.json`, `GET /track.png`: same as the old `viewer_2d` (track placement, or `null`; alpha-mask PNG). The sim starts with no track (`null`); they change when a track is loaded.

`ws://…:8765/ws` sends JSON at 30 Hz:

```json
{"type": "state", "source": "sim",
 "estimate": {"x": 0, "y": 0, "yaw": 0},
 "truth": {"x": 0, "y": 0, "yaw": 0},
 "link": {"heartbeat_age": 0.02, "teleop_age": 0.02, "stopping": false, "reason": "ok"}}
```

- `estimate`: the car's own odometry in the world frame (sim: `/sim/odom` shifted by the spawn pose). Null until known.
- `truth`: sim only (`/sim/ground_truth`). A real car's bridge omits it.
- `source`: `"sim"` or `"car"`.
- `link`: the latest car_link status, or null.

The sim's bridge also sends, once on connect and on every change:

```json
{"type": "track_status", "name": "oval", "status": "loading", "error": null}
```

- `status`: `"none"` (no track loaded), `"loading"`, `"ready"`, or `"failed"` (with `error`, a short reason). `name` is the track being loaded or loaded, or null.

Server → bridge (sim only):

- `{"type": "load_track", "name": "<name>"|null}`: load bundled track `<name>` (`simulator/src/arc_sim/tracks/<name>.yaml`) into the running sim, or clear it with null. The bridge replaces the walls and moves the car to the track's start pose; the car's `estimate` restarts there. A repeat of the loaded track is ignored. Requests arriving during a load wait for it, and only the newest is loaded. Load time is about 1 s (TBC).

## Server websocket (server → page), `ws://localhost:8080/ws`

Server → page:

- Every bridge `state` message, relayed unchanged.
- `{"type": "link", "mode": "sim"|"real", "connected": bool, "car_found": bool, "sim": "stopped"|"starting"|"running", "udp": {"host": "127.0.0.1", "port": 9870}|null}`, sent on every change and once on connect. `udp` is where the helper should send, null when not connected.
- `{"type": "tracks", "names": ["oval", ...], "selected": "oval"}`, once on connect: the sim tracks the page may pick, from `simulator/src/arc_sim/tracks/*.yaml`. `selected` starts as config `sim.track`.
- The bridge's `track_status`, relayed unchanged, and to pages that connect later while connected.
- `{"type": "track", "meta": {...}|null, "png": "<base64>"|null}`: the bridge's `/track.json` and `/track.png` after each (re)connect and after each finished track load, and to pages that connect later. The image is embedded so the page needs no separate request (Firefox failed to load it from `/track.png`). `/track.png` is still proxied for other tools.

Page → server:

- `{"type": "switch", "mode": "sim"|"real"}`. `sim` starts the simulator if it is not running; `real` stops it and tries the car.
- `{"type": "sim", "track": "<name>"}`: the picked sim track. The server asks the bridge to load it now if connected to the sim, and on every later connect. Must be one of the `tracks` names; anything else is ignored.

## Helper websocket (page ↔ helper), `ws://localhost:8081`

Page → helper:

- `{"type": "hb", "seq": n}` every 50 ms. The helper sends UDP heartbeats only while page heartbeats are fresh: more than 0.15 s without one (3 intervals) and it stops.
- `{"type": "teleop", "speed": m/s, "steer": rad}` at 50 Hz while driving. The helper sends the latest at 50 Hz, and only while page heartbeats are fresh.
- `{"type": "target", "host": "…", "port": n}` or `{"type": "target", "host": null}`: where to send UDP (from the server's `link.udp`). Null stops sending.
- `{"type": "cutoff", "on": true|false}`: Test heartbeat cutoff. While on, the helper sends no UDP heartbeats (teleop continues, so the car's heartbeat gate is what stops it).

Helper → page:

- `{"type": "sent", "count": n, "last_seq": n, "t": s, "sending": bool, "cutoff": bool, "reason": "ok"|"no-page-hb"|"no-target"|"cutoff"}` at 5 Hz: the sent-only heartbeat log. `count` is heartbeats sent since the previous report.
