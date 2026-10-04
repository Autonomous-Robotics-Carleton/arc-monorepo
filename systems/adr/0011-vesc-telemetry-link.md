# ADR-0011: VESC telemetry on two CAN-FD buses; commands stay on one classic CAN bus

- **Status:** Accepted, amended by ADR-0013 (daisy-chained FD buses, discrete MCP2518FD + SO-8 transceiver instead of the MCP251863, rework-only fallbacks) and ADR-0024 (telemetry frames stamped at sampling; telemetry thread at high thread priority, not low)
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-06, SYS-19, SYS-24, SYS-25, RSK-02, RSK-03, RSK-12
- **Amends:** ADR-0009 (the classic bus now carries commands and health status only)

## Context

SYS-24 asks for every sensor at its full useful rate. For the VESCs that means the full status frame (~80 bytes: speed, currents, duty, temperatures, voltages, fault code) at ≥ 1 kHz per corner. The single classic CAN bus (ADR-0009) can't carry that: at 1 Mbit/s, full status from four VESCs at 1 kHz is ~520% load.

Fork base: **VESC 6.4** (STM32F405, DRV8301, IRF7749, TJA1051T/3). The VESC 75/300 was rejected as oversized (300 A class, 18 FETs) for a ~30 A corner and the ~30 × 40 mm target. Both are stock VESC firmware targets.

## Options

| Option | Data at 1 kHz | VESC changes | Sync board | Corner connector | Notes |
| --- | --- | --- | --- | --- | --- |
| A. Classic CAN, one bus per VESC | Trimmed (~2 frames) | Maybe none | 4 CAN controllers (2 external) | 6-pin | Loses most of the data |
| B. **CAN-FD telemetry, two buses (front pair, rear pair)** | Full | FD controller + transceiver + 40 MHz crystal; SPI driver; telemetry over FD | STM32H723: 3 FDCAN covers command bus + 2 FD buses | 8-pin | ~60% per FD bus at 5 Mbit/s |
| C. One-way UART (RS-422) | Full, up to ~2 kHz at ~3 Mbaud | RS-422 driver; streaming mode | 4 UART RX + receivers | 8-pin | Requests and big dumps over the command bus |
| D. Two-way UART (RS-422) | Full, up to ~2 kHz | RS-422 transceiver; streaming mode | 4 UARTs + transceivers | 10-pin | Full VESC protocol at ~3 Mbaud |

## Criteria

Hardware reliability first; firmware effort is not a constraint.

| Factor | CAN-FD (B) | Two-way UART (D) |
| --- | --- | --- |
| Harness: conductors and connector contacts (vibration is the main failure mode on RC cars) | **Fewer:** one added pair per corner, 8-pin, two bus runs to the sync board | More: two added pairs per corner, 10-pin, four runs to the sync board |
| Error handling | **In hardware:** CRC, acknowledge, automatic resend, a faulty node takes itself off the bus | CRC in the packet only; a bad frame is lost |
| Diagnostics for SYS-19 | **Hardware error counters and bus-off state:** a live measure of link quality | CRC and framing error counts, frame-counter gaps |
| Dead-node detection | **Immediate**, from missing acknowledgements | After a timeout |
| Fault isolation | A shorted pair or lost termination takes out telemetry from two corners | **One corner only** |
| Signal integrity | 5 Mbit/s on a shared bus needs the right topology; must be proven on the bench (RSK-03) | **None to prove:** point-to-point, no stubs |
| Parts added on the VESC | Controller + transceiver in one package, plus a crystal | **One transceiver** |

A telemetry bus fault loses data, never control: commands stay on the classic bus, and SYS-25 turns any corner's loss of telemetry into a controlled stop of all four. The signal-integrity risk is retired early by a cheap bench test. The harness advantage can't be engineered back into UART later.

## Decision

Three buses, one job each:

| Bus | Type | Nodes | Carries | Expected load |
| --- | --- | --- | --- | --- |
| Command | Classic CAN, 1 Mbit/s | Sync MCU + all 4 VESCs | Motor commands (200–500 Hz) + one low-rate health status frame per VESC (~50 Hz) | ~15–25% |
| Telemetry front | CAN-FD, 1 Mbit/s arbitration / 5 Mbit/s data | Sync MCU + FL + FR | Full VESC status at ≥ 1 kHz | ~60% |
| Telemetry rear | CAN-FD, 1 / 5 Mbit/s | Sync MCU + RL + RR | Full VESC status at ≥ 1 kHz | ~60% |

On each VESC 6.4 fork:

- **Command bus:** the F405's built-in CAN controller and the existing TJA1051T/3. Stock VESC behaviour, untouched.
- **Telemetry bus:** an added CAN-FD controller with integrated transceiver (MCP251863 class) on SPI, with its own 40 MHz crystal. Telemetry firmware runs here only.
- **Rule:** motor commands go over the command bus only. The FD buses never carry torque or speed commands.

Sync MCU: **STM32H723** (3 FDCAN controllers: 1 classic command bus + 2 FD telemetry buses, no external CAN controllers).

## Requirements on the added FD hardware (VESC side and sync side)

| Requirement | Why |
| --- | --- |
| ISO CAN FD (ISO 11898-1:2015) | The H723 is ISO; non-ISO Bosch FD can't talk to it |
| Data phase ≥ 5 Mbit/s with transmitter delay compensation | Needed above ~2 Mbit/s |
| SPI ≥ 20 MHz, interrupt pin, DMA-driven in a low-priority thread | Must never delay the 20–30 kHz motor control interrupt |
| Transmit FIFO ≥ several frames | Firmware jitter must not drop frames |
| Hardware timestamp counter on receive and transmit | VESC clock aligned to the sync MCU with a periodic sync frame (SYS-07) |
| 40 MHz CAN clock; same clock and bit timing on every FD node | CiA 601 guidance for accurate sample points at 5 Mbit/s |
| Readable error counters and bus-off status | SYS-19 |
| 3.3 V logic; −40 to 125 °C (AEC-Q100) | Sits on a board whose FETs bolt to the motor mount |
| Transceiver rated for 5 Mbit/s CAN FD (ISO 11898-2:2016); SIC (CiA 601-4) if the RSK-03 bench test shows ringing | Ringing on a shared bus |
| Bus pins survive ≥ ±40 V; extended common-mode range; ≥ ±8 kV ESD | Shorts to the 16.8 V bus in a crash; ground shift at 120 A; hand-plugged connectors |
| Consider isolated transceivers if the grounding strategy calls for it | Removes the motor-ground loop |

## Topology rules

- **Command bus:** linear trunk; 120 Ω at the sync board and at the far end of the trunk, both off the corner boards (ADR-0009).
- **Each FD bus:** the sync board at one end (terminated on the board); the near VESC on a stub as short as possible (≤ 0.1 m TBC by the RSK-03 bench test); termination at the far VESC's harness connector, not on the board. Split termination (2 × 60 Ω + capacitor) on FD buses for EMC.
- **Load ceiling:** ≤ 70% on any CAN bus (SYS-24).

## Consequences

- **RSK-03 reopens:** bench-test both FD buses with the real harness at 5 Mbit/s before laying out the VESC fork.
- **RSK-12 (new):** the FD telemetry firmware on the VESC (SPI driver, DMA, routing) must not disturb motor control.
- **SYS-25 (new):** loss of commands or telemetry from any one corner triggers a controlled stop of all four.
- The corner connector becomes 8-pin (ICD-corner-connector rev C).
- Each FD bus gets a `.dbc`, the source for the frame layout and the decoder.
