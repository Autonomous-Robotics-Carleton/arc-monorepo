# Power scenarios (inputs for the power board design)

- **Status:** Draft. System-level estimates for the electrical engineer to check and refine. Rail sizing, regulator selection and fusing belong to the EE.
- **Loads:** `power.csv`
- **Traces to:** SYS-03, SYS-05, SYS-14, SYS-16, SYS-30, SYS-31, RSK-06

## Assumptions (TBC)

| Quantity | Value | Source |
| --- | --- | --- |
| Car mass | 4–5 kg (TBC) | Expected at 4 kg or more (2026-10-09); the mass budget (SYS-16) replaces it |
| Top speed | 9 m/s | SYS-01 (ADR-0034) |
| Wheel | ~Ø100 mm rolling, ~96 mm worn (TBC) | ADR-0040, MQ-6 |
| Gearing | 18:1 baseline: stage 1 16/64 × stage 2 ~4.5:1 | RSK-21, `architecture.md` |
| Grip on tile | ~1 g maximum acceleration | Typical for 1/10 rubber on clean tile; check on the floor |
| Drivetrain + motor + VESC efficiency | ~0.8 | Estimate |
| Pack | 4S LiPo, 14.8 V nominal, 16.8 V full, ~20 mΩ total resistance | Typical 1/10 hardcase |
| Low-voltage load | ~80–90 W peak (sum of `power.csv`) | Estimate |

## Scenarios

| Scenario | Battery-side estimate | What it sizes |
| --- | --- | --- |
| Bench on 19 V wall supply | ~90 W low-voltage peak | Wall brick (~150 W for margin), OR-ing parts |
| Idle on battery (logging, compute) | ~60–90 W, ~4–6 A | Idle run time |
| Average hard driving | ~260–440 W total: the driving part of the earlier 230–330 W estimate, scaled from 3.5 kg to 4–5 kg | Pack capacity: 10 min ≈ 43–73 Wh ≈ 2.9–4.9 Ah used. A 5,000 mAh 4S hardcase has no margin at the high end; ~6,000–6,500 mAh if it fits the bay (TBC) |
| **Peak: ~1 g acceleration at 9 m/s** | 39–49 N × 9 m/s ≈ 350–440 W at the wheels, ~440–550 W from the motor path, plus LV → **~36–43 A** | Pack C rating (trivial at 5 Ah), wire gauge, pack connector (ICD-battery-pack), the XT30 at each motor controller, anti-spark switch, distribution |
| Beyond grip | Wheelspin; no useful work | Enforced by the motor controllers' current limits. The old "~120 A peak" was motor capability, not what the car can use; `architecture.md` and BOM E-40 now size to ~36–43 A |
| **Phase current per corner at ~1 g** | 4–5 kg × 9.81 ≈ 39–49 N, ~10–12 N per wheel; × 0.05 m (100 mm wheel) ≈ 0.49–0.61 N·m; ÷ 18 gearing ÷ ~0.8 efficiency ≈ 0.034–0.043 N·m at the motor; ÷ Kt ≈ 60 / (2π × 4400 kV) ≈ 0.0022 N·m/A → **~16–20 A (TBC)**. Kt from kV is the ideal-motor estimate. It was ~12 A at 3.5 kg, 62 mm and 13.5:1 | Motor controller choice (ADR-0034), LIM-07's current resolution, the XT30 and phase wiring |
| **E-stop from 9 m/s, pack disconnected** | ~½ × (4–5) × 9² ≈ **160–200 J** over ~1.9 s, **~185–230 W peak** at brake onset (160 W at 3.5 kg, scaled with mass) | Bus clamp resistor (energy and peak), heatsink, threshold above 16.8 V |
| Voltage sag at peak | ~43 A × 20 mΩ ≈ 0.9 V | Brownout immunity of the LV rails (SYS-31) |
| Connect / disconnect | Motor controller input capacitors charging | Anti-spark / loop key (E-42) |
| Low battery | Pack approaching its cutoff | Clean Orin shutdown before cutoff (SYS-30) |

## Behaviour requirements for the power board (already decided)

- Per-rail fuse, switch and current sense; battery voltage and current monitoring; all logged (SYS-19).
- Wall/battery ideal-diode OR-ing with no reboot on handover (SYS-14); wall feeds LV only.
- Every regulator rated ≥ 30 V input, with TVS (RSK-06). Reverse-polarity protection.
- E-stop circuit sources ESTOP to the four motor controllers (ADR-0012, ICD-corner-connector rev E), and after the delay T (TBC 2.4 s, after the ~1.9 s stop) switches off the motor bus to them (ADR-0034); the switched steering output goes off at T + ~1 s (ADR-0019).
- Aero fan, when it exists, comes off the motor bus with its own controller and fuse (SYS-15).

## Still needed from others

- Mass and CG (SYS-16) to replace the 4–5 kg estimate.
- Steering motor peak current (from the actuator design).
- Datasheet currents for the Hokuyo, cameras and switch.
- Aero fan power (capstone).
