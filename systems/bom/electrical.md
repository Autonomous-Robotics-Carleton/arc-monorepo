# Electrical BOM (v1)

- **Status:** Draft. This is a functional BOM: what each item is, what it must achieve, and what it connects to. Part numbers come later. Rows where a part is already decided name it.
- **Selection:** `Decided` (ADR or spec), `Planned` (named in the spec, not confirmed) or `TBD`.
- `TBC` values are engineering proposals for the owner to confirm. `TBD` values wait on a SYS requirement.

## Compute and communication

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-01 | Compute module | 1 | Decided: Jetson Orin NX 16GB (ADR-0010) | Runs the classical stack plus a learned policy that fits in 16 GB; sustained load with no thermal throttling | Carrier | Compute | SYS-08, RSK-08 |
| E-02 | Carrier board | 1 | Decided: fork of Antmicro Orin baseboard | 4× 22-pin CSI ports; camera trigger connector to sync board; input from fixed compute rail; M.2 key M and key E; GbE | E-01, cameras, E-04, E-05, E-06 | Compute | SYS-08, RSK-01 |
| E-03 | Compute cooling (heatsink + fan) | 1 | TBD | Holds full NX power (~25 W, ~40 W Super mode) inside the enclosure without throttling | Carrier fan header | Compute | RSK-08 |
| E-04 | NVMe SSD | 1 | TBD | Sustained write ≥ total log rate (TBD, dominated by cameras) with margin; capacity ≥ `TBD` runs of ≥ 10 min | Carrier M.2 key M | Compute | SYS-09, SYS-23 |
| E-05 | Wi-Fi card + antennas | 1 | Decided: MediaTek MT7922 (Filogic 330), M.2 2230 key E; fallback Intel AX210 (ADR-0015) | Wi-Fi 6E client on the team router's 6 GHz band; hotspot (AP) mode on 5 GHz for the button hotspot; driver in mainline on JetPack 7.2.1's kernel 6.8 (ADR-0016); power saving off; antennas mounted high, away from motors and carbon-filled parts | Carrier M.2 key E | Compute | SYS-11, SYS-27, RSK-09, RSK-14 |
| E-06 | Ethernet switch | 1 | TBD: small PTP-capable switch (ADR-0020) | ≥ 5 GbE ports (Orin, LiDAR, sync MCU, chassis service port E-09, spare); **IEEE 1588 transparent clock** so PTP holds ≤ 10 µs alignment | E-02, E-10, E-30, E-09 | 5 V or sensor rail (TBC) | SYS-07 |
| E-07 | *Deleted:* RC receiver (ExpressLRS). Not adopted; Wi-Fi stays the only wireless link (ADR-0005, ADR-0015) | — | — | — | — | — | — |
| E-08 | Hotspot button + LED | 1 | Decided (ADR-0015) | Momentary, panel-mount; press toggles the car's Wi-Fi between the team network and its own 5 GHz hotspot; LED shows hotspot on | Carrier expansion header GPIO (button, LED) | 3.3 V | SYS-27 |
| E-09 | Service Ethernet port | 1 | Decided (ADR-0015) | Panel-mount RJ45 (or locking equivalent) on the chassis; fixed car address, no gateway handed out | E-06 spare port | — | SYS-27 |

## Perception sensors

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-10 | 2D LiDAR | 1 | Decided: Hokuyo UST-10LX (ADR-0001) | 40 Hz, 0.25°, 10 m, 270° field of view clear of the chassis; crash guard | E-06 (Ethernet) | Sensor (12 or 24 V, check datasheet) | SYS-08, SYS-12 |
| E-11 | Forward stereo camera | 1 kit (2 cams) | Planned: Arducam AR0234 stereo | Global shutter; hardware trigger input; fixed baseline on a rigid bar | CSI port 1; trigger from E-30 | 3.3 V / 5 V | SYS-07, SYS-08 |
| E-12 | Side/rear cameras | 1 kit (4 cams) | Planned: Arducam quad OV9281 | Global shutter mono; hardware trigger input | CSI port 2; trigger from E-30 | 3.3 V / 5 V | Mission 4 (experiment TBD) |
| E-13 | Event camera | 1 | Planned: Prophesee GenX320 | 2-lane MIPI; Jetson Orin NX driver; trigger or timestamp sync input | CSI port 3; sync from E-30 | 3.3 V / 5 V | Mission 4 (experiment TBD) |

