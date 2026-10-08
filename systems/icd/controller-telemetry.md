# ICD-controller-telemetry

- **Revision:** draft (2026-10-07)
- **Status:** Draft. Needs an owner on each side and their sign-off.
- **Side A:** motor controller firmware (`firmware/vesc/`, our VESC build on the A50S; E-50) (owner TBD)
- **Side B:** sync MCU firmware (`firmware/sync-mcu/`, the `controller_links` module; E-30) (owner TBD)
- **Traces to:** SYS-06, SYS-07, SYS-19, SYS-24, SYS-25, SYS-29, ADR-0021, ADR-0024, ADR-0031, ADR-0034

Each of the four motor controllers streams its full status to the sync MCU over its own UART (ADR-0034). Commands don't use this link: they stay on the classic CAN command bus (ADR-0009).

## Transport

| Item | Value |
| --- | --- |
| Link | One UART per controller, point to point: the A50S's TX/RX pins (ICD-corner-connector) to a sync MCU UART |
| Electrical | 3.3 V logic, each line twisted with GND |
| Rate | 3 Mbit/s (TBC), 8N1; 2 Mbit/s fallback |
| Load ceiling | ≤ 50% (SYS-24, a data link) |
| Encoding | MAVLink 2, the ARC message set in [`sync-link.xml`](sync-link.xml) (ADR-0031): one schema and one generated library for both links |

## Controller → sync MCU

| Message | Content | Rate | Timing |
| --- | --- | --- | --- |
| `ARC_MOTOR_STATUS` | Speed, tachometer, q/d/input currents, duty, bus voltage, FET and motor temperatures, fault code | 1 kHz | `time_ns` is the sample time in the **controller's** clock, taken when the frame is built (ADR-0024). The sync MCU maps it to its own clock and forwards the message to the Orin unchanged in shape |
| `ARC_LINK_STATUS` | Protocol version, receive gaps and rejects | 1 Hz | |

At ~60 bytes per `ARC_MOTOR_STATUS` frame (46-byte payload plus the 12-byte MAVLink 2 header and 2-byte CRC), 1 kHz is ~0.6 Mbit/s: ~20% of a 3 Mbit/s link, ~30% at 2 Mbit/s.

## Sync MCU → controller

| Message | Content | Rate |
| --- | --- | --- |
| Clock alignment | TBD: how the sync MCU maps the controller's clock to its own (ADR-0021, ADR-0024) | TBD |
| `ARC_LINK_STATUS` | As above | 1 Hz |

## Faults (SYS-19, SYS-25)

- Gaps in MAVLink's sequence numbers and rejected frames are counted on both sides and reported in `ARC_LINK_STATUS`.
- ~5 ms without a valid `ARC_MOTOR_STATUS` from any controller counts as losing that corner (SYS-25): all four brake.

## Open issues

1. Clock alignment method between each controller and the sync MCU.
2. Final link rate (3 Mbit/s TBC) once the harness is built.
3. Owners on both sides.
