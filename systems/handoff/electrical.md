# Electrical handoff

The starting point for the electrical engineer. It collects, per board, what's decided, where the details live, and what's still open. Circuit-level choices (regulators, fuse values, layout, part selection within the stated requirements) are yours.

## How this repo works

- `systems/` is the source of truth. `architecture.md` is the old narrative overview; where it disagrees with an ADR, ICD or requirement, those win.
- Changes go through PRs. A change to an ICD needs sign-off from the owners on both sides.
- Nothing is fabbed until it passes `reviews/fab-gate.md`.
- **Funding is a single order**, so there's no early hardware testing. Design in rework-only fallbacks (unpopulated footprints, jumpers, harness options), and order extra bare PCBs of every custom board.

## Read first

| What | Where |
| --- | --- |
| System requirements | `requirements/system.md` |
| Decisions (ADRs 0001–0019) | `adr/README.md` |
| Functional BOM + sync MCU I/O tally | `bom/electrical.md` |
| Corner connector (rev D, 10-pin) | `icd/corner-connector.md` |
| Power loads and scenarios | `budgets/power.csv`, `budgets/power-scenarios.md` |
| Bus load, latency, cost | `budgets/bus-load.csv`, `budgets/latency.csv`, `budgets/cost.csv` |
| Risks | `risks.md` |
| Bench test for the CAN-FD buses | `tests/rsk-03-canfd-bench.md` |

## Boards

### 1. Power board (+ e-stop circuit) and bus clamp board

- **Decided:**
  - LV rails: compute, sensor, 5 V / 3.3 V. The servo rail is removed (ADR-0019).
  - Per-rail fuse, switch and current sense; battery V/I monitoring.
  - Wall (19 V) / battery ideal-diode OR-ing with no reboot.
  - Regulators rated ≥ 30 V with TVS.
  - E-stop circuit sourcing ESTOP_EN to the four corners (ADR-0012).
  - Switched, fused motor-bus output for steering, cut by a delayed e-stop line at T_drive + ~1 s (ADR-0019).
  - Bus clamp on the motor distribution bus.
  - Per-cell battery monitor through the balance lead, ≥ 100 Hz to ±10 mV, data to the sync MCU (SYS-32, E-46).
- **Requirements:** SYS-03, -05, -14, -19, -30, -31, -32; RSK-06, RSK-11.
- **Open:**
  - Rail current ratings and regulator choices (yours, from `power.csv`)
  - ESTOP_EN logic level, driver and current per corner
  - Bus clamp threshold and resistor sizing (~250 J, ~200 W peak estimate)
  - Low-battery thresholds (on the lowest cell) and the clean-shutdown signal to the Orin (SYS-30)
  - Cell-monitor part and balance-lead connection while the pack is installed (E-46)
  - Stacking-header pinout with the sync board (ICD power-sync-stack, not written yet)
  - Anti-spark / loop key and motor distribution parts (E-42, E-45)
  - Grounding strategy (shared with all boards; see Cross-cutting)

### 2. Sync MCU board

- **Decided:**
  - STM32H723 + Ethernet PHY (ADR-0011); firmware on Zephyr (ADR-0017).
  - Buses: one classic command CAN bus, two CAN-FD telemetry buses (front, rear), and a CAN-FD steering bus via a populated MCP2518FD (ADR-0011, ADR-0019).
  - SO-8 CAN transceivers with SIC drop-in; unpopulated split termination, common-mode choke and TVS footprints (ADR-0013).
  - I/O per the tally in `bom/electrical.md`:
    - 12 SPI devices (2 IMUs, 4 wheel encoders, 4 suspension angle sensors, 2 knuckle encoders), each with a data-ready or chip-select line as needed
    - I2C + mux for 5 ToF sensors
    - 4 camera triggers + spare
    - ground-speed lighting: 3 mode outputs with current-limited drivers, including the Class 1 VCSEL with a hardware current limit
    - e-stop status input
    - status LEDs
