# ARC remote ground-height sensor PCB

**Current build under ADR-0045:** design and assemble our own remote ground-height sensor PCBs **in house**, separate from the motherboard. Use **five populated boards under E-26**: one at each corner and one beside the downward ground-speed camera for its scale (ADR-0007). A common PCB design is the current proposal; no schematic, layout or fabrication package is released.

The [native Altium scaffold notes](altium/README.md) and [ARC-Sensor-ToF project](altium/ARC-Sensor-ToF.PrjPcb) provide a draft schematic with source/TBD notes and an empty PCB placeholder. Circuit capture and layout remain unfinished; template geometry and stackup are not released design choices.

| Item | Current plan / status |
| --- | --- |
| Sensing IC | **VL53L4CDV0DH/1 remains the preferred candidate and costing baseline**. The custom-board decision does not newly select or qualify the final ToF part |
| Installed quantity | **5 boards**: front left, front right, rear left, rear right and ground-speed camera |
| Acquisition | Proposed **3.3 V sensor domain and I²C**, one remote sensor per motherboard TCA9548 mux channel; acquisition remains centralized at the H723, without a local MCU or CAN node |
| PCB stackup / outline | Common **2-layer design is TBC**; exact stackup, outline, mounts, optical aperture and mechanical variants remain TBC/TBD |
| Connector / harness | **5-conductor minimum per board, TBC**: supply, GND, SCL, SDA and spare; final connector contacts, extra returns, pinout and protection remain TBD |
| Startup / reset | Per-channel pull-up placement and remote XSHUT/reset handling need review; extra reset or interrupt conductors are not included in the minimum harness budget |
| Assembly / purchase plan | **In-house assembly**: buy bare boards and loose chips/support parts separately. Costing scenario: **5 fabricated / 5 populated boards**. Buy **5 sensing ICs**; retained LCSC 1+ VL53L4CD price **USD $3.1988 each**. No JLC assembly/setup/X-ray charge or populated spare set |

Before release, confirm the sensor choice and capture its local support circuit, land pattern, address/reset behavior and connector assignment. Follow the optical package's assembly/reflow, handling and cleanliness guidance; inspect and test the populated boards before installation. Qualify sensor-to-floor clearance through full bump and droop, glossy tile response, optical aperture/recess and any cover-window crosstalk. Review the real I²C harness, mux reset/startup sequence and fault recovery. Test ground-camera scale under SYS-10 and RSK-07; a minimum-range specification alone does not prove measurement accuracy.

See the [sensor-board overview](../README.md), [parts selection](../../../systems/bom/pcb-parts-selection.md), [wiring schedule](../../../systems/bom/motherboard-wiring.md), [CAD funding estimate](../../../systems/bom/jlc-pcba-cost-estimate.md) and [fab gate](../../../systems/reviews/fab-gate.md). Grounding and signal returns remain subject to ADR-0041; all circuit and assembly details need review before ordering.
