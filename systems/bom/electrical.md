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
| E-05 | Wi-Fi card + antennas | 1 | Planned: Wi-Fi 6/6E | M.2 key E; 5/6 GHz; antennas mounted high, away from motors and carbon-filled parts | Carrier | Compute | SYS-11, RSK-09 |
| E-06 | Ethernet switch | 1 | TBD (off-the-shelf in v1) | ≥ 4 GbE ports (Orin, LiDAR, sync MCU, spare); hardware PTP if the PTP time-sync ADR goes that way | E-02, E-10, E-30 | 5 V or sensor rail (TBC) | SYS-07 |
| E-07 | RC receiver (ExpressLRS) | 1 | Pending ADR (reopens ADR-0005) | Low-latency manual control and independent remote kill; UART (CRSF) to sync MCU | E-30 | 5 V | SYS-04, SYS-11 |

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
| E-20 | IMU, at CG | 1 | TBD | SPI; output rate ≥ SYS-06 rate; gyro ≥ ±2000 °/s and accel ≥ ±16 g (TBC, covers crashes); data-ready interrupt so samples timestamp at the pin; vibration-isolated mount | E-30 (SPI + interrupt) | 3.3 V | SYS-06, SYS-07 |
| E-21 | IMU, front axle | 1 | TBD (same part as E-20) | As E-20 | E-30 (SPI + interrupt) | 3.3 V | SYS-06, mission 4 |
| E-22 | Wheel encoder | 4 | Decided: AS5047 on wheel output shaft (ADR-0008) | Read over SPI as absolute angle; good to ≥ 3,700 rpm (peak wheel speed at 13.5:1) with margin | E-30 (SPI, one chip-select each) | 3.3 V | SYS-06, SYS-10 |
| E-23 | Suspension potentiometer | 4 | TBD | Full suspension travel within the electrical angle; ratiometric to the sync board's ADC reference | E-30 (ADC) | ADC reference | SYS-06 |
| E-24 | Steering encoder | 1 | TBD | On the steering knuckle; resolution ≤ `TBD`°; absolute (no homing) | E-30 (SPI or encoder input) | 3.3 V | SYS-06 |
| E-25 | Optical ground-speed sensor | 1 | TBD | Tracks at ≥ SYS-01 top speed **at its actual mounting distance**, on glossy tile. See note below | E-30 | 3.3 V | SYS-10, RSK-07 |
| E-26 | Ride-height ToF sensor | 5 (4 corners + 1 at E-25) | TBD (ADR-0007) | Minimum range ≤ sensor-to-floor distance at full compression (recess the sensor if needed); works on glossy tile; individually addressable (I2C mux or one XSHUT line each) | E-30 (I2C) | 3.3 V | SYS-10, SYS-15, RSK-07 |
| E-27 | Steering servo current sense | 1 | TBD (on power board servo rail) | Bandwidth high enough to see self-aligning torque changes (TBD) | E-30 (ADC) or E-41 monitor | — | SYS-10, mission 4 |
| E-28 | Motor temperature NTC | 4 | Decided (spec) | Glued to motor can; matches VESC motor-temp input curve | E-50 | — | SYS-19 |

**Note on E-25.** Common optical-flow modules are far too slow: the PMW3901 is rated 7.4 rad/s with an 80 mm minimum distance, so 0.58 m/s at 80 mm. That's an order of magnitude short of SYS-01. Gaming mouse sensors handle the speed but need a fixed working distance of a few millimetres, which suspension travel breaks. No candidate is known yet; RSK-07 is now a selection problem, not just a tile test.

## Sync and control

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-30 | Sync MCU board | 1 | Decided: STM32H7 class with Ethernet MAC + PHY | Owns the time base; timestamps every input at the pin; enforces the safety envelope and heartbeat watchdog in firmware, outside Linux; I/O per the tally below, plus one spare per channel type | E-06, E-02, all vehicle-state sensors, E-50 (CAN), E-60 | 3.3 V / 5 V | SYS-04, SYS-07, SYS-19, SYS-22 |
| E-31 | Steering servo | 1 | TBD | Torque and speed TBD from steering geometry; signal driven by E-30 so the watchdog can act on it | E-30 (PWM) | Servo | SYS-01, SYS-22 |
| E-32 | Physical e-stop button | 1 | TBD | Latching; reachable on the car; drives ESTOP_EN low on all four VESCs in hardware | E-41 e-stop circuit | — | SYS-05 |
| E-33 | Status LEDs | TBD | TBD | Show e-stop state, watchdog state, power-rail faults | E-30 GPIO | 3.3 V | SYS-19 |

## Drive

| ID | Item | Qty | Selection | Requirements | Connects to | Rail | Traces to |
| --- | --- | --- | --- | --- | --- | --- | --- |
| E-50 | Motor controller | 4 | Decided: fork of VESC 6 open hardware (ADR-0006) | Single motor; classic CAN at 1 Mbit/s (ADR-0009); hardware ESTOP_EN into gate-driver enable, fail-safe pull-down (ICD-corner-connector); command timeout; motor and FET temperature logged; phase current ≥ motor peak (TBD) | E-51, E-28, CAN trunk, ESTOP_EN | Motor bus | SYS-01, SYS-05, SYS-13 |
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

### Low-voltage rails (E-41)

| Rail | Feeds | Requirement |
| --- | --- | --- |
| Compute | E-01, E-02, E-03 (and E-04, E-05 via the carrier) | Fixed regulated voltage within the carrier's input range; current for NX Super mode plus carrier, NVMe and Wi-Fi |
| Sensor | E-10 (and E-06 if powered here) | Voltage per the Hokuyo datasheet |
| 5 V / 3.3 V | Cameras, E-30, vehicle-state sensors, E-07 | Low noise for the IMUs and ADC reference |
| Servo | E-31 only | Voltage per servo; isolated so servo stall current can't brown out logic |

Rail currents stay `TBD` until `budgets/power.csv` exists.

## Sync MCU I/O tally (E-30)

The sync board's channel counts, from the BOM above, plus the one-spare-per-type rule.

| Interface | v1 users | Count | + Spare |
| --- | --- | --- | --- |
| SPI devices (chip-selects) | 2 IMUs, 4 wheel encoders, steering encoder (if SPI) | 7 | ≥ 1 |
| Interrupt inputs (data-ready) | 2 IMUs | 2 | ≥ 1 |
| ADC channels | 4 pots, servo current, battery V, battery I | 7 | ≥ 1 |
| I2C | 5 ride-height ToF (via mux or XSHUT) | 1 bus + mux, or 5 XSHUT GPIO | ≥ 1 |
| Camera trigger outputs | Stereo kit, quad kit, event camera | 3 | ≥ 1 |
| PPS / time-sync output | To Orin | 1 | — |
| PWM output | Steering servo | 1 | ≥ 1 |
| CAN (classic) | 4 VESCs | 1 bus | Second FD-capable controller unused |
| UART | ExpressLRS receiver (pending), optical flow sensor (if UART) | 1–2 | ≥ 1 |
| Ethernet | Link to Orin via switch | 1 | — |
| GPIO | E-stop status in, status LEDs | TBD | ≥ 2 |
