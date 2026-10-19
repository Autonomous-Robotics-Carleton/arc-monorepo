# PCB parts selection

- **Status:** Draft. Main-component selection list; schematic completion and qualification pending.
- **Prepared:** 2026-10-03
- **Prices checked:** 2026-10-12 (selected parts; other dated snapshots are retained)
- **Currency:** USD supplier prices; CAD funding budget

Scope: the car's PCBs: **one combined power/sync motherboard, the custom Jetson carrier (v1.1; v1.0 uses the developer kit's carrier, ADR-0046), four bought A50S controllers using UART telemetry**, and the autonomous motor-bus clamp. Cameras, camera modules and lenses are excluded; the carrier's four CSI connectors are included because they affect PCB design.

This is the main-component selection list for schematic capture. It supplements [electrical.md](electrical.md) and the [electrical handoff](../handoff/electrical.md). Exact passives, magnetics, protection ratings and reference designators are completed from the schematics before the [fab gate](../reviews/fab-gate.md). New recommendations here do not change an accepted ADR automatically.

**Controller baseline:** four A50S V2.3c on the deck plus two spares under ADR-0034, with **four independent 2 Mbit/s UARTs**, full ≥1 kHz status per controller, with classic CAN commands kept separate. The moteus steering FD link remains separate. Controllers are bought from Triforce (ADR-0034); landed quote TBD.

**Latest main reconciled, 2026-10-12:** upstream `5a59c53` selects custom corners (ADR-0040), **100 mm wheels / 18:1 gearing / TBC 4–5 kg**. Review **TBC 36–43 A pack peak, 16–20 A phase current per corner, and 160–200 J / 185–230 W regen** in [ADR-0044](../adr/0044-ee-sizing-after-custom-corners.md). Dump-resistor sizing remains open. One power/sync motherboard and a separate carrier remain the plan.

**UART readiness:** calculated status-stream load is **29.5% per 2 Mbit/s link at 1 kHz**. Both firmware endpoints are still stubs; clock alignment, DMA ownership, fault handling, forwarding and loaded ground/reference margins need qualification. PB10/PB11 are the A50S's USART3 TX/RX. See [UART ICD](../icd/controller-telemetry.md), [source review](../reviews/a50s-uart-readiness.md) and [arrival checks](../tests/a50s-arrival.md).