- **Requirements:** SYS-04, -07, -19, -22, -24, -25, -29.
- **Open:**
  - **PPS vs PTP** for time sync to the Orin (Zephyr supports PTP natively)
  - Package and pin-mux check for 12 SPI chip-selects + 3 FDCAN + Ethernet + triggers
  - Connector map and pinouts for every sensor run
  - Camera-trigger connector to the carrier (ICD carrier-sync, not written yet)
  - Isolated vs non-isolated transceivers (follows the grounding decision)
  - Lighting driver design

### 3. VESC 6.4 fork (×4 corners + spares)

- **Decided** (ADR-0006, -0011, -0012, -0013):
  - VESC 6.4 base; MPU9150 and NRF24 not populated.
  - Command bus on the built-in CAN + TJA1051T/3, untouched.
  - Telemetry via an added MCP2518FD + 40 MHz crystal + SO-8 CAN FD transceiver (SIC drop-in), with the FD pair passed straight through (daisy-chain).
  - Rework footprints.
  - ESTOP_EN: GPIO read for the firmware brake ramp, plus a hardware delay (T ≈ 3 s) ANDed into DRV8301 EN_GATE, with a fail-safe pull-down.
  - Connector rev D.
  - Layout rules: transceiver within 1–2 cm of the connector; 120 Ω differential pair; no vias; away from phase outputs.
- **Requirements:** SYS-05, -13, -24, -25; RSK-02, -03, -11, -12.
- **Open:**
  - **RSK-03:** bench testing can't happen before the order, so do the LTspice step in the test procedure before layout
  - ESTOP_EN level; final delay T and brake-ramp values
  - Confirm the DRV8301 re-init behaviour after EN_GATE low
  - Size target (~30 × 40 mm from the original spec) against reality; FET thermal path to the motor mount

### 4. Orin carrier (fork of Antmicro's baseboard)

- **Decided** (ADR-0010, -0014, -0015, -0016):
  - Orin NX 16GB.
  - Four 22-pin CSI ports (stereo, quad, forward event, downward event).
  - Camera-trigger connector to the sync board.
  - Input from the fixed compute rail; PoE removed.
  - M.2 key M (NVMe, swappable) and key E (MT7922 Wi-Fi).
  - GbE.
  - Expansion-header GPIO for the hotspot button and LED.
  - JetPack 7.2.1.
- **Requirements:** SYS-08, -26, -27, -29; RSK-01, -08, -13.
- **Open:**
  - CSI lane mapping for the Arducam kits and both GenX320s
  - Trigger connector pinout
  - Device tree on JetPack 7.2
  - Camera driver availability for 7.2 (RSK-01)
  - Cooling (E-03)

### 5. Harness

- **Decided:**
  - JST-GH for signal, XT60 for corner power; everything locking.
  - Corner connector rev D (10-pin).
  - Command-bus trunk terminated at the sync board and the far end in the harness; FD buses daisy-chained with a sealed terminator pigtail at the far VESC.
  - One spare conductor per run; labels match schematic net names; strain relief at every entry.
  - Chassis service Ethernet port (E-09).
- **Open:** run lengths (from CAD), connector part numbers, terminator construction.

## Cross-cutting open questions

| Question | Who decides | Blocks |
| --- | --- | --- |
| **Grounding strategy:** motor ground vs logic ground vs CAN/FD returns; isolated transceivers or a single-point ground | EE, with the systems lead | Harness, transceiver choice, all boards |
| **PPS vs PTP** time sync | EE + software | Sync board, carrier |
| **Mass and CG** (SYS-16) | Mechanical | Power scenarios, bus clamp, pack choice |
| Steering motor peak current | Mechanical (actuator design) | Steering switched output |
| Aero fan power (SYS-15) | Capstone | Motor bus reserve |
| ICD owners and sign-offs (corner connector, and the unwritten power-rails, power-sync-stack and carrier-sync ICDs) | Team | Fab gate |
| v1 budget (SYS-17) | Team | Upgrade choices |
