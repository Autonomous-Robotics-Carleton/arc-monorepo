# ADR-0008: Wheel encoders and suspension pots wire directly to the sync board

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-06, SYS-07, SYS-10, RSK-02

## Context

ADR-0006 removed the corner sensor nodes. Each corner still has an AS5047 wheel encoder (on the wheel output shaft, in the gearbox housing) and a suspension pot, and they need a reader.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| Read on the custom VESC board, send over CAN | Short local wiring; stock VESC already sends ADC values over CAN | No sample timestamp unless the sync line goes to every VESC plus custom firmware; adds firmware to the riskiest custom board (RSK-02); uses the encoder header reserved for the commutation-encoder upgrade; sensor traffic shares the motor command bus |
| Wire directly to the sync MCU | Timestamped at the pin on the car's one clock; VESC firmware stays near stock; encoder header stays free; motor bus carries only motor traffic | Longer runs near motor phase leads; 4 SPI chip-selects and 4 ADC channels on the sync MCU |

## Criteria

1. Time alignment of the MPC's primary state inputs (SYS-06, SYS-07).
2. Keeping the custom VESC board's firmware close to stock (RSK-02).
3. Keeping the reserved commutation-encoder port free.
4. Wiring length and noise, which can be engineered around.

## Decision

The wheel encoders and suspension pots wire directly to the sync MCU.

- **Encoders:** read the AS5047 over SPI as an absolute angle, about 1 MHz SCK, one chip-select per corner, sampled at the SYS-06 rate. A noise glitch corrupts one sample instead of adding a lasting count error. At about 3,700 rpm peak wheel speed, the wheel turns about 22° per 1 ms sample, well inside the 180° limit for telling direction apart.
- **Pots:** twisted pair with a ground return, powered from the sync board's ADC reference (ratiometric), with an RC filter at the ADC input.
- **Routing:** both harness runs stay away from the motor phase leads.

## Consequences

- The sync board needs 4 encoder SPI chip-selects (on a shared or split SPI bus) and 4 pot ADC channels, plus spares under the "every row gets a spare" rule.
- Encoder and pot wiring does not pass through the corner connector, which carries only motor-controller signals.
- Assumes the gearboxes are inboard on the sprung chassis, so these runs don't cross moving suspension. If the CAD shows otherwise, reopen this.
- Reopen if SYS-07's tolerance turns out to be milliseconds, or if wiring at the corners becomes the binding constraint. Reading on the VESC is then the cheaper option.
