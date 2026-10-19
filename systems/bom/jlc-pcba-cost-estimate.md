# PCB funding estimate — CAD

- **Status:** Draft (TBC planning estimate)
- **Updated:** 2026-10-16
- **Currency:** CAD funding figures; USD supplier evidence
- **Traces to:** SYS-02, SYS-07, SYS-08, SYS-14, SYS-17, SYS-19, SYS-24, E-02, E-20, E-21, E-22, E-23, E-24, E-26, E-30, E-41, E-43, E-46, ADR-0042, ADR-0043, ADR-0045, ADR-0046

This prices our PCBs for each build (ADR-0046), with **30% contingency** on the PCB package. The [total electrical cost](total-electrical-cost.md) adds the bought hardware and tax.

| Build | Boards | Working base, CAD | With 30%, CAD | Funding allowance, CAD |
| --- | --- | --- | --- | --- |
| **v1.0** | Combined power/sync motherboard, optional bus clamp, 10 angle + 2 IMU + 5 ToF sensor boards | **$1,500.22** | **$1,950.29** | **$2,000** |
| **v1.1** | Jetson carrier (Antmicro fork) | **$924.26** | **$1,201.54** | **$1,300** |

The v1.0 planning range is **CAD $1,183.68–1,816.73 base**, or **$1,538.79–2,361.76 with 30%**; v1.1's is **$753.01–1,095.51**, or **$978.91–1,424.16**. Each allowance is the buffered midpoint rounded **up** to the next CAD $100: a working figure, not a maximum or a supplier quote.

JLC factory-assembles the motherboard, the clamp and (in v1.1) the carrier, with its two-board minimum per design: one installed and one minimum-run surplus, not a second car. **All seventeen sensor boards are assembled in house** from bare PCBs and loose parts (ADR-0045). No populated spare sensor set or spare sensor ICs are funded. Final Gerber/BOM/CPL and loose-component carts must be reconciled before ordering.

## Exchange rate and calculation

