# ADR-0004: Separate power and signal connectors at each corner

- **Status:** Superseded by ADR-0034 (the motor controllers move to the deck, so no power or signal connector crosses the corner boundary; a corner sends its motor phases and Hall cable, ICD-corner-connector rev E). Earlier: signal pinout redefined in ICD-corner-connector rev B (ADR-0006, ADR-0009)
- **Date:** before 2026-09-23
- **Deciders:** TBD
- **Traces to:** SYS-13, RSK-05

## Context

Each corner module joins the car with a power connection and a signal connection.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| XT30(2+2) combined | One connector | 20 A max, 100 mating cycles |
| XT60-class power + 6-pin JST-GH signal | Current headroom, locking signal connector | Two connectors per corner |

## Criteria

Current rating against peak per-corner draw, and mating cycles for a module that gets pulled often.

## Decision

XT60-class for power, 6-pin JST-GH for signals.

## Consequences

- Six pins are all used (2 CAN pairs, GND, sync). That leaves no e-stop enable line and no spare conductor, both of which `architecture.md` requires elsewhere. See RSK-05; this ADR may need superseding once `icd/corner-connector.md` is closed.
- XT60 rating still to be confirmed against final per-corner peak current.
