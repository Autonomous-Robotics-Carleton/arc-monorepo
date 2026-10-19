# PCB component breakdown

- **Status:** Draft. Complete extracted carrier reference; ARC component selections and missing schematic details recorded below
- **Scope:** One separate Jetson carrier (v1.1, ADR-0046), one combined power/sync motherboard, three remote sensor designs and the optional separate bus-clamp design
- **Traces to:** E-02, E-20, E-21, E-22, E-23, E-24, E-26, E-30, E-41, E-43, E-46; ADR-0025, ADR-0042, ADR-0034, ADR-0045

This breaks down the parts for **each physical PCB design**, following [the main selection list](pcb-parts-selection.md). **Every carrier-reference part is listed**, including all passives, in the [complete 669-reference inventory](jetson-carrier-reference-parts.md). The motherboard and remote sensor boards have no captured/released schematics yet; their exact passive values, package suffixes, connector MPNs, designators and many quantities remain **TBD**. Their tables therefore list all currently named parts and the supporting component groups that still need capture; they are not falsely presented as a finished production BOM.

Use these files together: [total electrical cost](total-electrical-cost.md) includes bought hardware such as the NX, cameras, LiDAR, controllers, motors and packs; [the existing PCB estimate](jlc-pcba-cost-estimate.md) keeps fabrication, parts, assembly, consumables and shipping separate.

## Physical board and purchase quantities

| PCB design | Installed | Bare fabrication scenario | JLC populated | In-house populated | Scope |
| --- | --- | --- | --- | --- | --- |
| Jetson carrier — E-02 (v1.1) | 1 | 5 | 2 | 0 | Separate carrier; NX is bought separately |
| Combined power/sync motherboard — E-30 / E-41 / E-46 | 1 | 5 | 2 | 0 | One PCB containing both functional sections |
| AS5047P angle PCB — E-22 / E-23 / E-24 | 10 | 10 | 0 | 10 | 4 wheel + 4 suspension + 2 front-knuckle sites |
| ISM330DHCXTR IMU PCB — E-20 / E-21 | 2 | 5 | 0 | 2 | Body/CG + front axle |
| Ride-height ToF PCB — E-26 | 5 | 5 | 0 | 5 | 4 corners + 1 beside the ground-speed camera |
| Optional separate clamp PCB — E-43 | 1 if separate | 5 | 2 | 0 | Integrate only after reviewed motherboard repricing; do not duplicate parts |

Per-PCB quantities below are for **one populated board**, not the entire minimum factory batch. Multiply a confirmed population by two for each retained JLC core/clamp design, by ten for angle boards, by two for IMUs and by five for ToFs. Bare unused boards do not receive components. Attrition/rework extras, MOQ excess and replacement-design deltas require separate final quotes; no spare sensing ICs are added.

## Price and population basis

Named part prices are retained from [pcb-parts-selection.md](pcb-parts-selection.md), with its JLC/loose-stock sources and caveats, rather than new reservations for every MPN. Sensor chip prices were checked on 2026-10-12: AS5047P uses the **10+ loose tier** for the ten-board purchase, ISM330DHCXTR and VL53L4CD use their **1+ loose tiers**. Other figures retain the selection document's snapshots/preorder/consign status; recheck the final JLC BOM/cart. Convert **USD 1 = CAD 1.4271**, the existing dated budget FX. Table unit prices are before fabrication, assembly, MOQ/attrition, shipping, tax and contingency.

**Do not sum alternatives, provisional quantities, source DNPs or replacement parts into a complete PCB price.** No full component total is given for a design with missing support values/quantities. The existing build allowances stay in the PCB cost model until the actual schematics produce an orderable BOM/CPL.

## Jetson carrier (E-02)

