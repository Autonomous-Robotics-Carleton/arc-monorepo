# ADR-0027: Suspension, steering and wheel parts from a competition 1/10 touring car (XRAY X4 family)

- **Status:** Proposed (recommendation, not yet decided)
- **Date:** 2026-10-03
- **Deciders:** TBD (recommended by Shrikar Vempati; for review by the mechanical team)
- **Traces to:** SYS-01, SYS-02, SYS-06, SYS-10, SYS-12, SYS-13, ADR-0019, ADR-0026, RSK-04, MQ-5

> [!IMPORTANT]
> **This is a recommendation, not a decision.** It answers MQ-5 in `mechanical/layout-brief.md` so the block layout can start from real parts. Prices, Canadian availability and the exact current model are to be checked before it's accepted.

## Context

M1 puts a custom chassis on off-the-shelf 1/10 suspension parts: arms, knuckles, hubs, shocks, wheels. It doesn't say whose. That choice fixes the geometry the block layout needs: wheel offset, the knuckle envelope for the steering linkage and the AS5047 knuckle encoder (RSK-04), the CVDs and how they meet our gearbox output (ADR-0026), and the shock mounts.

What matters, given the rest of the design:

- **Track and wheelbase come from our tub, not the kit.** A touring car's own width (~190 mm) doesn't matter: the custom tub and arm mounts set the track to fit SYS-02's 238–341 mm.
- **Each corner is fed from our own inboard gearbox** (ADR-0026). We need driveshafts (CVDs) and the matching output joint that we can buy separately, or machine to match.
- **Wheels:** 1/10 touring, Ø64–65 × 24–26 mm, 12 mm hex (layout brief).
- **Precision:** steering backlash ≤ 0.1° (ADR-0019), and accurate, repeatable geometry for state estimation and MPC (SYS-06, SYS-10).
- **Crashes will happen** (SYS-12), and spares come from one order: parts must be sold individually, and we buy spares up front.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Competition touring car parts (XRAY X4 family; Yokomo and others similar)** | Tight tolerances and aluminium hubs: suits the backlash and model-accuracy targets; CVDs (ECS) and every part sold individually; large aftermarket | Expensive spares (a pair of driveshafts ~US$49); built for carpet and asphalt racing, so crash spares are a real budget line |
| B. Budget touring car parts (Tamiya TT-02 family) | Cheap; sold everywhere in Canada; universal-shaft and double-cardan hop-ups exist, with a separate gearbox joint (Tamiya 53792 + 54477) | Plastic knuckles and arms with noticeable play: hard to meet ≤ 0.1° backlash; friction dampers stock |
| C. Mid-range touring (Tamiya TB-05 / TRF series) | Better precision than B, Tamiya's availability | Fewer individually sold precision parts than A; middle on every axis |
| D. Traxxas 4-Tec 3.0 | Durable; Traxxas parts common in North America | 54 mm wheels, not the 64–65 mm touring size: changes the wheel decision |
| E. Traxxas Slash 4x4 (the stock F1TENTH base) | Shares spares with the team's existing car; very durable | Short-course wheels and long-travel suspension, not suited to tile; not touring wheels |

## Criteria

1. Corner parts, including CVDs and the output joint, sold individually and compatible with our own gearbox.
2. Precision: backlash and repeatable geometry (ADR-0019, SYS-06, SYS-10).
3. Crash spares affordable and obtainable in Canada (SYS-12, single order).
4. Touring wheels, 12 mm hex.

## Decision

**Recommended, not decided:**

- **Option A.** Arms, knuckles, hubs, CVDs, shocks and steering parts from the current XRAY 1/10 touring platform (the X4, successor to the T4), with standard 1/10 touring wheels and tires.
- Our gearbox output carries the matching drive cup, bought from XRAY or machined to its drawing.
- **Buy crash spares in the order:** arms, knuckles, hubs and CVDs, at least one corner's worth.
- **If cost or availability rules A out, fall back to C, not B.** B's play works against the backlash target.

## Consequences

- **MQ-5 closes;** the block layout uses XRAY vendor CAD or measured envelopes for the corner parts.
- **ICD-corner-mechanical:** the corner's suspension mounts follow XRAY's arm and hinge-pin geometry, held by our aluminium plates.
- **The knuckle encoder (E-24)** needs a magnet mount on the XRAY knuckle's steering axis: a small custom part.
- **`bom/mechanical.md` and `cost.csv`:** suspension parts and spares get real prices once checked.
- **To check before accepting:** current model and part numbers, prices in CAD, a Canadian supplier, and the CVD length range against our corner layout.
- **Reopen if:** the block layout shows XRAY's knuckle can't fit the encoder and steering at full lock (RSK-04), or prices are well beyond the `cost.csv` range for suspension parts.
