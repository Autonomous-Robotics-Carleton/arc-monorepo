# Electrical BOM (v1)

- **Status:** Draft. This is a functional BOM: what each item is, what it must achieve, and what it connects to. Part numbers come later. Rows where a part is already decided name it.
- **Selection:** `Decided` (ADR or spec), `Planned` (named in the spec, not confirmed), `Custom` (our own board, designed by the EE) or `TBD`. A dropped item keeps its ID and row, marked *Deleted*.
- `TBC` values are engineering proposals for the owner to confirm. `TBD` values wait on a SYS requirement.

## Compute and communication

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-01 | Compute module | 1 | Decided: Jetson Orin NX 16GB (ADR-0010) | Runs the classical stack plus a learned policy that fits in 16 GB; sustained load with no thermal throttling | Carrier | Compute | SYS-08, RSK-08 |
| E-02 | Carrier board | 1 | Decided: fork of Antmicro Orin baseboard (ADR-0025) | 4× 22-pin CSI ports; camera trigger connector to sync board; input from fixed compute rail; M.2 key M and key E; GbE; optional Intel i210/i226 NIC on a spare PCIe x1 lane, point-to-point to the sync MCU (ADR-0021) | E-01, cameras, E-04, E-05, E-06 | Compute | SYS-08, RSK-01 |
| E-03 | Compute cooling (heatsink + fan) | 1 | TBD | Holds full NX power (~25 W, ~40 W Super mode) inside the enclosure without throttling | Carrier fan header | Compute | RSK-08 |
| E-04 | NVMe SSD | 1 | Planned: 4 TB, swappable | Sustained write ≥ total log rate (TBD, dominated by cameras) with margin; capacity ≥ `TBD` runs of ≥ 10 min | Carrier M.2 key M | Compute | SYS-09, SYS-23 |
| E-05 | Wi-Fi card + antennas | 1 | Decided: MediaTek MT7922 (Filogic 330), M.2 2230 key E; fallback Intel AX210 (ADR-0015) | Wi-Fi 6E client on the team router's 6 GHz band; hotspot (AP) mode on 5 GHz for the button hotspot; driver in mainline on JetPack 7.2.1's kernel 6.8 (ADR-0016); power saving off; antennas mounted high, away from motors and carbon-filled parts | Carrier M.2 key E | Compute | SYS-11, SYS-27, RSK-09, RSK-14 |
| E-06 | Ethernet switch | 1 | TBD: small unmanaged switch (ADR-0021) | ≥ 5 GbE ports (Orin, LiDAR, sync MCU, chassis service port E-09, spare); no PTP support needed | E-02, E-10, E-30, E-09 | 5 V or sensor rail (TBC) | SYS-24 |
| E-07 | *Deleted:* RC receiver (ExpressLRS). Not adopted; Wi-Fi stays the only wireless link (ADR-0005, ADR-0015) | — | — | — | — | — | — |
| E-08 | Hotspot button + LED | 1 | Decided (ADR-0015) | Momentary, panel-mount; press toggles the car's Wi-Fi between the team network and its own 5 GHz hotspot; LED shows hotspot on | Carrier expansion header GPIO (button, LED) | 3.3 V | SYS-27 |
| E-09 | Service Ethernet port | 1 | Decided (ADR-0015) | Panel-mount RJ45 (or locking equivalent) on the chassis; fixed car address, no gateway handed out | E-06 spare port | — | SYS-27 |

