# Remote sensor carrier PCBs

**Current build, ADR-0045:** ARC designs and assembles all three remote sensor PCB families **in house**: **ten AS5047P angle boards**, **two ISM330DHCXTR IMU boards** and **five ToF boards**. No schematic or layout is released. A common 2-layer design per family is the costing proposal; actual outlines, connectors, mechanical variants and stackups remain TBC/TBD.

| Board family | Installed per car | Assembly plan | Release work |
| --- | --- | --- | --- |
| AS5047P angle carrier | **10**: four wheel + four suspension + two front knuckle | JLC bare PCB; loose components bought separately; **in-house assembly** | Magnet/shaft alignment, local support circuit, locking connector, SPI/individual CS, installed-harness noise |
| [Custom ARC ISM330DHCXTR PCB](imu/README.md) | **2**: body/CG + front axle | JLC bare PCB; loose components bought separately; **in-house assembly** | Pinout/rails, separate CS and DRDY, mounts, timestamps, bias/thermal/vibration and slip qualification |
| [Height ToF board](tof/README.md) | **5**: one per corner + one beside the ground-speed camera | JLC bare PCB; loose components bought separately; **in-house assembly**; VL53L4CD remains preferred/TBC | Optical aperture/recess, window crosstalk, mux-channel/reset/pull-up design |

The [wiring plan and diagram](../../systems/bom/motherboard-wiring.md) now budget **seventeen sensor runs / 111 minimum conductors TBC**: ten angle runs / 70 conductors, two IMU runs / 16 and five ToF runs / 25. A common PCB design is reused at different locations; it does not combine signals or remove dedicated chip selects. These boards add **no corner MCU or CAN node** (ADR-0006 / ADR-0008). Motor Hall and temperature go to each A50S.

The dedicated [IMU](imu/README.md) and [corner-height](tof/README.md) folders are the workspaces for schematic capture and layout. Wheel, suspension and knuckle sensor sites remain selected.

## AS5047P in-house purchase and assembly plan

Populate **ten separate angle boards**: four wheel, four suspension and two front-knuckle boards. The costing scenario fabricates **ten bare angle PCBs** and buys **ten loose AS5047P chips**. No extra sensing ICs or second populated sensor set are included. Final bare-board order quantity/panelization and any mechanical variants remain TBC. The sensors are not mounted on the motherboard.

| Buy / provide | Quantity basis | Source / budget basis | Design status |
| --- | --- | --- | --- |
| AS5047P-ATSM, TSSOP-14 | 1 per board; **10 chips** in the order | [LCSC C962063](https://www.lcsc.com/product-detail/C962063.html), retained **USD $4.231 each at 10+**; ten chips cost **USD $42.31** | Selected chip; stock and cart price must be rechecked before order |
| Bare angle PCB | **10 fabricated / 10 populated** | JLCPCB bare-board fabrication; the [funding estimate](../../systems/bom/jlc-pcba-cost-estimate.md#fabrication-evidence-and-design-assumptions) distinguishes the ten-board batch from the five-board screenshot | Proposed 2-layer common design; ten angle measurement locations per car |
| Local supply decoupling | Manufacturer 3.3 V example uses **100 nF per IC**; stock/rework extras TBD | Loose capacitors from LCSC, DigiKey Canada or Mouser Canada | Exact MPN, package, rating and any extra bulk capacitance completed in schematic |
| Locking PCB connector | 1 per populated board; mating harness contacts counted separately | Same distributors; within support-parts allowance | 7-conductor minimum cable TBC: supply, GND, SCK, MOSI, MISO, dedicated CS, spare. Contact count may grow with returns/protection |
| Reviewed straps, damping and protection parts | BOM quantities TBD | Same loose-component order; included in the [support-parts allowance](../../systems/bom/jlc-pcba-cost-estimate.md) | Values/MPNs need signal-integrity and grounding review; do not order invented resistor/TVS values |
| Solder/flux, cleaning and optional stencil | Shared across **all seventeen sensor boards** | Shared in-house allowance in the [funding model](../../systems/budgets/jlc-pcba-cost.csv) | No JLC sensor assembly/setup fee or paid labour included; do not charge shared supplies again per family |
| Diametric magnet and mounting hardware | 1 magnet per installed angle sensor | Supplier, geometry and quote TBD | Required to operate; **excluded from this PCB funding total** pending mechanical selection |

Retained LCSC pricing inspected 2026-10-12: AS5047P is **USD $4.231 at 10+**, ISM330DHCXTR **USD $5.7392 at 1+**, and the preferred VL53L4CDV0DH/1 **USD $3.1988 at 1+**. The order uses **ten / two / five loose chips**, with no extra ICs for unused bare boards. Stock and prices are snapshots, not reserved; recheck every cart. JLC's parts entries are PCBA-library references and **cannot supply loose chips for our in-house assembly**. All families need separate reviewed support BOMs, locking connectors and manufacturer assembly/handling instructions.

Follow the [manufacturer's AS5047P datasheet, v2-00, Figure 10](https://www.mouser.com/datasheet/2/588/asset_pdf_25493104-3419339.pdf): in 3.3 V mode tie VDD/VDD3V3 as specified and place local decoupling close to the IC; connect TEST to ground. The 5 V/LDO example differs. Review MCU-reset input states and installed cable protection; the manufacturer's minimum circuit is not a qualified remote harness design.

Assembly checks: inspect solder joints and shorts, confirm supply/current, read SPI diagnostics and absolute angle with a correctly aligned magnet, label location, and verify sustained ≥2 kHz reads in the actual loom. Keep signal returns separate from motor-current paths. ESD/MSL handling, magnet geometry and spare/contact policy must be in the final assembly record.

Funding and cost detail: [PCB estimate](../../systems/bom/jlc-pcba-cost-estimate.md). Grounding remains subject to ADR-0041; all designs pass the [fab gate](../../systems/reviews/fab-gate.md) before ordering.
