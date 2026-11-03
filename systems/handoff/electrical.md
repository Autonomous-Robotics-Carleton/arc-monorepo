# Electrical handoff

The starting point for the electrical engineer. It collects, per board, what's decided, where the details live, and what's still open. Circuit-level choices (regulators, fuse values, layout, part selection within the stated requirements) are yours.

## How this repo works

- `systems/` is the source of truth. `architecture.md` is the overview; where it disagrees with an ADR, ICD or requirement, those win.
- Changes go through PRs. A change to an ICD needs sign-off from the owners on both sides.
- Nothing is fabbed until it passes `reviews/fab-gate.md`.
- **Funding comes in orders, one per build (v1.0, then v1.1; ADR-0046)**, so there's no early hardware testing. Design in rework-only fallbacks (unpopulated footprints, jumpers, harness options), and order extra bare PCBs of every custom board.

**Builds (ADR-0046, [builds](../builds.md)):** v1.0 runs on the Orin Nano developer kit (its own carrier and module) with a UST-20LX, the stereo pair and a downward B0223; v1.1 adds the NX on the Antmicro carrier and the remaining cameras. **The motherboard you design is the v1.1 board**: all four camera trigger groups plus the spare, every sensor connector, and a compute rail sized for the NX whose voltage also suits the dev kit's 9–20 V barrel input (NVIDIA SP-11324-001 v1.3). On v1.0, PPS, the hotspot button and LED and the shutdown request use the dev kit's 40-pin header, and the camera triggers go straight to the cameras. Still open for v1.0: which dev kit signal confirms the Orin has halted, and whether it powers on by itself when the rail comes up ([builds](../builds.md)).

## Read first

**Deferred, 2026-10-09:** grounding/isolation and power-stage component/sizing decisions remain **TBD for later review**. Existing candidate tables and ADR-0041 are retained as proposals; no approval or layout release is inferred. ADR-0034 selects four 2 Mbit/s A50S UARTs; implementation and harness qualification are tracked in the [UART readiness review](../reviews/a50s-uart-readiness.md). Carrier CAD source and conversion status are in [hardware/orin-carrier](../../hardware/orin-carrier/README.md).

| What | Where |
| --- | --- |
| System requirements | `requirements/system.md` |
| Decisions (ADRs) | `adr/README.md` |
| Coloured parts options + board map | [pcb-parts-selection.md](../bom/pcb-parts-selection.md) |
| CAD funding budget with 30% contingency, including in-house assembly of all seventeen sensor boards | [PCB estimate per build](../bom/jlc-pcba-cost-estimate.md), [cost model](../budgets/jlc-pcba-cost.csv); final BOM/process and shipment remain TBC (ADR-0045) |
| A50S UART transport / operating rate | [controller-telemetry.md](../icd/controller-telemetry.md) |
| UART firmware gaps / pin and ground review | [UART source review](../reviews/a50s-uart-readiness.md) |
| Functional BOM + sync MCU I/O tally | `bom/electrical.md` |
| Corner connector (rev F: motor phases and Hall cable to the deck) | `icd/corner-connector.md` |
| Power loads and scenarios | `budgets/power.csv`, `budgets/power-scenarios.md` |
| Bus load, latency, cost | `budgets/bus-load.csv`, `budgets/latency.csv`, `budgets/cost.csv` |
| Risks | `risks.md` |
| Verification plan (rigs R1–R8, pre-order S0 checklist) | `verification/plan.md` |

## Boards

**Updated sizing:** [ADR-0044](../adr/0044-ee-sizing-after-custom-corners.md) aligns this handoff with upstream custom corners, TBC 100 mm wheels / 18:1 / 4–5 kg, revised current/regen and LiDAR startup demand. Resistor value/count and the final motor remain TBD; review the current [power scenarios](../budgets/power-scenarios.md) before rating parts.

**Current packaging, 2026-10-09 (ADR-0042):** power and sync MCU are two functional sections of one [motherboard PCB](../../hardware/motherboard/README.md); the [Jetson carrier](../../hardware/orin-carrier/README.md) remains separate. Sections 1 and 2 retain their circuit responsibilities. Internal monitor/control signals replace the power/sync stacking header. A later two-PCB split remains possible after a recorded change and physical-interface review.

