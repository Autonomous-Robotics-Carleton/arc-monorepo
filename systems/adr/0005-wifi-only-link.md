# ADR-0005: Wi-Fi is the only wireless link

- **Status:** Accepted (backfilled)
- **Date:** before 2026-09-23
- **Deciders:** TBD
- **Traces to:** SYS-04, SYS-05, SYS-11, RSK-09

## Context

The car needs teleop, telemetry, log transfer and a remote stop.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Wi-Fi only, gamepad on laptop | One link, one command path, demonstrations recorded on the same path the autonomy uses | Wi-Fi jitter; false stops on long dropouts; no independent radio kill |
| Wi-Fi + separate RC radio / kill link | Independent remote stop | Second command path to arbitrate |

## Criteria

TBD: record the reasoning.

## Decision

Wi-Fi to one laptop; safety comes from the heartbeat watchdog on the sync MCU, the physical e-stop and the VESC command timeout.

## Consequences

- The only remote stop is losing the heartbeat. The physical e-stop is on the car, so someone must be able to reach it. Write down how that works at speed on the track.
- The watchdog timeout comes from SYS-04 and RSK-09 data, not a guess.