## Perception sensors

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-10 | 2D LiDAR | 1 | Decided: Hokuyo UST-10LX (ADR-0001) | 40 Hz, 0.25°, 10 m, 270° field of view clear of the chassis; crash guard | E-06 (Ethernet) | Sensor (12 or 24 V, check datasheet) | SYS-08, SYS-12 |
| E-11 | Forward stereo camera | 1 kit (2 cams) | Planned: Arducam AR0234 stereo | Global shutter; hardware trigger input; fixed baseline on a rigid bar | CSI port 1; trigger from E-30 | 3.3 V / 5 V | SYS-07, SYS-08 |
| E-12 | Side/rear cameras | 1 kit (4 cams) | Planned: Arducam quad OV9281 | Global shutter mono; hardware trigger input | CSI port 2; trigger from E-30 | 3.3 V / 5 V | Mission 4 (experiment TBD) |
| E-13 | Event camera | 1 | Planned: Prophesee GenX320 | 2-lane MIPI; Jetson Orin NX driver; Trigger In pin broken out on the MIPI module and wired to a sync-board trigger output (ADR-0021) | CSI port 3; sync from E-30 | 3.3 V / 5 V | Mission 4 (experiment TBD) |

## Vehicle-state sensors

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-20 | IMU, at CG | 1 | Decided: navigation-grade MEMS (Analog Devices ADIS1650x or Murata SCHA63T class) | Low bias instability and vibration sensitivity (it carries velocity through slip); SPI; ≥ 4 kHz output; gyro ≥ ±2000 °/s, accel ≥ ±16 g (TBC); data-ready interrupt timestamped at the pin; vibration-isolated mount | E-30 (SPI + interrupt) | 3.3 V | SYS-06, SYS-07, SYS-10 |
| E-21 | IMU, front axle | 1 | TBD (consumer grade) | SPI; ≥ 4 kHz; data-ready interrupt; vibration-isolated mount | E-30 (SPI + interrupt) | 3.3 V | SYS-06, mission 4 |
| E-22 | Wheel encoder | 4 | Decided: AS5047 on wheel output shaft (ADR-0008) | Read over SPI as absolute angle; good to ≥ 3,700 rpm (peak wheel speed at 13.5:1) with margin | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-10 |
| E-23 | Suspension angle sensor | 4 | Decided: contactless magnetic (AS5047 family) at each suspension pivot, replacing potentiometers | Covers full suspension travel; no wiper noise or wear; ≥ 2 kHz | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-18 |
| E-24 | Steering knuckle encoders | 2 (front left + front right) | Decided: AS5047, 14-bit absolute | One on each front knuckle (no homing): both wheel steer angles measured directly, toe change under load, real Ackermann geometry; absolute reference for the steering actuator at startup and a continuous cross-check against the actuator's own angle (ADR-0019) | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-19, SYS-24 |
| E-25 | Ground-speed sensor: downward event camera | 1 | Decided: Prophesee GenX320 (ADR-0014) | Forward and sideways ground velocity at 0–12 m/s on the school floor; ~90° M12 lens, aperture ~f/8 for speckle; mounted inside the chassis looking through a floor window with a light shroud; scale from E-26 | Carrier CSI port 4; Trigger In from E-30 (ADR-0021) | 3.3 V / 5 V | SYS-10, RSK-07 |
| E-29 | Ground-speed illumination | 1 set | Decided (ADR-0014) | Three switchable modes: (1) cross-polarized LED ring, constant-current DC (no PWM); (2) grazing-angle LEDs; (3) off-the-shelf **certified IEC 60825-1 Class 1** IR VCSEL module with a hardware current limit, for laser speckle | E-30 GPIO (mode select) | 5 V via current-limited drivers | SYS-10, RSK-07 |
| E-26 | Ride-height ToF sensor | 5 (4 corners + 1 beside E-25 for scale) | TBD: ToF (ADR-0007); laser triangulation considered and rejected on cost | Minimum range ≤ sensor-to-floor distance at full compression (recess the sensor if needed); works on glossy tile; individually addressable (I2C mux or one XSHUT line each) | E-30 (I2C) | 3.3 V | SYS-10, SYS-15, RSK-07 |
| E-27 | *Deleted:* steering servo current sense. Steering torque comes from the steering controller's current (ADR-0019) | — | — | — | — | — | — |
| E-28 | Motor temperature NTC | 4 | Decided (spec); bought as a fallback | Glued to motor can; wired to the controller's Motor temp input (3.3 V max). Castle doesn't document a temperature pin on the 1010's sensor cable, and the 1010 isn't ROAR certified, so its pinout can't be assumed: buy these in the single order, meter the motor's sensor port on arrival (a thermistor reads ~10 kΩ to GND at room temperature), and fit E-28 only if there's none | E-50 | — | SYS-19 |

