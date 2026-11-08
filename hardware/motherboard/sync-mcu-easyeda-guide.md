# ARC motherboard | EasyEDA Pro design guide

**Working design reference · 2026-11-08 · Sheets 00–14**

Use this while drawing the combined ARC motherboard in EasyEDA Pro. Sheets **00–06** cover the power tree, switching, protection and shutdown. Sheets **07–14** cover the Sync MCU and the vehicle interfaces. The working scope is four A50S UARTs, one steering UART, direct Ethernet to the Orin, eight angle ports, two IMU ports, and four ToF ports. The direct ToF draft uses the pull-ups already on each remote board.

This scope differs from the current [drive ICD](../../systems/icd/controller-telemetry.md), [steering ADR](../../systems/adr/0019-steering-off-the-shelf-controller.md), [sensor ADR](../../systems/adr/0045-vehicle-state-sensor-boards.md), and [network ICD](../../systems/icd/network.md). Use this sheet to draft the changes. Review and record the revised interfaces before releasing the PCB or its firmware pinout.

**Parts lookup:** Search the JLCPCB parts library in EasyEDA using the **JLC part number (C-number)**, then check the manufacturer part and footprint. Counts are per motherboard unless marked off-board. **TBD** means the exact part or value still needs to be chosen. A catalog code identifies the candidate and its footprint reference. Check assembly availability before ordering.

