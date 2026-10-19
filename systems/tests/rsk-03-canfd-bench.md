# RSK-03: Steering CAN-FD harness qualification

- **Verifies:** RSK-03, SYS-19, SYS-24, SYS-25, ADR-0019, ADR-0038, ADR-0039, ADR-0034
- **Status:** Not started; the former drive-FD procedure is retired by ADR-0034. Only steering remains on CAN-FD.
- **Rig:** R2 motor bench with the final sync → moteus-c1 harness and loaded motors
- **Owner:** kunshpreet; separate safety reviewer TBD

## Goal and setup

Qualify the existing point-to-point steering link at **1 Mbit/s arbitration / 5 Mbit/s data with BRS**, full 1 kHz command/query traffic and measured CAN load ≤70%. Use the actual sync-side controller/transceiver, final harness, terminations and moteus-c1. Record clock/bit timing, delay compensation, grounding, physical routing, firmware and supplied power states. The external MCP2518FD remains the BOM baseline; moving to a freed native H723 FDCAN requires owner review.

## Procedure

1. Inspect the reviewed harness and termination at its two ends; confirm reference/ground design and exact transceiver variant. UART wiring is separate and has no CAN termination.
2. Run the steering traffic with the motor off, then repeat beside loaded drive motors in the installed harness arrangement. Use the approved secured bench and stop protection.
3. Record bus errors, lost/rejected replies, TEC/REC, bus-off, load, reply timestamps and ground-offset/waveform captures. Run the retained **≥1 h loaded soak** under SYS-24 / RSK-03.
4. Exercise reviewed disconnect, stale reply, fault and bus-off injections. Verify coordinated stop, steering-health drive refusal and latched reset under SYS-25 / ADR-0038 / ADR-0039. Separate safety tests and review remain required; no result is claimed here.
5. If margins fail, record the cause and review grounding, termination/tap, SIC/transceiver or harness rework before repeating. The operating steering baseline remains 1 / 5 Mbit/s; a changed rate/protocol needs its own owner decision.

## Pass and evidence

Zero error/lost frames over the loaded soak; CAN load ≤70%; correct operating rate/BRS and documented receiver/common-mode margins; fault/stop/reset behavior meets the referenced requirements. Populate the actual measured timing/error result and reviewer; nominal link capability is not acceptance.

| Harness / power state | Rate / bit timing | Load / error counts | Waveform / ground margin | Fault-stop result | Reviewer |
| --- | --- | --- | --- | --- | --- |
| Final steering harness, loaded motors | TBD | TBD | TBD | Not run | TBD |

Drive telemetry now follows [UART qualification](a50s-arrival.md), not this procedure.