## Vehicle-state sensors

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-20 | IMU, at CG | 1 | Decided: navigation-grade MEMS (Analog Devices ADIS1650x or Murata SCHA63T class) | Low bias instability and vibration sensitivity (it carries velocity through slip); SPI; ≥ 4 kHz output; gyro ≥ ±2000 °/s, accel ≥ ±16 g (TBC); data-ready interrupt timestamped at the pin; vibration-isolated mount | E-30 (SPI + interrupt) | 3.3 V | SYS-06, SYS-07, SYS-10 |
| E-21 | IMU, front axle | 1 | TBD (consumer grade) | SPI; ≥ 4 kHz; data-ready interrupt; vibration-isolated mount | E-30 (SPI + interrupt) | 3.3 V | SYS-06, mission 4 |
| E-22 | Wheel encoder | 4 | Decided: AS5047 on wheel output shaft (ADR-0008) | Read over SPI as absolute angle; good to ≥ 3,700 rpm (peak wheel speed at 13.5:1) with margin | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-10 |
| E-23 | Suspension angle sensor | 4 | Decided: contactless magnetic (AS5047 family) at each suspension pivot, replacing potentiometers | Covers full suspension travel; no wiper noise or wear; ≥ 2 kHz | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-18 |
| E-24 | Steering knuckle encoders | 2 (front left + front right) | Decided: AS5047, 14-bit absolute | One on each front knuckle (no homing): both wheel steer angles measured directly, toe change under load, real Ackermann geometry; absolute reference for the steering actuator at startup and a continuous cross-check against the actuator's own angle (ADR-0019) | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-19, SYS-24 |
| E-25 | Ground-speed sensor: downward event camera | 1 | Decided: Prophesee GenX320 (ADR-0014) | Forward and sideways ground velocity at 0–12 m/s on the school floor; ~90° M12 lens, aperture ~f/8 for speckle; mounted inside the chassis looking through a floor window with a light shroud; scale from E-26 | Carrier CSI port 4; trigger/sync from E-30 | 3.3 V / 5 V | SYS-10, RSK-07 |
| E-29 | Ground-speed illumination | 1 set | Decided (ADR-0014) | Three switchable modes: (1) cross-polarized LED ring, constant-current DC (no PWM); (2) grazing-angle LEDs; (3) off-the-shelf **certified IEC 60825-1 Class 1** IR VCSEL module with a hardware current limit, for laser speckle | E-30 GPIO (mode select) | 5 V via current-limited drivers | SYS-10, RSK-07 |
| E-26 | Ride-height ToF sensor | 5 (4 corners + 1 beside E-25 for scale) | TBD: ToF (ADR-0007); laser triangulation considered and rejected on cost | Minimum range ≤ sensor-to-floor distance at full compression (recess the sensor if needed); works on glossy tile; individually addressable (I2C mux or one XSHUT line each) | E-30 (I2C) | 3.3 V | SYS-10, SYS-15, RSK-07 |
| E-27 | *Deleted:* steering servo current sense. Steering torque comes from the steering controller's current (ADR-0019) | — | — | — | — | — | — |
| E-28 | Motor temperature NTC | 4 | Decided (spec) | Glued to motor can; matches VESC motor-temp input curve | E-50 | — | SYS-19 |

**E-25 selection** is recorded in ADR-0014, including the rejected candidates (PMW3901 0.58 m/s, PAA5100JE 1.14 m/s, SparkFun OTOS 2.5 m/s, mouse chips, radar, Correvit). Fallback: the dead-wheel pod design in ADR-0014, with its underfloor space and sync-board channels reserved.

