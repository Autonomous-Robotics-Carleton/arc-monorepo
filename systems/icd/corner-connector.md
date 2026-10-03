# ICD-corner-connector

- **Revision:** B (2026-10-02). Rev A, the 6-pin layout with two CAN pairs and sync from `architecture.md`, is superseded per ADR-0006 and ADR-0009.
- **Status:** Proposed. Needs sign-off from both owners.
- **Side A:** corner VESC board (owner TBD)
- **Side B:** chassis harness, sync board, power board e-stop circuit (owner TBD)
- **Traces to:** SYS-05, SYS-13, ADR-0004, ADR-0006, ADR-0009, RSK-05

Each corner has two connectors to the chassis: power, and signal. The wheel encoder and suspension pot do **not** pass through this interface; they wire directly to the sync board (ADR-0008).

## Power connector

| Item | Value |
| --- | --- |
| Connector | XT60 (TBC against final per-corner peak current) |
| Voltage | 4S LiPo bus: 12.0–16.8 V; transients up to the bus clamp threshold (TBD) |
| Current | Peak `TBD` A, continuous `TBD` A per corner (from `budgets/power.csv`) |
| Polarity | Keyed by the XT60 housing |

## Signal connector

Connector: JST-GH 6-pin (GHR-06V-S housing on the harness side; board header TBC: vertical or right-angle).

| Pin | Net | Direction (from VESC) | Electrical | Notes |
| --- | --- | --- | --- | --- |
| 1 | CAN_H | Bidirectional | ISO 11898-2, 1 Mbit/s | Twisted with pin 2 |
| 2 | CAN_L | Bidirectional | ISO 11898-2 | |
| 3 | GND | — | Signal reference | CAN and ENABLE return |
| 4 | ESTOP_EN | Input | Logic high = run, low or open = gate drive disabled. Level `TBC` (5 V, input 3.3 V-tolerant) | See below |
| 5 | GND | — | Signal reference | ENABLE return, paired with pin 4 |
| 6 | SPARE | — | Not connected on either board | Wired through the harness (architecture rule: one spare conductor per run) |

### ESTOP_EN behaviour

Controlled braked stop, then a hardware torque cut (SYS-05, ADR-0012).

- **Fail-safe:** the VESC board pulls ESTOP_EN low on board. An unplugged connector, a broken wire or an unpowered e-stop circuit all count as an e-stop.
- **Stage 1, braking (firmware):** the VESC reads ESTOP_EN on a GPIO. When it's low, it ignores CAN commands and ramps brake current to the target deceleration until the wheel stops.
- **Stage 2, torque cut (hardware):** a delay circuit on the VESC board ANDs the delayed ESTOP_EN into the DRV8301 EN_GATE. After T (TBC 3 s), gate drive is off whatever the firmware is doing.
- **Release:** doesn't restart motion. The VESC re-initialises the DRV8301 and waits for fresh commands.

## Termination

No termination on corner boards. The 120 Ω terminators sit at the sync board and at the far end of the harness trunk (ADR-0009).

## Open issues

0. **Pending ADR-0011:** a high-rate telemetry link may add 2 pins (one-way UART, 8-pin) or 4 pins (two-way UART, 10-pin). Don't lay out the connector until ADR-0011 is decided.

1. ESTOP_EN logic level and driver: who sources it (power board e-stop circuit), and the current per corner.
2. Sync-board CAN ground and power-board ground both reach battery negative through motor power, which forms a loop. Settle this in the grounding strategy before harness layout.
3. Brake ramp, target deceleration and delay T (ADR-0012).
4. XT60 rating against final peak current.
