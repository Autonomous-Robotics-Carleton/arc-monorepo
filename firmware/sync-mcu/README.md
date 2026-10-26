# sync-mcu

Zephyr firmware for the STM32H723 on the sync board (ADR-0017).

It owns the car's time base: µs-critical sensors are timestamped in this clock, and the Orin syncs to it in software (ADR-0021). It also runs the heartbeat watchdog and the safety envelope (SYS-04, SYS-22), drives the command CAN bus and the steering bus (ADR-0009, ADR-0019), reads each motor controller's telemetry UART (ADR-0034), and streams everything to the Orin over Ethernet: every sample is forwarded within ≤ 0.25 ms of arriving, over UDP, with no per-millisecond batching (ADR-0024, SYS-29).

Pin and I/O tally: `systems/bom/electrical.md`. Board handoff: `systems/handoff/electrical.md`. How it fits the rest: `systems/software/architecture.md`.

## Layout

| Path | What |
| --- | --- |
| `west.yml` | Pins Zephyr (v4.4.2) and its modules. The dev container bakes this workspace in; nothing is fetched by hand |
| `app/` | The application, one module per job: `time_base` (kernel uptime until the hardware timer), `sensors`, `can_buses` (stub), `controller_link_core` (the any-corner-stop decisions, no I/O) and `controller_links` (its UARTs: `native_sim` only so far), `safety` (fail-safe stubs: no envelope, so nothing can move the car; the watchdog always reads expired), `sync_link` (exchanges `ARC_LINK_STATUS` over UDP on `native_sim`; a stub on the boards until Ethernet is set up) |
| `app/Kconfig`, `app/prj.conf`, `app/boards/` | Configuration (below); `boards/<board>.conf` adds per-board settings (`native_sim.conf` turns on the host's sockets and the controller UARTs, `native_sim.overlay` defines the four pseudo-terminals) |
| `app/generated/` | Generated code (see Rules) |
| `boards/arc/arc_sync/` | Our board definition (skeleton: pins, crystal and package TBD from the EE; until then it runs from the 64 MHz internal oscillator with no console) |
| `tests/` | Tests that run on `native_sim` with twister |

## Targets (ADR-0028)

| Target | Board | Status |
| --- | --- | --- |
| Simulation | `native_sim`: runs as a program on a laptop or in CI; its sync link uses the host's sockets, so it talks to the ROS car backend (`nx test firmware-in-loop`); its four controller UARTs are pseudo-terminals that the fake controllers in `tools/sim` attach to (`tools/sim/firmware_in_loop.sh`) | Builds, runs and links up; stops on a silent, faulted or braking corner |
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
| `sync_mcu.can_command_core` | The frames for each decision (drive, zero, brake, nothing), byte by byte against the stock VESC packing: 11 cases |
| `sync_mcu.safety` | The envelope (ADR-0052 defaults, capped at the car's maximum), the heartbeat watchdog (150 ms; a stop holds until re-armed) and the drive decision, in virtual time: 36 cases |
| `sync_mcu.controller_link` | The any-corner stop's logic, in virtual time: a corner silent for more than 5 ms, fault, e-stop and command-timeout states, CRC and sequence handling, frames made by pymavlink (`tools/sim/make_golden.py`) |
| `sync_mcu.sync_link` | `ARC_LINK_STATUS` round-trips through the generated MAVLink code |
| `sync_mcu.can_command` | The generated command-bus frames pack like VESC's own `comm_can.c` |

Twister's output goes to `firmware/sync-mcu/build/twister`. `npx nx test firmware-in-loop` runs the `native_sim` build against the ROS car backend; it needs UDP ports 52000 and 52001 free.

## Configuration

Options in `app/Kconfig`, set with `west build … -- -DCONFIG_<name>=<value>`:

| Option | Default | |
| --- | --- | --- |
| `ARC_HEARTBEAT_TIMEOUT_MS` | 150 | Operator heartbeat timeout (SYS-04, TBC) |
| `ARC_CAR_MAX_SPEED_MMPS` | 9000 | The car's top speed, the cap for raising the envelope (SYS-01); moves to the car's configuration (ADR-0051) |
| `ARC_CAR_MAX_ACCEL_MMPS2` | 5000 | The car's maximum acceleration (TBD until measured; equal to the default, so it can't be raised yet) |
| `ARC_CAR_MAX_STEER_MRAD` | 436 | The car's steering range each side, about ±25° (ADR-0019, TBC); the default envelope allows all of it (ADR-0052) |
| `ARC_CAN_BRAKE_CURRENT_MA` | 0 | Brake current sent to the four controllers on a stop (TBD). 0 sends nothing, and the controllers brake on their own timeout, later than SYS-25's 20 ms |
| `ARC_CAN_ERPM_PER_MPS` | 0 | eRPM per m/s of commanded speed (TBD: wheel, gear ratio, pole pairs). 0 sends 0 A |
| `ARC_COMMAND_RATE_HZ` | 500 | Expected `ARC_DRIVE_COMMAND` rate; a stream off by more than the tolerance is logged, never acted on (TBC; ICD-sync-link open issue 4) |
| `ARC_COMMAND_RATE_TOLERANCE_PCT` | 20 | How far off the expected rate is reported (TBC) |
| `ARC_COMMAND_TIMEOUT_MS` | 150 | Age after which an `ARC_DRIVE_COMMAND` is dropped and the output is zero (TBC; ICD-sync-link open issue 7) |
| `ARC_CONTROLLER_SILENCE_MS` | 5 | Silence from a motor controller that stops the car (SYS-25, TBC). The firmware-in-the-loop run builds with 100: a laptop can't hold 5 ms in real time |
| `ARC_SYNC_LINK_PORT_RX` | 52000 | UDP port the sync MCU listens on (ICD-sync-link, TBC) |
| `ARC_SYNC_LINK_PORT_TX` | 52001 | UDP port the Orin listens on (ICD-sync-link, TBC) |
| `ARC_SYNC_LINK_PEER` | `10.73.1.20` | The Orin's address on the car's internal network (proposed, TBC; ICD-sync-link); `native_sim` sets `127.0.0.1` |

## Flash (from your own computer, not the container)

```bash
probe-rs download --chip STM32H723ZGTx firmware/sync-mcu/build/nucleo_h723zg/zephyr/zephyr.elf
```

## Rules

- **Safety code** (`app/src/safety.*`: the envelope and the heartbeat watchdog) needs its own tests and a second reviewer for every change.
- **Generated code** is never edited by hand. `tools/gen-interfaces.sh` (mavgen and cantools, in the dev container) makes the sync-link messages in `app/generated/arc_mavlink/` from `systems/icd/sync-link.xml`, with a copy for the ROS car backend, and the command-bus frames in `app/generated/can_command/` from `systems/icd/can-command.dbc`. `nx check sync-mcu` (and `nx check ros`) fail if any of it is stale.