## Sync and control

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-30 | Sync MCU board | 1 | Decided: STM32H723 (3 FDCAN, Ethernet MAC) + PHY (ADR-0011), firmware on Zephyr (ADR-0017); SO-8 CAN FD transceivers (SIC drop-in), unpopulated rework footprints, MCP2518FD + CAN FD transceiver populated for the dedicated steering bus (ADR-0019) | Owns the time base; timestamps every input at the pin; enforces the safety envelope and heartbeat watchdog in firmware, outside Linux; I/O per the tally below, plus one spare per channel type | E-06, E-02, all vehicle-state sensors, E-50 (CAN), E-60 | 3.3 V / 5 V | SYS-04, SYS-07, SYS-19, SYS-22 |
| E-31 | Steering actuator | 1 | Decided: mjbots moteus-c1 ($69) + gimbal-style brushless motor + ~4–6:1 belt (ADR-0019). Fallback: integrated actuator, CubeMars AK60-6 class | Lock to lock ≤ 0.1 s; ≥ 10 Hz position bandwidth; ≥ 1.5 N·m (TBC); backlash ≤ 0.1°; angle and torque at ≥ 1 kHz; absolute angle from E-24; e-stop: sync MCU commands rate-limited return to centre; controller timeout mode decelerate-and-hold; power cut at T_drive + ~1 s | Dedicated CAN-FD steering bus (sync board MCP2518FD); switched power-board output | Motor bus via switched output | SYS-06, SYS-08, SYS-10, SYS-22 |
| E-32 | Physical e-stop button | 1 | TBD | Latching; reachable on the car; drives ESTOP_EN low on all four VESCs in hardware | E-41 e-stop circuit | — | SYS-05 |
| E-33 | Status LEDs | TBD | TBD | Show e-stop state, watchdog state, power-rail faults | E-30 GPIO | 3.3 V | SYS-19 |

## Drive

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-50 | Motor controller | 4 | Decided: fork of VESC 6 open hardware (ADR-0006) | Single motor; classic CAN at 1 Mbit/s (ADR-0009); ESTOP_EN: GPIO read for the firmware brake ramp, plus an on-board hardware delay (T ≈ 3 s TBC) into DRV8301 EN_GATE, fail-safe pull-down (ADR-0012, ICD-corner-connector); command timeout; motor and FET temperature logged; phase current ≥ motor peak (TBD). Fork base: VESC 6.4. Quantity 4 corners (steering uses a moteus-c1, ADR-0019). Commands on built-in CAN + TJA1051T/3; telemetry on an added MCP2518FD controller (SPI) + 40 MHz crystal + separate SO-8 CAN FD transceiver (SIC drop-in: TJA1462 / TCAN1462 class), full status at ≥ 1 kHz; FD pair daisy-chained through the board; unpopulated split termination, common-mode choke and TVS footprints (ADR-0011, ADR-0013) | E-51, E-28, command bus, FD telemetry bus, ESTOP_EN | Motor bus | SYS-01, SYS-05, SYS-13 |
| E-51 | Drive motor | 4 | Decided: Castle 1010-4400kV sensored | 2S–4S; Hall sensored; 28 mm can | E-50 | — | SYS-01 |

## Power

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-40 | Battery pack | 1 per run | TBD: 4S LiPo, 1/10 hardcase | ≥ 50C; peak ~120 A; capacity giving ≥ 10 min hard driving; fits the battery bay; connectors per ICD-battery-pack | E-42, E-41 | — | SYS-03, SYS-16 |
| E-41 | Central power board | 1 | Custom | Rails below; per-rail fuse, switch and current sense; battery V and I monitor; e-stop circuit sourcing ESTOP_EN; ideal-diode OR-ing of wall and battery; all regulators rated ≥ 30 V input with TVS (RSK-06) | E-40, E-44, all low-voltage loads | — | SYS-05, SYS-14, SYS-19 |
| E-42 | Loop key / anti-spark switch | 1 | TBD | Rated for peak pack current | E-40 → motor bus | Motor bus | SYS-05 |
| E-43 | Bus clamp board | 1 | Custom | Comparator + MOSFET + power resistor; threshold just above 16.8 V (TBD); resistor and heatsink sized for worst-case regen from all four corners | Motor bus | Motor bus | SYS-14 |
| E-44 | Wall supply | 1 | Planned: 19 V brick | Above full pack voltage, so OR-ing selects it; power ≥ total low-voltage peak load | E-41 | — | SYS-14 |
| E-45 | Motor distribution | 1 | TBD: bus bar or heavy wire | Carries peak pack current; XT60 to each corner (ICD-corner-connector) | E-42 → E-50 ×4 | Motor bus | SYS-01 |
| E-46 | Per-cell battery monitor | 1 | TBD (EE): on the power board | Measures each of the 4 cell voltages through the pack's balance lead: ≥ 100 Hz (TBC), ±10 mV; fused, high-impedance taps with negligible drain when the car is off; data to the sync MCU over the stacking header so it's timestamped and logged; lowest cell drives the low-battery warning and shutdown | Pack balance connector (ICD-battery-pack); E-41; E-30 | 3.3 V | SYS-19, SYS-30, SYS-32 |

