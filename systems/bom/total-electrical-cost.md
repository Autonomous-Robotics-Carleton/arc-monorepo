# Total electrical cost by build

- **Status:** Draft estimate; final BOMs, quotes and qualification pending
- **Date:** 2026-10-16
- **Currency:** CAD totals; CAD, USD and GBP supplier prices
- **Traces to:** SYS-01, SYS-03, SYS-05, SYS-08, SYS-09, SYS-10, SYS-17, SYS-19, SYS-24, ADR-0034, ADR-0042, ADR-0045, ADR-0046

The car is bought in two orders, one per build (ADR-0046). Each estimate includes 13% Ontario HST and a 30% contingency on the PCB package only.

| Build | Priced estimate, CAD | Funding request, CAD | Planning range, CAD |
| --- | --- | --- | --- |
| **v1.0** | **$6,723.58** | **$6,800** | $5,761.83–$7,685.33 |
| **v1.1** (adds to v1.0) | **$7,582.27** | **$7,600** | $6,911.42–$8,253.12 |
| Both | $14,305.85 | $14,400 | |

Not included: mechanical parts (in [cost.csv](../budgets/cost.csv)), ground equipment, import duty and brokerage, the Class 1 laser module (quote needed) and the GenX320s' actual price (an allowance until quoted). The 30% covers only the PCBs; bought hardware has no contingency.

## Summary

| Category | v1.0 working base, CAD | v1.1 working base, CAD |
| --- | --- | --- |
| PCB package ([PCB estimate](jlc-pcba-cost-estimate.md)) | $1,500.22 | $924.26 |
| Compute | $265.99 | $2,247.53 |
| Perception | $549.19 | $3,061.12 |
| Drive and steering | $1,993.00 | — |
| Power, harness and accessories | $991.82 | — |
| Bought-hardware shipping | $199.79 | $199.79 |
| **Base** | **$5,500.01** | **$6,432.69** |
| 30% of the PCB package | $450.06 | $277.28 |
| Subtotal before tax | $5,950.07 | $6,709.97 |
| Estimated 13% HST | $773.51 | $872.30 |
| **Priced estimate** | **$6,723.58** | **$7,582.27** |

