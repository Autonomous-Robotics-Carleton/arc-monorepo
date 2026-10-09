# Mechanical layout brief (v1)

- **Status:** Draft; mechanical design starting 2026-10-03
- **How the work is done:** [`cad-workflow.md`](cad-workflow.md) (Fusion hub structure, parameters, releases). Parts: [`bom/mechanical.md`](../bom/mechanical.md). Mass: [`budgets/mass.csv`](../budgets/mass.csv)
- **Goal of the first pass:** a **CAD block layout**: every component as a simple envelope box, placed in the car. It settles wheelbase, track and length (RSK-16), front-corner fit (RSK-04), the battery and compute bays, deck heights, and a first CG estimate, before any part is designed in detail.

## Requirements that drive the mechanics

| Req | Mechanical consequence |
| --- | --- |
| SYS-01 ≥ 9 m/s | Gearing per `architecture.md`; stiff, aligned drivetrain |
| SYS-02 size box | Overall width 238–341 mm, **length 454–654 mm** (F1TENTH/Roboracer). A 1/10 touring car (~260 mm wheelbase) is far too short; even ~310 mm is borderline (RSK-16) |
| SYS-05 e-stop | Physical button reachable on the car while it's moving or just stopped |
| SYS-12 crash survival | Crash guard for the LiDAR, cameras and front corners (impact speed TBD) |
| SYS-13 corner swap | A corner (motor, gearbox) removable without disturbing the rest (time TBD); its phase leads and Hall cable unplug at the corner boundary (ADR-0034) |
| SYS-15 aero reserve | Flat underfloor volume kept for the suction fan (size TBD with the capstone) |
| SYS-16 mass/CG | Not limited, but tracked; heavy parts low |
| SYS-24, sensors | Clear fields of view, rigid camera mounts, vibration-isolated IMUs |
| ADR-0014 ground speed | A floor window + light shroud for the downward camera, with the ToF beside it |
| ADR-0015 ground link | Wi-Fi antennas high, clear of carbon; hotspot button and service Ethernet port on the chassis |
| ADR-0019 steering | Belt actuator + moteus-c1 at the front; AS5047 on both knuckles |
| ADR-0008 / BOM | AS5047 on each wheel output shaft (in the gearbox) and at each suspension pivot |

## Component envelopes

Model every component as a box first. TBD: needs a datasheet or drawing. TBC: from a vendor listing, or standing in for a part not yet chosen; check it against the vendor's CAD. Sources are below the table.

