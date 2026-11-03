# ARC remote IMU PCB

**Current build under ADR-0045:** design and assemble our own remote IMU PCB **in house**, separate from the motherboard. Both measurement sites use **ST ISM330DHCXTR**: one at the body/CG and one at the front axle. Reuse a common PCB design at the two locations; each populated board has its own cable, chip select and data-ready input. No schematic, layout or fabrication package is released.

The [native Altium scaffold notes](altium/README.md) and [ARC-Sensor-IMU-ISM330DHCX project](altium/ARC-Sensor-IMU-ISM330DHCX.PrjPcb) provide a draft schematic with source/TBD notes and an empty PCB placeholder. Circuit capture and layout remain unfinished; template geometry and stackup are not released design choices.

| Item | Current plan / status |
| --- | --- |
| Installed quantity | **2 boards per car**: body/CG and front axle; E-20 and E-21 |
| Acquisition | Proposed **3.3 V sensor domain, SPI and DRDY** wired to the H723 on the motherboard; acquisition remains centralized, without a local MCU or CAN node |
| Timing | Retain ≥4 kHz output and hardware-timestamped data ready; configure and verify the selected part's bandwidth, filtering, FIFO and sample timing against SYS-06, SYS-07 and SYS-10 |
| PCB stackup / outline | Common **2-layer design is TBC**; exact stackup, outline, mounting and mechanical variants remain TBC/TBD |
| Connector / harness | **8-conductor minimum per board, TBC**: supply, GND, SCK, MOSI, MISO, CS, DRDY and spare; final connector contacts, extra returns, protection and pinout remain TBD |
| Assembly / purchase plan | **In-house assembly**: buy bare boards and loose chips/support parts separately. Costing scenario: **5 fabricated / 2 populated boards**, with three unused bare boards. Buy **2 ISM330DHCXTR ICs**; retained LCSC 1+ price **USD $5.7392 each**. No JLC assembly/setup/X-ray charge or populated spare set |

Before release, capture the exact supply/decoupling and interface circuit from the manufacturer guidance; review the land pattern, protection, mounting and connector assignment. Follow the LGA package's assembly/reflow and ESD/MSL instructions, inspect solder joints and test shorts/supply current before interface bring-up. Qualify power-up, installed SPI/DRDY timing, sustained acquisition, calibration, thermal drift, motor-on vibration and velocity-through-slip performance. A selected chip or board architecture does not establish achieved navigation performance.

See the [sensor-board overview](../README.md), [parts selection](../../../systems/bom/pcb-parts-selection.md), [wiring schedule](../../../systems/bom/motherboard-wiring.md), [sensor review](../../../systems/bom/sensor-candidate-review.md), [CAD funding estimate](../../../systems/bom/jlc-pcba-cost-estimate.md) and [fab gate](../../../systems/reviews/fab-gate.md). Grounding and signal returns remain subject to ADR-0041; all circuit and assembly details need review before ordering.