The [reference inventory](jetson-carrier-reference-parts.md#every-component-group) enumerates **669 unique designators in 121 groups**, extracted from every pinned KiCad sheet. It preserves exact source MPNs, values, footprints and DNP/non-BOM flags; **544 source-BOM references + 77 DNP + 48 test points** are accounted for. Multi-unit symbols do not become duplicate purchases. This is the source reference, before ARC changes and Altium repairs.

### Known ARC choices / inherited sourcing references

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Module socket | **2309413-1**; [JLC reference C2766246](https://jlcpcb.com/partdetail/TEConnectivity-23094131/C2766246) | 1 | Recommendation; verify against upstream footprint/height | $2.33 / $3.33 |
| NVMe socket | **91302-67-067R2M**; [JLC reference C2922445](https://jlcpcb.com/partdetail/UMAX-PCI_Express_M_2/C2922445) | 1 | Recommendation; key M / mounting review | $1.25 / $1.78 |
| Wi-Fi socket | **APCI0085-P005A**; [JLC reference C7498138](https://jlcpcb.com/partdetail/Lian_XinTechnology-APCI0085P005A/C7498138) | 1 | Recommendation; key E / mounting review | $0.67 / $0.96 |
| ARC CSI connectors | **5051102296**; [JLC reference C6071729](https://jlcpcb.com/partdetail/5051102296/C6071729) | 4 | Proposed ARC replacement for reference J7/J8 50-pin outputs; pinouts and modes TBD | $1.67 / $2.38 |
| Local carrier converters | **SIC431AED-T1-GE3**; [JLC reference C2071422](https://jlcpcb.com/partdetail/VishayIntertech-SIC431AED_T1GE3/C2071422) | 2 in reference: U1, U40 | Exact count extracted; ARC retention/rail review pending | $2.66 / $3.80 |
| Timing / GPIO translation | **SN74AXC8T245PWR**; [JLC reference C882718](https://jlcpcb.com/partdetail/SN74AXC8T245PWR/C882718) | 1 provisional | Proposed ARC interface; reference uses other translators too | $1.11 / $1.58 |
| USB debug bridge | **FT230XQ-R**; [JLC reference C128629](https://jlcpcb.com/partdetail/Ftdi-FT230XQR/C128629) | 1 if retained: U5 | Optional debug population; do not add a second U5 | $6.95 / $9.92 |
| Proposed port ESD | **TPD4E05U06DQAR**; [JLC reference C138714](https://jlcpcb.com/partdetail/TPD4E05U06DQAR/C138714) | TBD | Recommendation; source Q1 suffix differs, verify exact variant/channel loading | $0.0838 / $0.12 |

The selection document is a recommendation list, not a promise of footprint equivalence to the source. For example, the proposed CSI connectors **replace/adapt** source J7/J8 rather than simply adding four more ports, and the exact automotive-grade ESD suffix in the reference must not inherit another suffix's price automatically. Source parts are priced only when the exact MPN matches the quoted reference.

### Remaining ARC carrier component work

| Circuit / component group | Exact part and quantity still required |
| --- | --- |
| Carrier input protection, fuse/switch/monitor interface | MPNs, ratings and quantities TBD for the motherboard's regulated branch |
| PoE removal / changed input path | Reference U22/U23 and associated circuitry are not automatically retained. Final remove/retain designator list TBD under ADR-0025 |
| SiC431 support | Source inductors, input/output capacitors, bootstrap/feedback/compensation/snubber parts are in the extracted inventory; validate final ARC rail loads before retaining every value |
| Module-ID / sequencing / power-enable / reset | Source logic, transistor, divider/strap and switch parts are all enumerated; reconcile the actual Orin NX mode and changed power path |
| Ethernet | Source magnetics/RJ45/PoE-related parts and protection are in the inventory; retain/change after PoE removal and routing review |
| CSI module control | Exact mux/translator/reset/reference-clock parts, per-kit rails and I²C address handling TBD after the bandwidth review |
| Camera-trigger / PPS / shutdown / GPIO boundary | Reviewed translators/buffers, pulls/termination, connectors and reference designators TBD |
| NVMe / Wi-Fi / USB / display interfaces | Every inherited resistor/capacitor/IC/socket is in the source table; final optional-interface population and high-speed protection TBD |
| EEPROM / debug / fan / RTC / buttons / LEDs | Source parts are enumerated; final desired population, fan interface and external-control adaptation TBD |
| Mounting / retention | Source mechanical items are enumerated; final module, SSD and carrier enclosure hardware TBD |
| High-speed release | Freeze supported lane mapping, pinout, stackup and operating modes; repair/recheck Altium GND/+3V3 and USB net merges before BOM/CPL release |

The NX module, SSD and Wi-Fi module are bought assemblies listed in total electrical cost. They are not components placed by JLC on the carrier; the corresponding sockets remain carrier PCB parts.

## Combined power/sync motherboard — E-30 / E-41 / E-46

Power and sync are **sections of one PCB** under ADR-0042. The H723 is the only main sensor MCU. The following tables retain the current named candidates and **one steering FD link**, with classic drive commands and four A50S UARTs under ADR-0034.

### Sync / communications named components

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Sync MCU | **STM32H723ZGT6**; [JLC reference C730146](https://jlcpcb.com/partdetail/Stmicroelectronics-STM32H723ZGT6/C730146) | 1 | Selected H723 family; package suffix / final pin map TBC | $11.05 / $15.77 |
| Ethernet PHY | **LAN8742AI-CZ-TR**; [JLC reference C621425](https://jlcpcb.com/partdetail/Microchip-LAN8742AI_CZTR/C621425) | 1 | Recommended; clock/straps/magnetics required | $1.64 / $2.34 |
| Steering CAN-FD controller | **MCP2518FDT-E/SL**; [JLC reference C626758](https://jlcpcb.com/partdetail/MicrochipTech-MCP2518FDT_ESL/C626758) | 1 | Existing steering baseline only; drive telemetry is UART | $1.50 / $2.14 |
| MCP crystal | **TX322540M4FBCE2T**; [JLC reference C5186937](https://jlcpcb.com/partdetail/5858100-TX322540M4FBCE2T/C5186937) | 1 | Recommended 40 MHz; load capacitors TBD | $0.0763 / $0.11 |
| Steering SIC transceiver | **TJA1462AT/0Z**; [JLC reference C3234095](https://jlcpcb.com/partdetail/NXPSemicon-TJA1462AT0Z/C3234095) | 1 | Recommended steering interface; pin/supply review | $1.00 / $1.43 |
| Classic command transceiver | **TJA1051T/3/CM,118**; [JLC reference C2671195](https://jlcpcb.com/partdetail/NXP-TJA1051T_3CM118/C2671195) | 1 | Existing family; 5 V bus / 3.3 V I/O | $0.82 / $1.17 |
| ToF I²C mux | **TCA9548APWR**; [JLC reference C130026](https://jlcpcb.com/partdetail/TexasInstruments-TCA9548APWR/C130026) | 1 | Recommendation; four used channels / reset and pull-ups TBD | $0.81 / $1.16 |
| Quiet sensor-domain LDOs | **TPS7A2033PDBVR**; [JLC reference C2862740](https://jlcpcb.com/partdetail/TexasInstruments-TPS7A2033PDBVR/C2862740) | 2 provisional | Motherboard domains; do not duplicate on every sensor automatically | $0.2151 / $0.31 |

### Power / sensing named components

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| 12 V buck-boost controller | **LM5176PWPR**; [JLC reference C442493](https://jlcpcb.com/partdetail/TexasInstruments-LM5176PWPR/C442493) | 1 | Candidate converter; FET/inductor/passive values TBD | $3.85 / $5.49 |
| 5 V buck controller | **LM5146RGYR**; [JLC reference C3188679](https://jlcpcb.com/partdetail/TexasInstruments-LM5146RGYR/C3188679) | 1 | Candidate converter; external FET/inductor/passives TBD | $2.15 / $3.07 |
| 3.3 V buck | **TPS54360DDAR**; [JLC reference C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | 1 | Candidate; external diode/inductor/support TBD | $0.75 / $1.07 |
| Wall/battery low-voltage ORing | **LM74700QDBVRQ1**; [JLC reference C2941042](https://jlcpcb.com/partdetail/TexasInstruments-LM74700QDBVRQ1/C2941042) | 2 | Candidates plus external FETs; wall stays off motor bus | $1.78 / $2.54 |
| Converter / ORing MOSFETs | **CSD18540Q5B**; [JLC reference C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | 8 candidates | Four LM5176 + two LM5146 + two ORing; excludes clamp and unselected high-current drive cut. Preorder / MOQ 4 | $2.29 / $3.27 |
| Steering switch | **TPS26630RGET**; [JLC reference C2864391](https://jlcpcb.com/partdetail/TexasInstruments-TPS26630RGET/C2864391) | 1 conditional | Candidate only if peak/inrush/thermal/regen fits; preorder / MOQ 3 | $3.59 / $5.12 |
| Battery / branch current and voltage | **INA226AIDGSR**; [JLC reference C49851](https://jlcpcb.com/partdetail/TexasInstruments-INA226AIDGSR/C49851) | 6 provisional | Battery, compute, sensor, 5 V, 3.3 V, steering; shunts/addresses TBD | $0.75 / $1.07 |
| Battery shunt | **HoLRS3920-5W-1mR-1%**; [JLC reference C558628](https://jlcpcb.com/partdetail/Milliohm-HolRS3920_5W_1mR_1/C558628) | 1 | Candidate; 1 mΩ; Kelvin/thermal validation pending | $0.3167 / $0.45 |
| Four-cell monitor | **LTC6810IG-1#3ZZPBF**; [JLC reference C689559](https://jlcpcb.com/partdetail/AnalogDevices-LTC6810IG_13ZZPBF/C689559) | 1 | Recommended candidate; taps/supply/logic-level design TBD; preorder | $10.73 / $15.31 |
| Steering delayed cut timer | **TLC555IDR**; [JLC reference C6987](https://jlcpcb.com/partdetail/TexasInstruments-TLC555IDR/C6987) | 1 candidate | Timing network and fail-safe behavior TBD; use current ICD policy | $0.3749 / $0.54 |
| Raw low-voltage input TVS | **SMCJ24A**; [JLC reference C2996490](https://jlcpcb.com/partdetail/SETsafe_SETfuse-SMCJ24A/C2996490) | TBD protected inputs | Candidate; rating coordination and quantity pending | $0.1000 / $0.14 |

These are candidates/recommendations where marked, not a released high-current stage. The eight listed converter/ORing FETs do not include the unselected high-current drive-bus cut. Branch shunts are additional to the battery shunt but their actual values/counts are not frozen.

### Supporting components to capture

Each line below still needs actual reference-designator entries; no blanket “passives included” row completes a schematic BOM.

| Circuit | Parts / values / quantities to specify |
| --- | --- |
| H723 power | VDD/analog/reference/internal-regulator decoupling, bulk capacitance and reviewed analog supply filtering: MPNs, values, ratings and quantities TBD |
| H723 boot / reset / debug | BOOT straps, reset pull/RC/protection, reset control and SWD/debug connectors: TBD |
| H723 main time base | Exact HSE crystal/oscillator, loading/bias/damping parts and optional low-frequency clock population: TBD |
| Ethernet PHY power / clock | Decoupling/filtering, 25 MHz clock source/loading or reviewed clock drive, strap/address/bias resistors: TBD |
| Ethernet physical connection | Magnetics, RJ45/connector, termination/bias/protection and shield/chassis-return components: TBD |
| MCP2518FD | Decoupling, crystal load network, reset/interrupt bias and reviewed SPI damping/protection: TBD |
| Classic and steering CAN segments | Local termination population, transceiver decoupling, control-pin pulls, connector, ESD and optional choke parts: topology/MPNs/counts TBD |
| Four A50S UARTs | Four functional ports; exact connectors, voltage/grounding/isolation/protection/bias/damping parts and population TBD. No isolator chosen |
| Angle-sensor cable ports | Ten measurement runs; header/contact design and per-signal series/ESD/return parts TBD after grouping and harness tests |
| IMU cable ports | Two SPI/CS/DRDY runs; connector/contact/protection population TBD |
| ToF cable ports / mux | Four channels; I²C pull-ups, mux reset/power bypass, XSHUT/interrupt/reset routing and connector/protection population TBD |
| Quiet LDO domains | Input/output capacitor values, domains/current budgets and optional discharge/enable parts: TBD |
| Trigger / PPS / GPIO outputs | Four camera triggers plus spare, PPS and required controls; exact translator/buffer, pull/series/protection and connector population TBD |
| Hotspot / status / external control | Debounce/bias, LED current-setting/drivers and external headers: TBD; chassis controls costed separately |
| 12 V LM5176 stage | Inductor, input/output ceramic/bulk caps, bootstrap/gate/feedback/current-limit/compensation/snubber network and thermal components: TBD |
| 5 V LM5146 stage | Inductor, input/output capacitors, bootstrap/feedback/compensation/current-limit/gate/snubber components: TBD |
| 3.3 V TPS54360 stage | Inductor, catch diode, input/output caps, bootstrap/feedback/compensation components: TBD |
| Wall/battery ORing | Control decoupling/bias and input protection; exact FET/connector and reverse-current behavior verification: TBD |
| Drive-bus switch / inhibit | FET or other switching device, driver, enable bias/interlocks, power-loss behavior and transient protection: exact topology/MPNs/counts TBD |
| Steering switched output | Candidate eFuse setting/soft-start/UVLO/fault parts, decoupling and regenerative-current path: TBD |
| Fuse / rail / input protection | Real physical fuses/holders, coordinated TVS/clamp/protection, test access and each branch's rating: TBD |
| INA226 monitoring | Five additional branch shunts if all six proposed channels are retained; Kelvin/input filtering, bypass, addresses/pull-ups/alert wiring: actual parts/counts TBD |
| LTC6810 cell monitor | 4S/unused-input network, protected balance taps, RC filters, VREG/DRIVE implementation, decoupling and reviewed logic-level parts: TBD |
| Hardware cut timing | Timer RCs/threshold/control gating/rearm/interlocks and fault behavior per current stop policy: TBD |
| Low-battery shutdown | Request/acknowledgement and final cutoff logic, analog rework fallback if chosen, pulls/protection: TBD |
| Ground-speed illumination | Exact constant-current LED driver/control, mode selection and certified module interface: TBD after final optics/loads |
| Physical board I/O | Input, regulated branches, balance lead, service/debug, sensor and controller connector MPNs/contact ratings/quantities: TBD |

The cell-monitor supply, quiet LDOs and MCU supply are distinct functions. External magnets, battery packs, wall brick, power wire/mating crimps and bought controllers/motors stay in total electrical cost; avoid buying them again as motherboard parts.

## AS5047P angle PCB — ten populated boards

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Absolute angle sensing | **AS5047P-ATSM**; [Loose LCSC C962063](https://www.lcsc.com/product-detail/C962063.html) | 1 | Selected; buy 10 for ten fitted boards; in-house assembly | $4.231 / $6.04 |

| Per-board support part | Population detail / status |
| --- | --- |
| VDD / VDD3V3 local bypass capacitors | Manufacturer-required connection/decoupling in 3.3 V operation; exact capacitor MPNs, values and counts TBD |
| Local bulk / supply filtering | Add only as needed from power/harness qualification; exact parts/counts TBD |
| Connector | One functional 7-conductor interface proposed TBC: supply, return, SCK, MOSI, MISO, CS and spare; locking MPN/contact/return design TBD |
| CS / unused-output straps | Review reset/idle state and unused ABI/PWM/output pins; exact resistors/straps/quantities TBD |
| SPI / supply protection and damping | Installed cable determines ESD, series impedance and filter requirements; values/counts TBD |
| Test pads / mounting features | Bare PCB features and mechanical geometry TBD; pads/holes are not loose purchased ICs |
| Magnet | One reviewed diametric magnet per site, **10 total**, external to PCB assembly; dimensions/field/air gap TBD |

The ten fitted chips cost **USD $42.3100 / CAD $60.38** at the retained 10+ loose tier. The existing budget separately includes ten bare PCBs and **USD $10–30** total support allowance TBC; this does not identify the missing passive/connector MPNs. A common 2-layer design remains TBC across wheel, suspension and knuckle mounting variants. No local MCU/CAN node, board-specific crystal or separate motor-temperature circuit is added.

## ISM330DHCXTR IMU PCB — two populated boards

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Inertial sensing | **ISM330DHCXTR**; [Loose LCSC C2655101](https://www.lcsc.com/product-detail/C2655101.html) | 1 | Selected at both sites; buy 2; in-house assembly | $5.7392 / $8.19 |

| Per-board support part | Population detail / status |
| --- | --- |
| VDD / VDDIO bypass capacitors | Values/counts/packages from reviewed ST circuit and actual supply arrangement; exact MPNs TBD |
| Bulk / supply filtering | Exact local network TBD from noise and cable qualification; central quiet LDOs are already on motherboard |
| SPI / interface-mode / unused-pin straps | Appropriate CS/interface state and unused I²C/interrupt configuration; exact parts/values/counts TBD |
| Connector | One functional 8-conductor interface proposed TBC: supply, GND, SCK, MOSI, MISO, CS, DRDY and spare; MPN/contact/return design TBD |
| SPI / DRDY / supply protection and damping | ESD/series/filter part values and quantities TBD for installed harness |
| Test / mounting features | Supply/interface test pads and vibration-mount holes/keepouts TBD; bare board features do not become purchased sensors |

Two fitted IMU chips cost **USD $11.4784 / CAD $16.38** at the 1+ loose tier. The existing model fabricates five bare PCBs, populates two and allows **USD $2–6** total support parts TBC. No additional precision IMU, breakout, local MCU, crystal or separate motor-temperature board is bought. LGA soldering/reflow, inspection, calibration and vibration qualification remain required.

## Corner-height ToF PCB — four populated boards

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Short-range optical sensing | **VL53L4CDV0DH/1**; [Loose LCSC C3178291](https://www.lcsc.com/product-detail/C3178291.html) | 1 | Preferred/TBC IC; four custom corner boards, in-house assembly | $3.1988 / $4.57 |

| Per-board support part | Population detail / status |
| --- | --- |
| Supply decoupling / local bulk | Actual ST circuit, selected supply and land pattern determine capacitor values/counts; exact MPNs TBD |
| I²C pull-ups | Placement/value per motherboard mux channel and cable capacitance; avoid duplicate overly strong pull-ups. Values/counts TBD |
| XSHUT / reset / GPIO bias | Startup/reset and optional interrupt routing still open; exact straps/resistors and connector contacts TBD |
| Connector | One functional 5-conductor minimum proposed TBC: supply, GND, SCL, SDA and spare; final MPN/contact design TBD. Extra reset/interrupt/returns can increase it |
| I²C / supply protection and filtering | ESD/series/filter parts and values/counts TBD after real-harness review |
| Test pads / optical keepout / mounting | Electrical test access and clean optical aperture/cover-window geometry TBD; cover/window/mechanics cost separately |

Five fitted preferred ICs cost **USD $15.9940 / CAD $22.83** at the 1+ loose tier. The model fabricates five bare PCBs, populates all five and allows **USD $5–15** total support TBC. The chip remains preferred/TBC pending installed glossy-floor/clearance/optical tests. No local MCU/CAN node is added.

## Optional motor-bus clamp PCB — E-43

| Function | Exact part / catalogue | Qty per populated PCB | Status / scope | Retained unit USD / CAD |
| --- | --- | --- | --- | --- |
| Bus comparator | **TLV3201AIDBVR**; [JLC reference C105188](https://jlcpcb.com/partdetail/TexasInstruments-TLV3201AIDBVR/C105188) | 1 | Recommendation; threshold/hysteresis network TBD | $0.97 / $1.38 |
| Threshold reference | **REF3325AIDBZR**; [JLC reference C69039](https://jlcpcb.com/partdetail/TexasInstruments-REF3325AIDBZR/C69039) | 1 | Recommendation; full tolerance budget TBD | $0.71 / $1.01 |
| Gate driver | **TC4427EOA**; [JLC reference C636891](https://jlcpcb.com/partdetail/MicrochipTech-TC4427EOA/C636891) | 1 | Recommendation; local gate-supply design TBD | $1.25 / $1.78 |
| Dump MOSFET | **CSD18540Q5B**; [JLC reference C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | 1 provisional | Candidate; separate from eight motherboard candidates; preorder / MOQ 4 | $2.29 / $3.27 |
| Autonomous housekeeping buck | **TPS54360DDAR**; [JLC reference C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | 1 | Separate from main 3.3 V buck; local support TBD | $0.75 / $1.07 |
| External dump load reference | **HS100 3R J**; [JLC reference C7202847](https://jlcpcb.com/partdetail/Arcol-HS100_3RJ/C7202847) | TBD; former pair not released | Historical ARCOL option, chassis mounted; re-size before selecting; consign quote | $11.63 / $16.60 |

The five named local IC/FET candidates, excluding the unselected external resistors, sum to **USD $5.97 / CAD $8.52 per provisional clamp**. That is only a named-device subtotal; support circuitry, fabrication and factory assembly remain additional within the existing clamp allowance. Qty/ratings can change after the power review.

| Supporting group | Actual part / quantity still required |
| --- | --- |
| Bus threshold divider / hysteresis | Resistor values/tolerance/pulse ratings, network topology and counts TBD |
| Comparator / reference supply and bypass | Local low-voltage supply/protection and capacitor/other support MPNs TBD |
| Gate supply / drive network | TC4427 bypass/bulk, gate resistors, pull-down/hold-off, transient and safe startup components: TBD |
| Autonomous buck support | Inductor, catch diode, capacitors, bootstrap/feedback/compensation and input-protection parts: TBD |
| Dump path protection | Reviewed FET/SOA, snubbers/TVS, power connectors and thermal design: MPNs/quantities TBD |
| External resistor / cooling assembly | Value/count/pulse capacity/heatsink and mounting TBD; original 3 Ω pair is historical, not released |
| Integration / power routing | Clamp location relative to drive-bus switch and merged-versus-separate population TBD; retain autonomous operation from generated bus energy |
| Test / assembly features | Bus/gate/supply test access and minimum-run panel/assembly setup TBD |

If integrated into the motherboard, transfer these parts into that board's final BOM and remove the separate-clamp build charge; do not leave two copies of the same function in procurement.

## Bought boards and final BOM closure

A50S controllers, moteus-c1, camera modules/HATs, NX, SSD and Wi-Fi are **bought assemblies**, not custom ARC bare PCBs in this breakdown. Their internal proprietary component lists are not ARC fabrication BOMs. They are purchased once through [total electrical cost](total-electrical-cost.md); no custom VESC PCB is reintroduced.

To finish a purchasable BOM, capture every missing circuit/support part; assign actual designators, MPNs, values, voltage/current/tolerance/temperature ratings, footprints and population status; reconcile connectors and in-house versus JLC quantities; obtain exact compatible stock and MOQ/attrition prices. Validate the carrier import and the camera/grounding/power/brake interfaces, then export Gerber/BOM/CPL and apply the [fab gate](../reviews/fab-gate.md). `TBD` rows are visible missing design work, not zero-cost components.
