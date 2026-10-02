# ADR-0011: High-rate VESC telemetry link

- **Status:** Proposed. **Open:** decide once the sync board design and the plan for future sensors and actuators are clearer.
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-06, SYS-24, SYS-19, RSK-02, RSK-03

## Context

SYS-24 asks for every sensor at its full useful rate. For the VESCs that means the full status frame (~80 bytes: speed, currents, duty, temperatures, voltages, fault code) at ≥ 1 kHz per corner. The single classic CAN bus (ADR-0009) can't carry that: at 1 Mbit/s, full status from four VESCs at 1 kHz is ~520% load.

Fork base under consideration: **VESC 6.4** (STM32F405, DRV8301, IRF7749, TJA1051T/3). The VESC 75/300 was rejected as oversized (300 A class, 18 FETs) for a ~30 A corner and the ~30 × 40 mm target. Both are stock VESC firmware targets.

## Options

| Option | Data at 1 kHz | VESC changes | Sync board | Connector | Notes |
| --- | --- | --- | --- | --- | --- |
| A. Classic CAN, one bus per VESC | Trimmed (~2 frames) | Maybe none (check the stock status rate limit) | 4 CAN controllers (2 external) | 6-pin | Loses most of the data |
| B. CAN-FD chip on each VESC (SPI controller + FD transceiver) | Full | External FD controller + crystal; new SPI driver and protocol routing | External FD controllers; still ~1 bus per VESC at 5 Mbit/s | 6–8 pin | RSK-03 (ringing) returns. Best if many other devices end up sharing an FD bus |
| C. One-way UART (RS-422), VESC → car | Full, up to ~2 kHz at ~3 Mbaud | RS-422 driver; firmware streaming mode | 4 UART RX + receivers | 8-pin | Requests, config and big dumps go over CAN, so mostly while parked |
| D. Two-way UART (RS-422) | Full, up to ~2 kHz | RS-422 driver + receiver; firmware streaming mode | 4 UART TX/RX + transceivers | 10-pin | Full VESC protocol at ~3 Mbaud. **Current recommendation** |

For every option: motor commands stay on classic CAN only, so only one path can command a motor.

## What would decide it

- **Sync board I/O budget:** how many UARTs, SPI ports and CAN controllers are left once the rest of the sensors are placed (BOM I/O tally).
- **Future bus devices:** if aero actuators or extra sensor boards are likely to share a CAN-FD bus, option B gets more attractive, though the sync MCU's spare FD controller may cover them anyway.
- **Firmware appetite:** C and D need a streaming mode in the fork; B needs more.
- **Corner packaging:** the connector grows to 8 or 10 pins for C and D.

## Requirements carried by any choice

- **Transceivers:** edge rates suited to the bit rate; extended common-mode range (about ±25 V) or isolation, since corner and sync grounds differ by volts at 120 A; fail-safe receivers (an open line reads idle); ±15 kV ESD on bus pins.
- **Frames:** a CRC on every frame (the VESC packet format already has CRC16); bad frames dropped and counted (SYS-19); a VESC sample counter in each frame for clock alignment (SYS-07).
- **Wiring:** twisted pairs with a ground return in the same connector, routed away from motor phase leads.
- **Definition:** a frame-definition ICD plus a decoder, from day one.