| Component | Qty | Envelope (mm) | Mass (g) | Placement constraint |
| --- | --- | --- | --- | --- |
| 4S LiPo hardcase (~5,000 mAh; E-40 not chosen) | 1 | 138 × 47 × 40–50 (TBC, two makers' packs) | 495–589 (TBC) | **Placed first:** low, central, quick swap; bay sized for the 50 mm pack |
| Castle 1010 motor | 4 | Ø28 × 58.4 (+ shaft) | 146.5 | Transverse, inboard, staggered fore/aft per axle |
| Gearbox (2 stages) + CVD | 4 | TBD from the CAD. Fixed by the gears: stage-1 shafts 20 mm apart; stage-2 shafts 33 × module apart; largest gears Ø34 (66 T stage-1 spur) and Ø56 × module (54 T) | TBD | Stage 1 swappable (80 teeth total, mod 0.5); stage 2 ~12/54 steel, module TBD (sized for the output torque); a 12 T pinion needs profile shift to avoid undercut; precise bearing bores; the output takes Arrma's steel diff outdrive (AR310439, ADR-0040); wheel-encoder magnet pocket |
| Motor controller, A50S V2.3c with heatsink (ADR-0034) | 4 | Board 35.5 × 21 × 13.8; heatsink case from Triforce's 3D models | 30 with the heatsink | On the lower deck, with airflow (sized once the deck is modelled); XT30, MR30, Pico-Clasp and micro-USB accessible |
| Steering actuator (gimbal motor + belt + moteus-c1 38 × 38 × 9) | 1 | TBD | TBD | Front, near the steering linkage; fits beside the staggered front motors |
| Orin NX + heatsink/fan (E-03) on the carrier | 1 | Carrier 120 × 60 × 36 with the module; the heatsink adds 17–30 (TBC, E-03 not chosen) | TBD | Real airflow path; NVMe reachable for swapping |
| Sync board + power board (stacked) | 1 | TBD | TBD | Close to the motor controllers on the deck (CAN and UART runs) and the battery; e-stop and service connectors reachable |
| Bus clamp board + resistor | 1 | TBD | TBD | On the motor bus; resistor needs airflow or a heatsink |
| Ethernet switch | 1 | TBD | TBD | Near the Orin and sync board |
| Hokuyo UST-10LX | 1 | 50 × 50 × 70 | 130 without cable | High, clear 270°+ view, crash guard |
| Stereo AR0234 bar | 1 | Two 40 × 40 boards (TBC); bar length = baseline, TBD | TBD | Front, rigid, forward view |
| Quad OV9281 cameras | 4 | 40 × 40 board each (TBC) | TBD | Sides and rear on the deck grid |
| Arducam Camarray HAT (one per kit; the cameras need it) | 2 | 65 × 56 (TBC) | TBD | Near a carrier CSI port, within ribbon reach of its cameras |
| Forward GenX320 | 1 | TBD: no Jetson module drawing found; ask Prophesee | TBD | Front, forward view |
| Downward GenX320 + lighting + shroud + ToF | 1 | TBD | TBD | Under the chassis, floor window; outside the aero-sealed area or sealed itself |
| IMU at CG (navigation grade) | 1 | TBD (part not chosen; an ADIS1650x is 15 × 15 × 6) | TBD | At the CG, vibration-isolated |
| IMU front axle | 1 | small | — | Near the front axle, vibration-isolated |
| Ride-height ToF | 4 | small | — | Underside at each corner; minimum range ≤ distance to floor at full compression |
| Suspension angle sensors (AS5047) | 4 | small | — | At each suspension pivot |
| Knuckle encoders (AS5047) | 2 | small | — | On both front knuckles |
| Wi-Fi antennas | 2 | TBD | — | High, clear of motors and carbon |
| E-stop button, hotspot button, service port, loop key | 1 each | TBD | — | Reachable on the outside of the car |
| Wheels (17 mm hex, belted on-road; ADR-0040) | 4 | Ø100 × 42 (TBC: dBoots Hoons 42/100) | TBD | Steering lock and suspension travel clear |

Sources for the envelopes (checked 2026-10-09):

- **UST-10LX:** [Hokuyo product page](https://www.hokuyo-aut.jp/search/single.php?serial=256)
- **A50S V2.3c:**
  - [Triforce product page](https://teamtriforceuk.com/a50s-v2/) (board size, mass with the heatsink)
  - Triforce's [3D models](https://drive.google.com/drive/folders/1Uu2ekqWRjQy-1kAd9qATjfwv0aw9FCOZ) (for `90 Purchased parts`)
  - The [heatsink case](https://teamtriforceuk.com/a50s-v2-3-heatsink-case/) is held by four M2 × 10 countersunk screws
- **Carrier:** [Antmicro Jetson Orin Baseboard](https://openhardware.antmicro.com/boards/jetson-orin-baseboard) (120 × 60); its 36 mm height with the module is from [eeNews Europe](https://www.eenewseurope.com/en/open-source-baseboard-targets-latest-nvidia-orin-module)
- **Orin NX heatsink heights:** [Seeed Studio](https://openelab.com/products/seeed-studio-aluminum-heatsink-with-fan) (17) to [a Super-mode heatsink with fan](https://pakronics.com.au/products/aluminum-heatsink-with-fan-for-jetson-orin-module-super-mode-ss100021109) (30)
- **Arducam kits:**
  - [AR0234 stereo kit for Jetson](https://www.arducam.com/arducam-2-3mp2-ar0234-color-global-shutter-synchronized-stereo-camera-bundle-kit-for-nvidia-jetson-agx-orin-orin-nano-orin-nx.html)
  - [Quad OV9281 kit](https://www.robotshop.com/en/arducam-1mp4-quadrascopic-monochrome-camera-bundle-kit.html) (retailer listing)
- **Battery examples:**
  - [Spektrum SPMX50004S100H5](https://store.hobbyetc.com/parts/view/128215)
  - [Gens Ace GEA50004S50D](https://horizonhobby.com/product/14.8v-5000mah-4s-50c-lipo-battery-deans/GEA50004S50D.html)
- **ADIS1650x:** [ADI product page](https://analog.com/en/products/ADIS16505-1.html) (package), [ADIS16505 breakout](https://it.farnell.com/en-IT/analog-devices/adis16505-2-pcbz/breakout-board-mems-imu/dp/4030173)

## Layout order

1. **Size box and wheels:** pick a wheelbase and track that give ≥ 454 mm overall length and fit the width box.
2. **Battery bay:** low and central.
3. **Corners:** staggered motors, gearboxes, CVDs, wheel encoders. Check the front at full steering lock and full bump with the steering actuator in place.
4. **Lower deck:** the four motor controllers, power and sync stack, bus clamp.
5. **Underfloor:** aero volume, ground-speed window and shroud, ToF sensors, the dead-wheel pod's reserved space.
6. **Upper deck:** hole grid; compute bay with airflow; IMU at the CG.
7. **Sensor mounts:** LiDAR height and crash guard, stereo bar, side/rear cameras, forward event camera, antennas.
8. **Access:** battery swap, NVMe swap, corner swap, buttons and ports.
9. **First CG and mass estimate.**

## First decisions

| # | Decision | Status | Choice |
| --- | --- | --- | --- |
| M1 | Base | **Superseded by ADR-0040** (2026-10-09) | Was: custom chassis on off-the-shelf 1/10 suspension parts (2026-10-03). Now: we design the corners (knuckles and uprights, arms, mounts) and buy only generic parts: shocks, driveshafts, wheels and tyres from Arrma 6S; bearings, rod ends and pins from industrial suppliers |
| M2 | Wheelbase and track | Open: block layout | Start at ~330–350 mm wheelbase so overall length clears 454 mm; track to suit the width box and steering lock |
| M3 | Ride height / floor clearance | Open: with the aero capstone | Also fixes the ground-speed window distance and ToF minimum range |
| M4 | Materials | **Decided** (2026-10-03) | **3D-printed lower deck:** a **PAHT-CF** (high-temperature CF nylon) ribbed tub, not a flat plate, with heat-set inserts. Material may change once the printers are known (see open questions). Built in: battery bay, ground-speed floor window and shroud, ToF pockets, cable channels. **Aluminium plates** bolted in wherever precision or heat matters: gearbox bearing plates, motor mounts, steering mount. Upper deck: hole-grid plate (material TBD). Printed covers, sensor mounts, shrouds |
| M5 | CAD | **Decided** (2026-10-03) | Fusion 360; STEP exports committed per release. Done by the systems lead and the mechanical engineers |

## Printed-deck design rules

- Stiffness from geometry (walls, ribs, a closed tub), not plate thickness.
- No printed bearing bores or gear-mesh features; those live in aluminium.
- Heat paths (the Orin) stay in aluminium; nothing hot rests directly on the print, the motor controllers' heatsinks included (their mounting and airflow wait on the deck model, ADR-0034).
- Every threaded joint uses a heat-set insert with a wide washer; joints are checked after runs.
- PAHT-CF needs an enclosed (ideally heated) chamber, a hardened nozzle and dried filament.
- The tub will be ~400+ mm long (car ≥ 454 mm overall), longer than most print beds. Unless a large-format printer is available, split it into 2–3 sections joined with bolted aluminium splice plates (which can double as motor-mount plates), with the split planned from the start.

## Open questions

| # | Question | Blocks |
| --- | --- | --- |
| MQ-1 | **Which printers are available?** Model, bed size, maximum nozzle and chamber temperatures, continuous-fibre capability. No list is published (checked 2026-10-09): ask the engineering makerspace and the library's Discovery Centre | Final tub material and whether the tub is split. If continuous fibre is available: Onyx + continuous carbon. If PPS-CF is printable: PPS-CF tub with tough PA-CF bumpers. Otherwise: PAHT-CF (current) |
| MQ-2 | Ride height / floor clearance (M3) | Ground-speed window distance, ToF minimum range, aero floor. Placeholder ~10–15 mm |
| MQ-3 | Aero capstone underfloor volume and fan power (SYS-15) | Underfloor layout, motor-bus reserve |
| MQ-4 | Crash speed (SYS-12) and corner swap time (SYS-13) | Crash guard and corner design |
| MQ-5 | **Which off-the-shelf suspension family?** **Closed by ADR-0040:** we design the corners; ADR-0027's bought-corner recommendation was never accepted | — |
| MQ-6 | **Tyre rolling diameter:** the speed figures in `architecture.md` assume 100 mm (ADR-0040). The chosen tyre's rolling diameter, new and worn, is TBD | SYS-01's margin, RSK-18, RSK-21, the eRPM cap |