### Low-voltage rails (E-41)

| Rail | Feeds | Requirement |
| --- | --- | --- |
| Compute | E-01, E-02, E-03 (and E-04, E-05 via the carrier) | Fixed regulated voltage within the carrier's input range; current for NX Super mode plus carrier, NVMe and Wi-Fi |
| Sensor | E-10 (and E-06 if powered here) | Voltage per the Hokuyo datasheet |
| 5 V / 3.3 V | Cameras, E-30, vehicle-state sensors, E-29 | Low noise for the IMUs and ADC reference |

Rail currents stay `TBD` until `budgets/power.csv` exists.

## Sync MCU I/O tally (E-30)

The sync board's channel counts, from the BOM above, plus the one-spare-per-type rule.

| Interface | v1 users | Count | + Spare |
| --- | --- | --- | --- |
| SPI devices (chip-selects) | 2 IMUs, 4 wheel encoders, 4 suspension angle sensors, 2 steering knuckle encoders | 12 | ≥ 1 |
| Interrupt inputs (data-ready) | 2 IMUs | 2 | ≥ 1 |
| ADC channels | Battery V, battery I | 2 | ≥ 1 |
| I2C | 5 ride-height ToF (via mux or XSHUT) | 1 bus + mux, or 5 XSHUT GPIO | ≥ 1 |
| Camera trigger outputs | Stereo kit, quad kit, forward event camera, downward event camera | 4 | ≥ 1 |
| PPS / time-sync output | To Orin | 1 | — |
| PWM output | None (steering moved to CAN, ADR-0019) | 0 | ≥ 1 |
| CAN | Command bus (classic, 4 VESCs); telemetry front and rear (CAN-FD, 2 VESCs each); steering bus (CAN-FD, moteus-c1) | 3 FDCAN + 1 MCP2518FD (all used) | Another bus needs another external controller |
| UART | Debug console | 1 | ≥ 1 |
| Ethernet | Link to Orin via switch | 1 | — |
| GPIO | E-stop status in, status LEDs, ground-speed lighting mode select (3) | TBD + 3 | ≥ 2 |

## Ground equipment

Not on the car, but part of the system: its configuration (DHCP options, WMM priorities, fixed addresses, the laptop gateway script) goes in version control.

| ID | Item | Qty | Selection | Requirements | Traces to |
| --- | --- | --- | --- | --- | --- |
| G-01 | Team router | 1 | Decided: GL.iNet Flint 3 (GL-BE9300), $209.99 (ADR-0015) | Wi-Fi 6 GHz band for the car link; OpenWrt (per-host DHCP options, WMM); ≥ 2 wired ports for laptops; USB tethering for phone internet backup; mounted high at the track | SYS-11, SYS-26, SYS-27, SYS-28 |
| G-02 | Travel router | 0 (later) | Optional: GL.iNet Slate 7 (GL-BE3600), $159.99 | USB-C powered for away events | SYS-27 |
| G-03 | Ground-station laptop | 1+ | Team laptops | Ethernet port (or USB-Ethernet); Linux or macOS for the gateway script | SYS-26, SYS-28 |