| Start here | Jump to a sheet |
| --- | --- |
| **Power and safety** | [00 Overview](#00--system-overview) · [01 Input protection and ORing](#01--input-protection-and-oring) · [02 Power rails](#02--power-rails) · [03 Drive and steering power](#03--drive-and-steering-power) · [04 E-stop and watchdog](#04--e-stop-and-watchdog) · [05 Monitoring](#05--battery-and-rail-monitoring) · [06 Compute shutdown](#06--compute-shutdown) |
| **Sync and I/O** | [07 MCU](#07--sync-mcu) · [08 Ethernet](#08--ethernet-to-the-orin) · [09 Drive UARTs](#09--four-a50s-drive-uarts) · [10 Steering UART](#10--steering-uart) · [11 Angle and IMU](#11--angle-and-imu-ports) · [12 ToF](#12--tof-ports) · [13 Camera and Orin timing](#13--camera-and-orin-timing) · [14 Lighting and status](#14--lighting-status-and-controls) |
| **References** | [STM32 pin sheet](#stm32-pin-sheet) · [Capture and review](#capture-and-review) · [ARC candidate BOM](../../systems/bom/pcb-component-breakdown.md) |

**Open while drawing:** [STM32H723 datasheet](https://www.st.com/resource/en/datasheet/stm32h723zg.pdf) · [LQFP144 top view, Figure 6](https://www.st.com/resource/en/datasheet/stm32h723zg.pdf#page=55) · [ST NUCLEO-H723ZG reference schematic](https://www.st.com/resource/en/schematic_pack/mb1364-h723zg-e01_schematic.pdf) (sheets 3, 4 and 6) · [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) · [EasyEDA device placement](https://prodocs.easyeda.com/en/schematic/place-device/) · [EasyEDA Footprint Manager](https://prodocs.easyeda.com/en/schematic/tools-footprint-manager/)

## STM32 pin sheet

**U1 candidate: STM32H723ZGT6, LQFP-144, 20 × 20 mm.** STM32 pin names and pad numbers below refer to this package. They are separate from connector contact numbers. Treat assignments as TBC until the EasyEDA symbol, alternate functions, clocks, DMA/interrupts, and firmware mapping agree.

### Peripheral pins

The number in parentheses is the physical **U1 pad**. AF is the STM32 alternate function. UART directions are from U1. SPI directions are from the controller. The port names are draft net groups.

| Sheet / net group | Peripheral | U1 pins and pads | AF |
| --- | --- | --- | --- |
| 08 PHY clock and management | ETH RMII | REF_CLK **PA1 (35)**, MDIO **PA2 (36)**, MDC **PC1 (27)** | 11 |
| 08 PHY receive | ETH RMII | CRS_DV **PA7 (43)**, RXD0 **PC4 (44)**, RXD1 **PC5 (45)** | 11 |
| 08 PHY transmit | ETH RMII | TX_EN **PG11 (126)**, TXD0 **PG13 (128)**, TXD1 **PB13 (74)** | 11 |
| 09 DRIVE_FL | USART1 | TX **PA9 (101)**, RX **PA10 (102)** | 7 |
| 09 DRIVE_FR | USART2 | TX **PD5 (119)**, RX **PD6 (122)** | 7 |
| 09 DRIVE_RL | UART4 | TX **PD1 (115)**, RX **PD0 (114)** | 8 |
| 09 DRIVE_RR | UART7 | TX **PE8 (59)**, RX **PE7 (58)** | 7 |
| 10 STEER | UART8 | TX **PE1 (142)**, RX **PE0 (141)** | 8 |
| 07 DEBUG_UART (optional) | USART3 | TX **PD8 (77)**, RX **PD9 (78)** | 7 |
| 07 UART_SPARE (optional) | UART9 | TX **PG1 (57)**, RX **PG0 (56)** | 11 |
| 11 ANGLE_1…4 | SPI1 | SCK **PA5 (41)**, MISO **PA6 (42)**, MOSI **PB5 (135)** | 5 |
| 11 ANGLE_5…8 | SPI2 | SCK **PD3 (117)**, MISO **PB14 (75)**, MOSI **PB15 (76)** | 5 |
| 11 IMU_CG | SPI3 | SCK **PC10 (111)**, MISO **PC11 (112)**, MOSI **PC12 (113)** | 6 |
| 11 IMU_FRONT | SPI4 | SCK **PE12 (65)**, MISO **PE13 (66)**, MOSI **PE14 (67)** | 5 |
| 12 TOF_1 | I2C1 | SCL **PB8 (139)**, SDA **PB9 (140)** | 4 |
| 12 TOF_2 | I2C2 | SCL **PB10 (69)**, SDA **PB11 (70)** | 4 |
| 12 TOF_3 | I2C3 | SCL **PA8 (100)**, SDA **PC9 (99)** | 4 |
| 12 TOF_4 | I2C4 | SCL **PF14 (54)**, SDA **PF15 (55)** | 4 |
| 07 PWR_I2C | I2C5 | SCL **PF1 (11)**, SDA **PF0 (10)** | 6 |
| 07 CELL_SPI (conditional) | SPI5 | SCK **PF7 (19)**, MISO **PF8 (20)**, MOSI **PF9 (21)** | 5 |
| 11 IMU_CG_DRDY | TIM8_CH2 or GPIO input | **PC7 (97)** | 3 for timer |
| 11 IMU_FRONT_DRDY | TIM8_CH3 or GPIO input | **PC8 (98)** | 3 for timer |
| 13 FRAME_TRIGGER_STEREO | TIM4_CH1 | **PD12 (81)** | 2 |
| 13 FRAME_TRIGGER_QUAD | TIM4_CH2 | **PD13 (82)** | 2 |
| 13 EVENT_SYNC_FRONT | TIM4_CH3 | **PD14 (85)** | 2 |
| 13 EVENT_SYNC_DOWN | TIM4_CH4 | **PD15 (86)** | 2 |
| 13 TRIGGER_SPARE | TIM3_CH1 | **PC6 (96)** | 2 |
| 13 PPS_SYNC | TIM2_CH1 | **PA0 (34)** | 1 |
| 07 Clock reserves | HSE and optional LSE | HSE_IN **PH0 (23)**, HSE_OUT **PH1 (24)**, LSE_IN **PC14 (8)**, LSE_OUT **PC15 (9)** | Dedicated |
| 07 Debug and boot | SWD, reset, boot | SWDIO **PA13 (105)**, SWCLK **PA14 (109)**, optional SWO **PB3 (133)**, **NRST (25)**, **BOOT0 (138)** | 0 for SWD/SWO |

### Chip selects

Use one GPIO per active-low CS. Set the idle state and boot-time bias to keep all devices deselected. Angle and IMU buses are separated so each can use its own SPI mode.

| Net | U1 pin (pad) | Net | U1 pin (pad) |
| --- | --- | --- | --- |
| ANGLE_1_CS_N | PE9 (60) | ANGLE_5_CS_N | PD4 (118) |
| ANGLE_2_CS_N | PE10 (63) | ANGLE_6_CS_N | PD7 (123) |
| ANGLE_3_CS_N | PE11 (64) | ANGLE_7_CS_N | PD10 (79) |
| ANGLE_4_CS_N | PE15 (68) | ANGLE_8_CS_N | PD11 (80) |
| IMU_CG_CS_N | PG2 (87) | IMU_FRONT_CS_N | PG3 (88) |
| SENSOR_CS_SPARE_N | PG6 (91) | CELL_CS_N (conditional) | PF6 (18) |

### Control, status and power reserves

These are STM32-side reservations. Polarity, levels, bias and protection depend on the connected circuit.

| Sheet / net group | U1 pins and pads | Use |
| --- | --- | --- |
| 08 PHY_NRST / PHY_IRQ | **PE2 (1)** / **PE3 (2)** | Reset output, optional interrupt on EXTI3 |
| 07 ESTOP_STATUS | **PF2 (12)** | Conditioned input on EXTI2 |
| 07 PWR_GOOD / PWR_ALERT_1 / PWR_FAULT_1 | **PE4 (3)** / **PE5 (4)** / **PE6 (5)** | Inputs. EXTI4/5/6 available. Confirm final fault count and source identification with the power designer. |
| 13 ORIN_SHUTDOWN_REQ / ORIN_HALTED | **PG8 (93)** / **PG12 (127)** | Request output, halt input. Confirm the physical interface and power-removal meaning first. |
| 14 LIGHT_RING_EN / LIGHT_GRAZE_EN / LIGHT_VCSEL_EN | **PF3 (13)** / **PF4 (14)** / **PF5 (15)** | Driver enable reserves |
| 14 LED_ESTOP / LED_WATCHDOG / LED_PWR_FAULT | **PG4 (89)** / **PG5 (90)** / **PG7 (92)** | Status output reserves. Population and polarity TBD. |
| 07 BAT_V_AIN / BAT_I_AIN (conditional) | **PB0 (46)** / **PB1 (47)** | ADC1/2 INP9 / INP5. Condition the signals and review range, reference and input impedance before enabling. |

### Pin-map checks before routing

- **Package and AF.** Compare the proposed pads with [ST's package pin file](https://github.com/STMicroelectronics/STM32_open_pin_data/blob/7d1f1514ed5583ec5007ad91236b4e1d377295b1/mcu/STM32H723ZGTx.xml) and [alternate-function table](https://github.com/STMicroelectronics/STM32_open_pin_data/blob/7d1f1514ed5583ec5007ad91236b4e1d377295b1/mcu/IP/GPIO-STM32H72_gpio_v1_0_Modes.xml). Power and analog package pins are listed in [DS13313, section 5](https://www.st.com/resource/en/datasheet/stm32h723zg.pdf#page=55). This worksheet covers assigned signal pads.
- **Five I²C buses.** Four direct ToFs plus the dedicated power monitor use I2C1–I2C5. The pinned Zephyr v4.4.2 [STM32H7 base](https://github.com/zephyrproject-rtos/zephyr/blob/v4.4.2/dts/arm/st/h7/stm32h7.dtsi) and [H723 include](https://github.com/zephyrproject-rtos/zephyr/blob/v4.4.2/dts/arm/st/h7/stm32h723.dtsi) declare I2C1–I2C4, while the pinctrl data has I2C5 pins. Bring up the I2C5 peripheral node, clock/reset/IRQ and driver before freezing this map. For a mux fallback, one ToF I²C plus one separate power I²C can use the declared controllers.
- **Ethernet clock.** ToF I2C3 uses both PA8 and PC9, the MCO1 and MCO2 pins. Use a reviewed PHY-driven or external 50 MHz RMII reference on PA1 with this map.
- **Timing and DMA.** TIM4 channels share a timer counter. Confirm the output-compare schedule can generate each camera's required period and pulse width. Decide whether the two DRDY inputs use timer capture or GPIO EXTI, then check interrupt-line ownership. Five full-duplex UARTs plus five full-duplex SPI peripherals would need 20 RX/TX streams if all use dedicated DMA, versus 16 DMA1/2 streams. Plan where to use IRQs or scheduled transfers, and check I2C4's separate BDMA path if using DMA. Test concurrent traffic and buffer placement on the [production board target](../../firmware/sync-mcu/boards/arc/arc_sync/arc_sync.dts), which is currently a scaffold. [ST DMA request mapping](https://github.com/STMicroelectronics/stm32h7xx-hal-driver/blob/master/Inc/stm32h7xx_ll_dmamux.h)

## 00 — System overview

Draw the block diagram and label the interfaces. Keep the power and Sync MCU sections on one PCB. Show the Orin carrier, motor controllers, cameras and 14 sensor boards at the ends of their harnesses. The overview page has no BOM placements. [Motherboard architecture](README.md) · [Internal power/sync contract](../../systems/icd/power-sync-stack.md)

| From → to | What to show on the overview |
| --- | --- |
| 01 → 02 | Battery/wall ORed low-voltage feed and its return. Wall feeds this path only. |
| 01 → 03 | Battery-only motor feed and rated return. 03 switches the drive and steering branches separately. |
| 02 → loads and 07 | Protected compute, sensor, 5 V and 3.3 V rails. Distinguish the proposed 12 V LiDAR branch from the low-voltage remote sensor rails. Label source, switched load and sensing point. |
| 04 → 03, 07 and A50S | Independent delayed drive/steering cut controls, conditioned **ESTOP_STATUS** to the MCU, and hardwired **ESTOP** to all four drive controllers. |
| 05 → 07 | **PWR_I2C**, identifiable fault/alert signals, and conditional **CELL_SPI** or analog fallback. These are internal PCB nets. |
| 07 / 13 → 06 and Orin | Shutdown request to the Orin, verified halt/power-removal permission back to the cutoff circuit, and separate timing signals. |
| 07 → 08–14 | One direct Orin Ethernet link, four drive UARTs, one steering UART, eight angle and two IMU SPI ports, four ToF I²C ports, timing and controls. |

## 01 — Input protection and ORing

Draw the **4S battery input (12.0–16.8 V planning range)**, the **19 V wall input**, the battery-only motor feed, and the ORed low-voltage feed. Show input fuses, reverse protection, surge paths, connector returns, the loop key/anti-spark interface and test points. Review handover under load, reverse current into the battery and supply transients. [Power rails ICD](../../systems/icd/power-rails.md)

| Function | Qty / exact candidate | JLC part number | Datasheet or selection check |
| --- | --- | --- | --- |
| Ideal-diode controllers | 2 × **LM74700QDBVRQ1** | [C2941042](https://jlcpcb.com/partdetail/TexasInstruments-LM74700QDBVRQ1/C2941042) | One per input. [TI datasheet](https://www.ti.com/lit/ds/symlink/lm74700-q1.pdf) |
| ORing MOSFETs | 2 × **CSD18540Q5B**, provisional | [C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | Check body-diode orientation, loss and transient stress. [TI datasheet](https://www.ti.com/lit/ds/symlink/csd18540q5b.pdf) |
| Surge TVS | **SMCJ24A**, SETsafe / SETfuse, qty TBD | [C2996490](https://jlcpcb.com/partdetail/SETsafe_SETfuse-SMCJ24A/C2996490) | Catalog clamp is 38.9 V at its specified pulse. Coordinate with downstream limits, including the INA226's 36 V operating bus range if it sees this node. |
| Connectors, fuses, support and anti-spark | TBD | TBD | Set contact and fuse ratings from the current budget. Select controller bypass, gate components and bulk capacitance for the chosen topology. |

## 02 — Power rails

Draw the proposed 12 V, 5 V and 3.3 V stages and the separate compute and sensor branches. Show branch fuse, enable/switch, shunt location and test points. Start sizing from the load table, including compute startup and LiDAR inrush. Confirm the regulated compute setpoint against both carriers before choosing feedback resistors. [Power budget](../../systems/budgets/power-scenarios.md) · [Power rails ICD](../../systems/icd/power-rails.md)

| Function | Qty / exact candidate | JLC part number | Datasheet or selection check |
| --- | --- | --- | --- |
| Proposed 12 V buck-boost | 1 × **LM5176PWPR** | [C442493](https://jlcpcb.com/partdetail/TexasInstruments-LM5176PWPR/C442493) | Size the four-switch stage, inductor, caps, sense and compensation. [TI datasheet](https://www.ti.com/lit/ds/symlink/lm5176.pdf) |
| Main 5 V buck | 1 × **LM5146RGYR** | [C3188679](https://jlcpcb.com/partdetail/TexasInstruments-LM5146RGYR/C3188679) | Size the external switches, inductor, feedback and compensation. [TI datasheet](https://www.ti.com/lit/ds/symlink/lm5146.pdf) |
| Main 3.3 V buck | 1 × **TPS54360DDAR** | [C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | Draw the catch diode, inductor, caps and feedback/compensation. [TI datasheet](https://www.ti.com/lit/ds/symlink/tps54360.pdf) |
| Converter MOSFETs | 6 × **CSD18540Q5B**, provisional | [C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513) | Four for the buck-boost and two for 5 V. With the two ORing FETs, the draft total is eight across sheets 01–02. |
| Quiet sensor supply | Up to 2 × **TPS7A2033PDBVR**, provisional | [C2862740](https://jlcpcb.com/partdetail/TexasInstruments-TPS7A2033PDBVR/C2862740) | Place here or on sheet 11 and count once. Check current, headroom, thermal load and cap requirements. [TI datasheet](https://www.ti.com/lit/ds/symlink/tps7a20.pdf) |
| Magnetics, passives and branch protection | TBD | TBD | Size from worst-case voltage/current, ripple, loss, transient response and startup. Include gate/bootstrap networks, fuses, switches, test points and rated returns. |

## 03 — Drive and steering power

Draw the battery-fed drive-bus switch to the four A50S controllers and a separate fused steering output. The drive switch opens when its control is unpowered. Review the drive switch's inrush, faults and thermal limits, and place the regen path so braking energy can still reach the clamp when the drive switch opens. Route motor current through rated power returns and keep it out of UART and sensor references. Confirm whether A50S Aux power is needed and which range applies to the actual hardware. [Power and clamp open items](../../systems/handoff/electrical.md) · [Power rails ICD](../../systems/icd/power-rails.md)

| Function | Qty / exact candidate | JLC part number | Datasheet or selection check |
| --- | --- | --- | --- |
| Steering eFuse | 1 × **TPS26630RGET**, conditional | [C2864391](https://jlcpcb.com/partdetail/TexasInstruments-TPS26630RGET/C2864391) | Check peak/inrush, thermal limits and reverse/regen behavior before choosing it. [TI datasheet](https://www.ti.com/lit/ds/symlink/tps2663.pdf) |
| Drive switch, driver, fuse, inrush network and high-current contacts | TBD | TBD | Set ratings using branch current, faults, inrush and the proposed delay. Include the rated return and gate/coil bias that produces a safe unpowered state. |
| Optional autonomous regen clamp | 1 each provisional **TLV3201AIDBVR**, **REF3325AIDBZR**, **TC4427EOA**, **CSD18540Q5B**, **TPS54360DDAR** | [C105188](https://jlcpcb.com/partdetail/TexasInstruments-TLV3201AIDBVR/C105188), [C69039](https://jlcpcb.com/partdetail/TexasInstruments-REF3325AIDBZR/C69039), [C636891](https://jlcpcb.com/partdetail/MicrochipTech-TC4427EOA/C636891), [C86513](https://jlcpcb.com/partdetail/TexasInstruments-CSD18540Q5B/C86513), [C44377](https://jlcpcb.com/partdetail/TexasInstruments-TPS54360DDAR/C44377) | Populate only if the clamp goes on this PCB. Comparator, reference, gate driver, dump FET and its local supply. Size the dump resistor, hysteresis, threshold, heatsink and transient protection from the braking model. [Clamp reference and candidate datasheets](../../systems/bom/pcb-parts-selection.md#motor-bus-clamp) |

## 04 — E-stop and watchdog

Draw the latching normally closed button input, independent brake request to all four A50S controllers, delayed drive and steering cut controls to sheet 03, and conditioned **ESTOP_STATUS** to sheet 07. The working timing is drive cut at **2.4 s TBC**, then steering **1 s later TBC**. Verify both cuts with the H723 reset/unpowered and check re-arm behavior. The firmware watchdog and UART command timeout remain separate safety functions. [Power sequencing](../../systems/icd/power-rails.md#operating-state-contract) · [Hardware stop decision](../../systems/adr/0034-off-the-shelf-motor-controllers.md)

| Function | Qty / exact candidate | JLC part number | Datasheet or selection check |
| --- | --- | --- | --- |
| Delay timer | 1 × **TLC555IDR**, candidate | [C6987](https://jlcpcb.com/partdetail/TexasInstruments-TLC555IDR/C6987) | Steering delay candidate. Review its protected supply, tolerance, reset, and behavior on power loss. [TI datasheet](https://www.ti.com/lit/ds/symlink/tlc555.pdf) |
| Physical button | **DFRobot FIT0156**, off-board pricing candidate | — | Verify the actual latching NC contact block. [ARC electrical BOM](../../systems/bom/electrical.md) |
| Input conditioning, brake driver, drive timer, steering timer network and connector | TBD | TBD | Review polarity, default state, opens/shorts, timing limits and independent hardware cut. Confirm the A50S stop input on the actual controllers. |

## 05 — Battery and rail monitoring

Draw the protected 4S balance tap input, pack shunt, branch shunts, and monitor alerts. Connect the bus monitors to **PWR_I2C**. The LTC6810 candidate needs a reviewed SPI/isoSPI implementation on **CELL_SPI** rather than a host I²C hookup. Agree on a single cell monitor implementation and verify the per-cell **≥100 Hz, ±10 mV** requirement, tap protection and off-state drain. [Internal power/sync contract](../../systems/icd/power-sync-stack.md) · [System requirements](../../systems/requirements/system.md)

| Function | Qty / exact candidate | JLC part number | Datasheet or selection check |
| --- | --- | --- | --- |
| Pack/branch voltage and current | 6 × **INA226AIDGSR**, provisional | [C49851](https://jlcpcb.com/partdetail/TexasInstruments-INA226AIDGSR/C49851) | Pack, compute, sensor, 5 V, 3.3 V and steering. Draw Kelvin connections, filter, bypass, address straps and identifiable alerts. [TI datasheet](https://www.ti.com/lit/ds/symlink/ina226.pdf) |
| Pack shunt | 1 × **HoLRS3920-5W-1mR-1%**, candidate | [C558628](https://jlcpcb.com/partdetail/Milliohm-HolRS3920_5W_1mR_1/C558628) | Check continuous/pulse dissipation and the board's copper and thermal limits. |
| Cell monitor | 1 × **LTC6810IG-1#3ZZPBF**, candidate | **C689559** (BOM reference, verify sourcing) | Apply the datasheet's 4S wiring, unused-input connections, tap filtering and supply scheme. [ADI datasheet](https://www.analog.com/media/en/technical-documentation/data-sheets/ltc6810-1-6810-2.pdf) |
| Cell monitor alternate | **LTC6810IG-2#3ZZTRPBF**, conditional replacement | [C689562](https://jlcpcb.com/partdetail/AnalogDevices-LTC6810IG_23ZZTRPBF/C689562) | Review the different interface/addressing and pin connections before substitution. Same ADI datasheet as above. |
| Analog current-sense alternate | **INA240A1PWR**, conditional replacement per selected channel | [C93965](https://jlcpcb.com/partdetail/TexasInstruments-INA240A1PWR/C93965) | Needs ADC, reference and separate bus-voltage sensing if required. [TI datasheet](https://www.ti.com/lit/ds/symlink/ina240.pdf) |
| Branch shunts, balance connector and tap/logic support | TBD | TBD | Size each branch separately. Choose the tap protection, level/interface parts and passives after deciding the cell-monitor circuit. |

## 06 — Compute shutdown

Draw the proposed sequence **lowest-cell warning → Orin shutdown request → verified halt/power-removal permission → compute switch off**. Route the carrier signals through sheet 13 and show the independent hardware undervoltage backstop. Resolve what signals actual halt on both Orin builds. Define the behavior on timeout or loss of the H723. [Carrier handshake](../../systems/icd/carrier-sync.md) · [Shutdown proposal](../../systems/adr/0022-power-board-telemetry-and-shutdown.md)

| Function | Qty / exact part | JLC part number | What needs a decision |
| --- | --- | --- | --- |
| Compute rail switch and driver | TBD | TBD | Rail, hold-up, inrush, sense point and cutoff behavior. Coordinate the switch with sheet 02. |
| Shutdown request and halt feedback | TBD | TBD | Actual carrier signal, direction, translation/bias, connector contacts and timeout. |
| Hardware undervoltage backstop | TBD | TBD | Threshold, hysteresis, supervisor/comparator and safe state. |

## 07 — Sync MCU

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| STM32 sync MCU | 1 × **STM32H723ZGT6**, JLC **C730146** | LQFP-144 symbol with all units and every power pin. [ST datasheet](https://www.st.com/resource/en/datasheet/stm32h723zg.pdf) · [JLC part](https://jlcpcb.com/partdetail/Stmicroelectronics-STM32H723ZGT6/C730146) |
| Core and rail support | Capacitor MPNs TBD after supply review | Draw every VDD/VSS, analog supply and reference, VBAT, and applicable supply/monitor pins. For the LDO topology, the ST reference shows **two 2.2 µF VCAP capacitors**, low ESR, and 100 nF local VDD bypass plus bulk. Confirm placement, effective capacitance and all package-specific connections against the chosen supply mode. [ST AN5419, Figure 4](https://www.st.com/resource/en/application_note/dm00663674.pdf#page=10) · [Nucleo sheet 4](https://www.st.com/resource/en/schematic_pack/mb1364-h723zg-e01_schematic.pdf#page=4) |
| MCU clocks | HSE source, load/bias parts and optional LSE population TBD | Reserve both oscillator pairs. Use the chosen clock tree to size HSE and any LSE components. The Ethernet PHY clock is handled on sheet 08. |
| Reset, boot and debug | 1 SWD connector or fixture, 1 optional console connection, support MPNs TBD | Wire SWDIO, SWCLK, target-voltage sense, GND and NRST. Allow SWO if wanted. Set BOOT0 and reset bias per the ST reference. Add accessible test points for power, reset and the console. |
| Internal power/safety nets | Net ports; conditioning and fault-input count TBD | Bring in logic supply/return, power I²C, ESTOP status, PWR_GOOD and separately identified alerts/faults. Reserve conditional cell SPI and analog battery-sense inputs if the power design uses them. The hardware torque cut remains on the power section. [Internal power/sync contract](../../systems/icd/power-sync-stack.md) |

## 08 — Ethernet to the Orin

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| 10/100 RMII PHY | 1 × **LAN8742AI-CZ-TR**, JLC **C621425** | Bind the 24-pin exposed-pad package. Connect the nine RMII/MDIO signals in the pin sheet, then PHY reset and a reviewed interrupt option. [Microchip datasheet and errata](https://www.microchip.com/en-us/product/LAN8742A) · [JLC part](https://jlcpcb.com/partdetail/Microchip-LAN8742AI_CZTR/C621425) |
| Clock and bias | 1 clock implementation, support MPNs TBD | Review the PHY's 25 MHz crystal / 50 MHz RMII reference arrangement or another supported clock arrangement. Draw the required straps, PHY address, RBIAS, supply bypass, regulator support and MDIO pull-up from the exact reference. Record which device drives REF_CLK. |
| Cable interface | 1 compatible 10/100 magnetics + RJ45 or integrated MagJack; MPN TBD | Draw the MDI pairs, center taps, terminations, ESD and shield connection from the reference. Label the jack **TO ORIN**. Review the cable and link addressing, and settle the LiDAR/service path if the Orin's existing port is taken by this link. [Microchip Ethernet schematic checklist](https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/SupportingCollateral/LAN8742AQFNRevASchematicChecklist.pdf) · [ARC network contract](../../systems/icd/network.md) |

## 09 — Four A50S drive UARTs

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| Drive signal ports | 4 independent motherboard connectors, MPN TBD | One each for FL, FR, RL and RR. For each port, MCU TX goes to A50S RX and MCU RX goes to A50S TX. Reserve a signal reference and the reviewed return arrangement. The A50S's MCU-side USART3 pins are **PB10 TX / PB11 RX**. Verify the physical connector contacts against the exact hardware before making the loom. |
| A50S-side harness housing | 4 × **Molex 501189-2010**, LCSC harness reference [C504999](https://www.lcsc.com/product-detail/C504999.html) | Pico-Clasp 20-way housing at the controller end. Match the 501193-series crimp suffix to wire gauge and plating, then verify populated contact positions. The motherboard housing is a separate choice. [Molex housing](https://www.molex.com/en-us/products/part-detail/5011892010) · [ARC harness contact reference](../../systems/icd/corner-connector.md) |
| UART line support | Per port, parts/values TBD | Reserve reviewed series damping/protection and test access on both signals. Establish the 3.3 V margin, ground offset and behavior if either end loses power. Keep the UART loom away from phase wiring. |
| Controller at the far end | 4 × **Team Triforce A50S V2.3c 12S** | Off-board controller. Start with the recorded **2 Mbit/s, 8N1** UART baseline. Keep the separate hardware ESTOP signal in the harness. [A50S product and connections](https://teamtriforceuk.com/a50s-v2/) · [ARC connector contract](../../systems/icd/corner-connector.md) · [UART readiness](../../systems/reviews/a50s-uart-readiness.md) |

For the UART-only draft, leave out the former **TJA1051T/3/CM,118** and its CAN termination. Firmware must add drive commands to the four UART links alongside telemetry. Test fresh-command detection, timeout braking, link errors, the four simultaneous links, and coordinated stop/re-arm before enabling drive. The current [controller telemetry ICD](../../systems/icd/controller-telemetry.md) covers the older command split.

## 10 — Steering UART

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| Motherboard steering port | 1 connector, MPN TBD | Dedicated TX, RX and signal return. Review short-harness routing, protection and off-state behavior. Keep service access to the controller's SWD. |
| Controller and mating connector | 1 × **mjbots moteus-c1**, AUX2 **JST GHR-07V-S** housing, [C398498](https://jlcpcb.com/partdetail/JST-GHR_07VS/C398498) | Off-board controller and cable housing. AUX2 **5 / C** (c1 TX) goes to MCU RX, **6 / D** (c1 RX) goes to MCU TX, **7** is signal GND. Keep the controller's AUX supply pins out of the UART power path. [mjbots AUX2 pinout](https://mjbots.github.io/moteus/reference/encoders/#c1n1x1-pins-aux2abs) · [Mating housing](https://mjbots.com/products/jst-gh7-housing) |

Use **firmware 1.1.2 or newer** and start at the vendor default **921600 baud, 8N1**. The c1 uses its own serial protocol. Measure the actual command/feedback cycle against ARC's steering update target, and verify timeout, centering and power-cut behavior. Keep the steering power and delayed cut interface on the power pages. The UART draft leaves out **MCP2518FDT-E/SL**, **TX322540M4FBCE2T**, **TJA1462AT/0Z** and CAN termination. [mjbots UART guide](https://mjbots.github.io/moteus/integration/uart/) · [Firmware announcement](https://blog.mjbots.com/2026/08/27/uart-command-and-control-for-moteus/) · [ARC steering safety](../../systems/adr/0038-steering-loss-stops-the-car.md)

## 11 — Angle and IMU ports

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| Angle connectors | **8**, connector MPN TBD | Power, GND, SCK, MOSI, MISO, one dedicated active-low CS per sensor, plus spare conductor provision if retained. Use provisional names ANGLE_1…8 until the eight locations are confirmed. The off-board IC is **AS5047P-ATSM**, [C962063](https://jlcpcb.com/partdetail/Ams-AS5047PATSM/C962063). [Angle datasheet](https://www.infineon.com/assets/row/public/documents/30/49/infineon-as5047p-datasheet-en.pdf) |
| IMU connectors | **2**, connector MPN TBD | Power, GND, SCK, MOSI, MISO, CS and separate DRDY, plus spare provision if retained. Use CG and front labels. The off-board IC is **ISM330DHCXTR**, [C2655101](https://jlcpcb.com/partdetail/Stmicroelectronics-ISM330DHCXTR/C2655101). [IMU datasheet](https://www.st.com/resource/en/datasheet/ism330dhcx.pdf) |
| Cable and supply support | Per-run protection/damping TBD | Review return conductors, connector locking, clock damping, inactive CS bias, power budget and cable length. If quiet rails are assigned here, the prior BOM allows **2× TPS7A2033PDBVR** (**C2862740**) provisionally, with input/output caps and enable routing. Count these once between sheets 02 and 11. [TI LDO datasheet](https://www.ti.com/lit/ds/symlink/tps7a20.pdf) · [ARC harness plan](../../systems/bom/motherboard-wiring.md) |

Confirm the removed angle locations before labeling connectors. If either is a front-knuckle sensor, resolve the absolute-angle reference and steering cross-check with the owner. Keep separate chip selects even when ports share an SPI peripheral.

## 12 — ToF ports

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| ToF connectors | **4**, MPN TBD | Each port carries supply, GND, SDA and SCL, with a spare if retained. Use four separately named SDA/SCL pairs. For the direct plan, wire each pair to its own open-drain STM32 I²C peripheral. |
| Pull-ups and support | Existing SDA/SCL pair **on each remote board** | Verify pull-up value and rail on the actual board. Keep the motherboard pair unpopulated for each direct bus if its remote board provides the pull-ups. Check rise time and low level on the full cable, then unplug, power-cycle and test stuck-bus recovery. Set the spare/XSHUT/IRQ policy before choosing contacts. |
| Remote sensor | **VL53L4CDV0DH/1**, preferred TBC, [C3178291](https://jlcpcb.com/partdetail/STMicroelectronics-VL53L4CDV0DH1/C3178291) | Off-board IC. The default address is **0x29** in 7-bit form. Four isolated buses let all four devices use it. Confirm which four measurement sites remain, including the effect if the ground-camera scale sensor is removed. [ST sensor datasheet](https://www.st.com/resource/en/datasheet/vl53l4cd.pdf) |
| Mux fallback | **TCA9548APWR**, JLC **C130026**, conditional | If the direct pin map fails review, assign one ToF per mux channel. Use an upstream pull-up pair at the MCU, keep the four downstream pairs on the sensor boards, and add address straps, bypass and RESET. Enable one same-address channel at a time. [TI mux datasheet](https://www.ti.com/lit/ds/symlink/tca9548a.pdf) |

Using one shared bus without the mux needs individual XSHUT control and address assignment after reset, plus review of the parallel pull-ups. Four connectors with only power/GND/SDA/SCL do not provide those separate reset lines. [ST multi-sensor setup guide](https://www.st.com/resource/en/user_manual/um2931-a-guide-to-using-the-vl53l4cd-timeofflight-high-accuracy-proximity-sensor-stmicroelectronics.pdf)

## 13 — Camera and Orin timing

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| Timing and carrier connectors | Topology/MPNs TBD | Reserve four trigger outputs (stereo, quad, front event, downward), one trigger spare, PPS, and signal references. v1.0 camera triggers may route directly to the camera connectors. CSI data belongs on the carrier. |
| Signal conditioning | Buffer/level translator, bias, series damping, ESD and test access TBD | Confirm direction, input voltage, polarity, pulse width and power-off behavior for each receiver. Draw PPS and the proposed shutdown request / halt handshake as distinct nets. Verify what signal actually means the Orin is safe to power down. [Carrier timing and shutdown ICD](../../systems/icd/carrier-sync.md) |

## 14 — Lighting, status and controls

| Part or group | Qty / lookup | What to place |
| --- | --- | --- |
| Lighting outputs | **3** control functions, driver/load MPNs TBD | Cross-polarized ring with DC constant-current drive, grazing LEDs, and the reserved certified Class 1 VCSEL mode with a hardware current limit. Set safe startup states and choose the drivers against the actual load. [ARC lighting design](../../systems/adr/0014-ground-speed-event-camera.md) |
| Status indicators | Count, LED MPNs and resistor values TBD | Reserve GPIOs for visible e-stop, watchdog and power-fault indications. Add current-setting resistors or drivers and a connector if the indicators are panel mounted. |
| Hotspot button and indicator | Parts/routing TBD | Confirm carrier-owned GPIO routing before assigning connectors. The dev-kit baseline uses the Orin header for these signals. [Carrier GPIO interface](../../systems/icd/carrier-sync.md) |

## Capture and review

1. Create pages 00–14 in one EasyEDA board. Start with the overview, the power budget and the STM32/PHY symbols. Search by exact MPN or C-number and check package, pad count, pin 1 and exposed pads in Footprint Manager. Record the exact BOM entry when you close each TBD.
2. Check the candidate STM32 assignments in CubeMX or the firmware pinctrl. Resolve clock, DMA, timer and EXTI ownership. Capture the UART, Ethernet and sensor pages while power and safety parts are being sized.
3. Review the power stages together. Trace battery/wall isolation, each protected rail, the hardware brake and cutoff path, the regen path when the drive switch opens, and the halt-to-compute-cut path. Assign rated connectors and returns.
4. Reconcile the netlist with the block diagram and harness pinout. Check **five operating UARTs, one PHY, ten sensor CS, two DRDY, four isolated ToF branches, and separate power-monitor I²C**. Run EasyEDA's schematic rule check and a trial PCB conversion. Review the current fab gate against the revised UART and sensor scope before release. [EasyEDA schematic checks](https://prodocs.easyeda.com/en/schematic/design-design-rule/) · [ARC fab gate](../../systems/reviews/fab-gate.md)

**Planning estimate for sheets 00–14:** **40–60 focused hours** for capture and footprint checks, plus **12–20 hours** for review and fixes. The rail and cutoff decisions, harness pinout, and I2C5 bring-up can extend the calendar. PCB routing, firmware implementation, harness testing and SIwave are separate.
