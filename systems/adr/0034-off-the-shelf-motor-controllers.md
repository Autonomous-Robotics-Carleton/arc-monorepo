# ADR-0034: Off-the-shelf motor controllers (A50S) on the deck; telemetry over UART; top speed 9 m/s

- **Status:** Accepted, amended by ADR-0035 (the telemetry reports each controller's state; the controllers brake on command loss). Set on 2026-10-08: telemetry link rate 2 Mbit/s, not 3 Mbit/s (ICD-controller-telemetry); T 2.4 s (TBC), since the ~1.8 s below is the stop time without the ramp and T must come after the stop (ICD-corner-connector)
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-01, SYS-04, SYS-05, SYS-06, SYS-07, SYS-13, SYS-19, SYS-24, SYS-25, SYS-29, RSK-02, RSK-03, RSK-11, RSK-12, RSK-18, RSK-19, RSK-20
- **Supersedes:** ADR-0011 (drive telemetry on two CAN-FD buses, and the FD hardware added to the VESC fork; its choice of the STM32H723 for the sync MCU stands), ADR-0004 (the corner's power and signal connectors)
- **Amends:** ADR-0006 (the controller leaves the corner), ADR-0009 (the command bus now runs on the deck), ADR-0012 (the hardware torque cut moves to the power board), ADR-0013 (now applies to the steering bus only), ADR-0024 (VESC telemetry path), ADR-0028 (VESC firmware targets), ADR-0019 (steering bus placement), ADR-0033 (our firmware changes)

## Context

ADR-0006 and ADR-0011 made each corner's motor controller a custom board: a fork of the VESC 6 open hardware with an added CAN-FD controller, a hardware e-stop timer into the gate driver, and FETs cooled through the motor mount. The electrical team would rather not design and lay out a motor controller. The requirements stay; the topology is open, including CAN-FD.

What a controller has to do, from the requirements and the motor (Castle 1010 4400 kV, 4-pole, Hall sensored, ADR-0026):

| Need | From |
| --- | --- |
| Hall-sensored FOC at high electrical speed: ~1.1 kHz electrical (67k eRPM) at 8 m/s with 13.5:1 gearing, ~1.7 kHz (100k eRPM) at 12 m/s | SYS-01, ADR-0026 |
| ~12 A phase current per corner at 1 g (~3.5 kg car), 4S | `budgets/power-scenarios.md` |
| A ramped brake on the e-stop line without Orin or sync-MCU software, then torque removed in hardware | SYS-05, ADR-0012 |
| Brake on its own on command loss | SYS-04, SYS-25 |
| Full status (~80 bytes: speed, currents, duty, temperatures, voltages, fault code) at 1 kHz, stamped at sampling, to the Orin within 2 ms p99 | SYS-06, SYS-07, SYS-24, SYS-29, ADR-0021, ADR-0024 |
| A datasheet at the very least; open source preferred | The team |

**Robotics servo controllers can't spin this motor.** moteus caps mechanical speed at 28,000 rpm (we need ~50,000 at 12 m/s) and electrical frequency at 4 kHz; the ODrive S1 recommends ≤ 700 Hz electrical. RC car ESCs (Castle, Hobbywing) have no CAN or telemetry interface. That leaves controllers running the open-source VESC firmware.

**Among VESC boards, the A50S V2.3c (Team Triforce) is by far the smallest:** 35.5 × 21 × 13.8 mm and 11 g, against 39 × 46 (Flipsky Mini 4.20), 65 × 40 (Flipsky Mini V6), 75 × 70 (Trampa VESC 6 MkVI, boxed), and 60 × 100 to 72 × 84 for the open-hardware Cheap FOCer 2 and Little FOCer. Its hardware config is in upstream VESC firmware (`hwconf/teamtriforceuk/a50s_v23c/`, GPL-3.0), so it runs the same firmware this repo vendors (ADR-0033). Triforce publishes no datasheet or schematic.

**Its limit is electrical speed, not current or voltage.** The A50S senses current with two low-side shunts and no phase shunts, so its control loop runs at half the switching setting: 12.5 kHz at its default 25 kHz. FOC is comfortable at ≥ 10 loop samples per electrical cycle (a rule of thumb, not a datasheet value). With 13.5:1 gearing and ~62 mm wheels:

| Car speed | Motor eRPM | Samples per electrical cycle (12.5 kHz loop) | Duty at nominal pack |
| --- | --- | --- | --- |
| 6 m/s | 50k | 15 | ~40% |
| 8 m/s | 67k | 11 | ~55% |
| **9 m/s** | **75k** | **10** | **~60%** |
| 12 m/s | 100k | 7.5 | ~80%, ~90% on a sagging pack |

Above ~9 m/s the samples per cycle drop below 10, and near 90% duty the two-shunt current sampling degrades as well. The VESC 6 (three phase shunts, a control loop up to 30 kHz, a DRV8301 that reports faults) handles 12 m/s comfortably, but four of them take ~7× the A50S's footprint. The team doesn't need to drive above ~32 km/h.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Keep the custom VESC fork (ADR-0006, ADR-0011) | Exactly what the requirements ask for; 12 m/s; designed to fit | A motor controller to design, lay out and bring up, with no bench testing before the single order |
| **B. A50S V2.3c on the deck; commands on classic CAN; telemetry over a UART per controller; our firmware; SYS-01 lowered to 9 m/s** | Smallest controller; no motor-controller design; runs our vendored firmware; removes the CAN-FD buses and their risks | Hardware not open and no datasheet; stable only up to ~75k eRPM; a single small vendor |
| C. Trampa VESC 6 MkVI on the deck | Open hardware, documented; three phase shunts and fault reporting; 12 m/s comfortable | ~7× the A50S's footprint for four; heavily oversized (80 A for a ~12 A need) |
| D. Stock firmware with LispBM scripts instead of our firmware | No fork to maintain | Can't stream full status over UART or stamp at sampling (scripts get a 100 µs tick); weakens ADR-0021 |

## Criteria

1. Keep every requirement, with SYS-01 set to what the team actually needs.
2. No motor-controller board to design.
3. Size: four controllers on the lower deck alongside the battery, compute and boards.
4. A datasheet or equivalent primary documentation; open source preferred.

## Decision

**Option B.**

| Item | Decision |
| --- | --- |
| Controller | **4 × A50S V2.3c, 12S version** (6–52 V; 20 A continuous without cooling, 40 A with heatsink, 80 A for 4 s), plus spares. Bought in one order |
| Location | **On the lower deck**, not on the motor. Each corner sends its three motor phases and its Hall sensor cable to the deck |
| Firmware | **Our build of the vendored VESC firmware** (ADR-0033), upstream target `a50s_v23c_12s`, plus our changes: the e-stop routine, UART telemetry, and the speed limit below. Per-unit current calibration lives in the board's EEPROM and survives reflashing |
| Commands | **The classic CAN command bus** (ADR-0009), 1 Mbit/s, stock VESC command frames, now laid out on the deck |
| Telemetry | **One UART per controller to the sync MCU**, point to point: full status at 1 kHz, each frame stamped with the controller's sample counter (ADR-0024's rules carry over: built at send time, high-priority thread, DMA). Link rate 3 Mbit/s (TBC), ≤ 50% load (SYS-24). Framing TBD; recommended: a MAVLink 2 message set, as on the sync link (ADR-0031), for its sequence numbers and CRC |
| Speed limit | **Maximum eRPM set in the controller to ~75k (TBC)**: ≥ 10 loop samples per electrical cycle, whatever is commanded. A limit on motor speed, so it holds at any gearing |
| E-stop, stage 1 | The e-stop line reaches each controller on a spare input (the PPM pin, TBC from Triforce's pinout). Our firmware ramps the brake on it (ADR-0012), with the MCU's internal pull-down so an open line reads as e-stop |
| E-stop, stage 2 | **The power board removes motor-bus power** after the delay T (TBC; ~1.8 s to stop from 9 m/s at ~5 m/s²), regardless of controller firmware. Replaces the on-board EN_GATE timer of ADR-0012 |
| SYS-01 | **Lowered from ≥ 12 m/s to ≥ 9 m/s** (~32 km/h) |

The decision is board-agnostic below the controller: if the A50S can't be supplied, a VESC 6 MkVI drops into the same design (same firmware, CAN bus, UART link and e-stop wiring), at the cost of deck space.

## Consequences

- **Removed:** the two CAN-FD telemetry buses, the MCP2518FD and transceiver on each corner, the corner-board layout, and the FET heat path into the motor mount. RSK-02, RSK-03, RSK-05 and RSK-12 close or shrink; RSK-11 changes form.
- **Sync MCU:** three FDCAN controllers, now one used for drive commands. The steering bus (ADR-0019) can move to a freed internal FDCAN instead of the MCP2518FD; that's the EE's call. Four more UARTs (the H723 has more than enough).
- **Corner swaps (SYS-13):** the controller stays on the deck, so swapping a corner means loading that motor's saved configuration (or re-running motor detection, about a minute). Still config only.
- **The corner connector** (ICD-corner-connector) changes completely: motor phases and the Hall cable cross the corner boundary; the controller's own connections are on the deck.
- **Wiring:** phase leads and Hall cables run from each corner to the deck, crossing the suspension. Hall cables and UART lines stay away from phase leads (`architecture.md` wiring rules).
- **Hardware fault reporting (SYS-19):** the A50S's FD6288Q gate driver doesn't report faults; overcurrent, voltage and temperature are still detected in firmware from the controller's own measurements.
- **Current resolution:** the A50S measures ±~165 A full scale, so the ~12 A signal is coarse; this affects force estimates from motor current (LIM-07).
- **Recovery:** in VESC firmware the running application receives firmware uploads, so a build that breaks USB and CAN needs SWD to recover. The A50S breaks SWD out on its signal connector (RSK-20).
- **Without a datasheet:** ratings come from Triforce's product page, the connector pinout from its pinout image (now in ICD-corner-connector), mechanical fit from its STEP models, and the electrical design (MCU pins, current sensing, voltage divider) from the upstream hardware config. Absolute maximum ratings and derating are unknown; the car runs far below the published ratings (~12 A of 20 A, 16.8 V of 52 V).
- **SYS-01 at 9 m/s** eases SYS-04 (stopping distance), the power scenarios, the bus clamp and the e-stop delay T.
- **New risks:** RSK-18 (top-speed control), RSK-19 (supply), RSK-20 (firmware recovery).
- **Reopen if:** the A50S can't be supplied in time, rig R2 shows unstable control below 9 m/s, or the team needs more than 9 m/s (then a VESC 6 or lower gearing).