**Sensor boards (ADR-0045):** ten angle (four wheel, four suspension, two front knuckle), two IMU and five ToF boards (four corners plus one beside the ground-speed camera), all our own and assembled in house.

### 1. Power board (+ e-stop circuit) and bus clamp board

- **Decided:**
  - LV rails: compute, sensor, 5 V / 3.3 V. The servo rail is removed (ADR-0019).
  - Per-rail fuse, switch and current sense; battery V/I monitoring.
  - Wall (19 V) / battery ideal-diode OR-ing with no reboot.
  - Regulators rated ≥ 30 V with TVS.
  - E-stop circuit sourcing ESTOP to the four A50S (ADR-0012).
  - **Switched motor bus to the four drive controllers, cut by the delayed e-stop line at T (TBC 2.8 s: after the ~2.3 s stop from the speed cap, with margin; ADR-0057. Its tolerance is TBD, with the longest delay within SYS-05's 3 s), rated for peak pack current (`budgets/power-scenarios.md`).** This is the hardware torque cut (ADR-0034). **It opens when unpowered**, so a dead e-stop circuit also cuts the bus. A broken ESTOP wire to one controller brakes that corner, which reports it in its telemetry, and the sync MCU stops the car (SYS-25, ADR-0035) without the cut; accepted, so no per-line wire detection is needed.
  - Switched, fused motor-bus output for steering, cut at T + ~1 s (ADR-0019).
  - Bus clamp on the motor distribution bus.
  - Per-cell battery monitor through the balance lead, ≥ 100 Hz to ±10 mV, data to the sync MCU (SYS-32, E-46).
- **Requirements:** SYS-03, -05, -14, -19, -30, -31, -32; RSK-06, RSK-11.
- **Open:**
  - Rail current ratings and regulator choices (yours, from `power.csv`)
  - ESTOP logic level, driver and current per controller; the motor-bus switch part
  - Bus clamp threshold and resistor sizing (energy and peak power in `budgets/power-scenarios.md`)
  - **Bus clamp position relative to the motor-bus switch.** The topology draws the clamp on the battery side of the switch, but it's sized for braking with the pack disconnected. If the switch opens while the controllers are still braking (the switch opens when unpowered, or T comes early) and it blocks reverse current (back-to-back FETs, a relay), the braking energy goes only into the controllers' input capacitors: they reach overvoltage, fault, and those corners coast. Options: the clamp on the controllers' side of the switch; a switch that conducts in reverse (a single high-side FET's body diode); or accept it. Yours to decide
  - **Inrush when the motor-bus switch closes again** after an e-stop: the anti-spark loop key (E-42) is upstream of it, so the four controllers' bulk capacitors charge through the switch. A controlled turn-on, or a switch rated for the surge
  - Low-battery thresholds (on the lowest cell) and the clean-shutdown signal to the Orin (SYS-30)
  - Cell-monitor part and balance-lead connection while the pack is installed (E-46)
  - Internal power/sync signal and MCU pin map ([power-sync-stack draft](../icd/power-sync-stack.md)); monitor topology / levels / approval remain open. No stacking header in the current motherboard plan (ADR-0042)
  - Whether battery V/I, rail currents and cell voltages are digitised here and sent to the sync MCU over a bus, or read as analogue by the sync MCU's ADC (`bom/electrical.md` I/O tally). **Recommendation in ADR-0022 (Proposed):** monitor ICs on this board, one dedicated I2C bus, fault pins as sync MCU interrupts, analog battery V/I as a rework fallback
  - Whether this board or the sync MCU drives the clean-shutdown signal to the Orin (SYS-30). **Recommendation in ADR-0022 (Proposed):** two tiers. The sync MCU warns and requests the Orin shutdown; this board cuts the compute rail once the Orin has halted, plus an independent hardware undervoltage cutoff that protects the pack if software fails
  - Anti-spark / loop key and motor distribution parts (E-42, E-45)
  - Grounding strategy (shared with all boards; see Cross-cutting)

### 2. Sync MCU board

