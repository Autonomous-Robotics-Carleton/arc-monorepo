# ADR-0001: Hokuyo UST-10LX over RPLidar A3

- **Status:** Accepted (backfilled)
- **Date:** before 2026-09-23
- **Deciders:** TBD
- **Traces to:** SYS-08, SYS-07, RSK-10

## Context

The car needs a 2D LiDAR for localization on an indoor track.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Hokuyo UST-10LX | 40 Hz, 0.25° resolution, Ethernet | 10 m range, cost, sourcing in Ontario |
| RPLidar A3 | Cheap, 25 m range | 10–20 Hz scan rate |

## Criteria

TBD: record whoever made the call. From the spec, scan rate appears to have decided it.

## Decision

Hokuyo UST-10LX.

## Consequences

- The car needs an Ethernet port for it (carrier or switch), and a sensor rail it accepts.
- `architecture.md` says "≤ 20 m range" for the LiDAR, but the UST-10LX is a 10 m unit. Fix the spec or confirm 10 m covers the track.
- Reopen if RSK-10 (sourcing) fails.
