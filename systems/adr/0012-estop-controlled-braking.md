# ADR-0012: E-stop is a controlled braked stop, then a hardware torque cut

- **Status:** Proposed (mechanism); the requirement itself is decided
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-05, ICD-corner-connector, RSK-11

## Context

The e-stop must brake: the car may not roll freely to a stop. The braking also must not be immediate or violent. ICD-corner-connector rev B cut gate drive on e-stop, which makes the motors coast, so it doesn't meet this.

Industrial standards name the two behaviours (IEC 60204-1 stop categories): **Category 0** removes power immediately (coast); **Category 1** brakes under control, then removes power. This requirement is Category 1.

Braking a BLDC motor needs the gate driver on and the VESC's current control running. Braking can't be pure hardware the way a torque cut can. The design question is how to keep the hardware guarantee anyway.

## Options

| Option | Brakes? | Independent of Orin and sync-board software? | Notes |
| --- | --- | --- | --- |
| Cut gate drive immediately (rev B) | No, coasts | Yes | Fails the requirement |
| Hardware phase short (all low-side FETs on) | Yes, but abrupt and uncontrolled | Yes | Harsh at speed, large currents; hard to add to the DRV8301 |
| **E-stop line read by VESC firmware → ramped brake; hardware timer cuts gate drive after T seconds** | Yes, controlled | Yes: only the VESC's own firmware is involved, and the hardware cut happens regardless | Category 1 |
| Brake command sent over CAN | Yes | No: depends on the sync MCU | Fails SYS-05 |

## Decision

Category 1 stop, with the timer on each VESC board:

1. ESTOP_EN goes low: pressed button, broken wire, unplugged connector or unpowered e-stop circuit.
2. **VESC firmware** sees it on a GPIO and ramps brake current to a target deceleration (TBC ~5 m/s²) over a short ramp (TBD ~100–200 ms, so braking isn't immediate), holding the brake until the wheel stops. CAN commands are ignored while ESTOP_EN is low.
3. **A hardware delay on the VESC board** (RC + comparator or a small timer IC) removes EN_GATE after T seconds, regardless of firmware. T must cover a stop from top speed with margin: 12 m/s at 5 m/s² is ~2.4 s, so T ≈ 3 s (TBC).
4. Releasing the e-stop doesn't restart motion: the VESC re-initialises the DRV8301 and waits for fresh commands.

The timer sits on each VESC rather than on the power board, so the connector keeps a single ESTOP_EN line, and a broken wire still gives brake-then-cut.

## Consequences

- **The fallback is coasting.** If a VESC's firmware has hung, that corner coasts until the timer cuts it. RSK-11 tracks this.
- The VESC fork adds the timer circuit and an ESTOP_EN GPIO input. The firmware gets an e-stop brake routine, which is a safety function: it needs its own test cases, and changes to it get extra review.
- The heartbeat watchdog (SYS-04) and the VESC command timeout should use the same brake ramp, so every stop feels the same.
- Braking energy flows back into the pack, and into the bus clamp if the pack is disconnected. The clamp is sized for a full four-corner stop from top speed.