- **Decided:**
  - STM32H723 + Ethernet PHY (ADR-0011); firmware on Zephyr (ADR-0017).
  - Time sync: µs-critical sensors timestamped in the sync MCU's clock; camera triggers and periodic pulses to each GenX320's Trigger In; Orin synced in software, PPS to an Orin GPIO as cross-check (ADR-0021).
  - Active transport under ADR-0034: classic command CAN at 1 Mbit/s; four independent A50S UARTs at 2 Mbit/s 8N1, full ≥1 kHz status each; separate steering CAN-FD at its 1 / 5 Mbit/s baseline. One native FDCAN serves commands, the existing external MCP2518FD baseline serves steering; two native FDCAN are freed. Final pin/DMA map TBC.
  - SO-8 CAN transceivers with SIC drop-in; unpopulated split termination, common-mode choke and TVS footprints on the steering bus (ADR-0019).
  - I/O per the tally in `bom/electrical.md`:
    - 12 SPI devices (2 IMUs, 4 wheel encoders, 4 suspension angle sensors, 2 knuckle encoders), each with a data-ready or chip-select line as needed
    - I2C + mux for 5 ride-height ToF sensors, plus at least one spare mux channel (ADR-0045)
    - 4 camera triggers + spare
    - ground-speed lighting: 3 mode outputs with current-limited drivers, including the Class 1 VCSEL with a hardware current limit
    - e-stop status input
    - status LEDs
- **Requirements:** SYS-04, -07, -19, -22, -24, -25, -29.
- **Open:**
  - **Deprecated historical feasibility tally (2026-10-07):** the following example predates ADR-0045. It is retained as a feasibility reference and requires a fresh clock/pin/DMA review for four operating UARTs and five ToFs; it does not establish the current spare-pin count. Against Zephyr's pin data for the STM32H723ZG (LQFP144), with SWD, HSE and LSE reserved, everything gets its own pin with 37 to spare: Ethernet RMII, 3 FDCAN, 5 UARTs, 4 SPI buses, 2 I2C, 6 timer outputs (4 triggers, a spare, PPS) and 22 plain GPIO (12 chip-selects, 4 interrupts, e-stop in, 2 LEDs, 3 lighting). One example assignment, to show feasibility only (the pin choice is yours):
    - Ethernet: REF_CLK PA1, MDIO PA2, MDC PC1, CRS_DV PA7, RXD0 PC4, RXD1 PC5, TX_EN PB11, TXD0 PB12, TXD1 PB13
    - FDCAN1 PA11/PA12, FDCAN2 PB5/PB6, FDCAN3 PD12/PD13
    - UARTs: USART1 PA9/PA10, USART2 PD5/PA3, USART3 PB10/PC11, USART6 PC6/PC7, USART10 PE3/PE2
    - SPI1 PA5/PA6/PD7, SPI2 PD3/PB14/PB15, SPI3 PB3/PB4/PB2, SPI4 PE12/PE13/PE14
    - I2C1 PB8/PB7, I2C2 PF1/PF0
    - Timer outputs: TIM2_CH1 PA0, TIM3_CH3 PB0, TIM3_CH4 PB1, TIM4_CH3 PD14, TIM4_CH4 PB9, TIM13_CH1 PF8
  - Connector map and pinouts for every sensor run
  - Four operating UARTs: verify A50S PB10 TX / PB11 RX contacts, assign H723 RX/TX and DMA resources, qualify 3.3 V logic and ground/reference/protection behavior. Both telemetry endpoints remain stubs; see [readiness review](../reviews/a50s-uart-readiness.md) and [UART ICD](../icd/controller-telemetry.md).
  - Recheck the earlier 37-spare-pin feasibility example against four operating UARTs + debug, steering SPI, cell-monitor SPI, all interrupts and final clocks. It is not a released pin/DMA map; the `arc_sync` board definition is still a skeleton.
  - ISM330DHCXTR at both IMUs, AS5047P angle boards and five ToF boards, all on our own [IMU](../../hardware/sensor-boards/imu/README.md), angle and [height](../../hardware/sensor-boards/tof/README.md) boards and assembled in house (ADR-0045); capture/layout, exact ToF variant and [qualification](../bom/sensor-candidate-review.md) remain pending. ELPM remains an unselected analog alternative
  - Camera-trigger connector to the carrier ([carrier-sync draft](../icd/carrier-sync.md)); GPIO/module map and contact numbering remain open
  - Isolated vs non-isolated transceivers (follows the grounding decision)
  - Ethernet path if the optional i210/i226 is fitted: point-to-point to the Orin instead of through the switch (ADR-0021). **Recommendation in ADR-0023 (Proposed):** don't fit it in v1; the sync MCU stays on the switch
  - Lighting driver design