Use **USD 1 = CAD 1.4271**, the [Bank of Canada daily rate for 2026-10-09](https://www.bankofcanada.ca/rates/exchange/daily-exchange-rates/). This is an indicative budgeting conversion; card/distributor fees and payment-date rates can differ. Refresh before submission or payment.

`Working USD base = (low + high) ÷ 2`; `CAD base = USD base × 1.4271`; `CAD with contingency = CAD base × 1.30`. Sum unrounded inputs, apply the 30% once, then round to cents; rounded rows can differ from the totals by a few cents. The [CSV model](../budgets/jlc-pcba-cost.csv) keeps the USD inputs, quantities, dated FX and a `build` column.

## v1.0 boards

| Assembly / allowance | Installed / bare / JLC populated / in-house populated | Base range, CAD | Working base, CAD | Working with 30%, CAD |
| --- | --- | --- | --- | --- |
| Combined power/sync motherboard | 1 / 5 / 2 / 0 | $773.27–$1,130.05 | $951.66 | $1,237.16 |
| AS5047P angle boards | 10 / 10 / 0 / 10 | $89.49–$124.60 | $107.05 | $139.16 |
| ISM330DHCXTR IMU boards | 2 / 5 / 0 / 2 | $26.66–$32.36 | $29.51 | $38.36 |
| Ride-height ToF boards (4 corners + 1 at the ground-speed camera) | 5 / 5 / 0 / 5 | $37.38–$51.65 | $44.52 | $57.87 |
| Shared in-house sensor assembly consumables | One shared allowance | $21.41–$42.81 | $32.11 | $41.74 |
| Optional separate bus-clamp PCBs | 1 / 5 / 2 / 0 | $92.76–$164.12 | $128.44 | $166.97 |
| JLC shipping allowance | One shipment | $114.17–$214.06 | $164.12 | $213.35 |
| Loose-component shipping allowance | One shipment | $28.54–$57.08 | $42.81 | $55.66 |
| **Total** | **18 installed / 35 bare / 4 JLC / 17 in house** | **$1,183.68–$1,816.73** | **$1,500.22** | **$1,950.29** |

The same v1.0 base by cost category:

| Cost category | Working base, CAD | Working with 30%, CAD |
| --- | --- | --- |
| Bare PCB fabrication — 35 boards | $174.74 | $227.16 |
| Purchased electronic parts — all populated boards | $718.95 | $934.64 |
| JLC factory assembly — motherboard and clamp, 2 each | $367.48 | $477.72 |
| Shared in-house sensor consumables | $32.11 | $41.74 |
| JLC shipping | $164.12 | $213.35 |
| Loose-component shipping | $42.81 | $55.66 |
| **Total** | **$1,500.22** | **$1,950.29** |

## v1.1 boards

| Assembly / allowance | Installed / bare / JLC populated | Base range, CAD | Working base, CAD | Working with 30%, CAD |
| --- | --- | --- | --- | --- |
| Jetson carrier (Antmicro fork) | 1 / 5 / 2 | $638.84–$881.45 | $760.14 | $988.19 |
| JLC shipping allowance (second order) | One shipment | $114.17–$214.06 | $164.12 | $213.35 |
| **Total** | | **$753.01–$1,095.51** | **$924.26** | **$1,201.54** |

The carrier's base is $153.63 bare fabrication, $299.69 parts and $306.83 JLC assembly. Its parts allowance (USD $160–260 for two boards) is thin for a 669-reference design; requote it from the repaired CAD before v1.1 is funded.

## Loose sensor component purchase basis

The following [LCSC](https://www.lcsc.com/) loose-component listings were checked on **2026-10-12**, in **USD**. They permit **minimum 1, multiple 1, sales unit piece**. Quantities match fitted boards exactly; no spare sensor ICs are included.

| Selected / preferred IC | Loose fitted quantity | Unit-price tier, USD | Chip total, USD | Chip total, CAD | Stock snapshot |
| --- | --- | --- | --- | --- | --- |
| [AS5047P-ATSM, C962063](https://www.lcsc.com/product-detail/C962063.html) | 10 | $4.231 at 10+ | $42.3100 | $60.38 | 134 |
| [ISM330DHCXTR, C2655101](https://www.lcsc.com/product-detail/C2655101.html) | 2 | $5.7392 at 1+ | $11.4784 | $16.38 | 657 |
| [Preferred/TBC VL53L4CDV0DH/1, C3178291](https://www.lcsc.com/product-detail/C3178291.html) | 5 | $3.1988 at 1+ | $15.9940 | $22.83 | 11,040 |
| **All seventeen fitted sensing ICs** | **17** | | **$69.7824** | **$99.59** | Not reserved |

These are distributor prices for loose stock, not JLC PCBA-library prices. Recheck availability and price before buying. The ICs are already included in the sensor rows; **do not add them again**. Magnets, mounts and harness mating parts are outside this PCB budget.

Sensor support allowances are **USD $10–30 for ten angle boards, $2–6 for two IMU boards and $5–15 for five ToF boards**: **USD $17–51 total**, or $1–3 per populated board TBC. They cover local decoupling, board connectors and reviewed straps, pull-ups, damping and protection where required. Exact MPNs, packages and quantities are TBD in schematics. One shared **USD $15–30** allowance covers solder, flux, cleaning and an optional stencil across all three families, assuming existing workshop tools and volunteer labour. The separate **USD $20–40 loose-component shipping allowance** covers one consolidated order.

## Fabrication evidence and design assumptions

Supplier evidence here is **USD**. Observed on JLCPCB's [public calculator](https://cart.jlcpcb.com/quote/) on **2026-10-12**, without uploading design files or saving/purchasing an order:

| Scenario | Calculator quantity / dimensions | Observed bare-board total, USD | Limits |
| --- | --- | --- | --- |
| Carrier | 5 boards, 8 layers, **130 × 70 mm one-up panel**, 1.6 mm, green, ENIG, 1 oz outer / 0.5 oz inner copper | **$107.65** | $99 engineering + $7.60 board + $1.05 production-file confirmation. Body reference 120 × 60 mm (ADR-0043); proposed 5 mm rails give the costing panel. Final rails, stackup and routing TBC |
| Motherboard | 5 boards, 6 layers, **150 × 100 mm**, 1.6 mm, green, ENIG, 2 oz outer / 0.5 oz inner copper | **$81.85** | $33 engineering + $38.10 outer-copper charge + $9.70 board + $1.05 confirmation. Layer/copper choices are costing assumptions, not qualified current/thermal ratings |
| Small sensor reference | 5 boards, 2 layers, **40 × 30 mm**, 1.6 mm, green, lead-free HASL, 1 oz, single PCB, deburring enabled, production-file confirmation disabled | **$5.20 web / CAD $7.42** | $4.00 base + $1.10 finish + $0.10 deburring. Bare fabrication only; chips, support, in-house assembly and shipping separate. Final outline/finish TBC |

**Screenshot evidence, captured 2026-10-12:** [five small bare PCBs and selected options](pricing/2026-10-12/sensor-pcb-5.jpg), [published JLC assembly fee schedule](pricing/2026-10-12/jlc-assembly-fees.jpg). The $5.20 screenshot covers **five bare PCBs**, not populated sensors. The factory assembly fee screenshot applies to **core/clamp assembly assumptions only**; all sensor boards are assembled in house.

For two five-board IMU/ToF lots, retain **USD $5.20 each** as the small-board reference. For ten angle boards, **USD $10.40–15.00 is a TBC fabrication allowance**: its lower endpoint is `2 × $5.20`, a planning proxy from the five-board evidence, **not an observed ten-board quote**. The total remote fabrication allowance is **USD $20.80–25.40**. Final quantity pricing, layer counts, finish, outlines and any mechanical variants must be quoted from CAD. Bare-board pricing does not impose JLC's Standard PCBA panel minimum on these in-house sensor boards.

No Gerbers/BOM/CPL were uploaded and no order placed; there is no final complete-PCBA cart screenshot. Prices exclude coupons, assembly, components, shipping and tax unless explicitly included elsewhere. Larger boards, HDI, different vias/impedance or a changed stackup require a revised quote. The motherboard envelope and cooler remain subject to ADR-0043.

## Core assembly and component allowance basis

The carrier, motherboard and optional clamp retain the previous **two factory-populated units per design** cost scenario, with one installed and one minimum-run surplus. The carrier is a v1.1 item (ADR-0046). Component and assembly ranges are planning allowances pending final schematics/BOM/CPL; installing one board per design does not remove those supplier-minimum charges.

The [JLC assembly fee schedule](https://jlcpcb.com/help/article/pcb-assembly-price), checked on 2026-10-12, gives single-side Standard setup/stencil **USD $33.77**, feeder loading **$1.53 per distinct part type**, placement **$0.0016 per joint**, and **$16.42 for two rigid-PCB fixtures** at prototype quantities. Core rows retain their double-sided **USD $180–250 assembly allowances**; optional clamp assembly retains **$35–50 TBC**. JLC's [assembly capabilities](https://jlcpcb.com/capabilities/pcb-assembly-capabilities) list a **70 × 70 mm minimum for Standard single-board assembly**; the carrier costing panel is 130 × 70 mm. These service rules relate to factory-assembled core/clamp designs, not sensor fabrication.

The ICs' JLC Standard-only/X-ray catalog conditions no longer create sensor factory fees: we buy loose distributor chips and assemble all seventeen boards in house. Retained core component price references include STM32H723 USD $11.05, LTC6810 USD $10.73 estimated/preorder and TPS26630 USD $3.59 estimated/preorder with MOQ 3. Exact core reference-designator counts, passives, magnetics, connectors, protection, stock and attrition remain TBD/TBC in the final quote.

## Exclusions and quote gates

Excluded: compute, SSD, Wi-Fi, cooling, cameras, LiDAR, Ethernet switch, the bought A50S and moteus controllers, motors, batteries, wiring and mating crimps, sensor magnets and mechanics, external dump resistors and heatsinks, tax, import and brokerage, paid design or assembly labour, new workshop equipment and functional validation. The 30% does not guarantee a respin or major scope change is covered.

Before ordering: finish schematics and the connector, grounding and power decisions; freeze stackups, panels and populations; export Gerbers/BOM/CPL; get actual PCB/PCBA and loose-parts carts; confirm stock, shipping and the in-house assembly process; then apply the [fab gate](../reviews/fab-gate.md). For v1.1, repair and validate the [carrier Altium conversion](../../hardware/orin-carrier/README.md) first. Recalculate if quantities, source, FX or scope change. No order has been placed.
