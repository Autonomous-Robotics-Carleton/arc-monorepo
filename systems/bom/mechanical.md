# Mechanical BOM (v1)

- **Status:** Draft. Functional BOM, like [`electrical.md`](electrical.md): what each part is, what it must do, and how it's made or bought. Part numbers come with the suspension family (MQ-5) and the detail design.
- **How it's made:** `Buy` (off the shelf), `Machine` (CNC service, from a drawing in `hardware/mechanical/<release>/drawings/`), `Print` (in-house). Bought and machined parts are in the single order and must be right the first time; printed parts can be reprinted.
- **Selection:** `Decided`, `Planned` or `TBD`, as in the electrical BOM. Motors and the steering controller are electrical items (E-51, E-31) and are listed there.
- Row IDs will be assigned when the rows firm up after the block layout.

## Suspension, steering and wheels (off the shelf, M1)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Suspension arms, upper and lower | 4 corners | Buy | TBD: suspension family (MQ-5) | Fit the track set by M2; mount to the tub through ICD-corner-mechanical | SYS-02, SYS-13 |
| Front steering knuckles | 2 | Buy | TBD (MQ-5) | Steering lock for the turning radius; room for an AS5047 knuckle encoder (E-24) | ADR-0019, RSK-04 |
| Rear uprights / hub carriers | 2 | Buy | TBD (MQ-5) | Accept the CVD and wheel bearings | ADR-0026 |
| Shocks | 4 | Buy | TBD (MQ-5) | Travel and ride height to suit M3 | SYS-15 |
| Steering linkage (rods, ball ends) | 1 set | Buy | TBD (MQ-5) | Backlash ≤ 0.1° at the wheels (ADR-0019) | ADR-0019 |
| Wheels and tires | 4 + spares | Buy | Planned: 1/10 touring, Ø64–65 × 24–26 mm, 12 mm hex; compound for tile TBD | Grip on the school floor | SYS-01 |

## Drivetrain (ADR-0026)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Stage 1 pinion + spur, mod 0.5, 80 teeth total | 4 corners, plus spare ratios | Buy | Planned: hardened steel pinions 14–20 T, off-the-shelf spurs 66–60 T (`architecture.md`) | Same bearing bores for every ratio | SYS-01 |
| Stage 2 spur pair, ~4.5:1 (~12/54) | 4 | Buy or machine | Planned: steel | Carries the highest torque; not printed | SYS-01 |
| CVD drive shafts | 4 | Buy | TBD: must match the knuckles and hubs (MQ-5) | Full steering lock and full bump without binding | ADR-0026, RSK-04 |
| Bearings | TBD | Buy | TBD from the gearbox layout | Bores held in aluminium | SYS-01 |
| Encoder magnets (diametric, for AS5047) | 4 wheel + 4 suspension + 2 knuckle | Buy | TBD | Magnet-to-chip gap per the AS5047 datasheet | E-22, E-23, E-24 |

## Steering mechanics (ADR-0019)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Gimbal-style brushless motor | 1 | Buy | TBD | ≥ 1.5 N·m at the output with the belt ratio (TBC) | ADR-0019 |
| Belt and pulleys, ~4–6:1 | 1 set | Buy | TBD | Zero backlash; lock to lock ≤ 0.1 s | ADR-0019 |
| Actuator mount | 1 | Machine or print | TBD | Stiff; beside the staggered front motors (RSK-04) | ADR-0019 |

## Machined aluminium (M4)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Gearbox bearing plates | 4 corners | Machine | TBD | Precise bearing bores and gear centres | ADR-0026 |
| Motor mounts (also the VESC FET heatsink) | 4 | Machine | TBD | Clamp the Ø28 mm can on its bolt pattern; FET side of the VESC bolts on | E-50, E-51 |
| Tub splice plates | TBD (if the tub is split) | Machine | TBD (MQ-1) | Join tub sections; may double as motor-mount plates | RSK-17 |

## Printed parts (M4)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Lower tub (one piece or 2–3 sections) | 1 | Print | Planned: PAHT-CF ribbed tub; material may change with the printers (MQ-1) | Battery bay, floor window and shroud, ToF pockets, cable channels; stiffness from ribs and walls | RSK-17, SYS-15 |
| Upper deck with hole grid | 1 | Print | TBD | Grid per ICD-deck-grid | SYS-24 |
| Sensor mounts | TBD | Print | TBD | Rigid stereo bar; vibration-isolated IMU mounts | SYS-24 |
| LiDAR and camera crash guard | 1 | Print | TBD | Survive the SYS-12 impact speed (MQ-4) | SYS-12 |
| Battery retention | 1 | Print | TBD | Quick swap; holds the pack in a crash | SYS-03 |

## Fasteners and inserts

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Heat-set inserts | TBD | Buy | Planned: brass, sized to the screws | Every threaded joint in a printed part (`layout-brief.md`) | RSK-17 |
| Screws, washers, nuts | TBD | Buy | TBD | Wide washers on printed joints | RSK-17 |
