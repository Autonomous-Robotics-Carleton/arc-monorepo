# ADR-0018: Steering is a VESC-driven brushless actuator with belt reduction

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-05, SYS-06, SYS-08, SYS-10, SYS-22, SYS-24, SYS-29, ADR-0011, ADR-0012
- **Amends:** ADR-0012 (steering has its own e-stop behaviour)

## Context

A hobby steering servo (~40–100 ms per 60°, ~3–5 Hz closed loop) is the slowest element in every loop the car closes. It also reports nothing beyond rail current. Steering should stop being the hardware limit, and it should report steering torque, because the tires' self-aligning torque shows how close the front tires are to their grip limit.

## Requirements (proposed, TBC from steering geometry and mass)

| Requirement | Target |
| --- | --- |
| Speed | Lock to lock (about ±25° at the wheels) ≤ 0.1 s |
| Closed-loop position bandwidth | ≥ 10 Hz |
| Torque at the steering output | ≥ 1.5 N·m |
| Backlash at the wheel | ≤ 0.1° |
| Data | Angle and motor current (steering torque) at ≥ 1 kHz |

## Options

| Option | Speed | Backlash | Torque reading | Data path | Effort |
| --- | --- | --- | --- | --- | --- |
| A. Fast brushless RC servo | ~0.05 s/60° | Internal gear train | Rough (servo rail current) | PWM in | Drop-in |
| B. Smart actuator (RMD class) | Fast | Planetary, low but nonzero | Motor current, filtered by planetary friction | Vendor CAN protocol | Mount + driver |
| **C. VESC-driven actuator** | Set by motor + reduction | **Zero (belt)** | **Motor current through a low-friction belt** | **Stock VESC frames on the command bus** | Actuator design; board and firmware already exist |

## Decision

Option C:

- **Motor:** gimbal-style brushless motor driving the steering through a ~4–6:1 **belt** reduction (zero backlash, back-drivable).
- **Sensor:** AS5047 on the output shaft. The VESC closes the position loop on it.
- **Board:** a fifth VESC 6.4 fork (same PCB) in position-control mode.
- **Fine current range:** the steering board is fitted with larger shunts and a matching AD8418 gain for ~±10 A full scale, so its current resolution is ~8× finer. This is a parts-list variant only; the PCB is unchanged. Shunts are sized for the steering motor's peak with margin, and overcurrent limits are configured to match.
- **Bus:** commands and telemetry on the **classic command bus only**. Stock VESC status frames (current/speed/duty + position) at 1 kHz plus commands at 500 Hz add ~32%, so the command bus totals ~58% (≤ 70%, SYS-24). The CAN-FD parts are left unpopulated on the steering board; the FD buses are unchanged.
- **Power:** from the motor bus (XT60), like the drive corners. The power board's dedicated servo rail is no longer needed.
- **E-stop and watchdog behaviour:**
  - On ESTOP_EN low, or on command timeout, the steering VESC **returns to centre at a limited rate** (~0.3–0.5 s, TBC) and holds centre.
  - It does **not** cut gate drive with the drive corners. Its own hardware timer cuts it after the drive corners: T_drive + ~1 s, about 4 s.
  - Release doesn't restart motion; it waits for fresh commands.

## Consequences

- The steering board is a BOM variant: different shunt and gain values, a longer e-stop timer RC value, and no FD parts fitted. It is labelled, and its VESC configuration is kept matched.
- Steering torque comes from motor current with a model that removes inertia, belt friction and cogging. A torque sensor in the steering link is a v2 option.
- The sync board loses its servo PWM output and servo-current ADC input.
- The steering actuator has to fit the front of the car alongside the staggered motors, gearboxes and CVDs (RSK-04). Fallback is option B.
