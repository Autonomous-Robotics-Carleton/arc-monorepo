# ground-station

The operator's app for driving the car, or the simulator standing in for it. It runs natively on your laptop (macOS or Linux), not in Docker and never on the car: Python 3 and a plain HTML/JS page in your browser.

## Run

```bash
./run.sh                       # real mode: looks for the car at config.json car.host
./run.sh --car 192.168.1.50    # override car.host
```

`run.sh` creates `.venv` and installs `requirements.txt` the first time (and again only when it changes), starts `server.py` and `helper.py`, and opens `http://localhost:8080/`. Ctrl+C stops both, and the simulator if the ground station started it. Both listen on this machine only and have no password.

Switching to the simulator needs Docker Desktop running, as for [`simulator/`](../simulator/README.md).

## How it connects

Two processes, two paths to the car. The full contract is [`protocol.md`](protocol.md).

```
page ──ws :8081──▶ helper.py ──UDP :9870──▶ car_link   heartbeat + teleop, 50 Hz
page ──ws :8080──▶ server.py ──ws :8765──▶ bridge     state, track, link status
```

- **`server.py`** (`:8080`) serves the page and relays the bridge's state to it, unchanged, along with which mode it's in and whether it's connected. It proxies the track image at `/track.png`.
- **`helper.py`** (`:8081`) sends UDP to car_link: heartbeats, and the page's teleop. The page tells it where (the server's `udp` address). Packets are marked DSCP EF.

`config.json` holds the car's address and the ports: `car.host` (the car's bridge and car_link), `sim.host` (the simulator, `127.0.0.1`), and `sim.track` (the track picked when the page opens). `--car` overrides `car.host`.

## Sim and real

- **Real** (at startup): the server connects to the car's bridge at `car.host`, retrying every 2 s.
- **Sim**: the page's switch makes the server start `../simulator/run.sh --bridge`, wait until its bridge answers on `127.0.0.1:8765`, and connect. The first start builds the sim image, which takes a while. If an `arc-sim` container is already running, the server says so in its log and uses it (it must have been started with `--bridge`). The sim starts with no track: on every connect, and whenever you pick another in the track menu (`simulator/src/arc_sim/tracks/*.yaml`), the server asks the bridge to load it into the running sim. The status panel's sim tile shows `loading track` while the bridge rebuilds the walls and moves the car to the start, and `track failed` with the reason if it can't. Switching back to real stops the simulator (`docker stop arc-sim` if `run.sh` didn't).

In sim mode the server keeps looking for the car in the background, so the page can say a live car was found. It never switches by itself.

## Heartbeat chain

The car moves only while heartbeats keep arriving, end to end: the page sends one to the helper every 50 ms, and the helper sends UDP heartbeats at 50 Hz only while the page's are fresh (within 0.15 s). If the page stops, the helper stops, and car_link stops the car after 0.15 s without a heartbeat (SYS-04).

So hiding the tab, closing it or putting the laptop to sleep stops the car. That is intended: browsers throttle hidden tabs, and a page you can't see shouldn't be driving.

Teleop is sent only while the page's heartbeats are fresh and a teleop message arrived within 0.15 s; otherwise the helper sends none and car_link's teleop timeout stops the car.

If several pages are open, a heartbeat from any of them counts, and the latest target, teleop and cutoff from any of them apply.

**Test cutoff** (in the Heartbeat panel) makes the helper stop sending heartbeats while teleop continues, so you can watch the car's own heartbeat gate stop it. The helper reports what it sent to the page at 5 Hz.

## The page

`web/index.html` is one file of plain HTML, CSS and JS: no build step, no framework, no external requests. The track viewport fills the window under a fixed top bar, and everything else is a panel floating over it.

