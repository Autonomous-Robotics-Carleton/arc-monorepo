# ADR-0043: Preliminary motherboard and Jetson size estimates

- **Status:** Proposed (TBC estimates for CAD block planning; no board outline or cooling solution approved)
- **Date:** 2026-10-09
- **Deciders:** Kunshpreet Baweja (proposer); review of the estimated dimensions pending
- **Traces to:** SYS-02, SYS-05, SYS-07, SYS-08, SYS-14, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32, E-01, E-02, E-03, E-30, E-41, E-46, ADR-0025, ADR-0042, RSK-01, RSK-08

## Context

The current plan combines power and sync MCU on one motherboard, with a separate Jetson carrier (ADR-0042). These estimates feed the chassis block layout. Motherboard capture/layout has not started and its power-stage parts and grounding remain open. The imported Antmicro carrier provides a known reference footprint, but ARC's four CSI connectors and other adaptations have not been laid out.

These estimates support the SYS-02 chassis block layout. They are packaging proposals, not requirements or fabrication dimensions. All dimensions below are in **millimetres**, shown as **long side × short side × total height** where a height is given. Installed blocks include the PCB and fitted parts; they do not prescribe orientation in the chassis.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Use provisional PCB sizes and larger installed blocks | Gives mechanical work a usable starting point while capture proceeds | Needs revision after placement, connector and thermal checks |
| B. Keep all board envelopes TBD until layout is complete | Avoids an estimated outline being treated as final | Prevents useful board/bay block planning now |

## Criteria

Keep measured/vendor reference sizes separate from ARC estimates. Include room for connectors, mounting and fitted parts; retain the combined motherboard and separate carrier plan. Satisfy power integrity, sensor timing and thermal requirements before reducing the envelopes.

## Proposed planning dimensions

| Item | PCB / part footprint | Installed block to reserve initially | Basis and limits |
| --- | --- | --- | --- |
| **Motherboard: power + sync MCU** | **150 × 100 — TBC** | **170 × 120 × 35 — TBC** | Preliminary engineering estimate for E-30 / E-41 / E-46, including connectors, power magnetics/capacitors, protection and routing space. Includes four UART connections. The size estimate is retained until placement supports a revision. No placement/thermal proof exists; SYS-05, SYS-07, SYS-14, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32 |
| **Jetson carrier** | **120 × 60 reference; ARC target TBC** | See complete compute assembly below | Antmicro reference PCB size; starting point for E-02, not a guaranteed final ARC outline. Four 22-pin CSI ports and trigger/power adaptations still need placement and high-speed routing; SYS-07, SYS-08, SYS-24 |
| **Orin NX module alone** | **69.6 × 45 vendor reference** | Included within the compute assembly | E-01 module footprint from NVIDIA; excludes carrier, heatsink/fan and connector/cable access; SYS-08 |
| **Jetson compute assembly: carrier + NX + cooling + M.2 parts** | Based on the carrier above | **140 × 80 — TBC; installed height TBD** | Provisional installed block for E-01 / E-02 / E-03, including underside M.2 population and mounting. Antmicro documents a 37 mm default cooled reference assembly. The updated mechanical brief uses a different 36 mm carrier/module reference plus a 17–30 mm cooler range; their stack/basis must be reconciled with the pinned CAD. Withdraw the former 50 mm installed-height estimate pending that review; ARC cooler and airflow remain unselected. Footprint and height need actual assembly verification; SYS-08, RSK-08 |
| **v1.0 compute: Orin Nano developer kit** | **100 × 79 carrier; ~100 × 79 × 21 kit, TBC** | **TBD: measure the kit we have, with cable clearance** | v1.0 uses the dev kit's own carrier (P3768) and module instead of E-02 / E-01 (ADR-0046). Retailer figure for the kit with its cooler; NVIDIA's carrier specification (SP-11324-001) has the envelope drawings. USB-A, DisplayPort, Ethernet and the barrel jack sit on one edge and need plug and cable clearance; the 40-pin header and both CSI connectors need cable access. The v1.1 NX cooler differs from the dev kit's |

