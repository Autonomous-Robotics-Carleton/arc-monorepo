# Electrical handoff

The starting point for the electrical engineer. It collects, per board, what's decided, where the details live, and what's still open. Circuit-level choices (regulators, fuse values, layout, part selection within the stated requirements) are yours.

## How this repo works

- `systems/` is the source of truth. `architecture.md` is the overview; where it disagrees with an ADR, ICD or requirement, those win.
- Changes go through PRs. A change to an ICD needs sign-off from the owners on both sides.
- Nothing is fabbed until it passes `reviews/fab-gate.md`.
- **Funding is a single order**, so there's no early hardware testing. Design in rework-only fallbacks (unpopulated footprints, jumpers, harness options), and order extra bare PCBs of every custom board.

## Read first

| What | Where |
| --- | --- |
| System requirements | `requirements/system.md` |
| Decisions (ADRs) | `adr/README.md` |
| Functional BOM + sync MCU I/O tally | `bom/electrical.md` |
| Corner connector (rev E: motor phases and Hall cable to the deck) | `icd/corner-connector.md` |
| Power loads and scenarios | `budgets/power.csv`, `budgets/power-scenarios.md` |
| Bus load, latency, cost | `budgets/bus-load.csv`, `budgets/latency.csv`, `budgets/cost.csv` |
| Risks | `risks.md` |
| Verification plan (rigs R1–R8, pre-order S0 checklist) | `verification/plan.md` |

## Boards

### 1. Power board (+ e-stop circuit) and bus clamp board

- **Decided:**
  - LV rails: compute, sensor, 5 V / 3.3 V. The servo rail is removed (ADR-0019).
  - Per-rail fuse, switch and current sense; battery V/I monitoring.
  - Wall (19 V) / battery ideal-diode OR-ing with no reboot.
  - Regulators rated ≥ 30 V with TVS.
  - E-stop circuit sourcing ESTOP to the four motor controllers (ADR-0012).
  - **Switched motor bus to the four drive controllers, cut by the delayed e-stop line at T (TBC ~1.8 s from 9 m/s), rated for peak pack current (~30–35 A).** This is the hardware torque cut (ADR-0034).
  - Switched, fused motor-bus output for steering, cut at T + ~1 s (ADR-0019).
  - Bus clamp on the motor distribution bus.
  - Per-cell battery monitor through the balance lead, ≥ 100 Hz to ±10 mV, data to the sync MCU (SYS-32, E-46).
- **Requirements:** SYS-03, -05, -14, -19, -30, -31, -32; RSK-06, RSK-11.
- **Open:**
  - Rail current ratings and regulator choices (yours, from `power.csv`)
  - ESTOP logic level, driver and current per controller; the motor-bus switch part
  - Bus clamp threshold and resistor sizing (~140 J, ~160 W peak estimate from 9 m/s)
  - Low-battery thresholds (on the lowest cell) and the clean-shutdown signal to the Orin (SYS-30)
  - Cell-monitor part and balance-lead connection while the pack is installed (E-46)
  - Stacking-header pinout with the sync board (ICD power-sync-stack, not written yet)
  - Whether battery V/I, rail currents and cell voltages are digitised here and sent to the sync MCU over a bus, or read as analogue by the sync MCU's ADC (`bom/electrical.md` I/O tally). **Recommendation in ADR-0022 (Proposed):** monitor ICs on this board, one dedicated I2C bus, fault pins as sync MCU interrupts, analog battery V/I as a rework fallback
  - Whether this board or the sync MCU drives the clean-shutdown signal to the Orin (SYS-30). **Recommendation in ADR-0022 (Proposed):** two tiers. The sync MCU warns and requests the Orin shutdown; this board cuts the compute rail once the Orin has halted, plus an independent hardware undervoltage cutoff that protects the pack if software fails
  - Anti-spark / loop key and motor distribution parts (E-42, E-45)
  - Grounding strategy (shared with all boards; see Cross-cutting)

### 2. Sync MCU board

- **Decided:**
  - STM32H723 + Ethernet PHY (ADR-0011); firmware on Zephyr (ADR-0017).
  - Time sync: µs-critical sensors timestamped in the sync MCU's clock; camera triggers and periodic pulses to each GenX320's Trigger In; Orin synced in software, PPS to an Orin GPIO as cross-check (ADR-0021).
  - Buses: one classic command CAN bus and a CAN-FD steering bus (ADR-0019); one UART per motor controller for telemetry (ADR-0034). The steering bus can move from the MCP2518FD to a freed internal FDCAN: your call.
  - SO-8 CAN transceivers with SIC drop-in; unpopulated split termination, common-mode choke and TVS footprints on the steering bus (ADR-0013).
  - I/O per the tally in `bom/electrical.md`:
    - 12 SPI devices (2 IMUs, 4 wheel encoders, 4 suspension angle sensors, 2 knuckle encoders), each with a data-ready or chip-select line as needed
    - I2C + mux for 5 ToF sensors
    - 4 camera triggers + spare
    - ground-speed lighting: 3 mode outputs with current-limited drivers, including the Class 1 VCSEL with a hardware current limit
    - e-stop status input
    - status LEDs
