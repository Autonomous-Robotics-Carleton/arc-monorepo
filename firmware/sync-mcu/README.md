# sync-mcu

Zephyr firmware for the STM32H723 on the sync board (ADR-0017).

It owns the car's time base: µs-critical sensors are timestamped in this clock, and the Orin syncs to it in software (ADR-0021). It also runs the heartbeat watchdog and the safety envelope (SYS-04, SYS-22), drives the command CAN bus and the steering bus (ADR-0009, ADR-0019), reads each motor controller's telemetry UART (ADR-0034), and streams everything to the Orin over Ethernet: every sample is forwarded within ≤ 0.25 ms of arriving, over UDP, with no per-millisecond batching (ADR-0024, SYS-29).

Pin and I/O tally: `systems/bom/electrical.md`. Board handoff: `systems/handoff/electrical.md`. How it fits the rest: `systems/software/architecture.md`.

## Layout

| Path | What |
| --- | --- |
| `west.yml` | Pins Zephyr (v4.4.2) and its modules. The dev container bakes this workspace in; nothing is fetched by hand |
| `app/` | The application, one module per job: `time_base` (kernel uptime until the hardware timer), `sensors`, `can_buses` and `controller_links` (stubs), `safety` (fail-safe stubs: no envelope, so nothing can move the car; the watchdog always reads expired), `sync_link` (exchanges `ARC_LINK_STATUS` over UDP on `native_sim`; a stub on the boards until Ethernet is set up) |
| `app/Kconfig`, `app/prj.conf`, `app/boards/` | Configuration (below); `boards/<board>.conf` adds per-board settings (`native_sim.conf` turns on the host's sockets) |
| `app/generated/` | Generated code (see Rules) |
| `boards/arc/arc_sync/` | Our board definition (skeleton: pins, crystal and package TBD from the EE; until then it runs from the 64 MHz internal oscillator with no console) |
| `tests/` | Tests that run on `native_sim` with twister |

## Targets (ADR-0028)

| Target | Board | Status |
| --- | --- | --- |
| Simulation | `native_sim`: runs as a program on a laptop or in CI; its sync link uses the host's sockets, so it talks to the ROS car backend (`nx test firmware-in-loop`) | Builds, runs and links up |
| Development hardware | `nucleo_h723zg`: same MCU, Ethernet and FD-CAN (CAN needs a transceiver breakout) | Builds; the sync link isn't enabled yet (no Ethernet config); 2 boards in the order (SQ-5) |
| The car | `arc_sync` | Builds from the skeleton; nothing to run on until the board exists |

## Build, run, test

In the dev container (ADR-0029, Dev setup in the handbook), from the repo root:

```bash
npx nx build sync-mcu      # all three targets, into firmware/sync-mcu/build/<board>
npx nx test sync-mcu       # twister on native_sim

# one target by hand
west build -b native_sim firmware/sync-mcu/app -d firmware/sync-mcu/build/native_sim
firmware/sync-mcu/build/native_sim/zephyr/zephyr.exe     # Ctrl-C to stop
```

| Test (twister) | What it checks |
| --- | --- |
| `sync_mcu.safety` | The safety stubs are fail-safe |
| `sync_mcu.sync_link` | `ARC_LINK_STATUS` round-trips through the generated MAVLink code |
| `sync_mcu.can_command` | The generated command-bus frames pack like VESC's own `comm_can.c` |

Twister's output goes to `firmware/sync-mcu/build/twister`. `npx nx test firmware-in-loop` runs the `native_sim` build against the ROS car backend; it needs UDP ports 52000 and 52001 free.

## Configuration

Options in `app/Kconfig`, set with `west build … -- -DCONFIG_<name>=<value>`:

| Option | Default | |
| --- | --- | --- |
| `ARC_HEARTBEAT_TIMEOUT_MS` | 150 | Operator heartbeat timeout (SYS-04, TBC) |
| `ARC_SYNC_LINK_PORT_RX` | 52000 | UDP port the sync MCU listens on (ICD-sync-link, TBC) |
| `ARC_SYNC_LINK_PORT_TX` | 52001 | UDP port the Orin listens on (ICD-sync-link, TBC) |
| `ARC_SYNC_LINK_PEER` | `127.0.0.1` | The Orin's address: localhost for `native_sim`; the car's is TBD |

## Flash (from your own computer, not the container)

```bash
probe-rs download --chip STM32H723ZGTx firmware/sync-mcu/build/nucleo_h723zg/zephyr/zephyr.elf
```

## Rules

- **Safety code** (`app/src/safety.*`: the envelope and the heartbeat watchdog) needs its own tests and a second reviewer for every change.
- **Generated code** is never edited by hand. `tools/gen-interfaces.sh` (mavgen and cantools, in the dev container) makes the sync-link messages in `app/generated/arc_mavlink/` from `systems/icd/sync-link.xml`, with a copy for the ROS car backend, and the command-bus frames in `app/generated/can_command/` from `systems/icd/can-command.dbc`. `nx check sync-mcu` (and `nx check ros`) fail if any of it is stale.
