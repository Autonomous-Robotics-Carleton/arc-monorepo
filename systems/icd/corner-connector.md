# ICD-corner-connector

- **Revision:** E (2026-10-07). The motor controllers move to the deck (ADR-0034), so a corner sends only its motor phases and Hall sensor cable to the chassis; the controller's own connections are on the deck. Rev D (10-pin JST-GH with daisy-chained CAN-FD, ADR-0013), rev C, rev B and rev A are superseded.
- **Status:** Draft. Needs pin numbers from Triforce's A50S pinout and sign-off from both owners.
- **Side A:** corner assembly: motor (E-51), gearbox (owner TBD)
- **Side B:** chassis harness, the motor controllers on the deck (E-50), sync board, power board e-stop circuit (owner TBD)
- **Traces to:** SYS-05, SYS-13, SYS-19, SYS-25, ADR-0008, ADR-0009, ADR-0012, ADR-0034, RSK-11

The wheel encoder and suspension angle sensor don't pass through this interface; they wire directly to the sync board (ADR-0008).

## At the corner boundary

Two connections per corner, both crossing the suspension.

| Item | Value |
| --- | --- |
| Motor phases | 3 leads, motor to its controller on the deck. Connector at the corner boundary `TBD` (the A50S end is an MR30) |
| Phase current | ~12 A per corner at 1 g (TBC, `budgets/power-scenarios.md`); bursts set by the controller's current limit |
| Hall sensor cable | The Castle 1010's 6-pin sensor cable (Hall A, B, C, 5 V, GND, motor temperature; pinout TBC from Castle), extended to the deck. Routed away from the phase leads |
| Length | Corner to deck, `TBD` from the CAD layout; as short as the layout allows |

## Motor controller connections (on the deck)

The A50S V2.3c (ADR-0034) has an XT30 for power, an MR30 for the motor, a micro-USB for configuration, and a 20-pin Pico-Clasp for everything else.

| Connection | Net | Electrical | Notes |
| --- | --- | --- | --- |
| XT30 | Motor bus | 4S: 12.0–16.8 V; transients up to the bus clamp threshold (TBD) | From the power board's switched motor bus (ADR-0034); cut by the e-stop delay |
| MR30 | Motor phases A, B, C | See above | |
| Pico-Clasp pin `TBC` | CAN_H | Classic CAN, 1 Mbit/s | Command bus (ADR-0009), twisted with CAN_L |
| Pico-Clasp pin `TBC` | CAN_L | | |
| Pico-Clasp pin `TBC` | UART_TX | 3.3 V UART, 3 Mbit/s (TBC) | Telemetry to the sync MCU, point to point (ADR-0034). Twisted with GND |
| Pico-Clasp pin `TBC` | UART_RX | 3.3 V UART | From the sync MCU (time sync, configuration). Twisted with GND |
| Pico-Clasp pin `TBC` | ESTOP | Logic high = run, low or open = e-stop. Level `TBC`: the input is an MCU pin, so 3.3 V unless Triforce confirms 5 V tolerance | On the PPM input (TBC) |
| Pico-Clasp pins `TBC` | Hall A, B, C, 5 V, GND, motor temperature | From the motor's sensor cable | |
| Pico-Clasp pins `TBC` | GND | Signal reference | |

### ESTOP behaviour

Controlled braked stop, then a hardware torque cut (SYS-05, ADR-0012 as amended by ADR-0034).

- **Fail-safe:** our firmware enables the MCU's internal pull-down on the ESTOP input (TBC that the board adds no pull-up), so an unplugged connector, a broken wire or an unpowered e-stop circuit reads as e-stop.
- **Stage 1, braking (firmware):** when ESTOP is low, the controller ignores commands on the command bus and ramps brake current to the target deceleration until the wheel stops.
- **Stage 2, torque cut (hardware):** the power board removes motor-bus power after T (TBC ~1.8 s to stop from 9 m/s at ~5 m/s², plus margin), whatever the controller firmware is doing.
- **A broken ESTOP wire to one controller** brakes that corner and stops the car through SYS-25, but doesn't trigger the hardware cut: only the button does.
- **Release:** doesn't restart motion. The controllers wait for fresh commands once the motor bus is back.

## Termination

- **Command bus:** 120 Ω at the sync board and at the far end of the trunk (ADR-0009). Nothing populated on the controllers.
- **UART links:** point to point, no termination.

## Open issues

1. Pin numbers on the A50S's 20-pin connector, from Triforce's pinout image.
2. ESTOP logic level and driver: the power board sources it; the current per controller.
3. The connector at the corner boundary for the phase leads and the Hall cable; lengths from the CAD layout.
4. Sync-board signal ground and power-board ground both reach battery negative through the motor bus, which forms a loop. Settle it in the grounding strategy before harness layout.
5. Brake ramp, target deceleration and delay T (ADR-0012).