### 3. Motor controllers: A50S V2.3c (×4 + 2 spares, off the shelf)

- **Decided** (ADR-0034):
  - Purchase four drive controllers, one per motor, plus two spares (ADR-0034).
  - Team Triforce A50S V2.3c, 12S version, bought, not designed. Mounted on the lower deck, not on the motors.
  - Runs our build of the vendored VESC firmware (ADR-0033); per-unit current calibration stays in the board's EEPROM.
  - Commands remain on classic CAN (ADR-0009); full ≥1 kHz drive telemetry uses one 2 Mbit/s UART per controller (ADR-0034). Firmware transmit/receive and clock mapping remain unimplemented.
  - PB10/PB11 retain USART3 TX/RX. The calculated status-stream load is 29.5% per UART; installed harness/DMA/timestamp/fault qualification is still required.
  - E-stop: our firmware reads ESTOP on a spare input (PPM, TBC) and ramps the brake; the power board's switched motor bus provides the hardware cut (section 1).
  - Motor phases (MR30) and the motor's Hall sensor cable come from each corner to the deck (ICD-corner-connector rev F).
- **Requirements:** SYS-05, -13, -19, -24, -25; RSK-11, -12, -18, -19, -20.
- **Open (integration, not design):**
  - Circuit numbers for the 20-pin Pico-Clasp are in ICD-corner-connector, provisional until checked on the first unit (ESTOP goes on Servo/PPM, which takes 3.3 or 5 V; SWD is on the connector)
  - Whether to feed each controller's Aux power input (12–48 V) from an always-on rail, so telemetry survives the e-stop's motor-bus cut. If it isn't, every e-stop silences all four controllers at T and the sync MCU logs four lost corners (SYS-25) per e-stop, which buries real faults in the failure records (SYS-18). The alternative is in software: the sync MCU ignores corner loss while its e-stop status input is active. Not decided
  - Use Triforce's supplied bulk capacitor on each XT30 cable; never tie the controllers' 5 V outputs together
  - Deck mounting and airflow: waits on the CAD model. The heatsink option is bought (decided 2026-10-07), giving 40 A continuous against the per-corner current in `budgets/power-scenarios.md`
  - Hall cable extensions and phase-lead routing across the suspension, away from command CAN, steering FD and UART pairs
  - Motor temperature: Castle doesn't document a thermistor on the 1010's sensor cable, so E-28 is bought as a fallback; meter the sensor port when the motors arrive
  - No datasheet exists: ratings come from Triforce's product page, the electrical design from the upstream hardware config (`hwconf/teamtriforceuk/a50s_v23c/`)

### 4. Orin carrier (fork of Antmicro's baseboard; v1.1)

v1.0 uses the developer kit's own carrier instead (ADR-0046); its interfaces are in [builds](../builds.md) and ICD-carrier-sync.


- **Decided** (ADR-0010, -0014, -0015, -0016):
  - Orin NX 16GB.
  - Four 22-pin CSI ports (stereo, quad, forward event, downward event).
  - Camera-trigger connector to the sync board.
  - Input from the fixed compute rail; PoE removed.
  - M.2 key M (NVMe, swappable) and key E (MT7922 Wi-Fi).
  - GbE.
  - Expansion-header GPIO for the hotspot button and LED.
  - Optional: Intel i210/i226 NIC on a spare PCIe x1 lane, point-to-point to the sync MCU (ADR-0021).
  - JetPack 7.2.1.
