# ARC 4WD Car: Architecture Overview (v1)

Rewritten 2026-10-03 from the original interface spec (Sep 23, 2026, @shrikar vempati). This is the narrative overview. The decisions and numbers live in `requirements/`, `adr/`, `icd/`, `bom/` and `budgets/`; where this page and those disagree, those win. The diagram version is the topology page (`topology.html`).

## What the car is

A 1/10-scale 4WD autonomy research platform: a sandbox where the team's engineers try ideas in software without building hardware first. Classical pipelines come first (state estimation, localization, planning, MPC), then learned and embodied-AI policies (VLAs). Hardware should never be the reason an experiment fails, and should never limit what software can try. Every sensor runs at its full useful rate, timestamped and logged (SYS-24). The places where v1 hardware deliberately limits software are listed as accepted limits LIM-01 to LIM-08 in `requirements/system.md`.

v1 is tightly integrated; modular hardware is a v2 goal. The car runs on an indoor school floor and is sized to the F1TENTH/Roboracer box (width 238–341 mm, length 454–654 mm), without following their single-motor, power-equivalence or head-to-head rules.

| Area | v1 | Decided in |
| --- | --- | --- |
| Drive | 4× Castle 1010-4400kV, one per wheel, two-stage gearbox, CVDs | Original spec |
| Motor control | 4× custom VESC 6.4 fork | ADR-0006, ADR-0011, ADR-0013 |
| Steering | Gimbal brushless motor + belt, driven by a moteus-c1 | ADR-0019 |
| Compute | Jetson Orin NX 16GB on a fork of Antmicro's baseboard; JetPack 7.2.1, ROS 2 Jazzy | ADR-0010, ADR-0016 |
| Sync and sensor hub | STM32H723 on Zephyr: owns the time base, safety envelope and watchdog | ADR-0011, ADR-0017, ADR-0021 |
| Power | 4S LiPo; power board with LV rails, monitoring and e-stop circuit; bus clamp | ADR-0012, `budgets/power-scenarios.md` |
| Ground link | Team router (GL.iNet Flint 3, 6 GHz), laptop gateway, button hotspot, service port | ADR-0015 |

## Drive

**Corners.** Each corner is a motor, gearbox and VESC; there's no separate corner module or corner sensor board (ADR-0006). Wheel encoders and suspension sensors wire directly to the sync board (ADR-0008).

