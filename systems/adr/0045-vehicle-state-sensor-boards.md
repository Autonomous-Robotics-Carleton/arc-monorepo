# ADR-0045: Vehicle-state sensors sit on our own small boards, assembled in house

- **Status:** Accepted
- **Date:** 2026-10-16
- **Deciders:** Shrikar Vempati, Kunshpreet Baweja
- **Traces to:** E-20, E-21, E-22, E-23, E-24, E-26, SYS-06, SYS-07, SYS-10, SYS-15, SYS-19, SYS-24, SYS-29, RSK-07
- **Related:** ADR-0006, ADR-0007, ADR-0008, ADR-0019, ADR-0021, ADR-0042

## Context

Each vehicle-state sensor (E-20 to E-26) needs a small board at its measurement site. Bought breakout modules were the earlier assumption. Our own boards let the mounts, connectors and pinouts fit the car, and assembling them ourselves avoids JLC's per-design setup, placement and X-ray fees on small batches. The sensor sites and counts stay as `main` has them.

## Decision

1. **Every vehicle-state sensor sits on our own small PCB**: the sensing IC, its local support parts and a locking connector. No MCU or CAN node (ADR-0006). Each board wires straight to the H723 on the motherboard, with its own chip select or mux channel (ADR-0008).
2. **Parts and counts:**

   | Site | Boards | Part |
   | --- | --- | --- |
   | Wheel angle, E-22 | 4 | AS5047P-ATSM |
   | Suspension angle, E-23 | 4 | AS5047P-ATSM |
   | Front knuckle angle, E-24 | 2 | AS5047P-ATSM |
   | IMU at the CG, E-20, and at the front axle, E-21 | 2 | ST ISM330DHCXTR, one board design for both |
   | Ride height, E-26 | 5 | VL53L4CDV0DH/1 preferred (TBC): four corners, plus one beside the ground-speed camera for its scale (ADR-0007) |

   One board design per family (angle, IMU, ToF), reused at each site. Mechanical variants are TBC.
3. **Assembled in house.** Bare PCBs come from JLCPCB, and the sensing ICs and support parts loose from a distributor (LCSC, DigiKey or Mouser), not from JLC's assembly library. We assemble and inspect all 17 boards. The motherboard, and the v1.1 carrier, keep JLC factory assembly.
4. **Harness:** 17 runs / 111 conductors minimum (TBC), each run with one spare: ten angle runs of 7 (70), two IMU runs of 8 (16) and five ToF runs of 5 (25). Final connectors, returns and shields can raise it.

## Consequences

- E-20 is an industrial-grade IMU: ST lists 3 °/h typical gyro bias instability (datasheet DS13012 Rev 7), not the navigation-grade class the BOM named before. With the LiDAR and the ground-speed camera correcting drift, that is expected to be enough. If testing shows the CG IMU limits slip estimation, a better part (Murata SCH16T-K10 class, ~2 °/h) is a candidate for v1.1. Qualify calibration, temperature drift, vibration, clipping and velocity-through-slip against SYS-06, SYS-07, SYS-10 and SYS-29.
- In-house assembly of the LGA IMU and optical ToF packages needs reviewed footprints, reflow instructions, ESD and MSL handling, inspection and power and interface tests before bring-up.
- The ToF boards need minimum-range checks at full compression, glossy tile and cover-window tests, mux addressing and reset handling. The angle boards need magnet alignment and shaft fit.
- Schematics, layouts, support BOMs and connector pinouts are open. See the [sensor board workspace](../../hardware/sensor-boards/README.md), [parts list](../bom/pcb-parts-selection.md), [wiring schedule](../bom/motherboard-wiring.md) and [fab gate](../reviews/fab-gate.md).
