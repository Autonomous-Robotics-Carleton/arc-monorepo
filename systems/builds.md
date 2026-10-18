# Builds

The v1 car is built twice (ADR-0046). **v1.1 is the full design** in these files. **v1.0** is built first, on compute and a LiDAR on loan from the team's existing car, with fewer cameras. This page tracks what each build contains and where its order stands; the decision and its reasons are in ADR-0046.

## What differs

| Item | v1.0 | v1.1 |
| --- | --- | --- |
| E-01 compute | Jetson Orin Nano 8 GB, the module of an Orin Nano Super developer kit, on loan | Orin NX 16 GB |
| E-02 carrier | Developer kit carrier (P3768) | Antmicro fork |
| E-03 cooling | Developer kit cooler | NX cooler |
| E-04 SSD | 1 TB | 4 TB |
| E-10 LiDAR | Hokuyo UST-20LX, on loan | Hokuyo UST-10LX |
| E-12 side/rear cameras | Not fitted | Quad OV9281 kit |
| E-13 forward event camera | Not fitted | GenX320 |
| E-25 ground-speed camera | Arducam B0223 frame camera | GenX320 |
| E-29 ground lighting | LEDs | LEDs + Class 1 laser module |

Every other BOM row is in both builds. Rows that differ carry a "v1.0" note in [bom/electrical.md](bom/electrical.md), and [budgets/cost.csv](budgets/cost.csv), [power.csv](budgets/power.csv) and [mass.csv](budgets/mass.csv) mark each line `both`, `v1.0` or `v1.1`.

## What v1.0 can't do

| Limit | Revisit |
| --- | --- |
| LIM-09: no learned policy alongside the platform stack (8 GB) | v1.1 |
| LIM-10: three cameras, no spare CSI port | v1.1 |
| LIM-11: frame-camera ground speed with LEDs, unproven above low speed | v1.1 |

## v1.0 on the developer kit

The Orin Nano Super developer kit is the Orin Nano developer kit's hardware (P3767-0005 module on the P3768 carrier) flashed for Super mode; an original kit becomes one when it's flashed that way. Everything here is v1.0 only.

| Item | On the developer kit | Source |
| --- | --- | --- |
| Power modes | 15 W, 25 W (the Super default) and MAXN_SUPER (uncapped; throttles at the module's thermal limit). Only with the `jetson-orin-nano-devkit-super` flash configuration: a kit updated to JetPack 7.2 in place keeps the profile it had, which may be the original 15 W / 7 W one, so flash it from a Linux host or SDK Manager | [Jetson Linux 39.2 power modes](https://docs.nvidia.com/jetson/archives/r39.2/DeveloperGuide/SD/PlatformPowerAndPerformance/JetsonOrinNanoSeriesJetsonOrinNxSeriesAndJetsonAgxOrinSeries.html) |
| Power input | Barrel jack, 5.5 × 2.5 mm, 9–20 V; the kit's own supply is 19 V. The motherboard's compute rail feeds it (ICD-power-rails); the jack doesn't lock (RSK-23) | NVIDIA SP-11324-001 v1.3 |
| Cameras | Two 22-pin CSI connectors: CAM1 (2 or 4 lanes) takes the stereo kit, CAM0 (2 lanes) the B0223. No spare (LIM-10) | [Dev kit hardware layout](https://docs.nvidia.com/jetson/orin-nano-devkit/user-guide/latest/hardware_layout.html) |
| Storage (E-04) | The 1 TB SSD in the M.2 key M 2280 slot (PCIe 3.0 x4); a second key M slot (2230, x2) stays free | Hardware layout |
| Wi-Fi (E-05) | The MT7922 replaces the kit's own wireless card in its M.2 key E slot | Hardware layout |
| Ethernet | The kit's GbE port to the switch (E-06) | ICD-carrier-sync |
| PPS, shutdown request, hotspot button and LED | 40-pin header GPIOs; pins and device tree TBD | ICD-carrier-sync |
| Camera triggers | From the motherboard straight to the stereo kit's and the B0223's trigger inputs, not through the carrier | ICD-carrier-sync |
| Software | JetPack 7.2.1 with the dev kit's stock board support (ADR-0016, ADR-0028) | — |

Open for v1.0:

- Which dev kit signal tells the power board the Orin has halted before it removes the compute rail (SYS-30, ADR-0022). TBD; the 40-pin header and the button header (J14) are where to look.
- Whether the kit should power on by itself when the compute rail comes up (its default; a jumper on J14 changes it).
- Swapping the SSD: the M.2 slots are on the underside of the carrier (TBC on our kit), so the compute bay needs access to it (LIM-08).
- Whether v1.0 runs in 25 W or MAXN_SUPER, and whether the classical stack meets its rates there (ADR-0046's reopen condition). Measure on the kit.

## Built in v1.0 for v1.1

- The motherboard is the v1.1 board: four camera trigger groups plus a spare, every sensor connector and chip select, and a compute rail sized for the NX, at a voltage both carriers take (the dev kit's 9–20 V and the Antmicro fork's input).
- The compute bay fits the dev kit and the Antmicro assembly (ADR-0043), with room for the NX cooler.
- Mounts for the quad and forward cameras are in the chassis from the start.

## Orders

| Build | Funding request | Ordered | Arrived |
| --- | --- | --- | --- |
| v1.0 | TBD | No | — |
| v1.1 | TBD | No | — |

When a build's design is frozen for ordering, tag it `car-v1.0` or `car-v1.1` (see [README](README.md), "Releases are tags").
