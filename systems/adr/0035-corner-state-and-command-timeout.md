# ADR-0035: Each corner reports its state, and the controllers brake on command loss

- **Status:** Accepted
- **Date:** 2026-10-08
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-05, SYS-19, SYS-25, ADR-0009, ADR-0012, ADR-0034, RSK-11, ICD-controller-telemetry, ICD-corner-connector
- **Amends:** ADR-0034 (the telemetry carries each controller's state; our controller firmware adds a command timeout)

## Context

The heartbeat watchdog (SYS-04) and the any-corner stop (SYS-25) both run on the sync MCU. Three faults get past them:

1. **The sync MCU hangs or resets.** Then only the controllers' own command timeout stops the car, and nothing sets it. Upstream VESC firmware defaults to 1000 ms and then 0 A of brake current (`applications/appconf_default.h`): about 9 m rolled at 9 m/s, then coasting. That fails SYS-04's 2 m roll limit and ADR-0012's "never free rolling".
2. **One controller stops receiving commands but keeps sending telemetry** (a failed transceiver or a broken stub on the command bus). Its telemetry looks healthy. After its timeout it brakes alone while the other three drive.
3. **The ESTOP wire to one controller breaks.** That controller brakes and ignores commands, but it's alive: its telemetry and CAN status keep arriving, so SYS-25 never trips. The trade-off accepted on 2026-10-07 (no per-line wire detection) assumed SYS-25 would stop the car here.

SYS-25's listed triggers can't separate one corner from another on the command bus. Every node that receives a CAN frame acknowledges it, so a command to a dead controller is still acknowledged by the other three. Bus-off is the sync MCU's own state. The CAN health status (STATUS_1) arrives every 20 ms, so it can't fit a 5 ms window.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. A state field in `ARC_MOTOR_STATUS`, and our own command timeout on the e-stop ramp** | Covers all three faults; detection at the 1 kHz telemetry rate; every stop uses the same ramp; no new hardware | A schema change; a second safety function in our controller firmware |
| B. STATUS_1 at ~500 Hz; upstream's timeout set short with a fixed brake current | No schema change | Command bus at ~59%; shows that a controller can transmit, not that it receives commands; doesn't see a broken ESTOP wire; a step in brake current, not a ramp |
| C. Per-line ESTOP wire detection on the power board | In hardware | Circuitry on every line; covers fault 3 only |

## Criteria

1. Every single-corner fault ends with all four corners braking on the same ramp.
2. A hung sync MCU still stops the car within SYS-04's limits.
3. No new hardware.

## Decision

**Option A.**

| Item | Decision |
| --- | --- |
| Command timeout | Our controller firmware brakes on the e-stop ramp (ADR-0012) when no command has arrived on the command bus for 150 ms (TBC, the same as SYS-04's watchdog). Replaces upstream's timeout behaviour |
| Corner state | `ARC_MOTOR_STATUS` gains `state`, a bitmask sent in every frame: `ESTOP` (the e-stop input is active) and `COMMAND_TIMEOUT` (braking on command loss) |
| SYS-25 triggers | The sync MCU stops all four corners when any one corner has sent no valid `ARC_MOTOR_STATUS` for 5 ms (TBC), reports `ESTOP` or `COMMAND_TIMEOUT`, or reports a non-zero fault code; and when the command bus itself fails (no acknowledgements, or bus-off) |
| Broken ESTOP wire | Stops the car through SYS-25, as the 2026-10-07 trade-off assumed. That trade-off stands: no per-line wire detection, and the hardware cut comes only from the button or the e-stop circuit losing power |

## Consequences

- **Schema:** `systems/icd/sync-link.xml` goes to version 2. The sync MCU, the controllers and the Orin bridge must be rebuilt together; a version mismatch is refused (ICD-sync-link).
- **Frame size:** `ARC_MOTOR_STATUS` grows by one byte to 59 bytes on the wire, ~30% of the 2 Mbit/s link.
- **A broken ESTOP wire becomes visible:** a corner reporting `ESTOP` while the sync MCU's e-stop status input says the button isn't pressed is a wiring fault, logged as such (SYS-19).
- **A hung sync MCU:** each controller times out on its own timer, within a few ms of the others, and brakes on the same ramp. The car rolls ≤ 150 ms (~1.4 m at 9 m/s) before braking, as SYS-04 requires.
- **The sync MCU must command continuously** while drive is enabled, zero included, so the timeout trips only on real loss.
- **A hung controller still coasts** until the motor-bus cut (RSK-11); its telemetry stops, so SYS-25 brakes the other three.
- **Safety functions:** the command-timeout routine and the SYS-25 triggers need their own tests and a second reviewer, like the e-stop routine.
- **Not decided:** also watching each corner's STATUS_1 on the command bus (~60 ms, TBC) as an earlier, independent check that it still receives commands. How a run resumes after a SYS-25 stop is TBD (sync MCU).
- **Reopen if:** rig R2 or the track shows false command timeouts at 150 ms.
