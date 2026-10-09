# ADR-0023: No i210/i226 network card on the Orin carrier in v1; the sync MCU stays on the switch

- **Status:** Proposed (recommendation, not yet decided)
- **Date:** 2026-10-03
- **Deciders:** TBD (recommended by Shrikar Vempati; for review by the electrical engineer and the team)
- **Traces to:** SYS-07, SYS-24, SYS-29, ADR-0021, RSK-01, E-02, E-06, E-30
- **Amends:** ADR-0021 (drops its optional i210/i226 network card), once accepted

> [!IMPORTANT]
> **This is a recommendation, not a decision.** It answers an open question left by ADR-0021 so the carrier and sync-board designs can proceed. Push back on anything here; it becomes `Accepted` only after review.

## Context

ADR-0021 put every µs-critical sensor on the sync MCU's clock, so sensor alignment (SYS-07) no longer depends on the Orin's clock. It left one option open: an Intel i210/i226 network card on the Orin carrier fork (E-02), if the Orin NX has a spare PCIe x1 lane, wired point-to-point to the sync MCU. That would give the Orin's own clock hardware PTP (~1 µs) instead of software PTP (≤ 1 ms).

Keeping it open blocks:
- the sync MCU's Ethernet route: through the switch (E-06), or point-to-point to the card;
- the switch's port count;
- the EE's check for a spare PCIe lane.

The only things the Orin timestamps itself are the LiDAR (needs ≤ 1 ms, SYS-07) and operator and policy messages (~ms). Sync-board traffic uses < 10% of the 100 Mbit link (`budgets/bus-load.csv`).

The carrier fork is already the highest-ranked risk (RSK-01).

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Drop it for v1; sync MCU stays on the switch** | Simplest; nothing added to the RSK-01 board; the laptop reaches the sync MCU through the service port (E-09) | If a future sensor needs the Orin itself to timestamp at µs level, that waits for v2 |
| B. Fit the card; point-to-point to the sync MCU | µs-accurate Orin clock; a dedicated link for sync traffic | PCIe routing, controller and magnetics on the riskiest board; depends on an unconfirmed spare lane; the sync MCU leaves the switch |
| C. Footprint only (unpopulated) | Keeps the option open | Most of B's layout effort and risk, for a capability no requirement needs |

## Criteria

1. No current requirement needs it: SYS-07, SYS-24 and SYS-29 are all met without it.
2. Don't add work or risk to the carrier fork (RSK-01) with a single order and no respin.
3. "Hardware never limits software": weighed against 1 and 2. Future PTP-capable Ethernet sensors can still sync to the sync MCU's clock through the switch, so the limit is narrow.

## Decision

**Recommended, not decided:**

- **Option A.** No i210/i226 network card, populated or as a footprint, on the v1 carrier.
- The sync MCU connects through the Ethernet switch (E-06).
- The Orin's clock follows the sync MCU's by software PTP with the PPS cross-check, as in ADR-0021.

## Consequences

- **What closes:**
  - the "Optional i210/i226" line on E-02 and in the carrier handoff;
  - the EE's PCIe-lane check;
  - the open Ethernet item in the sync MCU I/O tally.
- **E-06** keeps its port list: Orin, LiDAR, sync MCU, service port, spare.
- **Ruled out for v1:** µs-level timestamps taken by the Orin itself.
- **Reopen if:** a v1 experiment needs the Orin to timestamp something at µs level that can't be routed through the sync MCU, or the EE finds the card nearly free on the carrier.