**Compute bay:** reserve a bay that takes both the v1.0 dev kit and the v1.1 Antmicro compute assembly, with airflow for the NX at full power. The larger of the two blocks sets it in each direction.

**Motherboard growth allowance:** if the reviewed power stage, UART connector placement, copper/thermal requirements or connector bank cannot fit the initial target, use **180 × 120 mm PCB — TBC** as a second block-planning case, with **200 × 140 × 40 mm installed block — TBC**. These are contingency estimates under the same power/sync requirements, not a second PCB or an approved maximum. A larger outline remains possible after actual placement.

The motherboard estimate excludes the four bought A50S controllers and their heatsinks, external dump resistors, the Ethernet switch, and remote sensors. Bus-clamp circuitry placement remains open; substantial clamp circuitry on this PCB requires a revised estimate. UART connector/cable access and loaded signal margins still need qualification.

**Updated sizing input (ADR-0044):** recheck motherboard copper, connectors, switching parts, cooling and growth allowance against the TBC 36–43 A pack / 16–20 A per-corner scenario. The proposed envelopes do not establish those ratings. The carrier height sources describe different configurations; do not add heights or assume the earlier installed block is sufficient without assembly CAD.

Cable bend radii, plug insertion/removal, screwdriver access and an airflow duct are **additional service keep-outs, TBD** from selected connectors/cooling and the chassis CAD. The installed blocks above must not be used as proof of serviceability or adequate airflow. PCB thickness, stack-up, mounting-hole coordinates and final component heights remain TBD.

## Consequences and verification

- Use option A as a proposal for the first CAD block pass; approval remains pending. The estimates do not amend accepted ADR-0025 or ADR-0042.
- Keep the size estimates in this ADR. Existing TBD envelopes are not silently replaced with approved dimensions, and no part is selected solely to fit these estimates.
- Complete footprint/connector placement in Altium, include all top/bottom parts and mounting hardware, and inspect a STEP assembly in chassis CAD. Confirm cable access, UART cable access, copper/current capacity, switching-noise control and full-load cooling before freezing dimensions.
- Revise this proposal when power parts, UART/clamp/brake placement, four-port camera interfaces or the cooler change. Board release still requires the fab gate.

## Reference sources

- [Pinned Antmicro board overview, Mechanics section](https://github.com/antmicro/jetson-orin-baseboard/blob/34f0e7f5fbca441191e20ed55d93cf8614e12bed/doc/source/board_overview.md): 120 × 60 mm PCB and 37 mm default cooled assembly. The height belongs to that reference configuration, not the final ARC cooler.
- [Antmicro board introduction](https://antmicro.com/blog/2022/12/baseboard-for-nvidia-jetson-orin): corroborates the 120 × 60 mm footprint. Its older 36 mm configuration differs from the pinned overview's cooled assembly; use the pinned overview for this estimate's height basis.
- [NVIDIA Jetson Orin specifications, Mechanical row](https://www.nvidia.com/en-us/autonomous-machines/embedded-systems/jetson-orin/): 69.6 × 45 mm Orin NX module.
- [NVIDIA Jetson Orin Nano developer kit carrier specification](https://uelectronics.com/wp-content/uploads/2023/05/Jetson-Orin-Nano-DevKit-Carrier-Board-Specification.pdf) (SP-11324-001) and [Core Electronics' kit listing](https://core-electronics.com.au/nvidia-jetson-orin-nano-developer-kit.html): 100 × 79 × 21 mm for the kit including feet, module and cooler; confirm against NVIDIA's envelope drawing and our own kit.
- [Main parts decision list](../bom/pcb-parts-selection.md), [motherboard scope](../../hardware/motherboard/README.md) and [carrier reference/conversion status](../../hardware/orin-carrier/README.md) define the included functions and outstanding design work.
