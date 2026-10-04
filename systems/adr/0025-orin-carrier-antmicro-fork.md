# ADR-0025: The Orin carrier is a fork of Antmicro's open Jetson baseboard

- **Status:** Accepted (backfilled)
- **Date:** before 2026-09-23 (original spec); recorded 2026-10-03
- **Deciders:** Shrikar Vempati (original spec)
- **Traces to:** SYS-07, SYS-08, SYS-14, SYS-24, ADR-0010, ADR-0016, ADR-0021, ADR-0023, RSK-01, E-02

> [!NOTE]
> **Backfilled.** The decision was made in the original spec, before this ADR process existed. The context, decision and consequences come from the current docs. **The options and criteria below are drafted from those docs, not recorded from the original reasoning: confirm or correct them.**

## Context

The Orin NX module (ADR-0010) needs a carrier board. The car asks the carrier for:

- four 22-pin CSI camera ports: forward stereo, quad side/rear, forward event camera, downward ground-speed event camera (SYS-08, ADR-0014);
- a trigger connector to the sync board, so the sync MCU can trigger and timestamp the cameras (SYS-07, ADR-0021);
- power from the power board's fixed compute rail, not PoE or a barrel jack (SYS-14);
- M.2 key M for a swappable NVMe and key E for the MT7922 Wi-Fi card (SYS-09, ADR-0015);
- GbE, and expansion GPIO for the hotspot button and LED;
- a size and connector layout that fit the car's compute bay.

## Options

*Drafted, see the note above.*

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Fork Antmicro's open-source Jetson Orin baseboard** | Open design files: ports, power input and connectors can be changed while starting from a proven layout | A custom high-speed board (CSI, PCIe, USB) to fab and bring up; our own device tree and camera driver work (RSK-01) |
| B. Off-the-shelf commercial carrier | Proven hardware and board support | Fixed ports: few offer four 22-pin CSI with trigger routing; power input and size rarely match the car |
| C. NVIDIA dev kit carrier in the car | Cheapest; already used as the bench reference | Too few camera ports for four cameras, no trigger connector, barrel-jack power, large |
| D. A carrier designed from scratch | Exact fit | The most design effort and risk of all |

## Criteria

*Drafted, see the note above.*

1. Four triggered camera ports and the sync-board trigger connector (SYS-07, SYS-08).
2. Power from the fixed compute rail (SYS-14).
3. Least new high-speed design work for a board that must work first time (single order).
4. Fit in the compute bay.

## Decision

Fork Antmicro's open Jetson Orin baseboard: four 22-pin CSI ports, a camera-trigger connector to the sync board, input from the fixed compute rail with PoE removed, M.2 key M and key E, GbE, and expansion GPIO for the hotspot button and LED.

## Consequences

- **The carrier is the car's top risk (RSK-01).** The fork, its device tree on JetPack 7.2 (ADR-0016) and the camera drivers all have to work before the car has compute or cameras. The Orin Nano dev kit takes the NX module, so cameras and software come up there first (ADR-0010).
- **E-02 and the carrier section of `handoff/electrical.md`** define what the fork must provide. Open there: CSI lane mapping, the trigger connector pinout (ICD carrier-sync), the device tree and cooling.
- **Anything added to the fork adds to RSK-01.** That was the main reason for recommending against the i210/i226 network card (ADR-0023, Proposed).
- **Reopen if:** RSK-01's dev-kit bring-up shows a camera kit can't run on JetPack 7.2 in time, or a commercial carrier appears that meets criteria 1 and 2. **Fallback: TBD.** An off-the-shelf carrier with fewer cameras is the obvious candidate, but it isn't decided.
