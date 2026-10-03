# ARC 4WD Car — Interface Spec

Sep 23, 2026 · @shrikar vempati

> **Partly superseded.** ADR-0006 to ADR-0014 drop the corner module, corner sensor node and tire temperature for v1, move the corner encoders and pots to the sync board, put VESC commands on one classic CAN bus with telemetry on two CAN-FD buses, make the e-stop a controlled braked stop, and use a downward event camera as the ground-speed sensor. Where this overview disagrees with `systems/`, `systems/` wins.

## Purpose and roadmap

Every interface below stays fixed from v1 onward, so each upgrade is a swap, not a redesign. v1 builds the cheap version of each subsystem; the interfaces are sized for the later ones.

| Subsystem | v1 (build now) | Planned upgrade |
| --- | --- | --- |
| Drive | 4× 28 mm sensored motors (Hall), plus an AS5047 encoder on each wheel output shaft | Motor with a rear shaft, so an encoder can also drive commutation |
| Motor control | Custom single-motor VESC 6 derived boards, one per corner | Shrunk, mount-cooled revision |
| Compute | Jetson Orin NX 16GB on a custom carrier forked from Antmicro's open baseboard | Split carrier: fixed I/O baseboard plus swappable compute adapter |
| Power + sync | Power board and sync MCU board, stacked on a header | Merged board; later sync merged onto the carrier |
| Perception | 2D LiDAR, cameras, vehicle-state sensors | Event camera, more cameras |
| Aero | None | Active aero (capstone) |

## Corner module

All four corners are identical, self-contained modules: motor, gearbox, controller board and wheel drive, joined to the car by one mount pattern and a fixed connector set (power, plus a 6-pin signal connector with both CAN buses and the sync line).

