# ADR-0040: We design the corners; only generic parts are bought

- **Status:** Accepted
- **Date:** 2026-10-09
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-01, SYS-02, SYS-06, SYS-10, SYS-12, SYS-13, SYS-15, SYS-16, ADR-0008, ADR-0014, ADR-0019, ADR-0026, RSK-04, RSK-21, RSK-22, MQ-5
- **Supersedes:** M1 in `mechanical/layout-brief.md` (custom chassis on off-the-shelf suspension parts), and ADR-0027 (never accepted)

## Context

M1 put our own chassis on a bought car's suspension parts: its arms, knuckles, hubs and shocks. Every RC car's corners are designed around that car's drivetrain and steering. Ours aren't like any of them:

- **A motor and two-stage gearbox at each wheel** (ADR-0026), where a bought car has a diff on the centreline. The driveshafts fix where each gearbox output has to be.
- **Encoders at every joint:** on each wheel's output shaft (E-22), at every suspension pivot (E-23) and on both steering axes (E-24, ADR-0008). Bought arms hinge on a bare pin, and bought steering blocks have nowhere to mount a magnet.
- **Our own steering actuator** with ≤ 0.1° backlash (ADR-0019). Bought steering geometry assumes a servo and bellcranks, and composite steering blocks with play.
- **Corner swap** (SYS-13): a corner's motor and gearbox come out as a unit.
- **The underfloor** (SYS-15, ADR-0014): the aero volume, the ride-height sensors and the ground-speed window.
- **Mass:** the car is expected to weigh 4 kg or more (TBC; the mass budget isn't done). Touring-car parts are sized for ~1.35 kg, and SYS-12 says the car will crash at up to 9 m/s.

So the bought corner parts would mostly need designing around, and a different brand doesn't change that.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Design the corners: uprights and steering knuckles, arms, mounts. Buy only generic parts** | The corner is built around our gearbox, encoders, steering and floor; strength sized for our mass | More design work; the knuckles, uprights and arms are machined parts in the single order, right first time (RSK-22) |
| B. Bought corners (M1, ADR-0027: XRAY touring, or Arrma 6S on-road) | Proven parts; less design | Every interface above is a workaround; touring parts under-built; XRAY not in stock in Canada |
| C. A central motor and diffs, to use a whole platform's corners | Least design | Gives up four independent wheels (ADR-0026) and the sandbox philosophy: hardware should never limit what software can try |

## Criteria

1. The corner takes our per-wheel drive, encoders, steering and corner swap without workarounds.
2. Strong enough for 4 kg or more, including crashes (SYS-12, SYS-16).
3. Bought parts in stock from suppliers in Canada (single order).
4. Precision for state estimation and control (ADR-0019, SYS-06, SYS-10).

## Decision

**Option A.** We design each corner's uprights and steering knuckles, arms and mounts. We buy only parts that don't depend on whose car they're in:

- **RC drivetrain and wheel parts: Arrma 6S, Traxxas as the fallback.** This covers shocks, CVD driveshafts with their axles and the diff outdrives that become our gearbox output cups, wheel hexes and nuts, and wheels and tyres. Arrma's 6S parts are built for ~5 kg at 130 km/h, they mate with each other, and they're in stock in Ontario (Great Hobbies, Big Boys With Cool Toys, checked 2026-10-09). Of the brands those shops carry, only Arrma and Traxxas have shocks and driveshafts of this size in stock; XRAY, Mugen, TLR and Tekno have almost none.
- **General mechanical parts from industrial suppliers,** sized by our design: bearings, hinge pins, rod ends and spherical bearings, fasteners. The steering links use precision rod ends (ADR-0019). Supplier TBD.

## Consequences

- **MQ-5 closes.** The parts list and Ontario stock are in [`bom/mechanical.md`](../bom/mechanical.md).
- **Machined parts in the order:** two front knuckles, two rear uprights, the arms and the mounts (RSK-22). They follow the fab gate and the printed-deck rules: bearing bores in aluminium, never printed.
- **The corner geometry is ours:** pick-up points, kingpin and steering axis, camber and toe, travel, and where each encoder's magnet sits. ICD-corner-mechanical defines it from the block layout.
- **Driveshafts:** the gearbox output position and the upright set each driveshaft's length. Pick a stocked length (Arrma 6S CVDs come in 44 mm, 94 mm and others; aftermarket in 124 mm) and place the gearbox output to suit, or have them made. The same CVD at all four corners if the layout allows.
- **Wheels:** ~Ø100 × 42 mm on a 17 mm hex (dBoots Hoons 42/100, belted on-road), so RSK-21 applies: ADR-0034's gearing and the power figures assume 62 mm wheels and ~3.5 kg. The wheel size is no longer fixed by a bought upright, so it can be revisited with the gearing.
- **Reopen if** the machined parts' cost or lead time doesn't fit the order, or the team can't design and check four corners before the order.
