# ICD-corner-connector

- **Revision:** D (2026-10-02). Daisy-chains the CAN-FD telemetry bus with an FD pair in and out (ADR-0013). Rev C (8-pin, single FD pair), rev B (6-pin) and rev A (`architecture.md`) are superseded.
- **Status:** Proposed. Needs sign-off from both owners.
- **Side A:** corner VESC board (owner TBD)
- **Side B:** chassis harness, sync board, power board e-stop circuit (owner TBD)
- **Traces to:** SYS-05, SYS-13, SYS-25, ADR-0004, ADR-0006, ADR-0009, ADR-0011, ADR-0012, ADR-0013, RSK-03, RSK-05

Each corner has two connectors to the chassis: power, and signal. The steering VESC (ADR-0018) uses the same two connectors: its FD pins (4–7) are not connected on the board and are left unwired in the harness, and its ESTOP_EN timer is longer (T_drive + ~1 s). The wheel encoder and suspension pot do **not** pass through this interface; they wire directly to the sync board (ADR-0008).

## Power connector

| Item | Value |
| --- | --- |
| Connector | XT60 (TBC against final per-corner peak current) |
| Voltage | 4S LiPo bus: 12.0–16.8 V; transients up to the bus clamp threshold (TBD) |
| Current | Peak `TBD` A, continuous `TBD` A per corner (from `budgets/power.csv`) |
| Polarity | Keyed by the XT60 housing |

## Signal connector

Connector: JST-GH 10-pin (GHR-10V-S housing on the harness side; board header TBC: vertical or right-angle).

| Pin | Net | Direction (from VESC) | Electrical | Notes |
| --- | --- | --- | --- | --- |
| 1 | CAN_H | Bidirectional | Classic CAN, ISO 11898-2, 1 Mbit/s | Command bus. Twisted with pin 2. VESC built-in CAN + TJA1051T/3 |
| 2 | CAN_L | Bidirectional | | |
| 3 | GND | — | Signal reference | Command-bus return; separates command and FD pairs |
| 4 | FD_IN_H | Bus | CAN FD, ISO 11898-2:2016, 1 Mbit/s arbitration / 5 Mbit/s data | Telemetry bus from the sync board side. Twisted with pin 5 |
| 5 | FD_IN_L | Bus | | |
| 6 | FD_OUT_H | Bus | Same net as pin 4, passed straight through on the PCB | Telemetry bus onward to the far VESC. Twisted with pin 7 |
| 7 | FD_OUT_L | Bus | Same net as pin 5 | At the far VESC: short pigtail to the harness terminator |
| 8 | GND | — | Signal reference | FD and ESTOP_EN return |
| 9 | ESTOP_EN | Input | Logic high = run, low or open = e-stop. Level `TBC` (5 V, input 3.3 V-tolerant) | See below |
| 10 | SPARE | — | Not connected on either board | Wired through the harness (one spare conductor per run) |

**On-board FD routing (VESC fork, ADR-0013):** pins 4↔6 and 5↔7 pass straight through as a ~120 Ω differential pair; the FD transceiver (separate SO-8, SIC drop-in) taps the pair within ~1–2 cm of the connector, so the stub is a PCB trace. Unpopulated footprints on the pair: split termination behind a solder jumper, common-mode choke (0 Ω bypass), ESD/TVS.

### ESTOP_EN behaviour

Controlled braked stop, then a hardware torque cut (SYS-05, ADR-0012).

- **Fail-safe:** the VESC board pulls ESTOP_EN low on board. An unplugged connector, a broken wire or an unpowered e-stop circuit all count as an e-stop.
- **Stage 1, braking (firmware):** the VESC reads ESTOP_EN on a GPIO. When it's low, it ignores commands on the command bus and ramps brake current to the target deceleration until the wheel stops.
- **Stage 2, torque cut (hardware):** a delay circuit on the VESC board ANDs the delayed ESTOP_EN into the DRV8301 EN_GATE. After T (TBC 3 s), gate drive is off whatever the firmware is doing.
- **Release:** doesn't restart motion. The VESC re-initialises the DRV8301 and waits for fresh commands.

## Termination

No termination populated on any corner board, so all four boards stay identical.

- **Command bus:** 120 Ω at the sync board and at the far end of the harness trunk (ADR-0009).
- **Each telemetry bus:** daisy-chained sync board → near VESC → far VESC. Split termination (2 × 60 Ω + capacitor) on the sync board, and in a sealed terminator on a short pigtail from the far VESC's FD_OUT pins (ADR-0013). The on-board footprint behind a solder jumper is the fallback.

## Open issues

0. RSK-03 bench test (`tests/rsk-03-canfd-bench.md`) before the VESC fork layout; record the measured stub and termination limits here.
1. ESTOP_EN logic level and driver: who sources it (power board e-stop circuit), and the current per corner.
2. Sync-board CAN ground and power-board ground both reach battery negative through motor power, which forms a loop. Settle this in the grounding strategy before harness layout.
3. Brake ramp, target deceleration and delay T (ADR-0012).
4. XT60 rating against final peak current.
