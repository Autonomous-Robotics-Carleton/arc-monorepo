# Mechanical CAD workflow (v1)

- **Status:** Draft (2026-10-03)
- **Applies to:** everyone doing mechanical design on the car. CAD tool and export rule from M5 in [`layout-brief.md`](layout-brief.md).

How the team designs the car in Fusion 360, how that work reaches this repo, and what has to be true before parts are ordered.

## Where things live

| What | Where | Notes |
| --- | --- | --- |
| Live CAD | Fusion 360 team hub *Autonomous Robotics Carleton*, project **ARC Car** | Fusion keeps the version history. Everyone works here |
| Released geometry | `hardware/mechanical/<release>/` | STEP of the full assembly and of each part we make; committed at a release tag (git LFS) |
| Drawings for machined parts | `hardware/mechanical/<release>/drawings/` | PDF with dimensions, tolerances, material and finish: what the CNC service quotes from |
| Decisions, envelopes, open questions | [`layout-brief.md`](layout-brief.md) | Decisions with real trade-offs become ADRs |
| Interfaces | ICD-corner-mechanical, ICD-deck-grid (`systems/icd/`) | The boundaries between people's work |
| Parts list | [`bom/mechanical.md`](../bom/mechanical.md) | Bought, machined and printed parts |
| Mass and CG | [`budgets/mass.csv`](../budgets/mass.csv) | Updated from the CAD at each milestone (SYS-16) |

## Hub structure

One top-level assembly, with each subsystem as its own design inserted by reference, so people can work in parallel without locking each other out:

```
ARC Car (project)
├── 00 Car                 top-level assembly: references only, no geometry
├── 01 Parameters          the shared dimensions below, as user parameters
├── 10 Corner FL / FR / RL / RR   motor, gearbox, CVD, encoders (FL/FR also steering)
├── 20 Lower tub           printed tub sections, splice plates, battery bay, floor window
├── 30 Upper deck          hole grid, compute bay
├── 40 Steering            actuator, belt, linkage
├── 50 Sensor mounts       LiDAR + crash guard, stereo bar, cameras, antennas
├── 60 Electronics         board envelopes (STEP from the EE's KiCad; the A50S motor controllers from Triforce's), harness routing space
└── 90 Purchased parts     motors, suspension parts, wheels, bearings: vendor models or envelopes
```

- **One owner per design.** Change someone else's design by asking them, or through the ICD.
- **Corners share one design** where they're identical; mirror rather than copy.
- **Purchased parts** use the vendor's CAD when it exists, otherwise an envelope box with the critical features: mounting holes, shafts, connector clearance.

## Shared dimensions are parameters

Anything still open, or used by more than one design, is a **user parameter** in `01 Parameters`, never a typed-in number:

| Parameter | Decision | Current value |
| --- | --- | --- |
| `wheelbase` | M2 | ~330–350 mm (start) |
| `track_front`, `track_rear` | M2 | TBD |
| `ride_height` | M3 | ~10–15 mm placeholder |
| `wheel_dia`, `wheel_width` | MQ-5, MQ-6 | 100, 42 mm (TBC) |
| `deck_grid_pitch` | ICD-deck-grid | TBD |
| `motor_stagger` | ADR-0026 | TBD from the corner layout |

Changing a parameter must regenerate the car without broken references. This is how M2 and M3 stay open while design continues.

## Naming

- **Designs and components:** `<number> <name>` as in the tree above. Electrical parts carry their BOM ID, e.g. `E-51 Castle 1010`, so the CAD, the BOM and the docs match.
- **Milestones:** name Fusion milestones after the release they feed, e.g. `block-layout-1`, `mech-r1`.

## Phases

1. **Block layout.** Boxes only, in the order in [`layout-brief.md`](layout-brief.md). Settles M2, RSK-04 and RSK-16, and gives the first mass and CG estimate.
2. **Interfaces.** Write ICD-corner-mechanical and ICD-deck-grid from the block layout before detailing either side. Get board outlines from the EE as STEP.
3. **Detail design.** Bought and machined parts first: they're in the single order and must be right the first time. Printed parts can be reprinted after the order.
4. **Release.** Fab gate (`reviews/fab-gate.md`), then export and commit (below).

## Releasing

1. All mechanical items in [`reviews/fab-gate.md`](../reviews/fab-gate.md) are checked: fit at full steering lock and full suspension bump and droop, and load cases written down for torque- and crash-loaded parts.
2. Set a Fusion milestone.
3. Export STEP for the full assembly and each part we make, and PDF drawings for machined parts, into `hardware/mechanical/<release>/`.
4. Update [`bom/mechanical.md`](../bom/mechanical.md) and [`budgets/mass.csv`](../budgets/mass.csv).
5. Commit, tag the release (e.g. `mech-r1`), and open a PR.

## What to check before ordering

- [ ] SYS-02: overall width 238–341 mm and length 454–654 mm, measured in the assembly
- [ ] SYS-15: aero volume reserved under the floor
- [ ] RSK-04: front corners clear at full lock and full bump, steering actuator included
- [ ] Each corner's phase leads and Hall cable reach the deck at full bump and full lock, with their connectors accessible (ICD-corner-mechanical, ADR-0034)
- [ ] The motor controllers' heatsinks have an airflow path and don't rest on the print (ADR-0034, `layout-brief.md`)
- [ ] Every machined part has a drawing, a material and a quantity (plus spares) in the BOM
- [ ] Every bearing bore and gear mesh is in aluminium, never printed (`layout-brief.md`, printed-deck rules)
