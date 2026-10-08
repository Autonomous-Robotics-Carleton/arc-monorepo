# sync-mcu

Zephyr firmware for the STM32H723 on the sync board (ADR-0017).

It owns the car's time base: µs-critical sensors are timestamped in this clock, and the Orin syncs to it in software (ADR-0021). It also runs the heartbeat watchdog and the safety envelope (SYS-04, SYS-22), drives the command CAN bus and the steering bus (ADR-0009, ADR-0019), reads each motor controller's telemetry UART (ADR-0034), and streams everything to the Orin over Ethernet: every sample is forwarded within ≤ 0.25 ms of arriving, over UDP, with no per-millisecond batching (ADR-0024, SYS-29).

Pin and I/O tally: `systems/bom/electrical.md`. Board handoff: `systems/handoff/electrical.md`. How it fits the rest: `systems/software/architecture.md`.

## Layout

| Path | What |
| --- | --- |
| `west.yml` | Pins Zephyr (v4.4.2) and its modules. The dev container bakes this workspace in; nothing is fetched by hand |
| `app/` | The application. One module per job, all stubs: `time_base`, `sensors`, `can_buses`, `controller_links`, `safety`, `sync_link`. The `safety` stubs are fail-safe (nothing they guard can move the car) |
| `boards/arc/arc_sync/` | Our board definition (skeleton: pins, crystal and package TBD from the EE) |
| `tests/` | Tests that run on `native_sim` with twister |

## Targets (ADR-0028)

| Target | Board | Status |
| --- | --- | --- |
| Simulation | `native_sim`: runs as a program on a laptop or in CI | Builds and runs |
| Development hardware | `nucleo_h723zg`: same MCU, Ethernet and FD-CAN | Builds; no board owned yet |
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

## Flash (from your own computer, not the container)

```bash
probe-rs download --chip STM32H723ZGTx firmware/sync-mcu/build/nucleo_h723zg/zephyr/zephyr.elf
```

## Rules

- **Safety code** (`app/src/safety.*`: the envelope and the heartbeat watchdog) needs its own tests and a second reviewer for every change.
- **Generated code** is never edited by hand. The sync-link messages in `app/generated/arc_mavlink/` come from `systems/icd/sync-link.xml` (`tools/gen-sync-link.sh`); `nx check sync-mcu` fails if they're stale. CAN frames will come from `can-command.dbc`.
