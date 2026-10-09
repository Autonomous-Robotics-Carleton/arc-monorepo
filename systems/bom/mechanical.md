# Mechanical BOM (v1)

- **Status:** Draft. Functional BOM, like [`electrical.md`](electrical.md): what each part is, what it must do, and how it's made or bought. Part numbers come with the detail design; the bought corner parts are in ADR-0040.
- **How it's made:** `Buy` (off the shelf), `Machine` (CNC service, from a drawing in `hardware/mechanical/<release>/drawings/`), `Print` (in-house). Bought and machined parts are in the single order and must be right the first time; printed parts can be reprinted.
- **Selection:** `Decided`, `Planned` or `TBD`, as in the electrical BOM. Motors and the steering controller are electrical items (E-51, E-31) and are listed there.
- Row IDs will be assigned when the rows firm up after the block layout.

## Corners (ADR-0040)

We design the corners and buy only generic parts (ADR-0040).

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Front steering knuckles | 2 | Machine | Our design (ADR-0040) | Wheel bearings in aluminium bores; steering axis through two spherical joints; the knuckle encoder's magnet on that axis (E-24); a steering arm for the link | ADR-0019, E-24, RSK-04 |
| Rear uprights | 2 | Machine | Our design (ADR-0040) | Wheel bearings in aluminium bores; carry the CVD axle | ADR-0026 |
| Suspension arms, upper and lower | 4 corners | Machine (TBC) | Our design (ADR-0040) | Pick-up points per ICD-corner-mechanical; a magnet at each pivot for the suspension encoders (E-23); crash loads at the SYS-12 speed | E-23, SYS-12, SYS-16 |
| Suspension mounts | 4 corners | Machine | Our design (ADR-0040) | Bolt to the gearbox plates or the tub; hinge pins in reamed holes or bushings | SYS-13 |
| Shocks | 4 | Buy | Planned: Arrma 6S, 16 mm bore; 77 mm or 87 mm, set by our geometry | Travel and ride height to suit M3; springs for 4 kg or more | SYS-15, SYS-16 |
| Wheels and tires | 4 + spares | Buy | Planned: 17 mm hex, ~Ø100 × 42 mm belted on-road (dBoots Hoons 42/100); compound for tile TBD | Grip on the school floor | SYS-01 |
| Wheel hexes and hub nuts | 4 | Buy | Planned: Arrma AR310484 and AR320467 | Fit the CVD axle | SYS-01 |
| Steering links | 2 | Buy | TBD: precision rod ends from an industrial supplier | Backlash ≤ 0.1° at the wheels | ADR-0019 |
| Spherical joints, hinge pins, wheel bearings | TBD | Buy | TBD: industrial suppliers, sized by the design | For reference, Arrma's steering block takes 8 × 16 × 5 and 15 × 21 × 4 bearings | ADR-0040 |

### Bought RC parts and Ontario stock

Checked 2026-10-09 at [Great Hobbies](https://www.greathobbies.com/) (in stock or not; it doesn't show counts) and [Big Boys With Cool Toys](https://www.bigboyswithcooltoys.ca/) (units on hand; it takes orders at 0; — means not checked there). Prices in CAD, the lower of the two. Every part number is in Arrma's Infraction 6S V2 manual and exploded view (ARA7615V2), where these parts are used together. Lengths and quantities are TBC until the corner layout.

| Part | Pack | Packs (car + spares) | Great Hobbies | Big Boys | Price |
| --- | --- | --- | --- | --- | --- |
| AR310455 CVD driveshafts, 94 mm | 2 | 2 + 1 | In stock | 1 | $42.99 |
| AR310451 CVD axles 8 × 33.5 mm | 2 | 2 + 1 | Out | 0 | $33.99 |
| AR310452 CVD rebuild set | 2 | 1 | In stock | 0 | $17.99 |
| AR310439 diff outdrives, steel: the cup each gearbox output takes, or a pattern to copy | 2 | 2 + 1 | In stock | — | $23.99 |
| ARA330627 shocks, 16 mm bore, 77 mm | 2 | TBD | Out | 2 | $139.99 |
| ARA330628 shocks, 16 mm bore, 87 mm | 2 | TBD | Out | 1 | $139.99 |
| AR310484 wheel hexes, 17 mm, 16.5 mm deep | 2 | 2 + 1 | In stock | 0 | $17.99 |
| AR320467 hub nuts, aluminium | 4 | 1 + 1 | In stock | — | $28.99 |
| dBoots Hoons 42/100 belted tyres, mounted (ARA550062 / ARA550070) | 2 | 2 + 1 | In stock | 2 | $60.99 |

## Drivetrain (ADR-0026)

| Item | Qty | How | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| Stage 1 pinion + spur, mod 0.5, 80 teeth total | 4 corners, plus spare ratios | Buy | Planned: hardened steel pinions 14–20 T, off-the-shelf spurs 66–60 T (`architecture.md`) | Same bearing bores for every ratio | SYS-01 |
| Stage 2 spur pair, ~4.5:1 (~12/54) | 4 | Buy or machine | Planned: steel | Carries the highest torque; not printed | SYS-01 |
| Driveshafts | 4 | Buy | Planned: Arrma 6S CVDs at all four corners (length TBC with the layout); each gearbox output takes Arrma's steel diff outdrive or copies it (part numbers above) | Full steering lock and full bump without binding | ADR-0026, ADR-0040, RSK-04 |
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
| Motor mounts | 4 | Machine | TBD | Clamp the Ø28 mm can on its bolt pattern. No longer a heatsink: the motor controllers are on the deck (ADR-0034) | E-51 |
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