- **Requirements:** SYS-08, -26, -27, -29; RSK-01, -08, -13.
- **Open:**
  - [CSI feasibility checked](../../hardware/orin-carrier/README.md#four-port-csi-feasibility): Orin NX supports four 2-lane ports; the reference uses two 50-pin connectors and needs adaptation. Exact Arducam kit SKUs / 2-lane output modes, both GenX320s' lane modes and Trigger In breakouts, bandwidth, control pins and released 22-pin pinouts remain open
  - ~~Whether the Orin NX has a spare PCIe x1 lane~~ **Yes (checked 2026-10-07):** the module has 1 × x4 + 3 × x1 PCIe Gen4; NVMe takes the x4 and the Wi-Fi card one x1, leaving at least one x1 for an optional i210/i226. Whether Antmicro's baseboard routes it is for the carrier fork's layout. Only matters if ADR-0023 (Proposed: don't fit it) is rejected
  - Trigger connector pinout
  - Device tree on JetPack 7.2
  - Camera driver availability for 7.2 (RSK-01)
  - Cooling (E-03)

### 5. Harness

The [wiring diagram and count schedule](../bom/motherboard-wiring.md) propose 17 separate sensor runs / 111 minimum conductors including spares (94 used + 17 spares, TBC). The [remote sensor carrier workspace](../../hardware/sensor-boards/README.md) records ten angle, two IMU and five ToF boards assembled in house (ADR-0045); 2-layer layouts remain TBC, without corner MCUs; complete connector counts and non-sensor interfaces remain TBD.

- **Decided:**
  - JST-GH for signal; the controllers' own XT30, MR30 and Pico-Clasp on the deck; everything locking. The Pico-Clasp housing (Molex 501189-2010) has an inner positive lock.
  - Corner connector rev F: motor phases and the Hall cable from each corner to the deck.
  - Command CAN is terminated at the sync board and the far trunk end; steering FD is a separate point-to-point link. Four UART runs carry drive telemetry; proposed TX/reference + RX/reference + spare budgets total 20 conductors, TBC. No CAN termination on UART signals.
  - One spare conductor per run; labels match schematic net names; strain relief at every entry.
  - Chassis service Ethernet port (E-09).
- **Open:** run lengths (from CAD), connector part numbers, terminator construction.

## Review packages prepared for the selected EE issues

- **A50S integration (#100):** [UART readiness review](../reviews/a50s-uart-readiness.md), [active UART ICD](../icd/controller-telemetry.md) and [arrival/qualification procedure](../tests/a50s-arrival.md). Controller streaming, clock alignment, DMA/pins and loaded signal margins remain open; ground design is deferred TBD.
- **Grounding (#94 / #115):** proposed ADR-0041 plus [return-path bench checks](../tests/grounding-and-signal-integrity.md). Non-isolated star proposal / complete isolation alternative require approval and measured margins; FD does not eliminate ground shift.
- **Missing interfaces (#102):** [power-rails](../icd/power-rails.md), [power-sync-stack](../icd/power-sync-stack.md), [carrier-sync](../icd/carrier-sync.md). Logical contracts exist; ratings, physical pins and technical sign-off remain open.

## Cross-cutting open questions

| Question | Who decides | Blocks |
| --- | --- | --- |
| **Grounding approval:** proposed ADR-0041 recommends a controlled power-return star with non-isolated communications, retaining a complete isolation alternative. Approve the return map / reference conductors and verify loaded ground offsets, CAN common-mode margins and UART logic/reference behavior using [bench checks](../tests/grounding-and-signal-integrity.md). Operating UART needs logic-level and parallel-return review; steering FD still needs common-mode qualification | EE (kunshpreet); separate safety reviewer TBD | Harness, transceiver choice, all boards |
| **Mass and CG** (SYS-16) | Mechanical | Power scenarios, bus clamp, pack choice |
| Steering motor peak current | Mechanical (actuator design) | Steering switched output |
| Aero fan power (SYS-15) | Capstone | Motor bus reserve |
| ICD owners and sign-offs (corner connector, corner mechanical, deck grid, power-rails, power-sync-stack and carrier-sync; the last three now have draft logical contracts) | Team | Fab gate |
