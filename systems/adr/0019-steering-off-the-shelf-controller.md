# ADR-0019: Steering actuator driven by an off-the-shelf controller (moteus-c1) on its own CAN-FD bus

- **Status:** Accepted, amended by ADR-0034 (the steering bus can use a freed internal FDCAN instead of the MCP2518FD; the EE's call) and ADR-0038 (a steering failure stops the car, and drive is refused until the steering is healthy). Since ADR-0034 the "fourth CAN-FD bus" below is the only one, with its physical layer in ADR-0036; the steering power cut at T + ~1 s is ~3.4 s with T at 2.4 s (TBC, ICD-corner-connector), not "about 4 s"
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-05, SYS-06, SYS-08, SYS-10, SYS-22, SYS-24, SYS-29, ADR-0012, ADR-0013
- **Supersedes:** ADR-0018 (no fifth VESC)
- **Amends:** ADR-0012 (steering on e-stop: return to centre under sync-MCU command, hold if the sync MCU is silent, power cut after the drive corners)

## Context

ADR-0018 drove the steering actuator with a fifth VESC 6.4 fork. The team doesn't want a fifth VESC. The steering requirements from ADR-0018 still stand (TBC from steering geometry and mass):
- lock to lock (~±25° at the wheels) ≤ 0.1 s
- ≥ 10 Hz position bandwidth
- ≥ 1.5 N·m at the steering output
- backlash ≤ 0.1° at the wheel
- angle and steering torque at ≥ 1 kHz

## Options

| Option | Speed | Torque reading | Backlash | Bus | Effort |
| --- | --- | --- | --- | --- | --- |
| A. Fast brushless RC servo | ~0.05 s/60° | Rough (rail current) | Internal gears | PWM | Drop-in |
| B. Integrated smart actuator (CubeMars AK60-6 class: 6:1 planetary, 9 N·m peak, position/speed/torque/MIT modes over CAN) | Fast | Motor current through planetary friction | Low, nonzero | CAN, vendor protocol | Mount + driver; heavier; costlier |
| **C′. Off-the-shelf controller (moteus-c1) + belt mechanism** | Fast | **Motor current through a low-friction belt** | **Zero** | CAN-FD | Actuator mechanics; no board design |

## Decision

Option C′. **Fallback: option B** if the belt mechanism doesn't fit the front of the car (RSK-04).

| Element | Choice |
| --- | --- |
| Controller | **mjbots moteus-c1** ($69): 38 × 38 × 9 mm, 8.9 g, 10–51 V, 20 A peak phase current, CAN-FD 5 Mbit/s, > 1 kHz command/telemetry |
| Motor + reduction | Gimbal-style brushless motor, ~4–6:1 belt (zero backlash, back-drivable) |
| Position | The moteus onboard encoder on the motor × belt ratio for commutation and the position loop. The steering-knuckle encoders on the sync board (E-24, one per front knuckle, added 2026-10-03) give absolute output angle at startup (written into the controller as its output position) and a continuous cross-check |
| Torque | Controller-reported torque from q-axis current, at ≥ 1 kHz. A model removes inertia, belt friction and cogging. A torque sensor in the link is a v2 option |
| Bus | **A dedicated fourth CAN-FD bus**, using the MCP2518FD footprint reserved on the sync board (ADR-0013), now populated. Steering traffic stays isolated from the drive buses |
| Power | From the motor bus through a **switched, fused output on the power board** that the delayed e-stop line turns off |

**E-stop and watchdog behaviour**

| Situation | Behaviour |
| --- | --- |
| E-stop pressed, sync MCU alive | The sync MCU sees the e-stop and commands a **rate-limited return to centre** (~0.3–0.5 s, TBC), then holds centre |
| Sync MCU silent (command timeout) | moteus `servo.timeout_mode` = 10: decelerate to zero velocity and **hold position**. This is the one case where steering holds instead of centring |
| Hardware cut | The power board removes steering power at T_drive + ~1 s (about 4 s), whatever the firmware is doing |
| Release | No restart until fresh commands arrive |

## Consequences

- No fifth VESC. Fork quantity returns to 4 corners plus spares.
- The command bus loses the steering traffic (back to ~27% load).
- The sync board populates the MCP2518FD and a CAN-FD transceiver for the steering bus. The spare-FD-bus reserve from ADR-0013 is used up; any further FD bus needs another external controller.
- The power board gains a switched, fused steering output tied to the delayed e-stop line.
- The steering encoder E-24 becomes part of the steering control path (absolute reference), not only a logged sensor.
- The moteus firmware is mjbots', not ours. The configuration (timeout mode, limits, belt ratio) is kept in version control.
