# ICD-corner-connector

- **Revision:** E (2026-10-07). The motor controllers move to the deck (ADR-0034), so a corner sends only its motor phases and Hall sensor cable to the chassis; the controller's own connections are on the deck. Rev D (10-pin JST-GH with daisy-chained CAN-FD, ADR-0013), rev C, rev B and rev A are superseded.
- **Status:** Draft. Needs Molex pin numbering against Triforce's pinout layout, and sign-off from both owners.
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
| Hall sensor cable | The Castle 1010's 6-pin sensor cable (210 mm supplied), extended to the deck. Halls A, B, C, 5 V and GND; whether it carries a motor thermistor is undocumented (the 1010 isn't ROAR certified), so check on arrival; otherwise E-28 on the can. Routed away from the phase leads |
| Length | Corner to deck, `TBD` from the CAD layout; as short as the layout allows |

## Motor controller connections (on the deck)

The A50S V2.3c (ADR-0034) has an XT30 for power, an MR30 for the motor, a micro-USB for VESC Tool, and a 20-pin Molex Pico-Clasp for everything else. The harness side is the 501189-2010 receptacle crimp housing: 1.00 mm pitch, dual row, inner positive lock, −40 to +105 °C, with Molex 501193 gold crimp terminals (Molex's extraction tool for them is rated 28–32 AWG, so plan thin signal wire). Source: Triforce's product page and its pinout image (`Pinout_V2.3`, read 2026-10-07).

| Connection | Net | Electrical | Notes |
| --- | --- | --- | --- |
| XT30 | Motor bus | 4S: 12.0–16.8 V; transients up to the bus clamp threshold (TBD) | From the power board's switched motor bus (ADR-0034); cut by the e-stop delay. Triforce's supplied bulk capacitor goes on this cable as close to the board as possible |
| MR30 | Motor phases A, B, C | See above | |
| Micro-USB | VESC Tool | | Configuration and firmware upload |

**Signal connector, as laid out in Triforce's pinout image** (red-marked pins are 3.3 V max). Circuit numbers, provisional: Molex's drawing for 501189-2010 numbers the mating face with the latch on top as circuit 1 top-right, circuit 2 bottom-right, odd along the latch row and even along the other, counting leftwards. Assuming Triforce's image is that face, latch on top, position 10 is circuits 1 (top) and 2 (bottom) and position 1 is circuits 19 and 20. Verify on the first unit: the four corners are all GND, so check that 5 V sits at circuit 9 and 3.3 V at circuit 10 (if they're at 11 and 12, the image is mirrored):

| Position | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Circuits (provisional): top / bottom | 19 / 20 | 17 / 18 | 15 / 16 | 13 / 14 | 11 / 12 | 9 / 10 | 7 / 8 | 5 / 6 | 3 / 4 | 1 / 2 |
| Top row | GND | Hall 3 / CS | Hall 2 / MISO | Hall 1 / SCK | **Motor temp (3.3 V max)** | 5 V out | SWCLK | SWDIO | Servo / PPM | GND |
| Bottom row | GND | **SCK / ADC1 (3.3 V max)** | **MISO / ADC2 (3.3 V max)** | TX / SCL / MOSI | RX / SDA / NSS | 3.3 V out | CAN H | CAN L | Aux power in, 12–48 V | GND |

How the car uses it:

| Net | Use |
| --- | --- |
| CAN H / CAN L | Command bus (ADR-0009), twisted pair |
| TX / RX | Telemetry UART to the sync MCU, 2 Mbit/s (ICD-controller-telemetry), each twisted with GND (ADR-0034) |
| Servo / PPM | **ESTOP** input. Not marked 3.3 V max, so 5 V logic is acceptable per Triforce's marking (TBC with them); the controller MCU's pin (PB6 on the A50S, not the sync MCU's) is 5 V tolerant |
| Hall 1–3, Motor temp, 5 V, GND | From the motor's sensor cable. The 5 V output powers the motor's Hall sensors only; the controllers' 5 V outputs are never tied together (Triforce: "only use 1 BEC") |
| SWCLK / SWDIO | **SWD on the connector:** a bad firmware build is recovered with a probe through the harness, no soldering (RSK-20) |
| Aux power in | Optional logic supply. Feeding it from an always-on rail would keep the controllers' MCUs and telemetry alive after the e-stop cuts the motor bus: the EE's call |
| ADC1, ADC2, 3.3 V | Unused |

### ESTOP behaviour

Controlled braked stop, then a hardware torque cut (SYS-05, ADR-0012 as amended by ADR-0034).

- **Fail-safe:** our firmware enables the MCU's internal pull-down on the ESTOP input (TBC that the board adds no pull-up), so an unplugged connector, a broken wire or an unpowered e-stop circuit reads as e-stop.
- **Stage 1, braking (firmware):** when ESTOP is low, the controller ignores commands on the command bus and ramps brake current to the target deceleration until the wheel stops.
- **Stage 2, torque cut (hardware):** the power board removes motor-bus power after T, whatever the controller firmware is doing. **T = 2.4 s (TBC; set 2026-10-08):** the stop from 9 m/s at ~5 m/s² takes ~1.9 s with its ~150 ms ramp, and T adds ~25% margin, as ADR-0012 intended. Confirmed on rig R2. T must come after the stop: cutting the bus while the controllers are still braking ends the braking.
- **The motor-bus switch opens when unpowered,** so losing the e-stop circuit's power also gives the hardware cut.
- **A broken ESTOP wire to one controller** brakes that corner, which reports it in its telemetry (`ESTOP`, ADR-0035), and the sync MCU stops the car through SYS-25. It doesn't trigger the hardware cut: only the button does. Accepted as a trade-off (2026-10-07).
- **Release:** doesn't restart motion. The controllers wait for fresh commands once the motor bus is back.

## Termination

- **Command bus:** 120 Ω at the sync board and at the far end of the trunk (ADR-0009). Nothing populated on the controllers.
- **UART links:** point to point, no termination.

## Open issues

1. Confirm the provisional circuit numbers above on the first unit (5 V at circuit 9, 3.3 V at circuit 10).
2. ESTOP logic level (3.3 or 5 V; the PPM pin takes either) and driver: the power board sources it; the current per controller.
3. Whether the Aux power input is fed from an always-on rail (telemetry and logging survive the motor-bus cut).
4. The connector at the corner boundary for the phase leads and the Hall cable; lengths from the CAD layout.
5. Sync-board signal ground and power-board ground both reach battery negative through the motor bus, which forms a loop. Settle it in the grounding strategy before harness layout.
6. Brake ramp, target deceleration and delay T (ADR-0012).
