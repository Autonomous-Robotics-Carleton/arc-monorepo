# ADR-0022: Power-board data reaches the sync MCU digitally; low-battery shutdown is two-tier

- **Status:** Proposed (recommendation, not yet decided)
- **Date:** 2026-10-03
- **Deciders:** TBD (recommended by Shrikar Vempati; for review by the electrical engineer and the team)
- **Traces to:** SYS-19, SYS-30, SYS-31, SYS-32, E-30, E-41, E-46, RSK-06

> [!IMPORTANT]
> **This is a recommendation, not a decision.** It answers two open questions from `handoff/electrical.md` so the electrical engineer has a starting point. Circuit-level choices (parts, thresholds, bus speed) stay with the EE. Push back on anything here; it becomes `Accepted` only after review.

## Context

Two questions block the stacking-header pinout (ICD power-sync-stack) and part selection on the power board (E-41):

1. **How power-board data reaches the sync MCU (E-30).** The data: battery voltage and current; current on each low-voltage rail (compute, sensor, 5 V / 3.3 V) and on the switched steering output; fault flags (fuse or e-fuse trips, power-good); the four cell voltages from the per-cell monitor (E-46, ≥ 100 Hz to ±10 mV, SYS-32); e-stop status. All of it must be detected, timestamped and logged (SYS-19).
2. **Who drives the low-battery shutdown (SYS-30).** The car must warn, then shut the Orin down cleanly before the pack reaches its cutoff, and the NVMe must never lose power mid-write.

Two facts shape the answers:

- **The grounding strategy is still open** (handoff, cross-cutting). Analog signals that cross between the power board and the sync board share a ground with the motor current.
- **A LiPo hardcase has no protection circuit.** The VESCs stop the motors at low voltage, but nothing stops the low-voltage rails from draining the pack into damage. Something must cut low-voltage power in the end, and it must do so after the Orin has halted.

Hardware arrives in one order, so the design should carry rework-only fallbacks rather than rely on early bench tests.

## Options

### Data path

| Option | Pros | Cons |
| --- | --- | --- |
| A. Analog into the sync MCU's ADC | No digital parts on the power board; samples are on the sync MCU's clock | ±10 mV per cell is hard across two boards on a shared motor ground; ~9 analog header pins on a pin-limited MCU |
| **B. Digital monitor ICs on the power board, one bus to the sync MCU; fault pins as interrupts** | Digitised at the source, so inter-board ground noise doesn't matter; standard parts; few pins; fault edges still timestamped at the pin | Telemetry sample time is the read time (~1 ms uncertainty), acceptable for ≥ 100 Hz data |
| C. A small MCU on the power board sending packets | Most flexible | Another firmware, clock domain and failure mode on the board that should be simplest |

### Shutdown

| Option | Pros | Cons |
| --- | --- | --- |
| A. Sync MCU decides everything | Has the cell data and the time base; everything logged | If its firmware hangs, nothing protects the pack |
| B. Power board decides in hardware | Independent of software | Lowest-cell detection needs a comparator per cell; fixed thresholds; no warning stage; little visibility |
| **C. Two tiers: sync MCU soft shutdown, power-board hardware backstop** | Clean, logged shutdown in normal use; pack protected if software fails | A few more parts on the power board |

## Criteria

1. Reliability and fault visibility (mission 3, SYS-19): nothing silent, nothing that depends on one firmware being healthy to protect the pack.
2. Measurement accuracy that survives an unresolved grounding strategy (SYS-32).
3. Fewest new firmwares and clock domains.
4. Recoverable by rework if wrong (single order).

## Decision

**Recommended, not decided.**

**Data path: option B.**

- The power board carries a per-cell battery monitor IC (E-46) and digital current/voltage monitors (INA228 class or similar) for the battery and each rail.
- They share **one dedicated I2C bus** to the sync MCU over the stacking header. It isn't shared with the ride-height ToF bus; the STM32H723 has five I2C peripherals.
- Their alert and fault outputs, plus e-fuse fault and power-good lines, go to **sync MCU interrupt inputs**, so fault edges are timestamped at the pin.
- **No MCU on the power board.**
- **Fallback:** battery voltage and current are also routed as conditioned analog signals to spare stacking-header pins, with unpopulated divider and filter footprints on the sync board. That way the battery reading can be recovered over the ADC by rework if the digital path disappoints.

**Shutdown: option C, in three steps:**

1. **Warn.** At threshold W on the lowest cell, the sync MCU logs a warning and tells the operator.
2. **Clean shutdown.** At threshold S, the sync MCU requests an Orin shutdown in two ways: a dedicated GPIO into the carrier's power-button (or equivalent) input, and a message. When the Orin has halted, the power board removes the compute rail. The Jetson module provides a shutdown-request output for that handover; **the EE to confirm the signal and its behaviour in the Orin NX design guide.**
3. **Hardware backstop.** Independently of all software, a pack-undervoltage cutoff on the power board, set below S with hysteresis, disconnects the low-voltage rails to protect the pack. It should only ever fire if step 2 failed, and when it does, it's logged as a fault (SYS-19).

On wall power (SYS-14), OR-ing keeps the rails up and none of this triggers. Threshold values W and S and the cutoff level are `TBD` (EE, from the pack's datasheet and SYS-03).

## Consequences

- **ICD power-sync-stack** can be written:
  - one I2C bus;
  - N interrupt lines, N `TBD` (one per alert/fault source, plus spares);
  - e-stop status;
  - the shutdown-request GPIO;
  - the Orin shutdown handshake path;
  - the analog fallback pins.
- **Sync MCU I/O tally (`bom/electrical.md`):** the power-board data row becomes one I2C bus plus interrupt inputs; the shutdown signal is a sync MCU GPIO.
- **Firmware:** the sync MCU owns the low-battery policy (W, S, logging), and its unit tests cover it. That is the only new firmware. The power board stays firmware-free.
- **Reopen if:** the chosen cell monitor can't meet ±10 mV at ≥ 100 Hz over I2C, or the grounding decision makes analog clean enough to drop the ICs.
