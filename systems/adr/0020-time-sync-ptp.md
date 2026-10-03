# ADR-0020: Time sync between the sync MCU and the Orin uses PTP with hardware timestamping

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-07, SYS-29, ADR-0016, ADR-0017

## Context

SYS-07 now requires ≤ 10 µs alignment between sensors (LiDAR ≤ 1 ms). Sensors are timestamped at the pin by the sync MCU and cameras are hardware-triggered, so the remaining error is the offset between the sync MCU's clock and the Orin's clock.

## Options

| Option | Expected alignment | Notes |
| --- | --- | --- |
| PPS wire into an Orin GPIO | Tens of µs | Limited by Linux interrupt latency, even with PREEMPT_RT |
| PTP in software timestamping mode | Tens to hundreds of µs | Network stack jitter |
| **PTP (IEEE 1588) with hardware timestamping on both ends, through a PTP-aware switch** | **~1 µs** | STM32H7 Ethernet MAC timestamps in hardware; Zephyr supports PTP; linuxptp on the Orin |

## Decision

- **PTP with hardware timestamping.** The sync MCU is the grandmaster (it owns the time base); the Orin's PHC is disciplined with linuxptp and the system clock follows it.
- **The Ethernet switch (E-06) must be PTP-aware** (IEEE 1588 transparent clock), so its queueing delay is corrected rather than added.
- **The PPS output is kept** as a cross-check and fallback (it's one pin).

## Consequences

- E-06 changes from an unmanaged switch to a small PTP-capable switch (~$100–250 more).
- **Check before the order:** the Orin NX's Ethernet supports PTP hardware timestamping on JetPack 7.2 (RSK-15).
- The PPS vs PTP open question is closed.
