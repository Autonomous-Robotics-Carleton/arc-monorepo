# Mechanical layout brief (v1)

- **Status:** Draft; mechanical design starting 2026-10-03
- **Goal of the first pass:** a **CAD block layout**: every component as a simple envelope box, placed in the car. It settles wheelbase, track and length (RSK-16), front-corner fit (RSK-04), the battery and compute bays, deck heights, and a first CG estimate, before any part is designed in detail.

## Requirements that drive the mechanics

| Req | Mechanical consequence |
| --- | --- |
| SYS-01 ≥ 12 m/s | Gearing per `architecture.md`; stiff, aligned drivetrain |
| SYS-02 size box | Overall width 238–341 mm, **length 454–654 mm** (F1TENTH/Roboracer). A 1/10 touring car (~260 mm wheelbase) is far too short; even ~310 mm is borderline (RSK-16) |
| SYS-05 e-stop | Physical button reachable on the car while it's moving or just stopped |
| SYS-12 crash survival | Crash guard for the LiDAR, cameras and front corners (impact speed TBD) |
| SYS-13 corner swap | A corner (motor, gearbox, VESC) removable without disturbing the rest (time TBD) |
| SYS-15 aero reserve | Flat underfloor volume kept for the suction fan (size TBD with the capstone) |
| SYS-16 mass/CG | Not limited, but tracked; heavy parts low |
| SYS-24, sensors | Clear fields of view, rigid camera mounts, vibration-isolated IMUs |
| ADR-0014 ground speed | A floor window + light shroud for the downward camera, with the ToF beside it |
| ADR-0015 ground link | Wi-Fi antennas high, clear of carbon; hotspot button and service Ethernet port on the chassis |
| ADR-0019 steering | Belt actuator + moteus-c1 at the front; AS5047 on both knuckles |
| ADR-0008 / BOM | AS5047 on each wheel output shaft (in the gearbox) and at each suspension pivot |

## Component envelopes

Dimensions marked TBD need a datasheet or vendor drawing. Model them as boxes first.

| Component | Qty | Envelope (mm) | Mass (g) | Placement constraint |
| --- | --- | --- | --- | --- |
| 4S LiPo hardcase (~5,000 mAh) | 1 | TBD (standard 1/10 hardcase) | ~450–550 | **Placed first:** low, central, quick swap |
| Castle 1010 motor | 4 | Ø28 × 58.4 (+ shaft) | 146.5 | Transverse, inboard, staggered fore/aft per axle |
| Gearbox (2 stages) + CVD | 4 | TBD from gear layout | TBD | Stage 1 swappable (80 teeth total, mod 0.5); stage 2 ~12/54 steel; precise bearing bores; wheel-encoder magnet pocket |
| VESC 6.4 fork | 4 | TBD (target ~30 × 40) | TBD | FET side bolted to the aluminium motor mount; 10-pin + XT60 connectors accessible |
| Steering actuator (gimbal motor + belt + moteus-c1 38 × 38 × 9) | 1 | TBD | TBD | Front, near the steering linkage; fits beside the staggered front motors |
| Orin NX + heatsink/fan on the carrier (120 × 60) | 1 | ~120 × 60 × TBD height | TBD | Real airflow path; NVMe reachable for swapping |
| Sync board + power board (stacked) | 1 | TBD | TBD | Close to the corners (CAN runs) and the battery; e-stop and service connectors reachable |
| Bus clamp board + resistor | 1 | TBD | TBD | On the motor bus; resistor needs airflow or a heatsink |
| Ethernet switch | 1 | TBD | TBD | Near the Orin and sync board |
| Hokuyo UST-10LX | 1 | ~50 × 50 × 70 (check datasheet) | ~130 (check) | High, clear 270°+ view, crash guard |
| Stereo AR0234 bar | 1 | TBD (fixed baseline) | TBD | Front, rigid, forward view |
| Quad OV9281 cameras | 4 | TBD | TBD | Sides and rear on the deck grid |
| Forward GenX320 | 1 | TBD (module) | TBD | Front, forward view |
| Downward GenX320 + lighting + shroud + ToF | 1 | TBD | TBD | Under the chassis, floor window; outside the aero-sealed area or sealed itself |
| IMU at CG (navigation grade) | 1 | TBD | TBD | At the CG, vibration-isolated |
| IMU front axle | 1 | small | — | Near the front axle, vibration-isolated |
| Ride-height ToF | 4 | small | — | Underside at each corner; minimum range ≤ distance to floor at full compression |
| Suspension angle sensors (AS5047) | 4 | small | — | At each suspension pivot |
| Knuckle encoders (AS5047) | 2 | small | — | On both front knuckles |
| Wi-Fi antennas | 2 | TBD | — | High, clear of motors and carbon |
| E-stop button, hotspot button, service port, loop key | 1 each | TBD | — | Reachable on the outside of the car |
| Wheels (1/10 touring, 64–65 Ø, 24–26 wide, 12 mm hex) | 4 | Ø65 × 26 | TBD | Steering lock and suspension travel clear |

## Layout order

1. **Size box and wheels:** pick a wheelbase and track that give ≥ 454 mm overall length and fit the width box.
2. **Battery bay:** low and central.
3. **Corners:** staggered motors, gearboxes, CVDs, wheel encoders. Check the front at full steering lock and full bump with the steering actuator in place.
4. **Lower deck:** VESCs on motor mounts, power and sync stack, bus clamp.
5. **Underfloor:** aero volume, ground-speed window and shroud, ToF sensors, the dead-wheel pod's reserved space.
6. **Upper deck:** hole grid; compute bay with airflow; IMU at the CG.
7. **Sensor mounts:** LiDAR height and crash guard, stereo bar, side/rear cameras, forward event camera, antennas.
8. **Access:** battery swap, NVMe swap, corner swap, buttons and ports.
9. **First CG and mass estimate.**

## First decisions (recommendations; TBC)

| # | Decision | Recommendation |
| --- | --- | --- |
| M1 | Base: fully custom, or custom chassis on off-the-shelf 1/10 suspension parts | **Custom chassis plates + off-the-shelf 1/10 arms, knuckles, hubs, shocks and wheels**, so effort goes into the drivetrain, steering and sensor packaging |
| M2 | Wheelbase and track | Wheelbase ~**330–350 mm** so overall length clears 454 mm without long overhangs; track to suit the width box and steering lock. Confirm in the block layout |
| M3 | Ride height / floor clearance | Set jointly with the aero capstone; it also fixes the ground-speed window distance and ToF minimum range |
| M4 | Materials | Carbon or aluminium lower deck; aluminium motor mounts (FET heatsinks) and bearing plates; printed covers, mounts and shrouds |
| M5 | CAD tool and storage | Fusion 360 (the docs site already has a Fusion setup guide); STEP exports committed per release |
