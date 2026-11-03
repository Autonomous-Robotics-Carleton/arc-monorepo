# ICD-controller-telemetry

- **Revision:** B (2026-10-26)
- **Status:** Message set and IDs reviewed 2026-10-26 by the owner of both sides (#73). Firmware implementation, clock alignment (#74) and installed-harness qualification pending.
- **Side A:** motor controller firmware (`firmware/vesc/`, our VESC build on the A50S; E-50) (owner: Shrikar Vempati, until reassigned)
- **Side B:** sync MCU firmware (`firmware/sync-mcu/`, the `controller_links` module; E-30) (owner: Shrikar Vempati, until reassigned)
- **Traces to:** SYS-06, SYS-07, SYS-19, SYS-24, SYS-25, SYS-29, ADR-0021, ADR-0024, ADR-0031, ADR-0034

Each of the four motor controllers streams its full status to the sync MCU over its own UART (ADR-0034). Commands don't use this link: they stay on the classic CAN command bus (ADR-0009).

> [!NOTE]
> Both telemetry endpoints are stubs; see the [UART readiness review](../reviews/a50s-uart-readiness.md). Steering uses its own CAN-FD link (ADR-0019).

## Transport

| Item | Value |
| --- | --- |
| Link | One UART per controller, point to point: the A50S's TX/RX pins (ICD-corner-connector) to a sync MCU UART |
| Electrical | 3.3 V logic, each line twisted with a reviewed reference return; voltage/ground margins, return-current partition and unpowered-node behavior TBD under ADR-0041 |
| Rate | 2 Mbit/s, 8N1 (baseline, set 2026-10-08; installed-harness qualification pending). It divides exactly from the A50S's 42 MHz UART clock with upstream's 16× oversampling driver; 3 Mbit/s would need 8× oversampling set up outside that driver |
| Harness | Five conductors per controller (TBC): TX and RX, each with a reference return, plus a spare; return and reference design per ADR-0041 |
| Load ceiling | ≤ 50% (SYS-24, a data link) |
| Encoding | MAVLink 2, the ARC message set in [`sync-link.xml`](sync-link.xml) (ADR-0031): one schema and one generated library for both links |

## Controller → sync MCU

| Message | Content | Rate | Timing |
| --- | --- | --- | --- |
| `ARC_MOTOR_STATUS` | Speed, tachometer, q/d/input currents, duty, bus voltage, FET and motor temperatures, fault code, state (braking on e-stop or on command timeout, ADR-0035) | 1 kHz | `time_ns` is the sample time in the **controller's** clock, taken when the frame is built (ADR-0024). The sync MCU maps it to its own clock and forwards the message to the Orin unchanged in shape |
| `ARC_LINK_STATUS` | Protocol version, receive gaps and rejects | 1 Hz | |

At 59 bytes per unsigned, full-length `ARC_MOTOR_STATUS` frame (47-byte payload plus the 10-byte MAVLink 2 header and 2-byte CRC), 8N1 uses 590 wire bits per frame. At 1 kHz this is 0.59 Mbit/s: **29.5%** of each separate 2 Mbit/s link, with **295 µs** serialization time. Link status and any additional traffic must also be counted; this is calculated capacity, not a measured acquisition/forwarding latency.

## Sync MCU → controller

| Message | Content | Rate |
| --- | --- | --- |
| Clock alignment | TBD: how the sync MCU maps the controller's clock to its own (ADR-0021, ADR-0024) | TBD |
| `ARC_LINK_STATUS` | As above | 1 Hz |

## Faults (SYS-19, SYS-25)

- Gaps in MAVLink's sequence numbers and rejected frames are counted on both sides and reported in `ARC_LINK_STATUS`, as on the sync link (ICD-sync-link).
- A protocol version mismatch on any controller link is handled as on the sync link: logged as a fault, and the sync MCU refuses drive commands until it's fixed.
- The sync MCU stops all four corners (SYS-25, ADR-0035) when any controller has sent no valid `ARC_MOTOR_STATUS` for 5 ms (TBC), or reports a non-zero `fault` or any `state` bit (`ESTOP`, `COMMAND_TIMEOUT`).
- The controller sets `COMMAND_TIMEOUT` and brakes on the e-stop ramp after 150 ms (TBC) without a command on the command bus (ADR-0035).
- A corner reporting `ESTOP` while the sync MCU's e-stop status input shows the button released is logged as an ESTOP wiring fault (SYS-19).

## Open issues

1. Clock alignment method between each controller and the sync MCU.
2. ~~Link rate~~ **Set (2026-10-08): 2 Mbit/s baseline.** On the A50S, the UART (USART3) is clocked from APB1 at 42 MHz, and upstream's ChibiOS drivers use 16× oversampling: their maximum is 2.625 Mbit/s, and 2 Mbit/s divides exactly. Still to confirm on the built harness.
3. ~~Owners on both sides~~ **Set (2026-10-25):** Shrikar Vempati on both sides until reassigned.
4. ~~Corner~~ **Set (2026-10-26, #73):** from its VESC controller ID, 1–4 = FL, FR, RL, RR as in `can-command.dbc`; `corner` = ID − 1.
5. ~~MAVLink IDs~~ **Set (2026-10-26, #73):** system 1; the controllers are components 11–14 (FL, FR, RL, RR), so no two senders share an ID with the sync MCU (1) or the Orin bridge (2).
6. Final H723 UART/DMA pin map, A50S connector-contact verification and reference/grounding design. PB10/PB11 are the A50S's USART3 TX/RX.
7. The schema's `corner` enum is 0–3, while VESC command-bus IDs are 1–4. The explicit mapping must be implemented and verified; these values are not interchangeable.