**Motor (v1):** [Castle 1010-4400kV](https://www.powerhobby.com/products/castle-creations-060-0098-00-4-pole-sensored-brushless-motor-1010-4400kv), sensored, 2S–4S rated.

| Motor interface | Value |
| --- | --- |
| Can size | 28 mm dia × 58.4 mm |
| Mass | 146.5 g with wires |
| Shaft | 3.175 mm (1/8"), 15 mm long |
| Mount holes | M3 on 19 mm (also M2.5 on 14 and 16 mm) |
| Max speed | 80,000 rpm |

The motor mount clamps a 28 mm can on this bolt pattern, so any 28 mm motor drops in.

**Layout:** motors transverse (parallel to the axle), inboard, staggered fore and aft on each axle so two motors and gearboxes fit the width. Output reaches the wheel through a CVD.

**Gearbox:** two stages per corner.

- Stage 1 (swappable): hardened steel pinion into an off-the-shelf spur, mod 0.5, constant 80 total teeth so every pair uses the same bearing bores and mesh.
- Stage 2 (fixed): about 4.5:1 to the wheel, steel spur gears (around 12/54 teeth). Not printed; this stage carries the highest torque. A belt is the fallback only if the motor stagger offset gets too large for gears.
- Stage 1 bearing bores are held precisely (aluminum plate or press-fit inserts), not raw printed holes.

| Pinion/spur | Stage 1 | Overall | Approx. top speed |
| --- | --- | --- | --- |
| 14/66 | 4.71:1 | 21.2:1 | 8 m/s |
| 16/64 | 4.00:1 | 18.0:1 | 9.5 m/s |
| 18/62 | 3.44:1 | 15.5:1 | 11 m/s |
| 20/60 | 3.00:1 | 13.5:1 | 12.5 m/s |

Assumes 4S, about 50,000 rpm loaded, 65 mm tire. Front and rear may run different ratios on purpose.

**Controller board:** custom single-motor board derived from [VESC6\_OPEN\_HARDWARE](https://github.com/craigg96/VESC6_OPEN_HARDWARE) (KiCad, CC BY-SA). Target footprint about 30 × 40 mm after the stock board is validated. FET side bolts to the aluminum motor mount as the heatsink. An NTC glued to the motor can feeds the board's motor temperature input, and FET temperature is logged too.

**Wheel encoder (v1):** one AS5047 per corner, reading a diametric magnet on the end of the wheel output shaft. It measures true wheel rotation after the whole drivetrain, so it can be compared against Hall-derived speed to expose backlash, compliance and slip.

- The 1010's specs list only a front output shaft; its rear holds the Hall sensor board, so the encoder sits on a gearbox shaft instead of the motor.
- Wheel output peaks near 3,000 rpm at the fastest ratio.
- Read by the corner sensor node over a short local ABI run, not by the corner controller.
- Magnet pocket and sensor mount are part of the gearbox housing.

**Corner sensor node (v1):** a small separate board on each corner module (STM32G4 class) that reads that corner's wheel encoder (hardware quadrature timer), suspension pot (ADC), ride-height sensor and tire-temperature sensor (I2C), and sends them over a dedicated CAN-FD sensor bus. Separate from the motor controller so sensor firmware can never affect motor control. Powered by a short cable from the corner controller board.

- Timestamps every sample on a local timer, re-aligned on each pulse of the sync line (the spare pin in the CAN connector).
- Dedicated CAN-FD sensor bus, separate from the classic CAN bus the VESC-derived boards need (the two cannot share a bus). Each node sends one unbatched frame at 1 kHz with its latest samples, so corner data is at most about 1 ms old when it leaves the corner. Ride height and tire temperature ride in the same frame. Roughly 25% bus load for all four nodes at a 5 Mbit/s data rate; transceivers must be CAN-FD rated.
- Motor commands stay alone on the classic CAN bus, so sensor traffic can never delay them.

**Bus topology risk (potential fix, not yet decided):** CAN-FD at 5 Mbit/s is sensitive to ringing from star layouts and long stubs; [CiA guidance](https://jcom1939.com/can-bus-topology-and-network-design/) puts multi-drop CAN-FD around 2 Mbit/s without special measures, which would push 1 kHz from four nodes to roughly 50–60% bus load. Candidate fixes: daisy-chain trunk (sync board → FL → FR → RR → RL) terminated at both ends with transceivers right at the trunk; [SIC transceivers](https://www.can-cia.org/can-knowledge/cia-601-series-can-fd-guidelines-and-recommendations) that suppress ringing; compact frames of about 16 bytes. Validate on a bench harness with all four nodes at 1 kHz before the car is built.

**Future hook:** the AS5047 header on every corner board (SPI/ABI, next to the Hall port) stays unpopulated, for a later motor with a rear shaft whose encoder also drives commutation.

## Power

The car runs on 4S LiPo, with two separate paths: high current to the corners, and regulated low-voltage rails from the central power board.

**Motor path:** battery → loop key or anti-spark switch → bus bar or heavy wire → four corner boards. Motor current never flows through the central power or sync board.

**Bus clamp board (v1):** a small standalone board on the motor distribution bus, not on the central power board. A comparator watches bus voltage; above a threshold just over full-pack voltage (16.8 V), a MOSFET switches a heatsinked power resistor across the bus to burn off regen energy.

- Protects all four corner boards from bus spikes: full pack plus hard braking on all corners, or a battery connector bouncing loose mid-brake.
- Makes motor testing on a wall supply safe, since a normal supply cannot absorb regen.
- Open: exact clamp threshold, resistor wattage and heatsink, sized to worst-case braking power from all four corners.

**Low-voltage rails (central power board):** sized for the v3 load, not v1.

| Rail | Feeds | v1 | Sized for |
| --- | --- | --- | --- |
| Compute | Jetson module + carrier | Orin NX 16GB | Orin NX 16GB at full power + custom carrier |
| Sensor | 2D LiDAR (voltage per chosen model) | LiDAR | LiDAR + spare |
| 5 V / 3.3 V | Cameras, sync MCU, vehicle-state sensors | Initial cameras | More cameras + event camera |
| Servo | Steering servo, its own rail | Servo | Servo |

**Required features:**

- Per-rail fuse and switch, so one shorted device cannot brown out the Orin.
- Battery voltage and current monitoring, plus current sensing on every low-voltage rail (compute, sensors, logic, servo), all logged like any sensor.
- Hardware e-stop input that disables the corner boards through a relay or enable line, independent of software.

**Wall input:** the low-voltage rails run from either wall power or the battery, so bench work never cycles the LiPos.

- Ideal-diode OR-ing selects the higher source; unplugging the wall hands over to battery without rebooting the Orin.
- Wall input is a 19 V supply, above full-pack voltage, so the wall wins when plugged in.
- Every regulator on the power board is rated for at least 20 V input. The compute rail is regulated to a fixed voltage from whichever source is active, never passed straight through, because the Orin NX module accepts only 5–20 V and a 19 V brick leaves too little margin.
- Wall power feeds the low-voltage rails only; the motor path stays on the battery.
- No onboard LiPo charging. Packs are removed and charged at the team's separate charging station; the pack itself is the interface between the two systems (ICD-battery-pack).

Rail current ratings stay open until the battery and full power budget are settled. v1 builds the power board and sync board as two boards stacked on a header.

## Compute bay

v1 compute is the Jetson Orin NX 16GB, because the car is driven by onboard AI from day one. The bay is built around the Orin NX/Nano module footprint and a carrier with four CSI camera ports.

| Item | Size / interface | Price (USD, after July 2026 increase) |
| --- | --- | --- |
| Orin NX 16GB module (v1) | 69.6 × 45 mm module | $899 (1,000-unit price; single units cost more) |
| Antmicro Orin baseboard, forked as the v1 carrier | 120 × 60 mm | Open hardware |

Prices from [CNX Software](https://www.cnx-software.com/2026/07/22/nvidia-increases-the-price-of-jetson-modules-and-devkits-by-up-to-101/). Baseboard: [Antmicro](https://openhardware.antmicro.com/boards/jetson-orin-baseboard/), KiCad, Apache-2.0.

**Bay requirements:**

- Mounting envelope covers the 120 × 60 mm custom carrier plus the added camera ports.
- Height allows the heatsink plus a fan or duct; the printed enclosure has a real airflow path.
- Space and mounting for an NVMe drive for logging.
- Camera flex cable routes from the bay to the sensor deck.

**Camera ports:** the module supports up to four cameras (eight with virtual channels) over 8 CSI lanes; the carrier must break out all four ports.

| Port | Camera | Role |
| --- | --- | --- |
| 1 | [Arducam AR0234 stereo kit](https://www.arducam.com/arducam-2-3mp2-ar0234-color-global-shutter-synchronized-stereo-camera-bundle-kit-for-nvidia-jetson-agx-orin-orin-nano-orin-nx.html) (2× color global shutter, Camarray, $259.99) | Forward depth |
| 2 | Arducam quad OV9281 kit (4× 1 MP mono global shutter, Camarray) | Sides, rear, one spare |
| 3 | Prophesee GenX320 event camera (2-lane MIPI) | Motion and timing |
| 4 | Spare | Future camera |

Estimated CSI load is about 3–4 Gbps total, well inside the link, so compute and cooling are the real limits.

**v1 carrier: fork of Antmicro's open baseboard.** The stock board has two 50-pin CSI connectors carrying up to two cameras each, at 2.5 Gbps per lane, with lane mapping set by assembly variant. Changes for this car:

- Replace the two 50-pin connectors with four 22-pin FFC ports matching the Arducam kits and the GenX320, and set the lane mapping for them.
- Route camera trigger lines to a connector for the sync board.
- Power from the power board's fixed compute rail instead of USB PD or PoE; drop PoE.
- Keep M.2 key M (NVMe) and key E, gigabit Ethernet and the expansion connector.
- Device tree and camera driver work is ours: Arducam only guarantees support on NVIDIA's dev kit.

**Bench reference:** the existing Traxxas car's Orin Nano dev kit carrier accepts the NX module, so cameras and software are brought up there first. Board bugs and driver bugs are then debugged separately.

Watch item: Jetson Thor T2000 (about 50 × 87 mm, 16 GB) is due Q1 2027 with no dev kit or public price yet. It is a different module, so it would need a new carrier.

## Sync and sensor hub

One MCU owns the car's time base: it triggers the cameras, timestamps every vehicle-state signal at the pin, and streams it all to the Orin as one synced stream.

| Channel | v1 use | Reserve spare for |
| --- | --- | --- |
| Camera trigger outputs | Initial cameras | Added cameras, event camera sync |
| PPS-style pulse to Orin | Time alignment | — |
| IMU bus (SPI) | IMU at the CG plus a second IMU near the front axle, high rate | Additional IMU |
| Encoder input | Steering knuckle encoder | — |
| Sync pulse output | To all 4 corner sensor nodes, via the corner CAN connectors | At least 1 more |
| ADC inputs | Battery monitor (suspension pots moved to corner nodes) | New analog sensors |
| Optical ground-speed sensor | True velocity for slip | — |
| CAN | Classic CAN to the 4 corner controllers, plus CAN-FD to the 4 corner sensor nodes (two FD-capable controllers on the MCU) | Future CAN devices |
| Link to Orin | Ethernet via an onboard switch (shared with the Hokuyo); MCU with built-in Ethernet MAC (STM32H7 class) plus PHY | — |
| GPIO | E-stop status, LEDs | Unplanned additions |
| Ride height sensors | Via corner nodes over CAN: one downward distance sensor per corner (pitch, roll, ride height) | Aero pressure sensing later |
| Tire temperature sensors | Via corner nodes over CAN: one small IR sensor per tire | — |
| Steering servo current | Servo load as a proxy for tire self-aligning torque (grip feel) | — |

Channel counts are set once the camera list is final. Rule: every row gets at least one spare connector populated on the board.

LiDAR sync is hardware-synced or software timestamp-aligned depending on the chosen model's options (check before layout). Cameras chosen must accept a hardware trigger input.

## Sensor mounting

The upper deck carries a standard rail or hole grid, so sensors move or change without reprinting the chassis. Heavy parts (battery, motors, corner boards) stay on the lower deck for a low CG.

| Sensor | Status | Mount interface |
| --- | --- | --- |
| 2D LiDAR, ≤20 m range | Hokuyo UST-10LX (decided) | Deck grid, crash guard around it, clear 270°+ field of view |
| Forward stereo pair, global shutter (AR0234 kit) | Planned | Rigid bar on the grid, fixed baseline |
| Side and rear cameras (quad OV9281 kit) | Planned | Grid positions reserved |
| Event camera | Planned, CSI port 3 | Grid position, CSI route, trigger line reserved |
| IMU | v1 | At the CG, vibration-isolated |
| Steering encoder | v1 | On the steering knuckle |
| Suspension pots | v1 | One per corner |
| Optical ground-speed sensor | v1 | Underside, facing down; check its working height against ride height, and test it on the actual tile early (glossy, uniform tile can defeat optical flow and distance sensors) |
| Wheel encoders (4× AS5047) | v1 | In each gearbox housing, on the wheel output shaft |
| Second IMU | v1 | Near the front axle, vibration-isolated |
| Ride height sensors (4×) | v1 | Underside at each corner, facing down |
| Tire temperature sensors (4×) | v1 | Aimed at each tire's tread, clear of suspension travel |
| Motor temperature (4× NTC) | v1 | Glued to each motor can, wired to its corner board |

LiDAR: [Hokuyo UST-10LX](https://www.generationrobots.com/en/401755-hokuyo-ust-10lx-scanning-laser-range-finder.html) (40 Hz, 0.25°, 10 m; chosen, Ethernet interface) over [RPLidar A3](https://www.robotshop.com/products/slamtec-rplidar-a3-360-laser-scanner-25-m) (10–20 Hz). Event camera candidate: Prophesee GenX320, with a [documented Jetson Orin NX driver](https://developer.ridgerun.com/wiki/index.php/Event_Cameras_Developer_Guide/Getting_Started/Raspberry_Pi_5_+_Prophesee_GenX320).

## Harness and connectors

Every connection locks, and every harness run carries spare conductors, because vibration kills RC connections and adding wires later is painful.

| Connection | Standard |
| --- | --- |
| Signal, sensor, CAN | JST-GH (locking) |
| Battery and motor power | XT-series |
| Corner module to car | Separate connectors: XT60-class for power, 6-pin JST-GH carrying the classic CAN pair (motor control), the CAN-FD pair (sensor node), ground and the sync pulse. Combined XT30(2+2) rejected: 20 A max and 100 mating cycles |
| Motor phases | Soldered to corner board pads |
| Corner sensors to corner node | Short local runs on the corner module, JST-GH; data leaves the corner on CAN |

- Strain relief at every board entry.
- At least one spare conductor per run to each corner and to the sensor deck.
- Every connector and wire labeled to match the schematic net names.

## Software interfaces

Swapping a motor, sensor or compute module changes a config file, never a node's code.

- **ROS 2 message types** are fixed early for each data stream: corner state (wheel speed, current, per corner), vehicle state (steering, suspension, ground speed, IMU), and each sensor.
- **Per-module config files** in version control: each corner's VESC config, including its gear ratio, which maps motor RPM to wheel speed. A ratio swap without a config update silently corrupts wheel-speed data.
- **Vehicle model** reads gear ratios and geometry from the same config, never hard-coded.
- **One time base:** the sync MCU's clock, shared with the Orin through the PPS pulse. Every message carries that timestamp.
- **Logging:** every stream recorded to NVMe as MCAP.

## Control, safety and wireless

The car's only wireless link is Wi-Fi to one laptop. The gamepad plugs into the laptop, and the laptop sends every command. There is no separate RC radio or dedicated kill link.

**Link:** Wi-Fi 6/6E card in the carrier's M.2 key E slot. Used for commands, telemetry, logs, SSH and compressed camera streams to offboard compute (the NX hardware encoder handles the streams). Never part of a fast control loop. A dedicated access point at the track; antennas high, away from motors and outside carbon-filled printed parts.

**Stop layers, each independent of the ones above it:**

| Layer | Trigger | Where it runs |
| --- | --- | --- |
| Heartbeat watchdog | Laptop heartbeats (with a sequence counter, relayed through the Orin) stop for about 100–200 ms | Sync board MCU, not Linux |
| Physical e-stop button | Pressed | Power board e-stop input, cuts corner board enable in hardware |
| Command timeout | Corner boards stop receiving commands | VESC firmware on each corner board |

A dropped Wi-Fi link, a crashed laptop and a hung Orin all end the same way: the car stops. The sequence counter stops a stuck Orin from replaying an old heartbeat.

**Manual driving:** gamepad → laptop → Wi-Fi → Orin → sync board → corners and steering servo. The steering servo signal comes from the sync board so the watchdog can act on it directly.

**Accepted tradeoffs:** Wi-Fi hiccups longer than the timeout cause false stops; Wi-Fi jitter makes manual driving less consistent, which affects human-demonstration data quality; if the sync board firmware hangs, only the button and the command timeout remain.

## Open decisions and reserved space

**Open decisions** (in the order they unblock others):

- [ ] Confirm camera models: AR0234 stereo, quad OV9281, GenX320 (sets trigger count)
- [ ] Carrier fork details: 22-pin port lane mapping for the Arducam kits and GenX320, trigger connector, compute-rail input
- [ ] Source the Hokuyo UST-10LX (Ontario seller or used) and give it an Ethernet port on the carrier or a small switch
- [ ] Battery capacity from what fits the reserved bay; C rating of 50C or more (peak draw up to about 120 A)
- [ ] Front-corner fit check in CAD: staggered motor, both gear stages, CVD and wheel encoder at full steering lock and full suspension compression
- [ ] Confirm XT60-class rating against final per-corner peak current
- [ ] Upgrade motor with a rear shaft for commutation encoders (the 1010 lists only a front shaft)
- [ ] Small Ethernet switch for the car (off-the-shelf in v1; later a switch chip on the power/sync board)

**Electrical, still open:**

- [ ] CAN-FD bus topology and transceivers, proven on a bench harness
- [ ] Heartbeat rate and exact watchdog timeout
- [ ] Wi-Fi card and antenna choice, plus the track access point
- [ ] Grounding and noise strategy: motor bus, logic, cameras, shielding
- [ ] Component picks: IMUs, optical ground-speed sensor, ride-height and tire-temperature sensors, steering encoder, steering servo, Ethernet switch
- [ ] Power board rail current ratings from the final power budget
- [ ] Bus clamp threshold and resistor sizing

**Mechanical, not yet designed:**

- [ ] Tire compound for tile (form factor decided: 1/10 touring, about 64–65 mm diameter, 24–26 mm wide, 12 mm hex hubs; target surface is indoor tile)
- [ ] Wheelbase and track: starting targets about 310 mm and about 250 mm, finalized by the CAD block layout; stay inside the assumed Traxxas Slash 4x4 footprint (324 mm wheelbase, 296 mm track) for fair head-to-head racing
- [ ] Suspension: geometry, dampers, off-the-shelf vs custom arms and uprights
- [ ] Steering: geometry, servo mount, knuckle encoder mount
- [ ] Driveshafts/CVDs and gearbox housings
- [ ] Chassis structure: decks, material, stiffness, printed vs metal parts
- [ ] Mass budget and CG
- [ ] Cooling path for the Orin and corner boards
- [ ] Crash protection for the LiDAR and cameras
- [ ] Serviceability: pulling a corner, battery swap, board access

**Reserved space** (kept empty in v1 CAD):

- Flat underfloor volume for the active-aero suction fan.
- Encoder pocket behind each motor, for a future rear-shaft motor.
- Deck grid positions for side/rear cameras and the event camera.
- Compute bay envelope for the custom carrier.
- Battery bay, placed first in the layout: low and central for CG, sized to a standard 1/10 hardcase pack. Capacity is whatever fits the bay, checked against a minimum of about 10 minutes of hard driving.
