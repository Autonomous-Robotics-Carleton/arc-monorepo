# Power scenarios (inputs for the power board design)

- **Status:** Draft. System-level estimates for the electrical engineer to check and refine. Rail sizing, regulator selection and fusing belong to the EE.
- **Loads:** `power.csv`
- **Traces to:** SYS-03, SYS-05, SYS-14, SYS-16, SYS-30, SYS-31, RSK-06

## Assumptions (TBC)

| Quantity | Value | Source |
| --- | --- | --- |
| Car mass | ~3.5 kg | Guess until the mass budget (SYS-16) exists |
| Top speed | 9 m/s | SYS-01 (ADR-0034) |
| Grip on tile | ~1 g maximum acceleration | Typical for 1/10 rubber on clean tile; check on the floor |
| Drivetrain + motor + VESC efficiency | ~0.8 | Estimate |
| Pack | 4S LiPo, 14.8 V nominal, 16.8 V full, ~20 mΩ total resistance | Typical 1/10 hardcase |
| Low-voltage load | ~80–90 W peak (sum of `power.csv`) | Estimate |

## Scenarios

| Scenario | Battery-side estimate | What it sizes |
| --- | --- | --- |
| Bench on 19 V wall supply | ~90 W low-voltage peak | Wall brick (~150 W for margin), OR-ing parts |
| Idle on battery (logging, compute) | ~60–90 W, ~5–6 A | Idle run time |
| Average hard driving | ~230–330 W total | Pack capacity: 10 min ≈ 38–55 Wh ≈ 2.6–3.7 Ah, so ~4–5 Ah usable. A 5,000 mAh 4S hardcase is the starting point |
| **Peak: ~1 g acceleration at 9 m/s** | ~35 N × 9 m/s ≈ 315 W at the wheels, ~390 W from the motor path, plus LV → **~30–35 A** | Pack C rating (trivial at 5 Ah), wire gauge, XT60s (60 A class), anti-spark switch, distribution |
| Beyond grip | Wheelspin; no useful work | Enforced by the motor controllers' current limits. **The spec's "~120 A peak" looks like motor capability, not what the car can use; confirm** |
| **E-stop from 9 m/s, pack disconnected** | ~½ × 3.5 × 9² ≈ **140 J** over ~1.8 s, **~160 W peak** at brake onset | Bus clamp resistor (energy and peak), heatsink, threshold above 16.8 V |
| Voltage sag at peak | ~35 A × 20 mΩ ≈ 0.7 V | Brownout immunity of the LV rails (SYS-31) |
| Connect / disconnect | Motor controller input capacitors charging | Anti-spark / loop key (E-42) |
| Low battery | Pack approaching its cutoff | Clean Orin shutdown before cutoff (SYS-30) |

## Behaviour requirements for the power board (already decided)

- Per-rail fuse, switch and current sense; battery voltage and current monitoring; all logged (SYS-19).
- Wall/battery ideal-diode OR-ing with no reboot on handover (SYS-14); wall feeds LV only.
- Every regulator rated ≥ 30 V input, with TVS (RSK-06). Reverse-polarity protection.
- E-stop circuit sources ESTOP to the four motor controllers (ADR-0012, ICD-corner-connector rev E), and after the delay T (TBC ~1.8 s) switches off the motor bus to them (ADR-0034); the switched steering output goes off at T + ~1 s (ADR-0019).
- Aero fan, when it exists, comes off the motor bus with its own controller and fuse (SYS-15).

## Still needed from others

- Mass and CG (SYS-16) to replace the 3.5 kg guess.
- Steering motor peak current (from the actuator design).
- Datasheet currents for the Hokuyo, cameras and switch.
- Aero fan power (capstone).
