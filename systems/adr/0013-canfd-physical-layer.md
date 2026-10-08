# ADR-0013: CAN-FD telemetry physical layer: daisy-chained, discrete transceivers, rework-only fallbacks

- **Status:** Accepted, amended by ADR-0034 (the drive telemetry FD buses are gone; the transceiver choice and rework footprints apply to the steering bus only)
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-19, SYS-24, RSK-03
- **Amends:** ADR-0011 (FD bus topology and parts)

## Context

ADR-0011 put VESC telemetry on two CAN-FD buses at 5 Mbit/s. Rough judgment, not data: an outright failure at 5 Mbit/s on a two-node, ~1 m bus is unlikely (~1 in 10), and needing some tuning is fairly likely (~1 in 3). The predictable risk is ringing from the near node's stub; the one that has to be measured is EMI from the motors and ground shift at peak current. The goal is that every plausible bench-test failure is fixed by a parts swap, a jumper or a harness change, never a board respin.

## Decision

1. **Daisy-chain each FD bus through the VESCs.** The corner connector carries an FD pair in and an FD pair out, passed straight through on the VESC PCB with the transceiver tapped right next to the connector. The bus runs sync board → near VESC → far VESC; the near node's stub becomes a ~1 cm PCB trace. A VESC can also sit alone on its own bus, so one-bus-per-VESC is a harness change, not a board change. Connector grows 8 → 10 pins (ICD-corner-connector rev D).
2. **Far-end termination in the harness.** The far VESC's unused FD-out pins carry a short pigtail ending in a sealed split terminator (2 × 60 Ω + capacitor), the same harness-termination approach as the command bus. All boards stay identical. An unpopulated on-board split-termination footprint, behind a solder jumper, is the fallback.
3. **Discrete controller and transceiver, not the integrated MCP251863.** MCP2518FD controller on SPI plus a separate transceiver in the standard SO-8 CAN pinout. Signal-improvement (SIC) transceivers (TJA1462 / TCAN1462 class) then drop in if the bench test calls for them. Same on the sync board's FD transceivers.
4. **Unpopulated rework footprints** on the VESC fork and the sync board, on each FD pair: split termination (behind a jumper), a common-mode choke (bypassed with 0 Ω by default), and an ESD/TVS diode.
5. **VESC fork layout rules:** transceiver within ~1–2 cm of the connector; FD pair routed as a ~120 Ω differential pair over solid ground; no vias in the pair; kept away from phase outputs and switching nodes.
6. **Fallbacks in firmware and on the sync board:**
   - FD data rate configurable (2, 4, 5, 8 Mbit/s).
   - A trimmed telemetry frame ready in case a lower rate is forced.
   - An unpopulated MCP2518FD footprint on the sync board's SPI, so a fourth or fifth FD bus needs no new board.
7. **Prove it before the VESC fork is laid out:** LTspice simulation with the transceivers' IBIS models, then the bench test in `tests/rsk-03-canfd-bench.md` on breakout boards with a running motor beside the harness.

## Consequences

- Corner connector is 10-pin (rev D). The FD-out pins are unused at the far node apart from the terminator pigtail.
- One extra small chip per VESC (separate transceiver), plus unpopulated footprints.
- The fallback ladder, in order: SIC transceivers → shorter stub / tune termination → one FD bus per VESC (needs the extra sync-board controller). Dropping to 2 Mbit/s is not a fallback: two nodes at 1 kHz would load the bus to ~130%.
- The VESC fork layout waits on the RSK-03 result.