- **Requirements:** SYS-04, -07, -19, -22, -24, -25, -29.
- **Open:**
  - Package and pin-mux check for 12 SPI chip-selects + FDCAN + 5 UARTs + Ethernet + triggers
  - Connector map and pinouts for every sensor run
  - Camera-trigger connector to the carrier (ICD carrier-sync, not written yet)
  - Isolated vs non-isolated transceivers (follows the grounding decision)
  - Ethernet path if the optional i210/i226 is fitted: point-to-point to the Orin instead of through the switch (ADR-0021). **Recommendation in ADR-0023 (Proposed):** don't fit it in v1; the sync MCU stays on the switch
  - Lighting driver design

### 3. Motor controllers: A50S V2.3c (×4 + 2 spares, off the shelf)

- **Decided** (ADR-0034):
  - Team Triforce A50S V2.3c, 12S version, bought, not designed. Mounted on the lower deck, not on the motors.
  - Runs our build of the vendored VESC firmware (ADR-0033); per-unit current calibration stays in the board's EEPROM.
  - Commands on the classic CAN command bus (ADR-0009); telemetry over one UART per controller to the sync MCU, 3 Mbit/s (TBC).
  - E-stop: our firmware reads ESTOP on a spare input (PPM, TBC) and ramps the brake; the power board's switched motor bus provides the hardware cut (section 1).
  - Motor phases (MR30) and the motor's Hall sensor cable come from each corner to the deck (ICD-corner-connector rev E).
- **Requirements:** SYS-05, -13, -19, -24, -25; RSK-11, -12, -18, -19, -20.
- **Open (integration, not design):**
  - Molex pin numbers for the 20-pin Pico-Clasp layout now in ICD-corner-connector (ESTOP goes on Servo/PPM, which takes 3.3 or 5 V; SWD is on the connector)
  - Whether to feed each controller's Aux power input (12–48 V) from an always-on rail, so telemetry survives the e-stop's motor-bus cut
  - Use Triforce's supplied bulk capacitor on each XT30 cable; never tie the controllers' 5 V outputs together
  - Deck mounting and airflow: waits on the CAD model. The heatsink option is bought (decided 2026-10-07), giving 40 A continuous against ~12 A
  - Hall cable extensions and phase-lead routing across the suspension, away from CAN and UART lines
  - Motor temperature: Castle doesn't document a thermistor on the 1010's sensor cable, so E-28 is bought as a fallback; meter the sensor port when the motors arrive
  - No datasheet exists: ratings come from Triforce's product page, the electrical design from the upstream hardware config (`hwconf/teamtriforceuk/a50s_v23c/`)

### 4. Orin carrier (fork of Antmicro's baseboard)

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
  - CSI lane mapping for the Arducam kits and both GenX320s
  - Whether the Orin NX has a spare PCIe x1 lane for the optional i210/i226 (not needed if ADR-0023, Proposed, is accepted)
  - Trigger connector pinout
  - Device tree on JetPack 7.2
  - Camera driver availability for 7.2 (RSK-01)
  - Cooling (E-03)

### 5. Harness

- **Decided:**
  - JST-GH for signal; the controllers' own XT30, MR30 and Pico-Clasp on the deck; everything locking (check that the Pico-Clasp latches).
  - Corner connector rev E: motor phases and the Hall cable from each corner to the deck.
  - Command-bus trunk terminated at the sync board and the far end in the harness; UART links point to point, twisted with ground.
  - One spare conductor per run; labels match schematic net names; strain relief at every entry.
  - Chassis service Ethernet port (E-09).
- **Open:** run lengths (from CAD), connector part numbers, terminator construction.

## Cross-cutting open questions

| Question | Who decides | Blocks |
| --- | --- | --- |
| **Grounding strategy:** motor ground vs logic ground vs CAN/FD returns; isolated transceivers or a single-point ground | EE, with the systems lead | Harness, transceiver choice, all boards |
| **Mass and CG** (SYS-16) | Mechanical | Power scenarios, bus clamp, pack choice |
| Steering motor peak current | Mechanical (actuator design) | Steering switched output |
| Aero fan power (SYS-15) | Capstone | Motor bus reserve |
| ICD owners and sign-offs (corner connector, and the unwritten power-rails, power-sync-stack and carrier-sync ICDs) | Team | Fab gate |
| v1 budget (SYS-17) | Team | Upgrade choices |