**Sensor decisions (ADR-0045):** the car uses **10 AS5047P angle boards (4 wheel + 4 suspension + 2 front knuckle), 2 ISM330DHCXTR IMU boards and 5 ToF boards (4 corners + 1 beside the ground-speed camera)**. All **17 sensor PCBs are assembled in house** from JLC bare boards and separately purchased loose chips/support parts. No populated spare sensor set is included. See the [assembly purchase plan](../../hardware/sensor-boards/README.md#as5047p-in-house-purchase-and-assembly-plan) and [sensor review](sensor-candidate-review.md). Grounding/isolation and power-stage selections remain TBD for later review.

**Motherboard packaging, 2026-10-09 (ADR-0042):** [power and sync share one PCB](../../hardware/motherboard/README.md); the Jetson carrier stays separate. The sync MCU and power sections below list components for that same motherboard, not separate bare boards. A stacking header is not required in this plan.

## JLC PCBA cost estimate

**PCB estimate per build (2026-10-16), before tax and including 30% contingency:** v1.0 (motherboard, optional clamp and the 17 sensor boards) **CAD $1,950.29**, a $2,000 allowance; v1.1 (the carrier) **CAD $1,201.54**, a $1,300 allowance. JLC's two-board minimum per factory-assembled design leaves one surplus board, not a second car. All 17 sensor boards are assembled in house. Bought compute, controllers, cameras, LiDAR, magnets and the external dump load are excluded. See the [PCB estimate and JLC pricing screenshots](jlc-pcba-cost-estimate.md#fabrication-evidence-and-design-assumptions) and the [USD/CAD model](../budgets/jlc-pcba-cost.csv); final BOM/CPL quotes remain TBC.

The estimate separates [bare PCB fabrication, purchased parts and assembly costs](jlc-pcba-cost-estimate.md#bare-pcb-parts-and-assembly-cost-breakdown), with CAD working totals and the 30% contingency shown by category.

## Board map

The table shows which assembly holds each function. Power and sync are sections of the same motherboard. See the [board and harness diagram](motherboard-wiring.md#board-and-harness-diagram) for the connections between assemblies.

Green selection markers below mean existing or recommended choices; alternatives and unresolved choices remain labelled explicitly.

| Board / assembly | Installed | Parts mounted there | External connections |
| --- | --- | --- | --- |
| **Jetson carrier** | **1** | Module / M.2 / CSI sockets, SiC431 local power, AXC level translation, optional debug bridge, port protection | Orin NX module, NVMe, Wi-Fi, Ethernet switch, sync PPS / GPIO |
| **Motherboard — sync section** | **1 shared PCB** | H723, LAN8742 PHY, steering MCP2518FD, 1 FD + 1 classic transceivers, 40 MHz crystal, TCA9548 mux, quiet sensor LDOs | Remote IMUs / angle / ToF sensors, power-board monitors, classic command CAN, steering FD, four A50S UARTs, Ethernet switch |
| **Motherboard — power section** | **Same PCB** | LM5176 / LM5146 / TPS54360 rails, ORing, INA226 monitors / shunts, LTC6810 cell monitor, steering switch, protection | Battery / wall input, carrier / sensor / logic branches, steering, and e-stop interfaces TBD |
| **Bought A50S controllers** | **4 installed + 2 spares** | Bought power stages, Hall and motor-temperature inputs and USART3 | Motor phases, Hall cable and any E-28 lead to deck; classic command trunk; four 2 Mbit/s UARTs to motherboard |
| **Bus clamp** | **1 assembly** | Comparator, reference, gate driver, dump FET, autonomous local supply | Motor bus; external chassis-mounted dump resistors; **quantity TBD**. May share the power-board assembly |
| **ARC angle-sensor PCBs** | **10 per car** | AS5047P and reviewed local support; assembled in house | Separate remote PCBs → motherboard SPI/individual CS; mounting and 2-layer layout TBC |
| **ARC IMU PCBs** | **2 per car** | ISM330DHCXTR and local support, assembled in house, on [our reusable IMU PCB](../../hardware/sensor-boards/imu/README.md) | Body/CG and front → motherboard SPI + separate CS/DRDY; outline and stackup TBC |
| **ARC corner-height ToF PCBs** | **4 per car** | Preferred VL53L4CD and support, assembled in house, on [our reusable height PCB](../../hardware/sensor-boards/tof/README.md); final IC variant TBC | Four corners → motherboard I²C mux; optics/outline/stackup TBC; no local MCU |

## Wiring diagram

**Proposed sensor harness: 17 cable runs / 111 minimum conductors, including one spare per run.** The sensors sit on our custom remote PCBs at their measurement locations: 10 angle, 2 IMU and 5 ToF boards, all assembled in house, with no corner MCU. Two-layer implementations remain TBC. The complete motherboard wire total remains TBD because power, controller placement, grounding and brake interfaces are still open.

The [Markdown wiring plan](motherboard-wiring.md#board-and-harness-diagram) contains the board diagram and detailed cable schedule.

| Connector bank / loom | Separate cable runs | Minimum conductors, TBC |
| --- | --- | --- |
| FL | 4 | 26 |
| FR | 4 | 26 |
| RL | 3 | 19 |
| RR | 3 | 19 |
| IMU | 2 | 16 |
| Ground camera | 1 | 5 |
| Sensor-only total | 17 | 111 minimum, TBC |

[Detailed cable/net schedule and remaining interfaces](motherboard-wiring.md) · [Remote sensor PCB workspace](../../hardware/sensor-boards/README.md)

## Preferred sensor picks

Use this summary for the first schematic pass; detailed alternatives, sourcing and qualification notes follow.

| Sensor | Qty / car | 🟢 Preferred part | Supplier USD each | Mounted at / read by |
| --- | --- | --- | --- | --- |
| Body/CG IMU | **1** | ISM330DHCXTR (selected, ADR-0045) | **USD $5.7392 loose / 1+** | CG mount → sync SPI + DRDY |
| Front IMU | **1** | ISM330DHCXTR (selected, ADR-0045) | **USD $5.7392 loose / 1+** | Front axle mount → sync SPI + DRDY |
| Wheel angle | **4** | AS5047P-ATSM | **USD $4.231 loose / 10+** | Wheel output shafts → sync SPI |
| Suspension angle | **4** | AS5047P-ATSM | **USD $4.231 loose / 10+** | Suspension pivots → sync SPI |
| Steering angle | **2** | AS5047P-ATSM | **USD $4.231 loose / 10+** | Front knuckles → sync SPI |
| Corner ride height | **4** | VL53L4CDV0DH/1 | **$3.1988 loose / 1+** | Four corners → sync I²C |
| LiDAR | **1** | Hokuyo UST-10LX | **$1,600–2,200 team estimate; not JLC** | Chassis → Ethernet switch |

**Installed state-sensor chips: USD $69.7824 / CAD $99.59 per car**: `(10 × $4.231 AS5047P) + (2 × $5.7392 ISM330DHCXTR) + (5 × $3.1988 VL53L4CD)`, from loose LCSC purchasing at the actual 10+ / 1+ / 1+ tiers. Conversion uses 1.4271 CAD/USD dated 2026-10-09. These 17 fitted chips are included in the funding model; support parts, magnets, PCB fabrication, shared assembly consumables and shipping are separate.

**Motor temperature:** the four Castle 1010s (E-51) go to their A50S's motor-temp input. Castle doesn't document a thermistor on the 1010's sensor cable, so four E-28 NTCs are bought with the motors as a fallback; no temperature PCB or motherboard ADC channel is needed.

## Colour key and price basis

| Mark | Meaning |
| --- | --- |
| 🟢 **Existing** | Already selected in the project; where only a family was selected, the exact suffix below is a recommendation. |
| 🟢 **Recommend** | Preferred new choice for this design, subject to the specific checks in its row. |
| 🟡 **Alternative** | Viable option with a stated tradeoff; check footprint, interface and firmware before substituting. |
| 🔴 **Fails requirement** | Do not populate against the current requirements. |

Prices link directly to the **genuine manufacturer's JLCPCB catalog entry**, rather than similarly named assembly-service placeholders or substitute brands. Unless stated otherwise, prices are the public **1+ unit price**, rounded to cents; a 10+ price is included where useful. Quantities are **installed per car**, not prototype spares. Alternatives replace the preferred row; they are not additional purchases.

**Preorder** means JLC shows an estimated sourcing price and an MOQ; **consign** means the part must be supplied to the assembler. These are not ready-stock quotes. Prices exclude PCB fabrication, assembly/setup fees, attrition, MOQ excess, shipping and tax. JLC's parts library is for PCBA; all selected angle, IMU and ToF boards use separate loose-component purchasing and in-house assembly. Other manual/external items require appropriate sourcing. Stock is a snapshot, not a reservation.

## Vehicle sensors

### IMUs: one at the CG, one at the front axle

Both use [our custom ARC IMU PCB](../../hardware/sensor-boards/imu/README.md) under ADR-0045, with one sensing IC per board. The shared design is remote from the motherboard; connectors, support BOM and layer stack remain TBC.

| Role / choice | Exact manufacturer part | JLCPCB | USD each | Package / interface | Reason and remaining check |
| --- | --- | --- | --- | --- | --- |
| Both IMUs, qty 2 — 🟢 **Selected (ADR-0045)** | ST **ISM330DHCXTR** | [C2655101](https://jlcpcb.com/partdetail/Stmicroelectronics-ISM330DHCXTR/C2655101) | **$5.7392 loose each**, [LCSC C2655101](https://www.lcsc.com/product-detail/C2655101.html), 1+ tier | LGA-14, 2.5 × 3 mm; SPI; DRDY; 3.3 V domain | Same part and board at body/CG and front; industrial grade, 3 °/h typical gyro bias instability (DS13012 Rev 7). ≥4 kHz requirement: 6.667 kHz setting available; ±16 g / up to ±4000 °/s. Calibration, vibration, timestamps and velocity-through-slip qualification remain pending. Selected for in-house assembly; JLC's Standard-only catalog condition applies to its factory service, which is not used for these sensors. |
| Better CG IMU — 🟡 **v1.1 candidate** | Murata **SCH16T-K10-1** | [C46817724](https://jlcpcb.com/partdetail/MurataElectronics-SCH16T_K101/C46817724) | **~$64.78** JLC preorder estimate; DigiKey $47.04 (SCH16T-K10-10, 2026-10-16) | Inverted SOIC-24; SafeSPI | ±2000 °/s, ±16 g, ~2 °/h. Not in quantities or cost: the ISM330DHCXTR is used at both sites (ADR-0045). Consider for v1.1 if testing shows the CG IMU limits slip estimation; needs its own board design. |
| Both-role alternative — 🟡 **Not selected** | TDK **ICM-42688-P** | [C1850418](https://jlcpcb.com/partdetail/ICM-42688-P/C1850418) | **$20.64**, retained snapshot | LGA-14; SPI; data ready | Different timing/registers/pin map; qualifies only after a recorded substitution and installed performance review. |
| Legacy CG candidate — 🔴 **Fails existing targets** | Analog Devices **ADIS16505-3BMLZ** | [C1235471](https://jlcpcb.com/partdetail/AnalogDevices-ADIS165053BMLZ/C1235471) | **~$234.71**, historical estimate | Packaged module; SPI | 2 kSPS / ±8 g miss the ≥4 kHz / ±16 g targets; excluded. |
| Lower-cost alternative — 🟡 **Not selected** | ST **LSM6DSOTR** | [C2655100](https://jlcpcb.com/partdetail/STMicroelectronics-LSM6DSOTR/C2655100) | **$3.88**, historical snapshot | LGA-14; SPI; interrupt | Different registers/pins need review; not added to the selected BOM. |

Mount both selected IMUs away from switching currents and on vibration-isolated mounts. Configure and measure actual bandwidth, delay, clipping, temperature drift and installed timestamps; selecting the part does not close SYS-06 / SYS-07 / SYS-10 qualification. [ST datasheet](https://www.st.com/resource/en/datasheet/ism330dhcx.pdf) and [sensor review](sensor-candidate-review.md).

### Wheel, suspension and steering position

**User-proposed suspension alternative:** Variohm **ELPM** is a rugged linear contact potentiometer, read through a high-impedance analog front end and sync ADC. It remains 🟡 **evaluation / interface change**, rather than an approved AS5047 replacement. The smallest standard 25 mm version is 75 mm body / 95 mm retracted mounting distance in the inspected 10/24 datasheet. Final stroke, mounting, ≥2 kHz acquisition, accuracy and price are TBC / quote required; no JLC listing verified. See the [complete sensor review](sensor-candidate-review.md), including the blocked newer-PDF revision check.

The same preferred sensor and alternative apply to **all three roles** below. Total: **10 ICs plus 10 diametrically magnetized magnets**, not six encoders.

| Function | Project ID | Qty / car | 🟢 Selected part (ADR-0045) | 🟡 Alternative |
| --- | --- | --- | --- | --- |
| Wheel output-shaft angle | E-22 | **4** | AS5047P-ATSM | AS5047D-ATSM |
| Suspension pivot angle | E-23 | **4** | AS5047P-ATSM | AS5047D-ATSM |
| Front steering-knuckle angle | E-24 | **2** | AS5047P-ATSM | AS5047D-ATSM |

| Exact manufacturer part | JLCPCB | USD each | Package / interface | Selection notes |
| --- | --- | --- | --- | --- |
| 🟢 **AS5047P-ATSM — separate PCB, in-house assembly** | [Loose LCSC C962063](https://www.lcsc.com/product-detail/C962063.html); [JLC reference](https://jlcpcb.com/partdetail/AS5047P-ATSM/C962063) | **$4.231 at 10+**, loose parts checked 2026-10-12 | TSSOP-14; 14-bit absolute SPI; 3.3 V mode | Ten separate boards per car. Buy exactly ten fitted chips: USD $42.31 / CAD $60.38. No second populated sensor set or extra loose rework chips are included. JLC makes bare boards; its PCBA-library chips are not a loose-parts source |
| 🟡 **ams OSRAM AS5047D-ATSM** | [C2693743](https://jlcpcb.com/partdetail/AMSOSRAM-AS5047DATSM/C2693743) | **$5.57**; $4.75 at 10+ | TSSOP-14; absolute SPI | Family alternative, currently more expensive. Review diagnostic registers, timing and magnetic limits before substitution. |
| 🟡 **Variohm ELPM**, suspension only | External supplier; no verified JLC listing | Quote required; GBP £293.46 ELPM-25 distributor reference, ex VAT | Linear contact potentiometer; proposed high-impedance analog front end → sync ADC | Evaluation alternative, not included in the subtotal. Smallest standard 25 mm: 95 mm retracted mounting distance in 10/24 datasheet. Stroke, fit, accuracy, ≥2 kHz acquisition and exact revision TBC; see [sensor review](sensor-candidate-review.md). |
| Required magnets, qty 10 | No verified JLC selection | Quote required | Diametric magnet; diameter / thickness / air gap TBD | Mechanical selection must produce the datasheet field at the die over runout and suspension motion; an axially magnetized disc is unsuitable. |

**Assembly decision (ADR-0045):** place each AS5047P on a separate remote PCB at its measured shaft/pivot and **assemble these boards in house**. Buy loose chips and reviewed support parts separately; [purchase/assembly plan](../../hardware/sensor-boards/README.md#as5047p-in-house-purchase-and-assembly-plan). Wire SPI directly to the central sync MCU. [ADR-0008](../adr/0008-corner-sensors-to-sync-board.md) does not add a corner sensor MCU. In 3.3 V operation, follow the specified VDD / VDD3V3 connection and local decoupling. Qualify the actual harness for ≥2 kHz reads with individual chip-selects; SPI clock rate alone does not prove harness integrity. [AS5047P manufacturer datasheet, distributor mirror](https://www.mouser.com/datasheet/2/588/asset_pdf_25493104-3419339.pdf).

### Ride height / ground-speed scale

ADR-0045 uses [our own height board](../../hardware/sensor-boards/tof/README.md) at five sites: **four corners plus one beside the ground-speed camera** for its scale (ADR-0007), all assembled in house. The IC options remain recommendations; exact local support parts, optics and assembly qualification still require review.

| Function / choice | Qty / car | Exact manufacturer part | JLCPCB | USD each | Design notes |
| --- | --- | --- | --- | --- | --- |
| ToF — 🟢 **Recommend** | **4** | ST **VL53L4CDV0DH/1** | [C3178291](https://jlcpcb.com/partdetail/STMicroelectronics-VL53L4CDV0DH1/C3178291) | **$3.1988 loose at 1+**, [LCSC C3178291](https://www.lcsc.com/product-detail/C3178291.html) | Four corner ride-height sensors; no dedicated ground-camera scale sensor. Optical LGA-12, 4.4 × 2.4 × 1 mm; 2.6–3.5 V; I²C; short-range operation from 1 mm, up to 100 Hz. Preferred for close floor clearance. |
| ToF — 🟡 **Alternative** | 4, replacing above | ST **VL53L4CXV0DH/1** | [C3178292](https://jlcpcb.com/partdetail/STMicroelectronics-VL53L4CXV0DH1/C3178292) | **$5.67**; $4.88 at 10+ | Same package family; longer range / multi-target capability and more driver complexity. Linear close range starts around 10 mm; check full-compression clearance. |

ToF minimum distance is not an accuracy guarantee. Test glossy tile, full bump / droop, cover-window crosstalk and mechanical recess depth. The proposed wiring uses one I²C mux channel per sensor; reset/startup handling and any extra control conductors remain TBC. Datasheets: [VL53L4CD](https://www.st.com/resource/en/datasheet/vl53l4cd.pdf), [VL53L4CX](https://www.st.com/resource/en/datasheet/vl53l4cx.pdf).

### LiDAR — external sensor

| Choice | Qty | Part | JLCPCB price | Budget / connection |
| --- | --- | --- | --- | --- |
| 🟢 **Existing** | 1 | **Hokuyo UST-10LX** | Not a JLC PCB component | Existing team estimate **$1,600–2,200** in [cost.csv](../budgets/cost.csv), not a current supplier quote. 10–30 V input; use a protected 12 V branch; 100BASE-TX Ethernet. |
| 🟡 **Alternative, requires ADR change** | 1, replacing above | **Hokuyo UST-20LX** | Not a JLC PCB component | Supplier quote required; longer range adds no essential benefit to the existing 10 m requirement. Verify power, enclosure and mounting before changing. |

The UST-10LX startup allowance is up to **450 mA**: allow at least 5.4 W at 12 V for startup rather than sizing solely to the old 5 W budget. [Hokuyo specification](https://www.hokuyo-aut.jp/dl/UST-10LX_Specification.pdf). Battery, rail and phase-current sensor options are in the power and motor-controller sections below.

## Jetson carrier

Base the design on a **frozen revision** of the [Antmicro Orin baseboard](https://github.com/antmicro/jetson-orin-baseboard). Reuse its verified sequencing, module-ID handling and high-speed routing constraints; import that revision's full BOM before selecting remaining parts. The following are recommended sourcing choices, not a claim that every footprint matches upstream.

| Function / choice | Exact manufacturer part | JLCPCB | Qty / carrier | USD each | Package / design constraint |
| --- | --- | --- | --- | --- | --- |
| Module socket — 🟢 **Recommend** | TE **2309413-1** | [C2766246](https://jlcpcb.com/partdetail/TEConnectivity-23094131/C2766246) | 1 | **$2.33** | 260-pin, 0.5 mm pitch SODIMM, 9.2 mm height; verify module retention / standoff stack. Standard PCBA listing. |
| NVMe socket — 🟢 **Recommend** | UMAX **91302-67-067R2M** | [C2922445](https://jlcpcb.com/partdetail/UMAX-PCI_Express_M_2/C2922445) | 1 | **$1.25** | M.2 **key M**, 67 contacts, 6.7 mm height. Match 2280 SSD mounting and enclosure height. |
| Wi-Fi socket — 🟢 **Recommend** | Lian Xin Technology **APCI0085-P005A** | [C7498138](https://jlcpcb.com/partdetail/Lian_XinTechnology-APCI0085P005A/C7498138) | 1 | **$0.67** | M.2 **key E**, 67 contacts, 3 mm height; MT7922 / AX210 module interface. Check PCIe / USB routing. |
| CSI connectors — 🟢 **Recommend** | Molex **5051102296** | [C6071729](https://jlcpcb.com/partdetail/5051102296/C6071729) | **4** | **$1.67** | 22-pin, 0.5 mm FFC, right-angle, bottom contact, 1.9 mm height. Four 2-lane ports fit Orin NX; kit output modes / bandwidth and final pinouts remain open. See [CSI feasibility](../../hardware/orin-carrier/README.md#four-port-csi-feasibility). Validate cable contact side. Cameras excluded. |
| Local high-current buck — 🟢 **Upstream reference** | Vishay **SIC431AED-T1-GE3** | [C2071422](https://jlcpcb.com/partdetail/VishayIntertech-SIC431AED_T1GE3/C2071422) | From frozen upstream BOM | **$2.66** | MLP44-24L; 24 V input rating. Suitable on the protected regulated carrier input, not directly on a raw node clamped near 39 V. Keep calculated inductors / compensation / thermal layout. |
| 1.8 ↔ 3.3 V signals — 🟢 **Recommend** | TI **SN74AXC8T245PWR** | [C882718](https://jlcpcb.com/partdetail/SN74AXC8T245PWR/C882718) | 1 provisional | **$1.11** | TSSOP-24; PPS / trigger / GPIO boundary where required. Direction is grouped in sets of four; OE / DIR reference VCCA. Hold OE inactive through sequencing. |
| USB debug bridge — 🟡 **Retain if required** | FTDI **FT230XQ-R** | [C128629](https://jlcpcb.com/partdetail/Ftdi-FT230XQR/C128629) | 1 if fitted | **$6.95** | QFN-16; optional debug UART. Check I/O voltage and the actual upstream debug circuit. |
| Port ESD — 🟢 **Recommend where suitable** | TI **TPD4E05U06DQAR** | [C138714](https://jlcpcb.com/partdetail/TPD4E05U06DQAR/C138714) | Per exposed port; TBD | **$0.0838** | USON-10; four low-capacitance channels. Apply to supported signal interfaces after signal-integrity review; do not indiscriminately load MIPI / PCIe lanes. |

**Power input proposal:** regulated **12 V** from the power board, with a separately fused / switched / monitored compute branch. Antmicro's documented newer revisions accept 9–20 V and use MODULE_ID to select the module supply path. Retain that logic and verify the chosen Orin NX power mode against NVIDIA's design guide. Carrier-local 5 V / 3.3 V converters remain part of the carrier design; do not assume the external 3.3 V sensor rail can power the NVMe.

The Orin module already exposes Gigabit Ethernet: provide matching magnetics / RJ45 and protection. An external RTL8111 is not automatically required. The optional i210 / i226 link from [ADR-0021](../adr/0021-time-sync-sync-mcu-domain.md) is excluded from this default population, consistent with the newer [proposed ADR-0023](../adr/0023-no-orin-i210-nic.md); the sync MCU uses the switch. That proposal has not yet become an accepted decision. Remaining carrier connectors, Ethernet magnetics, reset / power sequencing, EEPROM, fan circuitry and passive quantities come from the frozen upstream schematic. Sources: [Antmicro board overview](https://github.com/antmicro/jetson-orin-baseboard/blob/main/doc/source/board_overview.md), [SiC431](https://www.vishay.com/docs/74589/sic431.pdf), [AXC translator](https://www.ti.com/lit/ds/symlink/sn74axc8t245.pdf).

## Sync MCU (motherboard)

| Function / choice | Exact manufacturer part | JLCPCB | Qty / sync board | USD each | Package / design notes |
| --- | --- | --- | --- | --- | --- |
| MCU — 🟢 **Existing family** | ST **STM32H723ZGT6** | [C730146](https://jlcpcb.com/partdetail/Stmicroelectronics-STM32H723ZGT6/C730146) | 1 | **$11.05** | LQFP-144, 20 × 20 mm; recommended pin-count / memory suffix for the selected H723. Check complete pin allocation and Zephyr support. |
| Ethernet PHY — 🟢 **Recommend** | Microchip **LAN8742AI-CZ-TR** | [C621425](https://jlcpcb.com/partdetail/Microchip-LAN8742AI_CZTR/C621425) | 1 | **$1.64** | QFN-24 EP, 4 × 4 mm; industrial 10/100 RMII. Add PHY clock, straps, magnetics and connector. |
| Steering CAN-FD controller — 🟢 **Existing family** | Microchip **MCP2518FDT-E/SL** | [C626758](https://jlcpcb.com/partdetail/MicrochipTech-MCP2518FDT_ESL/C626758) | 1 | **$1.50** | SOIC-14; SPI; dedicated steering bus baseline. Moving to a freed native H723 FDCAN remains an owner choice. |
| External CAN controller clock — 🟢 **Recommend** | Yajingxin **TX322540M4FBCE2T** | [C5186937](https://jlcpcb.com/partdetail/5858100-TX322540M4FBCE2T/C5186937) | 1 | **$0.0763** | 40 MHz crystal, 3225-4, 12 pF load; calculate load capacitors and verify clock error over temperature. |
| Steering FD transceiver — 🟢 **Recommend** | NXP **TJA1462AT/0Z** | [C3234095](https://jlcpcb.com/partdetail/NXPSemicon-TJA1462AT0Z/C3234095) | **1** | **$1.00** | SOIC-8 CAN SIC; steering only. VCC = 5 V, VIO = 3.3 V |
| Classic command transceiver — 🟢 **Existing family** | NXP **TJA1051T/3/CM,118** | [C2671195](https://jlcpcb.com/partdetail/NXP-TJA1051T_3CM118/C2671195) | **1** | **$0.82** | SOIC-8; 5 V bus supply / 3.3 V I/O. Pin 8 is silent-mode S, unlike the SIC transceiver's STB. |
| Four identical ToF addresses — 🟢 **Recommend** | TI **TCA9548APWR** | [C130026](https://jlcpcb.com/partdetail/TexasInstruments-TCA9548APWR/C130026) | 1 | **$0.81** | TSSOP-24; eight-channel I²C mux, one ToF per channel; pull-ups and reset behavior per channel. |
| Quiet sensor 3.3 V — 🟢 **Recommend** | TI **TPS7A2033PDBVR** | [C2862740](https://jlcpcb.com/partdetail/TexasInstruments-TPS7A2033PDBVR/C2862740) | **2 provisional** | **$0.2151** | SOT-23-5, 300 mA LDO, feed from protected 5 V. Budget each sensor domain; this is not the main H723 supply. |

Reserve channels for **12 sensor chip-selects** (2 IMUs + 10 angle sensors), IMU interrupts, MCP2518FD, power-board data, four trigger outputs, PPS, debug, lighting-mode controls and the specified spares. With MCP2518FD, direct cell-monitor SPI and one spare, allocate **15 chip-selects**; also reserve the MCP interrupt separately from the two IMU interrupts. Partition SPI buses / interrupt routing before committing the LQFP pin map. The cell-monitor select is additional to the twelve state-sensor selects.

One native H723 FDCAN serves classic drive commands. The existing external MCP2518FD baseline serves the separate moteus steering FD link at **1 Mbit/s arbitration / 5 Mbit/s data with BRS**; two native FDCAN peripherals are freed. Moving steering to one of them remains the owner's choice, so the steering MCP/crystal/transceiver stay listed. Allocate **four controller UARTs plus debug**, with DMA/pin mapping and spare provision checked alongside sensors and Ethernet. Each CAN segment needs its own correct termination; UART TX/RX do not use CAN termination. The selected TJA1462AT uses pins 1 TXD, 2 GND, 3 VCC (5 V), 4 RXD, 5 VIO (3.3 V), 6 CANL, 7 CANH, 8 STB. Verify exact suffix and pinout; follow ADR-0034 and the [UART ICD](../icd/controller-telemetry.md).

## Power and electrical sensing (motherboard)

These are **new proposed MPNs**. Input is 4S LiPo (up to 16.8 V) and a 19 V wall supply for low-voltage loads. The updated main scenario uses **TBC 36–43 A aggregate pack peak at 9 m/s**, with 4–5 kg mass, 100 mm wheels and 18:1 gearing. Pack/input and individual branch ratings must be reviewed separately; final motor and measured mass remain open. See [power scenarios](../budgets/power-scenarios.md) and [ADR-0044 sizing review](../adr/0044-ee-sizing-after-custom-corners.md).

| Function / choice | Exact manufacturer part | JLCPCB | Qty / power board | USD each | Package / design notes |
| --- | --- | --- | --- | --- | --- |
| 12 V rail — 🟢 **Recommend** | TI **LM5176PWPR** | [C442493](https://jlcpcb.com/partdetail/TexasInstruments-LM5176PWPR/C442493) | 1 | **$3.85** | HTSSOP-28 EP; 55 V four-switch buck-boost controller. Proposed ~8 A aggregate stage; validate compute + sensor load and thermals. Separate monitored / switched / fused branches. |
| Main 5 V rail — 🟢 **Recommend** | TI **LM5146RGYR** | [C3188679](https://jlcpcb.com/partdetail/TexasInstruments-LM5146RGYR/C3188679) | 1 | **$2.15** | VQFN-20 EP; 100 V synchronous buck controller. External FETs / inductor set current; ≥5 A is a sizing proposal. |
| Main 3.3 V rail — 🟢 **Recommend** | TI **TPS54360DDAR** | [C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | 1 | **$0.75** | SOIC-8 EP; 60 V, 3.5 A asynchronous buck. Add rated Schottky diode and magnetics; verify H723 + sensor aggregate load. |
| Battery / wall LV ORing — 🟢 **Recommend** | TI **LM74700QDBVRQ1** | [C2941042](https://jlcpcb.com/partdetail/TexasInstruments-LM74700QDBVRQ1/C2941042) | **2** | **$1.78** | SOT-23-6; ideal-diode controllers plus external N-FETs. Wall input feeds LV only; keep it off the motor bus. |
| Regulator / ORing FET — 🟢 **Candidate** | TI **CSD18540Q5B** | [C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | **8 candidates** | **~$2.29 preorder; MOQ 4** | SON 5 × 6 mm, 60 V. Four for LM5176, two for LM5146, two for ORing; each function still needs loss / gate / SOA qualification. Clamp adds one separately. |
| Steering switch — 🟢 **Conditional recommendation** | TI **TPS26630RGET** | [C2864391](https://jlcpcb.com/partdetail/TexasInstruments-TPS26630RGET/C2864391) | 1 | **~$3.59 preorder; MOQ 3; stock 0** | VQFN-24 EP; 60 V / 6 A eFuse. Use only if steering supply peaks, inrush and thermal loading fit; establish regenerative-current path. Otherwise design a higher-current external-FET switch. |
| Battery and branch V/I — 🟢 **Recommend** | TI **INA226AIDGSR** | [C49851](https://jlcpcb.com/partdetail/TexasInstruments-INA226AIDGSR/C49851) | **6 provisional** | **$0.75** | VSSOP-10; I²C shunt / bus monitor, 36 V bus range. Battery + compute + sensor + 5 V + 3.3 V + steering; each needs its own shunt and address. |
| V/I sensing — 🟡 **Alternative** | TI **INA240A1PWR** | [C93965](https://jlcpcb.com/partdetail/TexasInstruments-INA240A1PWR/C93965) | Per selected channel, replacing INA226 | **$2.48** | TSSOP-8; analog gain 20, −4 to 80 V common mode, PWM rejection. Adds ADC / midpoint reference and a separate bus-voltage divider. |
| Battery shunt — 🟢 **Recommend** | Milliohm **HoLRS3920-5W-1mR-1%** | [C558628](https://jlcpcb.com/partdetail/Milliohm-HolRS3920_5W_1mR_1/C558628) | 1 | **$0.3167** | 3920, 1 mΩ, 5 W. At 45 A: 45 mV / 2.03 W; Kelvin routing and thermal derating required. Select branch shunt values separately. |
| Four-cell voltage monitor — 🟢 **Recommend** | Analog Devices **LTC6810IG-1#3ZZPBF** | [C689559](https://jlcpcb.com/partdetail/AnalogDevices-LTC6810IG_13ZZPBF/C689559) | 1 | **~$10.73 preorder; MOQ 1; stock 0** | SSOP-44; six-cell precision monitor configured for 4S. Fits fresh ≥100 Hz / ±10 mV; sourcing unresolved. |
| Cell monitor — 🟡 **Alternative** | Analog Devices **LTC6810IG-2#3ZZTRPBF** | [C689562](https://jlcpcb.com/partdetail/AnalogDevices-LTC6810IG_23ZZTRPBF/C689562) | 1, replacing above | **$41.59** | SSOP-44; addressable interface variant. Only 5 orderable units seen. Review the different interface / pin connections. |
| Cell monitor — 🔴 **Fails requirement** | TI **BQ7692003PWR** | [C601650](https://jlcpcb.com/partdetail/BQ7692003PWR/C601650) | Do not fit | **~$1.88**, indexed JLC estimate | Approximately 250 ms fresh cell-voltage cycle (~4 Hz) fails ≥100 Hz. Low-cost BMS IC is not an equivalent high-rate logger. |
| Steering hardware delay — 🟢 **Candidate** | TI **TLC555IDR** | [C6987](https://jlcpcb.com/partdetail/TexasInstruments-TLC555IDR/C6987) | **1** | **$0.3749** | SOIC-8; proposed ~4 s cut, after the four drive-board cuts. Timing / reset / rearm / supply-loss behavior must be designed. |
| Raw LV surge TVS — 🟢 **Candidate** | SETsafe / SETfuse **SMCJ24A** | [C2996490](https://jlcpcb.com/partdetail/SETsafe_SETfuse-SMCJ24A/C2996490) | Per protected input; TBD | **$0.1000** | SMC; 24 V standoff, specified clamp about 38.9 V. Coordinate all connected component ratings and transient overshoot. |

**Cell-monitor implementation:** normal 7 kHz ADC mode gives all-six-cell conversion plus calibration in about 1.165 ms (2.328 ms with MCAL), with specified max C-input error about ±3.1 mV at 4.2 V over temperature. This leaves room for the board's ±10 mV target. Do not combine the fastest conversion headline with the best-accuracy headline. Implement the datasheet's 4S / unused-input wiring; provide external 5 V VREG with DRIVE floating, or the specified external-NPN supply arrangement for pack voltages above 12 V. Review direct-SPI logic levels. Fuse / filter balance taps and disconnect their off-state drain paths; sleep current alone is not zero drain. [LTC6810 datasheet, electrical characteristics, VREG configurations and Table 5](https://www.analog.com/media/en/technical-documentation/data-sheets/LTC6810-1-6810-2.pdf).

**Power-sync interface proposal:** keep INA226 monitors on a dedicated power-board I²C bus, separate from ToF; LTC6810 requires an additional SPI interface and select. This differs from the single-I²C-only suggestion in the newer [proposed ADR-0022](../adr/0022-power-board-telemetry-and-shutdown.md), so resolve the internal motherboard bus choice before layout. No power-board MCU is added. Reserve alert / fault inputs, shutdown request / acknowledgement and the proposed analog fallback; low-battery cutoff / handshake circuitry remains to be selected from the finalized shutdown policy.

**Monitor / protection details:** configure INA226 conversion time and averaging for fresh ≥100 Hz data where required; its default shunt + bus pair without averaging is about 2.2 ms. INA226's 36 V measurement limit needs protection if it sees a node whose TVS clamp is higher. The selected SMCJ24A cannot by itself protect 30 V rated input components; use the higher-rated raw-input parts above and coordinated protection. Carrier-local 24 V converters belong downstream of regulated 12 V. A TVS does not replace the motor-bus braking resistor. Sources: [LM5176](https://www.ti.com/lit/ds/symlink/lm5176.pdf), [LM5146](https://www.ti.com/lit/ds/symlink/lm5146.pdf), [TPS54360](https://www.ti.com/lit/ds/symlink/tps54360.pdf), [LM74700](https://www.ti.com/lit/ds/symlink/lm74700-q1.pdf), [TPS2663](https://www.ti.com/lit/ds/symlink/tps2663.pdf), [INA226](https://www.ti.com/lit/ds/symlink/ina226.pdf), [INA240](https://www.ti.com/lit/ds/symlink/ina240.pdf), [BQ76920](https://www.ti.com/lit/ds/symlink/bq76920.pdf).

## Motor controllers and UART

**Bought A50S is the active baseline (ADR-0034).** Four installed controllers with signal connector and heatsink were GBP £400 at the retained 2026-10-07 snapshot; two spares add £200. Final landed/USD quote TBD. This is bought hardware, not JLCPCB assembly.

| Function / choice | Exact manufacturer part | JLCPCB | Qty / car | USD each | Package / design notes |
| --- | --- | --- | --- | --- | --- |
| Drive controller — 🟢 **Existing** | Team Triforce **A50S V2.3c 12S** | Bought controller; not JLC | **4 + 2 spares** | **TBD**; retained GBP £100 each | Heatsink / signal connector baseline; classic CAN commands + own 2 Mbit/s UART. Qualify firmware, temperature input, harness and e-stop interfaces |

The steering FD controller, crystal and transceiver are in the sync MCU section. UART connector contacts, grounding/isolation/protection and any added interface parts/prices remain TBD; no replacement buffer or isolator is selected here.

Each A50S USART3 TX (PB10) goes to its assigned H723 RX; RX (PB11) receives from that H723 TX. The proposed harness is **five conductors per run / 20 across four runs, TBC**, including paired references and one spare per run. Do not parallel controller supply outputs or release the return wiring before the grounding review. See [wire schedule](motherboard-wiring.md), [UART ICD](../icd/controller-telemetry.md) and [readiness review](../reviews/a50s-uart-readiness.md).

**E-stop:** a standard latching button (E-32) pulls ESTOP low on all four A50S; their firmware brakes on the ramp and the power section cuts the motor bus after T (SYS-05, ADR-0012, ADR-0034). Motor temperature comes from the Castle's own sensor if it has one, otherwise from E-28 on the can; verify on arrival.

## Motor-bus clamp

This remains an electrical assembly even if merged mechanically with the power board. It must operate from the **motor bus with the battery removed and the MCU / switched LV rails off**. The updated 4–5 kg / 9 m/s normal-stop model is **TBC 160–200 J total / 185–230 W peak** in [power scenarios](../budgets/power-scenarios.md). Clamp placement relative to the motor-bus switch remains open (#113). The 17.2 V turn-on threshold below is an illustrative sizing point, not a frozen requirement; the earlier resistor pair needs review under [ADR-0044](../adr/0044-ee-sizing-after-custom-corners.md).

| Function / choice | Exact manufacturer part | JLCPCB | Qty | USD each | Design notes |
| --- | --- | --- | --- | --- | --- |
| Comparator — 🟢 **Recommend** | TI **TLV3201AIDBVR** | [C105188](https://jlcpcb.com/partdetail/TexasInstruments-TLV3201AIDBVR/C105188) | 1 | **$0.97** | SOT-23-5; divided bus input, explicit hysteresis; 2.7–5.5 V supply. |
| Threshold reference — 🟢 **Recommend** | TI **REF3325AIDBZR** | [C69039](https://jlcpcb.com/partdetail/TexasInstruments-REF3325AIDBZR/C69039) | 1 | **$0.71** | SOT-23-3; 2.5 V reference; calculate threshold divider including total tolerance. |
| Gate driver — 🟢 **Recommend** | Microchip **TC4427EOA** | [C636891](https://jlcpcb.com/partdetail/MicrochipTech-TC4427EOA/C636891) | 1 | **$1.25** | SOIC-8; regulated local gate supply, e.g. 12 V. Its operating supply limit is 18 V; do not expose it to an uncontrolled raw bus. |
| Dump FET — 🟢 **Candidate** | TI **CSD18540Q5B** | [C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | 1 provisional | **~$2.29 preorder; MOQ 4** | 60 V SON 5 × 6; qualify switching / pulse SOA, thermal path and bus overshoot. Additional FETs may be needed after sizing. |
| Autonomous housekeeping buck — 🟢 **Candidate** | TI **TPS54360DDAR** | [C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | 1, separate from main rails | **$0.75** | Motor-bus-derived local supply for gate driver; add suitable low-voltage regulation for comparator / reference. Verify startup during battery-disconnected regen. |
| Dump resistors — 🟡 **Re-size required** | ARCOL **HS100 3R J** | [C7202847](https://jlcpcb.com/partdetail/Arcol-HS100_3RJ/C7202847) | **TBD**; former pair was 2 in parallel | **~$11.63 each**, retained consign estimate | Historical pair: 3 Ω / 100 W each gives 1.5 Ω; 197.2 W at 17.2 V is below the new upper 230 W estimate. Final value/count/threshold TBD. Catalog showed stock 0 and consignment-only sourcing. External chassis-mounted parts, not ordinary SMT assembly. |

At the illustrative 17.2 V threshold, the former 1.5 Ω pair dissipates **197.2 W**, below the updated upper **230 W** peak estimate at that voltage. It is an **incomplete sizing example**, not a released dump load. Actual resistor value/count, threshold/overshoot and cooling are **TBD**. Equal splitting of the 160–200 J model would allocate 80–100 J per resistor; that calculation does not establish pulse or repeated-stop capacity. The 100 W catalog rating depends on its specified heatsink. Review actual mass, motor, stop profile, switch placement, FET stress, hysteresis and thermal recovery before selecting parts (#97 / #113). Sources: [TLV3201](https://www.ti.com/lit/ds/symlink/tlv3201.pdf), [REF3325](https://www.ti.com/lit/ds/symlink/ref3325.pdf), [TC4427](https://ww1.microchip.com/downloads/en/DeviceDoc/20001422G.pdf), [ARCOL resistor specifications](https://www.ohmite.com/hs-aluminium-housed-resistors/).

## Connectors and installed quantities

Use [ICD-corner-connector Rev H](../icd/corner-connector.md) for corner-to-deck phases / Hall wiring and A50S Pico-Clasp assignments. Actual circuit numbering still needs verification. UART connectors/references, Pico-Clasp crimps and power/balance plugs require an exact harness / mechanical BOM; remaining part numbers and JLC prices are TBD.

| Shared part / function | Installed per car | Count derivation |
| --- | --- | --- |
| AS5047 position ICs | **10** | 4 wheels + 4 suspension + 2 steering knuckles |
| ToF ICs | **4** | Four corner ride-height sites; camera scale derivation TBD |
| MCP2518FD controllers / 40 MHz crystals | **1 each** | Existing sync steering baseline only; no A50S-local circuits |
| ARC-added TJA1051 command transceivers | **1** | Sync board only; the four bought A50S include their own classic-CAN hardware |
| TJA1462 FD transceivers | **1** | Steering only |
| Power-side cut / steering timing | **TBD** | Final timing circuitry and quantities depend on the power sequence |
| CSD18540 power-board / clamp candidates | **9** | 4 buck-boost + 2 main buck + 2 ORing + 1 clamp; excludes any further switch / parallel FETs |

The selected **17 installed state-sensor ICs** cost **USD $69.7824 / CAD $99.59 per car**: ten loose AS5047P, two ISM330DHCXTR and five ToFs. All sensor boards are assembled in house (ADR-0045); no factory sensor assembly fees apply. Magnets, support parts, bare fabrication and shared consumables are priced separately; no populated spare sensor set is included.

## External parts and remaining schematic work

Custom carrier / power / sync prices in the old cost budget are build allowances. The A50S is bought hardware under ADR-0034; four installed units and two spares are separate from motherboard JLC component costs. UART interface/protection parts and prices remain TBD. Keep these external items separate:

| External item | Qty / car | Budget basis — not a live JLC quote |
| --- | --- | --- |
| Jetson Orin NX 16 GB module | 1 | **$950–1,100**, existing team estimate |
| 4 TB M.2 NVMe SSD | 1 | **$250–450**, existing team estimate |
| MT7922 Wi-Fi card + antennas | 1 set | **$25–50**, existing team estimate; AX210 is the accepted fallback |
| Compute heatsink + fan | 1 | **$30–60**, existing team estimate |
| ≥5-port unmanaged GbE switch | 1 | **$40–150**, existing team estimate; exact input rail depends on selected switch |
| moteus-c1 steering controller | 1 | **$69**, project baseline in ADR-0019; confirm supplier quote |
| Castle 1010-4400kV drive motors (E-51) | 4 | $100–130 each allowance; confirm stock for four |

These estimates come from [cost.csv](../budgets/cost.csv). They are context for the full car, not evidence of current JLC availability. The ground-speed lighting remains the LED modes plus a certified Class 1 module specified in [ADR-0014](../adr/0014-ground-speed-event-camera.md); select its exact module and driver once the optical test settles the load.

Before exporting an orderable assembly BOM, finish these specific items:

1. **Carrier:** frozen Antmicro revision and full inherited BOM, module power mode / input path, GbE magnetics / RJ45, all local rail currents, EEPROM / sequencing / reset / fan parts, connectors and mounting heights. Confirm the four CSI connector pinouts even though camera procurement is out of scope.
2. **Sync:** complete MCU pin map; HSE / PHY clock scheme; main MCU supply / decoupling; ToF pull-ups; sensor connectors / satellite PCBs; power-board bus and logic levels; CAN terminations / protection / optional chokes. Verify harnessed SPI before routing long runs.
3. **Power:** branch-current peaks and steering regen path; exact converter FET suitability, inductors, diodes, compensation and capacitors; branch shunt values; branch fuses / switches; cell-tap protection / off-state isolation; internal power/sync connections and test access. A current-limit IC does not remove the physical-fuse requirement.
4. **A50S / UART / e-stop:** retain the `a50s_v23c_12s` target and qualify bought-controller cooling / phase current; qualify four 2 Mbit/s UARTs, source timestamps/clock mapping, driver/DMA ownership and actual contacts, logic/reference/protection behavior. Preserve Hall / ESTOP / recovery. See the [readiness review](../reviews/a50s-uart-readiness.md) and [arrival checks](../tests/a50s-arrival.md). Define the fault-stop / re-arm sequence (ADR-0039). Confirm the motor-temperature source (Castle sensor or E-28) and Aux-power behavior. Timer-only coasting fallback cannot pass the fab gate.
5. **Clamp / assembly:** independent startup supply and protection, final threshold, resistor pulse data and heatsink; exact power connectors, switch / loop key, wire gauges and harness crimps; sensor magnets; prototype spare quantities. Recheck price / stock through the JLC BOM tool using the final reference-designator BOM.

Items in this list are required schematic / mechanical selections; their exact MPN, quantity and JLC cost remain **TBD**. A complete board total becomes meaningful once those are filled in; the tables above price the selected main components only.

