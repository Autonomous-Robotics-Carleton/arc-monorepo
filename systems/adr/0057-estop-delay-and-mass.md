# ADR-0057: E-stop delay T of 2.8 s, sized from the speed cap; 4.2 kg nominal mass, 5 kg where it's the safe side

- **Status:** Accepted (2026-10-30)
- **Date:** 2026-10-30
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-01, SYS-05, SYS-16, SYS-22, E-41, ADR-0012, ADR-0034, ADR-0044, ICD-corner-connector
- **Amends:** ADR-0034 (T is 2.8 s, not 2.4 s) and ADR-0012 (T covers a stop from the speed cap, not from 9 m/s)

## Context

After the e-stop, the motor controllers brake on a ramp, and the power board cuts the motor bus after a delay T (ADR-0012, ADR-0034). T must come after the stop: cutting the bus while the controllers are still braking ends the braking, and the car coasts the rest of the way. It must also stay within SYS-05's time to remove torque (TBC 3 s).

T was set to 2.4 s (TBC) from a stop at 9 m/s, SYS-01's speed. But the controllers cap the motor at ~75k eRPM (ADR-0034), about 10.9 m/s with 18:1 gearing and a 100 mm tyre (`architecture.md`), and SYS-22's envelope can be raised toward that. From the cap, at ~5 m/s² with the ~150 ms ramp, the stop takes 0.075 + 10.9 / 5 ≈ 2.26 s. That leaves T = 2.4 s about 0.14 s, so the brake can be at most ~6% weaker than planned. Neither the deceleration nor the tyres' grip on the floor has been measured (the tyre compound for tile is TBD).

The car's mass is also open: TBC 4–5 kg (ADR-0044, `budgets/power-scenarios.md`), with no mass budget yet (SYS-16). The e-stop brake routine (#80) sizes its brake current from a mass; the gym (#87) simulates a third value. A brake current is a fixed force, so the deceleration it gives depends on the car's real mass.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Keep T at 2.4 s | Nothing changes | ~6% margin from the cap; a weaker brake than planned means the bus is cut with the car still rolling |
| **B. T at 2.8 s** | Covers a brake down to ~4 m/s² from the cap (~20% weaker than planned) | Less room under SYS-05's 3 s for the delay circuit's tolerance |
| C. Lower the eRPM cap | Shorter stops | Eats SYS-01's margin as the tyres wear (RSK-18, MQ-6); needs a new sizing of ADR-0034 |

## Criteria

1. From the speed cap, the car stops before the bus is cut, with margin for an unmeasured brake.
2. The cut stays within SYS-05.
3. Each use of the mass errs on the safe side until the car is weighed.

## Decision

**Option B.**

1. **T is 2.8 s (TBC),** sized for a stop from the speed cap, not from 9 m/s. Its tolerance (the delay circuit's parts and temperature, E-41) is TBD with the circuit: the longest delay must stay within SYS-05's 3 s, and the shortest must still come after the stop. For example ±5% gives 2.66–2.94 s.
2. **The nominal mass is 4.2 kg (TBC),** until the mass budget (SYS-16) or a weighed car replaces it. The range stays TBC 4–5 kg.
3. **Where the mass sizes something safety-related, use the end of the range that's on the safe side:**

   | Use | Mass | Why |
   | --- | --- | --- |
   | The e-stop brake current (#80) | 5 kg | Sized so the heaviest car still decelerates at the target ~5 m/s² |
   | The stop time checked against T | 5 kg | The heaviest car stops slowest under a fixed brake force |
   | Braking energy (the bus clamp, `budgets/power-scenarios.md`) | 5 kg | More mass, more energy to absorb |
   | Everything else (simulation, estimates) | 4.2 kg | The nominal car |

## Consequences

- **The nominal car brakes harder than the target.** The brake current sized at 5 kg gives the 4.2 kg car about 5 × 5 / 4.2 ≈ 6 m/s², so its tyres need a grip coefficient of about 0.61 on average, more at the rear, which carries less load under braking. The grip on the floor is measured before high-speed runs; if it's short, the brake current comes down (and the stop-time check against T is redone).
- **Stops from the cap:** ~2.26 s at 5 m/s² (the 5 kg car), against T = 2.8 s. A brake down to ~4 m/s² still stops before the cut.
- **The steering's cut moves with T,** to T + ~1 s (ADR-0019).
- **Every statement of T** (ADR-0012's and ADR-0034's status lines, ICD-corner-connector, ICD-power-rails, E-41, `budgets/power-scenarios.md`, the electrical handoff, the verification plan, `architecture.md` and the topology page) changes to 2.8 s. The e-stop routine's check of its values (#80) reads T from this ADR.
- **RSK-21's ~3.5 kg** (ADR-0034's old sizing) is still the figure it names; the nominal is now 4.2 kg.
- **Reopen if:** the measured deceleration or grip is far from 5 m/s² and ~0.6, the delay circuit can't hold T within SYS-05, or the weighed car falls outside 4–5 kg.