**E-25 selection** is recorded in ADR-0014, including the rejected candidates (PMW3901 0.58 m/s, PAA5100JE 1.14 m/s, SparkFun OTOS 2.5 m/s, mouse chips, radar, Correvit). Fallback: the dead-wheel pod design in ADR-0014, with its underfloor space and sync-board channels reserved.

## Sync and control

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-30 | Sync MCU board | 1 | Decided: STM32H723 (3 FDCAN, Ethernet MAC, many UARTs) + PHY (ADR-0011), firmware on Zephyr (ADR-0017); SO-8 CAN transceivers (SIC drop-in), unpopulated rework footprints; steering on CAN-FD (ADR-0019), on the MCP2518FD or a freed internal FDCAN (EE's call, ADR-0034); one UART per motor controller (ADR-0034) | Owns the time base; timestamps every input at the pin; enforces the safety envelope and heartbeat watchdog in firmware, outside Linux; I/O per the tally below, plus one spare per channel type | E-06, E-02 (PPS), camera triggers (E-11, E-12, E-13, E-25), all vehicle-state sensors, E-29, E-33, E-50 (command CAN, telemetry UARTs), E-31 (steering CAN-FD), E-41 (stacking header) | 3.3 V / 5 V | SYS-04, SYS-07, SYS-19, SYS-22 |
| E-31 | Steering actuator | 1 | Decided: mjbots moteus-c1 ($69) + gimbal-style brushless motor + ~4–6:1 belt (ADR-0019). Fallback: integrated actuator, CubeMars AK60-6 class | Lock to lock ≤ 0.1 s; ≥ 10 Hz position bandwidth; ≥ 1.5 N·m (TBC); backlash ≤ 0.1°; angle and torque at ≥ 1 kHz; absolute angle from E-24; e-stop: sync MCU commands rate-limited return to centre; controller timeout mode decelerate-and-hold; power cut at T_drive + ~1 s | Dedicated CAN-FD steering bus (sync board MCP2518FD); switched power-board output | Motor bus via switched output | SYS-06, SYS-08, SYS-10, SYS-22 |
| E-32 | Physical e-stop button | 1 | TBD | Latching; reachable on the car; drives ESTOP low on all four motor controllers in hardware | E-41 e-stop circuit | — | SYS-05 |
| E-33 | Status LEDs | TBD | TBD | Show e-stop state, watchdog state, power-rail faults | E-30 GPIO | 3.3 V | SYS-19 |

## Drive

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-50 | Motor controller | 4 + 2 spares | Decided: Team Triforce A50S V2.3c, 12S version, **with the heatsink option** (decided 2026-10-07), off the shelf (ADR-0034) | Single motor; Hall-sensored FOC; 6–52 V; 40 A continuous with the heatsink (20 A without, 80 A for 4 s) against ~12 A needed; board 35.5 × 21 × 13.8 mm, ~30 g with the heatsink (heatsink envelope TBC from Triforce's STEP models); mounted on the lower deck, not the motor. Runs our build of the vendored VESC firmware (target `a50s_v23c_12s`, ADR-0033). Commands on classic CAN at 1 Mbit/s (ADR-0009); full status at 1 kHz over a UART to the sync MCU, stamped at sampling (ADR-0024); ESTOP on a spare input (PPM, TBC) read by firmware for the brake ramp, internal pull-down (ADR-0012); command timeout; maximum eRPM ~75k (TBC). No datasheet: ratings from the product page, pinout from its pinout image (ICD-corner-connector rev E; SWD, CAN, UART, PPM, Halls, motor temp and an aux logic-power input on a latching 20-pin Pico-Clasp), electrical design from the upstream hardware config | E-51 (phases, Hall cable), command bus, UART to E-30, ESTOP from E-41 | Motor bus (XT30) | SYS-01, SYS-05, SYS-13, SYS-24, SYS-25, RSK-11, RSK-12, RSK-18, RSK-19, RSK-20 |
| E-51 | Drive motor | 4 | Decided: Castle 1010-4400kV sensored, one per wheel (ADR-0026) | 2S–4S; Hall sensored; 28 mm can | E-50 | — | SYS-01 |

## Power

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-40 | Battery pack | 1 per run | TBD: 4S LiPo, 1/10 hardcase; ~5,000 mAh is the starting point | Peak ~30–35 A at 9 m/s (SYS-01), limited by grip (`budgets/power-scenarios.md`; the old ~120 A figure was motor capability), so the C rating is not a constraint; capacity giving ≥ 10 min hard driving (~4–5 Ah usable); fits the battery bay; connectors per ICD-battery-pack | E-42, E-41, E-46 (balance lead) | — | SYS-03, SYS-16, SYS-32 |
| E-41 | Central power board | 1 | Custom | Rails below; per-rail fuse, switch and current sense; battery V and I monitor; per-cell monitor (E-46); e-stop circuit sourcing ESTOP; **switched motor bus to the four drive controllers, cut by the delayed e-stop line at T (TBC ~1.8 s), open when unpowered, rated for peak pack current (ADR-0034)**; switched, fused motor-bus output for steering, cut at T + ~1 s (ADR-0019); ideal-diode OR-ing of wall and battery; all regulators rated ≥ 30 V input with TVS (RSK-06); reverse-polarity protection; no resets from voltage sag at peak current (SYS-31); clean-shutdown signal to the Orin on low battery (SYS-30); data to E-30 over the stacking header (ICD power-sync-stack) | E-40, E-44, all low-voltage loads, E-30 (stacking header), E-31 (switched output), E-32, E-50 ×4 (ESTOP, switched motor bus) | — | SYS-03, SYS-05, SYS-14, SYS-19, SYS-30, SYS-31, SYS-32 |
| E-42 | Loop key / anti-spark switch | 1 | TBD | Rated for peak pack current | E-40 → motor bus | Motor bus | SYS-05 |
| E-43 | Bus clamp board | 1 | Custom | Comparator + MOSFET + power resistor; threshold just above 16.8 V (TBD); resistor and heatsink sized for worst-case regen from all four corners | Motor bus | Motor bus | SYS-14 |
| E-44 | Wall supply | 1 | Planned: 19 V brick | Above full pack voltage, so OR-ing selects it; power ≥ total low-voltage peak load | E-41 | — | SYS-14 |
| E-45 | Motor distribution | 1 | TBD: bus bar or heavy wire | Carries peak pack current; from the power board's switched motor bus to an XT30 at each controller on the deck | E-42 → E-41 → E-50 ×4 | Motor bus | SYS-01 |
| E-46 | Per-cell battery monitor | 1 | TBD (EE): on the power board | Measures each of the 4 cell voltages through the pack's balance lead: ≥ 100 Hz (TBC), ±10 mV; fused, high-impedance taps with negligible drain when the car is off; data to the sync MCU over the stacking header so it's timestamped and logged; lowest cell drives the low-battery warning and shutdown | Pack balance connector (ICD-battery-pack); E-41; E-30 | 3.3 V | SYS-19, SYS-30, SYS-32 |

### Low-voltage rails (E-41)

| Rail | Feeds | Requirement |
| --- | --- | --- |
| Compute | E-01, E-02, E-03 (and E-04, E-05 via the carrier) | Fixed regulated voltage within the carrier's input range; current for NX Super mode plus carrier, NVMe and Wi-Fi |
| Sensor | E-10 (and E-06 if powered here) | Voltage per the Hokuyo datasheet |
| 5 V / 3.3 V | Cameras, E-30, vehicle-state sensors, E-29, E-33, E-46 | Low noise for the IMUs and ADC reference |

Loads per rail are in `budgets/power.csv` and the sizing scenarios in `budgets/power-scenarios.md`. Rail currents and regulator choices are the EE's.

## Sync MCU I/O tally (E-30)

The sync board's channel counts, from the BOM above, plus the one-spare-per-type rule.

| Interface | v1 users | Count | + Spare |
| --- | --- | --- | --- |
| SPI devices (chip-selects) | 2 IMUs, 4 wheel encoders, 4 suspension angle sensors, 2 steering knuckle encoders | 12 | ≥ 1 |
| Interrupt inputs (data-ready) | 2 IMUs | 2 | ≥ 1 |
| Power-board data (stacking header) | Battery V/I, per-rail currents and faults, 4 cell voltages (E-46) | **Open.** Recommended (ADR-0022, Proposed): 1 dedicated I2C bus + an interrupt input per alert/fault source (`TBD`), with analog battery V/I as a rework fallback | ≥ 1 |
| I2C | 5 ride-height ToF (via mux or XSHUT) | 1 bus + mux, or 5 XSHUT GPIO | ≥ 1 |
| Camera trigger outputs | Stereo kit, quad kit, forward event camera, downward event camera | 4 | ≥ 1 |
| PPS / time-sync output | To Orin | 1 | — |
| PWM output | None (steering moved to CAN, ADR-0019) | 0 | ≥ 1 |
| CAN | Command bus (classic, 4 motor controllers); steering bus (CAN-FD, moteus-c1) | 2 (of 3 FDCAN, plus the MCP2518FD if steering stays on it) | ≥ 1 |
| UART | Debug console; telemetry from each motor controller at 3 Mbit/s (TBC, ADR-0034) | 5 | ≥ 1 |
| Ethernet | Link to Orin via switch (E-06). **Open:** if the optional i210/i226 is fitted, this link runs point-to-point to it instead (ADR-0021), freeing a switch port. Recommended (ADR-0023, Proposed): don't fit it | 1 | — |
| GPIO | E-stop status in, status LEDs, ground-speed lighting mode select (3) | TBD + 3 | ≥ 2 |
| Low-battery shutdown signal to the Orin (SYS-30) | Orin | **Open.** Recommended (ADR-0022, Proposed): 1 E-30 GPIO; E-41 cuts the compute rail after the Orin halts and has its own undervoltage backstop | — |

## Ground equipment

Not on the car, but part of the system: its configuration (DHCP options, WMM priorities, fixed addresses, the laptop gateway script) goes in version control.

| ID | Item | Qty | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| G-01 | Team router | 1 | Decided: GL.iNet Flint 3 (GL-BE9300), $209.99 (ADR-0015) | Wi-Fi 6 GHz band for the car link; OpenWrt (per-host DHCP options, WMM); ≥ 2 wired ports for laptops; USB tethering for phone internet backup; mounted high at the track | SYS-11, SYS-26, SYS-27, SYS-28 |
| G-02 | Travel router | 0 (later) | Optional: GL.iNet Slate 7 (GL-BE3600), $159.99 | USB-C powered for away events | SYS-27 |
| G-03 | Ground-station laptop | 1+ | Team laptops | Ethernet port (or USB-Ethernet); Linux or macOS for the gateway script | SYS-26, SYS-28 |
| G-04 | Sync MCU development board | 2 | Decided: ST NUCLEO-H723ZG, ~$30 each (SQ-5, 2026-10-08) | Same MCU and package as the sync board (STM32H723ZG, LQFP144); on-board ST-Link and Ethernet. Runs the sync MCU firmware unchanged (ADR-0028's development-hardware target): firmware work before the sync board exists, a known-good reference for its bring-up, a fallback if it needs a respin, and the R1 bench rig. No CAN transceiver on board (G-05) | SYS-33, ADR-0028 |
| G-05 | CAN transceiver breakout | 2 | TBD: any 3.3 V CAN transceiver module, a few dollars each, plus 120 Ω terminators if the module has none | One per G-04, so a Nucleo can talk to an A50S on the bench command bus | SYS-33 |