- **Top bar:** the title `arc ground station` (always lowercase), three always-on pills (target, driving/idle, heartbeat), `Reset layout`, and `+ Add panel ▾`. Nothing else goes in the top bar.
- **Panels** are free-floating, never docked: drag by the header, resize from the corner (both snap to 8 px), collapse `▾`/`▸`, close `✕`, and reopen from `+ Add panel`. Positions, sizes, which panels are open and the plotted topics are kept in `localStorage` (`arc-gs-layout-v1`); `Reset layout` restores the default (Status and Heartbeat on the left, Topics and Plots on the right).
- **Panels today:** Status (the chain target → server → helper → link → sim: the first failure is red, every tile after it greyed), Heartbeat (what the helper sends, the heartbeat and teleop ages the car reports against the 150 ms timeout, the ARMED / STOPPING strip, Test cutoff, event log), Car (pose, odom, odom error), Topics, Plots, Settings, and Sim settings (shown only in sim mode; it picks the track and can hide the track from the view, which leaves the walls in the sim).
- **LiDAR:** off by default. Its panel switches `/scan` on and off, picks the scan colour and the FOV drawing (fill, outline or none); the scan itself is drawn on the main view (see the viewport rule below). Closing the panel also switches it off.
- **Not wired yet:** Camera and Computer (CPU, memory, GPU free) are in the menu but greyed out, because nothing sends their data.
- **Adding a panel:** add an entry to `PANELS` in the script (`title`, `group`, `build(body)` once on open, optional `update(body)` every frame, optional `available()` for when it can be opened). The menu lists it automatically.
- **Topics and Plots** show every numeric field the page receives, grouped by topic. Today that is only what the bridge's `state` message carries: in sim, `/sim/odom`, `/sim/ground_truth` and `/sim/link/status`. The page plots anything passed to `ingest(topic, type, object)`, so forwarding more topics needs only bridge and server changes plus one `ingest` call. How the bridge discovers and forwards topics, and how heavy ones (camera, scan) stay off the teleop path, is not decided yet.

**Look.** The page follows the ARC Ground Station design system (Claude Design; ask Yufeng for access). Its tokens are copied into `:root` at the top of the page. The rules that matter most when you change the page:

- Dark theme first; light through `prefers-color-scheme`. Every colour is a token defined for both themes.
- One monospace face. Lowercase keys and values with no colons (`heartbeat age`); uppercase only for alarm states (`LOST`, `ARMED`, `STOPPING`). Join a state and its reason with a middle dot: `STOPPING · heartbeat timeout`.
- Colour only for state (except sensor overlays, below): `ok`, `warn`, `bad`, always paired with a word. Red means the car is or will be stopping. `accent` is for interaction only (selected, focus), never a status.
- Plain data is `ink`; units and sub-labels `ink-muted`. Tabular figures with fixed decimals, so numbers don't jitter.
- No icons, only text glyphs (`▾ ▸ ✕ ✓ → + ·`). Status dots are 6 px squares. Nothing rounder than 4 px.
- In the viewport, the sim car and its trail are `car` red, the odom ghost is a dashed `odom` blue outline, and the real car is white with a dark outline.
- Sensor overlays in the viewport use their own colours, `overlay-1` to `overlay-4`, picked to stay clear of the state, car and odom colours. The operator picks one per sensor. The LiDAR draws its returns as 2 px squares and its FOV as a fan out to the scan's `range_max`, filled at 50 % opacity or outlined, both in that colour.

## Not built yet

- Topic discovery, and the Camera and Computer panels (see [The page](#the-page)).
- Take control, a driver-lost broadcast, and ownership of the car between several ground stations.
- The car side: the real car has no bridge or car_link yet, so real mode shows no car. `car.host` is TBD (`null`).
- The UDP packet format and ports are provisional (TBC) until the car's teleop receiver is recorded in [`systems/`](../systems/README.md).

## Layout

| Path | What |
| --- | --- |
| `run.sh` | Starts both processes; `--car` |
| `server.py` | Page, `/track.png` proxy, bridge relay, sim start/stop |
| `helper.py` | Page heartbeat and teleop → UDP to car_link |
| `config.json` | Car address (TBD), sim address, ports, default track |
| `web/index.html` | The page: viewport, top bar, panels, teleop and page heartbeat |
| `protocol.md` | Messages and packets between all of the above |

For testing, `ARC_GS_SIM_RUN=<script>` makes the server run that script instead of `../simulator/run.sh`, with the same arguments.
