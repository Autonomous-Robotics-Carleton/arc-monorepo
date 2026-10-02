# ADR-0010: Jetson Orin NX 16GB is the v1 compute; all inference runs onboard

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-08, SYS-03, SYS-16, SYS-17, RSK-01, RSK-08

## Context

The mission runs classical pipelines first, then VLA and other embodied-AI models. Running a learned policy is limited mainly by memory capacity and bandwidth, so a bigger module was considered.

## Options

Prices are NVIDIA 1,000-unit list prices after the July 2026 increase; single units cost more.

| Option | Memory / bandwidth | Size | Power | Price | Notes |
| --- | --- | --- | --- | --- | --- |
| Orin NX 16GB | 16 GB / ~102 GB/s | 69.6 × 45 mm | ~10–25 W (~40 W Super) | $899 | Fits the Antmicro baseboard fork; same footprint as Orin Nano, so the Traxxas dev kit is a bench reference |
| AGX Orin 64GB | 64 GB / ~205 GB/s | 100 × 87 mm | ~15–60 W | $2,999 | New bay and carrier |
| Thor T4000 | 64 GB / 273 GB/s | 100 × 87 mm | 40–70 W | $2,999 | New bay and carrier; Blackwell, FP4 |
| Orin NX + offboard GPU over Wi-Fi | — | — | — | GPU machine | Large models possible, but over a link that isn't reliable |

## Criteria

1. The classical stack comes first, and the NX covers it.
2. Carrier and bay effort: the NX keeps the Antmicro fork and the dev-kit bench reference.
3. Power and cooling inside a 4S car with a ≥ 10 min run time.
4. Cost.

## Decision

Orin NX 16GB for v1. All inference runs onboard; no offboard GPU machine.

## Consequences

- Learned policies in v1 are limited to what fits next to the platform software in 16 GB of shared memory: roughly sub-1B-parameter VLAs, or ~3B quantized to 4-bit, at a few Hz. 7B-class VLAs are out of scope for v1.
- Learned policies send targets to the classical controller (layered control); they don't need a fast loop.
- Platform software must leave headroom: memory and GPU limits on experiment processes (SYS-21).
- Reopen for v2, which needs a new carrier anyway. Thor T4000 is the better value than AGX Orin 64GB at the same price; the T2000 (due Q1 2027) is also worth comparing.
