# ADR-0024: Telemetry is stamped at sampling and forwarded within 0.25 ms; SYS-29 is measured at the driver process, p99

- **Status:** Accepted, amended by ADR-0034 (VESC telemetry travels over a UART per controller, not CAN-FD; stamping at sampling and the high-priority thread carry over)
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-06, SYS-24, SYS-29, ADR-0011, ADR-0016, ADR-0017, ADR-0019, RSK-03, RSK-12
- **Amends:** ADR-0011 (telemetry thread priority; frames stamped at sampling)

## Context

`budgets/latency.csv` gave VESC telemetry 1.5–2.5 ms from sampling to the Orin, but SYS-29 requires ≤ 2 ms. Breaking the path down shows the two largest terms are firmware assumptions, not hardware limits:

| Step | Previous assumption | Estimate |
| --- | --- | --- |
| VESC: sample → frame | up to one frame period stale | ≤ 1 ms |
| VESC: thread scheduling + SPI write to the FD controller | low-priority thread (ADR-0011) | 0.1–0.2 ms, and unbounded under load |
| CAN-FD bus: queueing at ~60% load + two 64-byte frames | `budgets/bus-load.csv` | 0.3–0.75 ms |
| Sync MCU: forwarding to the Orin | batched every ≤ 1 ms | ≤ 1 ms |
| Ethernet + switch + Orin kernel → driver process | PREEMPT_RT, reserved cores (ADR-0016) | 0.2–0.4 ms |
| **Worst case** | | **~3 ms** |

The stock VESC firmware isn't the limit: its status thread runs on a 10 kHz tick and VESC Tool allows status rates up to 10 kHz. The limit for stock status messages is the 1 Mbit/s classic bus, which is why ADR-0011 moved telemetry to CAN-FD.

SYS-29 also couldn't be verified as written. "Within ≤ 2 ms" reads as an absolute worst case, which Linux can't guarantee. And "reach the Orin" didn't say where on the Orin: the kernel, the driver process and a downstream ROS 2 node are different numbers. Delivery between ROS 2 processes over the default DDS typically adds 0.2–1 ms with heavier tails, and that belongs in SYS-06 (state-estimate age ≤ 5 ms p99), not in the hardware path.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Keep 2 ms; tighten the firmware; measure at the driver process, p99** | Meets SYS-29 with ~0.5 ms margin (estimate); no hardware change; a testable requirement | Firmware must be written deliberately (thread priority, forwarding path) |
| B. Relax SYS-29 to 2.5–3 ms | No firmware constraints | Spends SYS-06's budget for nothing; the loose terms would still be loose |

## Criteria

1. Hardware latency must not limit what software can do (SYS-29, mission 4).
2. The requirement must be testable (rig R5 in the verification plan).
3. Anything missed should be fixable in firmware, never by a respin (single order).

## Decision

Option A:

- **SYS-29** is measured to the sync-MCU driver process on the Orin, as p99: ≤ 2 ms for sync-board sensors, VESC telemetry and steering telemetry; ≤ 1 frame + 5 ms for frame cameras. Delivery to other processes counts against SYS-06.
- **VESC telemetry firmware** (amends ADR-0011):
  - Each telemetry frame is built from the motor controller's latest values at send time and stamped with the sample counter at that moment.
  - The telemetry thread runs at **high thread priority**, not low. Motor control runs in the ADC interrupt, which preempts every thread, so it stays protected (RSK-12). A low-priority thread could be starved for milliseconds by other VESC threads.
  - SPI transfers stay DMA-driven.
- **Sync MCU firmware** forwards every sample to the Orin within **≤ 0.25 ms** of receiving it, over UDP. No per-millisecond batching.
- **Steering telemetry** (moteus-c1, ADR-0019) follows the same forwarding rule. Its replies are built when the sync MCU's query arrives.

Estimated worst case after these changes: ~1.5 ms for VESC telemetry, under 1 ms for sync-board sensors and steering (`budgets/latency.csv`). These are estimates; rig R5 measures them.

## Consequences

- `requirements/system.md` SYS-29, the verification plan's SYS-29 row and `budgets/latency.csv` change to match.
- **Sync MCU firmware:** ~4,000 UDP packets/s carrying ~0.7 MB/s. This is modest for the STM32H723, but the Zephyr network path needs care: UDP only, and avoid copying buffers.
- **The margin depends on RSK-03.** If the CAN-FD data phase can't run at 5 Mbit/s, the telemetry buses exceed SYS-24's load ceiling before latency matters.
- **Reopen if:** rig R5 measures VESC telemetry above 2 ms p99 after firmware tuning, or SYS-06 can't be met with the default ROS 2 transport.
