# ICD-corner-connector

- **Revision:** C (2026-10-02). Adds the CAN-FD telemetry pair (ADR-0011). Rev B (6-pin: CAN, ESTOP_EN, spare) and rev A (`architecture.md`) are superseded.
- **Status:** Proposed. Needs sign-off from both owners.
- **Side A:** corner VESC board (owner TBD)
- **Side B:** chassis harness, sync board, power board e-stop circuit (owner TBD)
- **Traces to:** SYS-05, SYS-13, SYS-25, ADR-0004, ADR-0006, ADR-0009, ADR-0011, ADR-0012, RSK-03, RSK-05

Each corner has two connectors to the chassis: power, and signal. The wheel encoder and suspension pot do **not** pass through this interface; they wire directly to the sync board (ADR-0008).

## Power connector

| Item | Value |
| --- | --- |
| Connector | XT60 (TBC against final per-corner peak current) |
| Voltage | 4S LiPo bus: 12.0–16.8 V; transients up to the bus clamp threshold (TBD) |
| Current | Peak `TBD` A, continuous `TBD` A per corner (from `budgets/power.csv`) |
| Polarity | Keyed by the XT60 housing |

## Signal connector

Connector: JST-GH 8-pin (GHR-08V-S housing on the harness side; board header TBC: vertical or right-angle).

| Pin | Net | Direction (from VESC) | Electrical | Notes |
| --- | --- | --- | --- | --- |
| 1 | CAN_H | Bidirectional | Classic CAN, ISO 11898-2, 1 Mbit/s | Command bus. Twisted with pin 2. VESC built-in CAN + TJA1051T/3 |
| 2 | CAN_L | Bidirectional | | |
| 3 | GND | — | Signal reference | Command-bus return; separates the two pairs |
| 4 | FD_H | Output (telemetry) | CAN FD, ISO 11898-2:2016, 1 Mbit/s arbitration / 5 Mbit/s data | Telemetry bus (front or rear). Twisted with pin 5. Added FD controller + transceiver |
| 5 | FD_L | Output (telemetry) | | |
| 6 | GND | — | Signal reference | Telemetry and ESTOP_EN return |
| 7 | ESTOP_EN | Input | Logic high = run, low or open = e-stop. Level `TBC` (5 V, input 3.3 V-tolerant) | See below |
| 8 | SPARE | — | Not connected on either board | Wired through the harness (one spare conductor per run) |

### ESTOP_EN behaviour

Controlled braked stop, then a hardware torque cut (SYS-05, ADR-0012).

- **Fail-safe:** the VESC board pulls ESTOP_EN low on board. An unplugged connector, a broken wire or an unpowered e-stop circuit all count as an e-stop.
- **Stage 1, braking (firmware):** the VESC reads ESTOP_EN on a GPIO. When it's low, it ignores commands on the command bus and ramps brake current to the target deceleration until the wheel stops.
- **Stage 2, torque cut (hardware):** a delay circuit on the VESC board ANDs the delayed ESTOP_EN into the DRV8301 EN_GATE. After T (TBC 3 s), gate drive is off whatever the firmware is doing.
- **Release:** doesn't restart motion. The VESC re-initialises the DRV8301 and waits for fresh commands.

## Termination

No termination on any corner board, so all four boards stay identical.

- **Command bus:** 120 Ω at the sync board and at the far end of the harness trunk (ADR-0009).
- **Each telemetry bus:** split termination (2 × 60 Ω + capacitor) on the sync board and at the far VESC's harness connector. The near VESC sits on a stub ≤ 0.1 m (TBC by the RSK-03 bench test) (ADR-0011).

## Open issues

0. RSK-03 bench test at 5 Mbit/s on the real harness, before the VESC fork layout.
1. ESTOP_EN logic level and driver: who sources it (power board e-stop circuit), and the current per corner.
2. Sync-board CAN ground and power-board ground both reach battery negative through motor power, which forms a loop. Settle this in the grounding strategy before harness layout.
3. Brake ramp, target deceleration and delay T (ADR-0012).
4. XT60 rating against final peak current.
