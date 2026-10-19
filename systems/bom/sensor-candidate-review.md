# Sensor candidate review: ISM330DHCX and ELPM

- **Status:** Draft. Qualification review; selections are recorded in the ADRs.
- **Updated:** 2026-10-12
- **Traces to:** E-20, E-21, E-23, SYS-06, SYS-07, SYS-10, SYS-18, SYS-19, SYS-24, SYS-29

ADR-0045 selects ISM330DHCXTR at both IMU sites and in-house assembly for all 17 sensor PCBs: ten angle, two IMU and five ToF boards. ELPM remains an unselected evaluation option; qualification is pending.

## ISM330DHCXTR — selected for both IMU sites

The genuine ST part is **ISM330DHCXTR**, JLCPCB **C2655101**. The live JLC page checked **2026-10-12** showed **USD $5.7037 at 1+ / $4.9154 at 10+**, with stock available when checked. That JLC catalog price is a factory-service reference. Current in-house procurement uses [loose LCSC C2655101](https://www.lcsc.com/product-detail/C2655101.html), **USD $5.7392 each at 1+**, inspected 2026-10-12; buy two. Support parts, consumables, shipping and tax are separate. This is not the similarly named Tokmas ISM330DHCXTR-C.

| ARC need | Manufacturer evidence | Assessment |
| --- | --- | --- |
| E-21: ≥4 kHz accel and gyro output | Both support 6.667 kHz in high-performance mode | Meets the rate on paper; use the 6.667 kHz setting, since 3.332 kHz is below the current target |
| E-20 range targets: ±16 g, ≥±2000 °/s | ±16 g acceleration; gyro selectable up to ±4000 °/s | Range capability fits; select full scale against measured motion / clipping |
| Sync MCU interface and sample timestamp | Primary SPI, data-ready interrupts, FIFO and sensor timestamps; 1.71–3.6 V supply | Use the 3.3 V sensor domain, primary SPI and hardware-timestamped DRDY at the H723. Validate FIFO / filter timing and clock alignment rather than treating transfer time as sample time |
| Bias and vibration for velocity-through-slip | Rev 7 lists typical gyro bias instability 3 °/h, typical high-performance gyro noise 5 mdps/√Hz and accel noise 60 µg/√Hz | Useful industrial-IMU specifications; typical values are not guaranteed navigation performance on a vibrating car |
| Packaging | LGA-14, 2.5 × 3 × 0.83 mm typical; −40 to +105 °C | Suitable for a small sensor PCB; check exact pin assignment / land pattern and vibration-isolated mounting |

**Selection (ADR-0045):** use ISM330DHCXTR for E-20 at the body/CG and E-21 at the front axle. One common board serves both sites. Calibration, thermal/vibration testing and measured velocity-through-slip performance still need qualification. Murata's SCH16T-K10 (±2000 °/s, ±16 g, ~2 °/h, about USD 47) is the candidate for a better CG IMU in v1.1 if testing shows this one limits slip estimation.

Before release, test the real SPI harness, sustained full-rate acquisition without FIFO loss, clipping, stationary bias / Allan behavior, warm-up / temperature drift, motor-on vibration and the estimator's slip intervals. Record actual filter bandwidth/delay; output data rate is not measurement bandwidth. The selected 17 installed state-sensor ICs cost **USD $69.7824 / CAD $99.59 per car**: two loose ISM330DHCXTR at $5.7392, ten loose AS5047P at the $4.231 tier and five loose ToFs at $3.1988. This excludes support parts, bare sensor PCBs, shared in-house consumables and shipping; see the [CAD funding estimate](jlc-pcba-cost-estimate.md).

Sources: [loose AS5047P](https://www.lcsc.com/product-detail/C962063.html), [loose ToF](https://www.lcsc.com/product-detail/C3178291.html), [ST datasheet, Rev 7](https://www.st.com/resource/en/datasheet/ism330dhcx.pdf), [ST product page](https://www.st.com/en/mems-and-sensors/ism330dhcx.html), [genuine ST JLCPCB entry](https://jlcpcb.com/partdetail/Stmicroelectronics-ISM330DHCXTR/C2655101). Typical / characterized performance must be distinguished from guaranteed limits.

## ELPM — rugged linear suspension option, with interface and packaging changes

The ELPM is a **conductive-plastic contact potentiometer** with damped wipers. It measures linear displacement and returns an analog wiper voltage. The current E-23 baseline is a contactless AS5047-family angle sensor at each suspension pivot, read over SPI. ELPM can be evaluated as a replacement measurement approach, but does not meet that existing contactless / no-wiper specification unchanged.

> [!NOTE]
> The user's [2506_Q manufacturer PDF](https://www.variohm.com/contentfiles/product-datasheets/ELPM_Series_Motorsports_Linear-position_sensor_2506_Q.pdf) was blocked by the supplier's browser verification. The current manufacturer product page was readable, and the complete **10/24 manufacturer datasheet mirrored by Farnell** was inspected instead. Dimensions and electrical details below refer to 10/24; confirm the supplied revision before CAD or purchase. No personal data or supplier form was submitted.

| Item | Published 10/24 data | Implication for ARC |
| --- | --- | --- |
| Stroke | 25–175 mm, seven standard options | Choose from measured bump-to-droop sensor travel, with end-stop clearance; stroke TBC |
| Smallest standard ELPM-25 | 75 mm body; 95 mm retracted mounting distance; 26 g; nominal diameter 9.5 mm (drawing body diameter 9.4 mm) | Check joint centers and the complete motion envelope, not only diameter. Four standard units total 104 g before brackets / harness, per the published mass |
| ELPM-25-POP | 88 mm body; 115 mm retracted mounting distance; 23 g | The quick-release mounting is larger; exact suffix matters |
| Linearity | Typically ±1% of stroke; improved option by request | On a 25 mm stroke this is ±0.25 mm before mount geometry, calibration and ADC errors; acceptability TBC |
| Environmental / mechanical | IP67; >25 million operations; damped wipers; 10 m/s maximum operating speed | Strong published durability, with contact wear and mounting loads still to qualify |
| Electrical | Maximum 24 V, 0.25 W at 40 °C; recommended wiper current <10 µA. ELPM-25 track 2.5 kΩ ±20% | 24 V is a maximum, not a required supply. A proposed quiet, ratiometric 3.3 V excitation needs a high-impedance acquisition path; account for buffer, protection leakage and ADC settling |
| Wiring | Black ground; red excitation; white wiper | Exact harness / fault detection, connector and shielding strategy TBC |
| Procurement | External assembled sensor; no verified JLC component listing | Supplier quote required for the final stroke / suffix. Farnell ELPM-25 reference was GBP £293.46 each ex VAT; four at the 1+ price are £1,173.84 before shipping / tax. This is not a manufacturer quote or a JLC price |

### Proposed readout if adopted

```mermaid
flowchart LR
  Ref[Quiet excitation / ADC reference] --> Track[ELPM track]
  Track -->|Wiper voltage| Buffer[High-impedance buffer]
  Buffer --> Filter[ADC RC / protection — values TBD]
  Filter --> ADC[Timer-triggered sync-board ADC + DMA]
  ADC --> Log[Timestamped displacement / calibrated wheel travel]
```

- Reserve **four analog acquisition channels** if all four suspension sensors change. The SPI device tally would fall from 12 to 8; retain wheel and knuckle encoders and both IMUs. These are conditional counts, not a revised approved pin map.
- Acquire each suspension channel at ≥2 kHz under E-23 / SYS-06, preserving sample time and measured filter delay. Being an analog sensor does not establish acquisition bandwidth or resolution; choose buffer / RC / ADC timing to meet the final error budget.
- Do not assume a direct ADC connection meets the <10 µA wiper recommendation: prove static leakage and sample-and-hold loading, or buffer it. Final amplifier, ADC / reference, protection and RC parts are TBD.
- Calibrate displacement to actual wheel travel using the suspension linkage. Damper stroke or link stroke is not automatically wheel travel. Provide travel margin so the sensor never acts as a suspension end stop or carries side load.
- Diagnose unplugged / broken wires; qualify noise, temperature, vibration, repeatability and wear. A mid-scale analog voltage alone is not evidence of a healthy sensor.
- The ELPM measures suspension displacement relative to its mounts. It does not replace E-26 floor-distance ToF sensing, which measures chassis-to-ground distance.

**Recommendation:** a credible motorsport suspension-displacement sensor if its mounting envelope and cost fit, with an analog front end to design. Keep it as a yellow evaluation alternative until EE / mechanical owners select the stroke, mounting, accuracy and contact-wear tradeoff, then update E-23 and the sensing interface / decision record together. ADR-0008 retains direct sync-board sensing; its historical pot interface must be reconciled explicitly with the current magnetic-sensor BOM if the choice changes.

Sources: [current manufacturer ELPM product page](https://www.variohm.com/products/linear-position-sensors-/linear-potentiometer/elpm-linear-position-sensor), [manufacturer 10/24 datasheet mirror](https://www.farnell.com/datasheets/4421875.pdf), [Farnell ELPM-25 price reference](https://uk.farnell.com/variohm-eurosensor/elpm-25/linear-position-sensor-25mm-2/dp/4574665). The distributor's attribute list drops the “k” from resistance; use the manufacturer table's **kΩ**, not its malformed “ohm” field.

Return to [PCB parts and board map](pcb-parts-selection.md).
