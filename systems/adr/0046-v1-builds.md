# ADR-0046: The v1 car is built twice: v1.0 now, v1.1 when funded

- **Status:** Accepted
- **Date:** 2026-10-16
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-08, SYS-09, SYS-10, SYS-17, SYS-22, SYS-24, SYS-33, E-01, E-02, E-03, E-04, E-10, E-12, E-13, E-25, E-29, LIM-09, LIM-10, LIM-11, RSK-01, RSK-23
- **Amends:** ADR-0001, ADR-0010, ADR-0014 and ADR-0025, for v1.0 only (borrowed LiDAR and compute, a frame camera for ground speed, the developer kit carrier)
- **Related:** ADR-0028, ADR-0043

## Context

Funding is approved per request (SYS-17). The full electrical design comes to about CAD $14,300 with tax, and more than half of the second build is the Orin NX, the custom carrier and a new LiDAR. The team's existing car already has an Orin Nano 8 GB developer kit and a Hokuyo UST-20LX. Classical autonomy, mission 1, doesn't need the NX's 16 GB; learned policies, mission 2, do. Hardware arrives with each funded order, so anything a later build needs has to be designed in from the start.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. One order for the full design | No interim limits | The whole cost now |
| **B. v1.0 now on borrowed compute and LiDAR with fewer cameras; v1.1 adds the rest** | About CAD $6,700 now instead of $14,300 ([total electrical cost](../bom/total-electrical-cost.md)); nothing bought for v1.0 is wasted | Two orders; v1.0 has limits; the existing car goes without its dev kit and LiDAR until v1.1 |
| C. A permanently cut-down car | Cheapest | Gives up mission 2 and the sensor suite of mission 4 |

## Criteria

1. Cost of the first order.
2. Nothing bought for v1.0 is thrown away, and v1.1 only adds parts.
3. Safety functions are complete in v1.0.

## Decision

Option B. **v1.1 is the full design recorded in `systems/`.** v1.0 is built first and differs only in these items:

| Item | v1.0 | v1.1 |
| --- | --- | --- |
| E-01 compute | Orin Nano 8 GB, on loan from the existing car (developer kit module) | Orin NX 16 GB (ADR-0010) |
| E-02 carrier | The developer kit's own carrier (P3768) | Antmicro fork (ADR-0025) |
| E-03 cooling | The developer kit's cooler | NX cooler |
| E-04 SSD | 1 TB, with DRAM and good sustained writes | 4 TB |
| E-10 LiDAR | Hokuyo UST-20LX, on loan from the existing car: the UST-10LX's body, interface, power, 40 Hz and 0.25°, with 20 m range | UST-10LX (ADR-0001), bought; the UST-20LX goes back |
| E-12 side/rear cameras | Not fitted | Quad OV9281 kit |
| E-13 forward event camera | Not fitted | GenX320 |
| E-25 ground-speed camera | Arducam B0223 OV9281 frame camera at up to 240 fps (cropped) | GenX320 (ADR-0014) |
| E-29 ground lighting | Cross-polarized and grazing LEDs | Adds the Class 1 laser module |

On v1.0 the stereo kit uses the dev kit's 4-lane camera connector (CAM1) and the B0223 its 2-lane one (CAM0).

Everything else is the same in both builds: the motherboard, every sensor board, motors, controllers and their spares, steering, packs, harness, brakes and safety functions.

**v1.0 is built for v1.1**, so v1.1 only adds parts:

- The motherboard is the v1.1 board: all four camera trigger groups plus the spare, every sensor connector and chip select, and a compute rail sized for the NX at full power (~40 W in Super mode). Its voltage must suit both the dev kit's barrel input (range TBC from NVIDIA's carrier specification, SP-11324-001) and the Antmicro carrier.
- The compute bay takes both the dev kit and the Antmicro assembly (ADR-0043). The mounts for the quad and forward cameras, and the room for the NX cooler, are kept from the start.
- Software is the same source on both builds (ADR-0028). v1.0's differences are configuration: its sensor set and the dev kit's stock board support.

## Consequences

- LIM-09, LIM-10 and LIM-11 record v1.0's limits; each is revisited in v1.1. The requirements don't change: SYS-08's learned-policy part and SYS-10 at full speed are verified on v1.1, and v1.0 verifies the rest.
- RSK-01, the custom carrier, applies from v1.1. v1.0 adds RSK-23: the dev kit's barrel jack and USB-A ports don't lock.
- The dev kit module (P3767-0005) is sold for developer kits only. v1.0 runs it on its own carrier, so bringing up a module on a custom carrier is a v1.1 question.
- Each build's purchase request comes from its rows in `budgets/cost.csv` (SYS-17). v1.1 buys the NX, carrier, NX cooler, UST-10LX, quad kit, two GenX320s, the 4 TB SSD and the laser module.
- The B0223 has no slot in v1.1; it becomes a bench camera.
- Reopen if the dev kit can't meet v1.0's classical-stack rates (SYS-08 without the policy); the NX then moves to v1.0.
- [builds.md](../builds.md) tracks what each build contains and what has been ordered.