**Motor:** [Castle 1010-4400kV](https://www.powerhobby.com/products/castle-creations-060-0098-00-4-pole-sensored-brushless-motor-1010-4400kv), sensored, 2S–4S.

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

- Stage 1 (swappable): hardened steel pinion into an off-the-shelf spur, mod 0.5, constant 80 total teeth, so every pair uses the same bearing bores and mesh.
- Stage 2 (fixed): about 4.5:1 to the wheel, steel spur gears (around 12/54). Not printed; this stage carries the highest torque. A belt is the fallback only if the motor stagger offset gets too large for gears.
- Bearing bores are held precisely (aluminium plate or press-fit inserts), not raw printed holes.

| Pinion/spur | Stage 1 | Overall | Approx. top speed |
| --- | --- | --- | --- |
| 14/66 | 4.71:1 | 21.2:1 | 8 m/s |
| 16/64 | 4.00:1 | 18.0:1 | 9.5 m/s |
| 18/62 | 3.44:1 | 15.5:1 | 11 m/s |
| 20/60 | 3.00:1 | 13.5:1 | 12.5 m/s |

Assumes 4S, about 50,000 rpm loaded and a 65 mm tire. Peak wheel speed at 13.5:1 is about 3,700 rpm. Front and rear may run different ratios on purpose. SYS-01 requires ≥ 12 m/s.

**Motor control:** a custom fork of the VESC 6.4 (STM32F405, DRV8301), MPU9150 and NRF24 not populated (ADR-0006, ADR-0011).

- Commands arrive over one classic CAN bus (1 Mbit/s) using the stock VESC path.
- Full telemetry at ≥ 1 kHz goes out over two CAN-FD buses (front pair, rear pair) through an added MCP2518FD + SO-8 transceiver, daisy-chained through the boards (ADR-0013).
- The FET side bolts to the aluminium motor mount as the heatsink. An NTC on the motor can feeds the board's motor-temperature input.
- Corner connectors: XT60 for power, and the 10-pin JST-GH signal connector in ICD-corner-connector rev D.

## Steering

A gimbal-style brushless motor drives the steering through a ~4–6:1 belt (zero backlash), controlled by an mjbots moteus-c1 on its own CAN-FD bus (ADR-0019).

- Targets: lock to lock ≤ 0.1 s, ≥ 10 Hz bandwidth, ≥ 1.5 N·m, ≤ 0.1° backlash, angle and torque at ≥ 1 kHz.
- Absolute AS5047 encoders on **both** front knuckles give the real wheel angles, and are the actuator's absolute reference.
- Fallback actuator: an integrated CubeMars AK60-6-class unit.

## Power

4S LiPo hardcase, charged off the car at the team's charging station (a separate system; the pack is the interface, ICD-battery-pack).

- **Motor path:** battery → loop key / anti-spark → bus bar → four XT60 corners, plus the switched steering output. The bus clamp board burns off regen spikes above full-pack voltage.
- **Power board:** LV rails (compute, sensor, 5 V / 3.3 V), with per-rail fuse, switch and current sense. It also carries:
  - battery V/I monitoring and **per-cell monitoring** through the balance lead (SYS-32)
  - wall (19 V) / battery ideal-diode OR-ing
  - regulators rated ≥ 30 V with TVS
  - the e-stop circuit
- **Low battery:** the lowest cell triggers a warning, then a clean Orin shutdown before cutoff (SYS-30). Voltage sag at peak current must never reset anything (SYS-31).
- **Sizing inputs:** `budgets/power.csv` and `budgets/power-scenarios.md`. The tires limit useful peak current to roughly 40–45 A, not the old 120 A figure. Rail sizing is the EE's.

## Compute and software

- **Orin NX 16GB** on a fork of Antmicro's open baseboard: four 22-pin CSI ports, a trigger connector to the sync board, a fixed compute-rail input, M.2 key M (swappable 4 TB NVMe) and key E (MT7922 Wi-Fi), GbE, and expansion GPIO for the hotspot button. Optional Intel i210/i226 NIC if a PCIe lane is free (ADR-0021).
- **JetPack 7.2.1** (Ubuntu 24.04, kernel 6.8), **ROS 2 Jazzy**, **PREEMPT_RT**. Platform software runs on reserved cores; experiments run in containers with resource limits (ADR-0016, SYS-21).
- **Layered control:** learned policies send targets to the classical controller. The safety envelope is enforced on the sync MCU (SYS-22).
- **Bench reference:** the Traxxas car's Orin Nano dev kit takes the NX module, so cameras and software come up there first.

## Sync and sensing

The sync MCU (STM32H723, Zephyr) owns the car's time base. It timestamps sensors at the pin, triggers the cameras, runs the heartbeat watchdog and safety envelope, and drives four CAN buses: command, two telemetry, steering. µs-critical sensors are timestamped in its clock; the Orin follows in software to ≤ 1 ms (ADR-0021, SYS-07 ≤ 10 µs).

| Sensor | Part | Rate | Connects to |
| --- | --- | --- | --- |
| 2D LiDAR | Hokuyo UST-10LX (10 m, 0.25°) | 40 Hz | Ethernet switch |
| Forward stereo | Arducam AR0234 (global shutter) | Native | CSI 1, triggered |
| Side/rear cameras | Arducam quad OV9281 | Native | CSI 2, triggered |
| Forward event camera | Prophesee GenX320 | Continuous | CSI 3, Trigger In |
| Ground-speed camera | Second GenX320, pointing down, three lighting modes (cross-polarized LED, grazing LED, Class 1 laser speckle) | Continuous | CSI 4, Trigger In (ADR-0014) |
| IMU at CG | Navigation-grade MEMS (ADIS1650x / SCHA63T class) | ≥ 4 kHz | Sync SPI |
| IMU, front axle | Consumer grade | ≥ 4 kHz | Sync SPI |
| Wheel encoders ×4 | AS5047 on the wheel output shaft | 2 kHz | Sync SPI |
| Suspension angle ×4 | AS5047-family, contactless | 2 kHz | Sync SPI |
| Steering knuckle ×2 | AS5047 | 2 kHz | Sync SPI |
| Ride height ×5 | ToF (4 corners + 1 for ground-speed scale) | Native | Sync I2C + mux |
| Cell voltages ×4 | Power-board cell monitor | ≥ 100 Hz | Stacking header |
| Motor/FET temperature, currents | VESC telemetry | ≥ 1 kHz | CAN-FD |

Latency from sample to the Orin is ≤ 2 ms for sync-board sensors and VESC telemetry (SYS-29, `budgets/latency.csv`).

## Communication

Wi-Fi is the only wireless link (ADR-0005). The car joins the team's Flint 3 router on 6 GHz. The operator laptop is wired to the router and keeps its own Wi-Fi for internet. Only the car gets a gateway, through the laptop or the router's phone tether. A button on the car turns it into its own 5 GHz hotspot, and a chassis Ethernet port always works (ADR-0015, SYS-26 to SYS-28). Teleop is a fixed-rate UDP stream with priority marking: ≤ 20 ms p95 (SYS-11).

## Stopping

| Layer | Trigger | Runs on | Behaviour |
| --- | --- | --- | --- |
| Heartbeat watchdog | Laptop heartbeat lost (~150 ms) | Sync MCU | Rolls ≤ 2 m, then the same brake ramp (SYS-04) |
| Any-corner stop | One corner silent ~5 ms | Sync MCU | All four corners brake within 20 ms (SYS-25) |
| Physical e-stop | Button, broken wire, unplugged connector | VESC firmware + hardware timer | Ramped brake (~5 m/s²), gate drive cut at ~3 s; steering returns to centre, cut ~1 s later (ADR-0012, ADR-0019) |
| Command timeout | Corners stop hearing commands | VESC / moteus firmware | Brake / hold |

## Harness

Every connection locks; every run carries a spare conductor; every wire is labelled with its schematic net name; strain relief at every board entry. JST-GH for signal, XT60 for corner power. Twisted pairs for every bus, routed away from motor phase leads. Command-bus terminators sit at the sync board and the far end of the trunk; FD buses terminate at the sync board and in a sealed pigtail at the far VESC.

## Mechanical

Mechanical design hasn't started. Constraints carried from the original spec and the decisions since:

- **Wheels:** 1/10 touring, about 64–65 mm diameter, 24–26 mm wide, 12 mm hex. Tire compound for tile is TBD.
- **Size:** F1TENTH/Roboracer box: width 238–341 mm, length 454–654 mm (SYS-02). A ~310 mm wheelbase may come in under the 454 mm minimum length (RSK-16).
- **Battery bay:** placed first in the layout, low and central, sized to a 1/10 hardcase; ~5,000 mAh 4S is the starting point.
- **Mass:** heavy parts (battery, motors, VESCs) on the lower deck. The upper deck carries a hole grid for sensors.
- **Front corners must fit:** staggered motors, both gear stages, CVDs, wheel encoders, suspension sensors, knuckle encoders and the steering actuator, at full lock and full bump (RSK-04).
- **Underfloor:** reserve a flat volume for the active-aero suction fan, plus the ground-speed camera's floor window and shroud, and the 4 corner ToF sensors. The dead-wheel pod's space is reserved as the ground-speed fallback.
- **Clear sightlines:** LiDAR with a clear 270°+ field of view and a crash guard; rigid stereo bar; positions for side/rear and event cameras; Wi-Fi antennas high and clear of carbon-filled parts.
- **Mounting:** IMU at the CG and the second IMU near the front axle, both vibration-isolated.
- **Access:** service port, hotspot button and e-stop button on the chassis.
- **Cooling:** for the Orin and the VESC FETs (FETs into the motor mounts).
- **Reserved:** an encoder pocket behind each motor for a future rear-shaft motor.

## Where to look next

- Open questions per board: `handoff/electrical.md`
- Risks: `risks.md`
- Verification: `verification/plan.md`