HST uses the [CRA's Ontario rate](https://www.canada.ca/en/revenue-agency/services/tax/businesses/topics/gst-hst-businesses/charge-collect-which-rate/calculator.html). Supplier amounts are treated as before tax, with no rebate assumed. Replace the estimate with checkout totals when ordering, without taxing a tax-inclusive invoice twice.

## On loan, no cost

v1.0 runs on hardware the team already has: the **Orin Nano 8 GB developer kit** (module, carrier and cooler) and the **Hokuyo UST-20LX**, both from the existing car, and the **19 V wall supply**. The dev kit and LiDAR go back when v1.1 arrives.

## v1.0 bought hardware

Line totals for the stated quantity. Fixed prices are used directly; TBC ranges use their midpoints. Supplier checks were made on 2026-10-12/13 unless noted.

| Item | Quantity | Supplier price | Working CAD | Source |
| --- | --- | --- | --- | --- |
| MT7922 / RZ616 Wi-Fi card + antennas — E-05 | 1 set | CAD $24.00 + $19.00 | $43.00 | [Framework Canada card](https://frame.work/ca/en/products/amd-rz616-wi-fi-6e?v=FRANRFFX01), [antenna module](https://frame.work/ca/en/products/antenna-module) |
| Unmanaged ≥5-port GbE switch — E-06 | 1 | CAD $22.99 | $22.99 | [TP-Link TL-SG105 at Canada Computers](https://www.canadacomputers.com/en/unmanaged-switches/58859/tp-link-tl-sg105-soho-5-port-10-100-1000mbps-desktop-switch-tl-sg105.html) |
| 1 TB NVMe with DRAM — E-04 | 1 | CAD $150–250 (TBC allowance) | $200.00 | Not yet chosen; needs good sustained writes ([logging](#camera-links-and-logging)) |
| Arducam AR0234 stereo kit — E-11 | 1 kit / 2 cameras | USD $259.99 | $371.03 | [B0492N listed price](https://www.arducam.com/blog/product/arducam-2-3mp2-ar0234-color-global-shutter-synchronized-stereo-camera-bundle-kit-for-nvidia-jetson-agx-orin-orin-nano-orin-nx/) |
| Arducam B0223 OV9281 MIPI, downward — E-25 | 1 | USD $51.99 | $74.19 | [Arducam B0223](https://www.arducam.com/blog/product/arducam-ov9281-mipi-1mp-monochrome-global-shutter-ir-camera-module-for-jetson-board/) |
| Ground LEDs, polarizer film, optics/driver — E-29 | 1 set | USD $6.95 + $35.90 + $15–45 (TBC) | $103.96 | [Adafruit 754](https://www.adafruit.com/product/754), [Edmund 86-178](https://www.edmundoptics.com/p/50-x-50mm-018mm-thickness-polarizing-laminated-film/28636/), [Adafruit 6354 driver candidate](https://www.adafruit.com/product/6354) |
| Triforce A50S V2.3c 12S + heatsink — E-50 | 6: four installed, two spares | GBP £600 | $1,132.74 | [Triforce](https://teamtriforceuk.com/a50s-v2/): £70 + £30 heatsink each |
| Castle 1010-4400kV — E-51 | 4 | USD $399.80 | $570.55 | [GalliMods 060-0098-00](https://www.gallimods.com/shop/c/p/114-1010-4400KV-Sensored-Brushless-Motor-18-Shaft-x84282061.htm): $99.95 each, one shown in stock; confirm four |
| moteus-c1 + mj5208 + belt/pulleys/mount — E-31 | 1 actuator | USD $69 + $74 + $30–90 (TBC) | $289.70 | [moteus-c1](https://mjbots.com/products/moteus-c1), [mj5208](https://mjbots.com/products/mj5208), [SDP/SI belts and pulleys](https://www.sdp-si.com/products/Timing-Belt-Pulleys/index.php) |
| Gens ace 5300 mAh 4S hardcase packs — E-40 | 3–4 | USD $95.99 each | $479.46 | [Gens ace GEA534S60E5GT](https://genstattu.com/gens-ace-5300mah-4s-60c-14-8v-hardcase-g-tech-lipo-battery14-with-ec5-plug/) |
| Harness and motor distribution — E-45 | 1 car set | USD $145.05–345.05 (TBC) | $349.71 | [Molex Pico-Clasp 501189](https://www.molex.com/en-us/products/series-chart/501189), [Adafruit 30 AWG wire](https://www.adafruit.com/product/2051) |
| E-stop, loop key, hotspot button, status LEDs, panel cable — E-32 / E-42 / E-08 / E-33 / E-09 | 1 each | CAD $8.43 + $9.99; USD $1.95 + $12.00 + $4.95 | $45.39 | [DFRobot FIT0156](https://ca.robotshop.com/products/emergency-stop-switch), [XT90-S pair](https://www.adrenalinehobby.com/products/connectors-xt90s-male-female-anti-spark), [Adafruit 1479](https://www.adafruit.com/product/1479), [297](https://www.adafruit.com/product/297) / [298](https://www.adafruit.com/product/298), [909](https://www.adafruit.com/product/909) |
| Dump resistors + heatsink — E-43 | 1 load | CAD $75–125 (TBC) | $100.00 | [Ohmite HS100 3R F](https://www.digikey.ca/en/products/detail/ohmite/HS100-3R-F/5307114) |
| Encoder magnets ×10 and E-28 thermistors ×4 | 10 + 4 | USD $4.60 + $5–10 (TBC) | $17.26 | [K&J D42DIA](https://www.kjmagnetics.com/d42dia-neodymium-diametric-disc-magnet); NTC allowance |
| Bought-hardware shipping | One allowance | USD $80–200 (TBC) | $199.79 | [Triforce](https://teamtriforceuk.com/shipping-returns/), [Adafruit](https://www.adafruit.com/shipping) shipping policies |

The camera kit prices include their HATs, stock lenses and cables. The A50S's optional fully populated signal connector is another £5 each and isn't included.

## v1.1 additions

| Item | Quantity | Supplier price | Working CAD | Source |
| --- | --- | --- | --- | --- |
| Jetson Orin NX 16GB — E-01 | 1 | USD $1,199.99 | $1,712.51 | [Waveshare SKU 24056](https://www.waveshare.com/jetson-orin-nx.htm?sku=24056); NVIDIA's 1KU list is $899, so shop around |
| NX cooler — E-03 | 1 | USD $24.90 | $35.53 | [Seeed Super-mode cooler](https://www.seeedstudio.com/Aluminum-Heatsink-with-Fan-for-Jetson-Orintm-NX-Orintm-Nano-Module-in-MAXN-SUPER-p-6636.html) |
| 4 TB NVMe — E-04 | 1 | USD $250–450 (TBC) | $499.49 | Estimate |
| Hokuyo UST-10LX — E-10 | 1 | USD $1,200.00 | $1,712.52 | [Acroname new unit](https://acroname.com/store/scanning-laser-rangefinder-ethernet-r359-ust-10lx) |
| Arducam quad OV9281 kit — E-12 | 1 kit / 4 cameras | USD $244.99 | $349.63 | [B0267 listed price](https://www.arducam.com/blog/product/arducam-1mp4-quadrascopic-camera-bundle-kit-for-raspberry-pi-nvidia-jetson-nano-xavier-nx-four-ov9281-global-shutter-monochrome-camera-modules-and-camarray-camera-hat/) |
| Prophesee GenX320 — E-13, E-25 | 2 | USD $600–800 (allowance until quoted) | $998.97 | [Prophesee starter kit](https://www.prophesee.ai/event-based-starter-kit-genx320-raspberry-pi-5/) (quote only) |
| Class 1 speckle module — E-29 | 1 | Quote needed | TBD | [Brightlaser VM-0850F-025M-HL-0A0](https://www.brightlaser.com.hk/download/en/202201/VM-0850F-025M-HL-0A0.pdf) candidate |
| Bought-hardware shipping | One allowance | USD $80–200 (TBC) | $199.79 | |

## Camera links and logging

Rates are screening modes from the vendors, not qualified operating points; the 10% protocol allowance is TBC.

| Camera | Mode | Payload, Gbit/s | Notes |
| --- | --- | --- | --- |
| AR0234 stereo (both builds) | 3840 × 1200 RAW10, 38 fps | 1.751 | v1.0: dev kit CAM1, 4 lanes |
| B0223 downward (v1.0) | 640 × 400 RAW10, 240 fps | 0.614 | Dev kit CAM0, 2 lanes |
| Quad OV9281 (v1.1) | 5120 × 720 RAW10, 40 fps | 1.475 | The stock kit's two 1 Gbit/s lanes are ~81% loaded with 10%, over SYS-24's 50%; interface fix and cost TBD |
| GenX320 ×2 (v1.1) | Event stream | TBD | Rate depends on scene and settings |

**v1.0 logging:** the stereo pair and B0223 packed losslessly to 10 bits write about **296 MB/s**, or **178 GB per 10-minute run**. SYS-24's 50% rule needs **≥ 591 MB/s sustained** from the 1 TB drive at installed temperature and fill. Check the drive's speed after its cache fills before buying. **v1.1 logging** depends on the GenX320 rate; size the 4 TB drive's sustained speed once it's known.

## Open items

| Item | Work |
| --- | --- |
| GenX320 price and driver | Request the Prophesee quote now; v1.1 depends on it |
| Class 1 module | Quote, certification evidence and installed classification (v1.1) |
| Quad camera bandwidth (RSK-01) | The stock kit exceeds SYS-24's 50% at full resolution; interface fix and cost TBD (v1.1) |
| Castle 1010 stock | Only one shown in stock at GalliMods; confirm four before the v1.0 order |
| 1 TB SSD | Choose a drive with DRAM and verified sustained writes |
| Imports | Shipping, duty and brokerage on the UK and US orders TBC |
| Contingency | Only the PCBs carry 30%; adding 10% on bought hardware would add about CAD $400 to v1.0 and $550 to v1.1 before tax |

## Calculation basis

Use the [Bank of Canada 2026-10-09 rates](https://www.bankofcanada.ca/rates/exchange/daily-exchange-rates/): **USD 1 = CAD 1.4271; GBP 1 = CAD 1.8879**. Canadian prices stay in CAD. Add unrounded line totals, use range midpoints, add 30% of the PCB base only, then 13% HST. Round each funding request up to the next CAD $100. Refresh carts and FX before submitting. [cost.csv](../budgets/cost.csv) lists every line, in USD, with a `build` column (`both`, `v1.0`, `v1.1`).
