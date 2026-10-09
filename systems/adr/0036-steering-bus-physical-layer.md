# ADR-0036: Steering CAN-FD bus physical layer: discrete transceiver, rework-only fallbacks

- **Status:** Accepted
- **Date:** 2026-10-08
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-19, SYS-24, ADR-0019, ADR-0034, RSK-03
- **Supersedes:** ADR-0013. Its rules for the two drive telemetry buses no longer apply (ADR-0034); the parts that still matter carry over here for the steering bus.

## Context

ADR-0013 set the physical layer for two CAN-FD drive telemetry buses at 5 Mbit/s: daisy-chained through the VESC fork, terminated in the harness, discrete transceivers with rework footprints, and a bench test before the VESC layout. ADR-0034 removed those buses and the VESC fork. The one CAN-FD bus left is the steering bus (ADR-0019): the sync board and one moteus-c1, with ~12% load (`budgets/bus-load.csv`), on the MCP2518FD or a freed internal FDCAN (the EE's call, ADR-0034).

ADR-0013's goal still holds for this bus: every plausible bench-test failure is fixed by a parts swap, a jumper or a harness change, never a board respin. Most of its decisions (the daisy-chain, the VESC fork's layout rules, the trimmed telemetry frame, the spare MCP2518FD footprint) no longer have anything to apply to.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Carry over ADR-0013's sync-board rules: a discrete SO-8 transceiver, unpopulated rework footprints** | Every fallback is a parts swap or a jumper | A few unpopulated footprints on the sync board |
| B. One transceiver, no rework footprints | Fewer parts and less board area | A failed bench test means a respin |

## Criteria

1. A failed bench test is fixed without a respin.
2. Board area on the sync board.

## Decision

**Option A.**

1. **Point to point:** the sync board to the moteus-c1, terminated at both ends. The moteus end's termination is in the harness (TBC: whether the moteus-c1 terminates on board).
2. **A discrete transceiver in the standard SO-8 CAN pinout** on the sync board, so a signal-improvement (SIC) transceiver (TJA1462 / TCAN1462 class) drops in.
3. **Unpopulated rework footprints** on the sync board's steering pair: split termination behind a jumper, a common-mode choke (bypassed with 0 Ω by default), and an ESD/TVS diode.
4. **The fallback ladder**, in order: SIC transceiver → split termination or the choke → a lower data rate (TBC that the moteus-c1 supports it; the traffic leaves room).
5. **Proof:** the restated RSK-03 test on the bench, before the sync board's layout freezes.

## Consequences

- ADR-0013 is superseded; its daisy-chain, its harness pigtail terminator and its VESC fork layout rules are gone with the drive buses.
- The sync board keeps its SO-8 footprint and the three rework footprints on the steering pair (`handoff/electrical.md`).
- **Reopen if:** steering moves off CAN-FD, or a second CAN-FD bus returns.
