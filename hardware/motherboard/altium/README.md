# ARC-Motherboard Altium scaffold

**Status: Draft organizational scaffold, 2026-10-25. Circuit capture and layout are not complete.**

[Open ARC-Motherboard in Altium 365](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/435E9B62-9567-4537-918E-7E4772E07F96#design). Cloud schematic and PCB rendering verified 2026-10-25; desktop project opening checked, with save/compile not tested. This remains a draft scaffold with no circuitry or layout.

One combined power/sync PCB (E-30, E-41 and E-46), installed in both v1.0 and v1.1. The full v1.1 interface set is reserved from the start (ADR-0042, ADR-0046).

## Design inputs

[Electrical handoff](../../../systems/handoff/electrical.md) is the starting point. The requirements, accepted ADRs and ICDs in `systems/` remain the source of truth. Native sheets contain source and open-item notes; they contain **zero electrical components, pins, wires or net labels**. The PCB contains **zero components, nets, pads, tracks, vias or pours**.

Grounding/isolation, power-stage parts and ratings, e-stop driver/switch/timing, clamp interface, monitoring transport, shutdown acknowledgement, MCU pins/DMA/clocks, connectors, board envelope and copper stack remain TBD/TBC.

## Placeholder PCB settings

The native empty PCB template includes a **default rectangular outline (4.5 x 2.6 inches), two-layer stack and default rules**. These are software-template placeholders only: **they are not chosen ARC dimensions, stackup, copper specifications, clearance limits or released design rules**. Replace them after the relevant mechanical, thermal, electrical and interface reviews. No fabrication or assembly output is provided or approved.

## Native documents

| File | Purpose |
| --- | --- |
| `00-System-Overview.SchDoc` | Draft schematic sheet |
| `01-Input-Protection-ORing.SchDoc` | Draft schematic sheet |
| `02-Power-Rails.SchDoc` | Draft schematic sheet |
| `03-Drive-Steering-Power.SchDoc` | Draft schematic sheet |
| `04-EStop-Watchdog.SchDoc` | Draft schematic sheet |
| `05-Battery-Power-Monitoring.SchDoc` | Draft schematic sheet |
| `06-Compute-Shutdown.SchDoc` | Draft schematic sheet |
| `07-Sync-MCU.SchDoc` | Draft schematic sheet |
| `08-Ethernet.SchDoc` | Draft schematic sheet |
| `09-Drive-CAN-UART.SchDoc` | Draft schematic sheet |
| `10-Steering-CANFD.SchDoc` | Draft schematic sheet |
| `11-Angle-IMU-Interfaces.SchDoc` | Draft schematic sheet |
| `12-ToF-Interfaces.SchDoc` | Draft schematic sheet |
| `13-Camera-Carrier-Sync.SchDoc` | Draft schematic sheet |
| `14-Lighting-Status-Controls.SchDoc` | Draft schematic sheet |
| `ARC-Motherboard.PcbDoc` | Draft PCB placeholder |

Open `ARC-Motherboard.PrjPcb` in Altium Designer. These files are source snapshots of the active ARC cloud project and retain its managed-project/workspace identity. `altium-sync.json` records the cloud URL and imported file hashes. The project has been opened in Altium Designer; desktop save/compile has not been tested. Review the native documents before continuing circuit capture; opening and rendering do not approve circuit or layout content.

## Template provenance and validation

The empty native source files are `Sheet1.SchDoc` and `PCB1.PcbDoc` from Altium's released project template, as cached locally by Altium Designer. The underlying Altium titleblock/embedded-logo graphics remain template assets; their presence does not imply Altium endorsement. They are not ARC-designed circuitry.

Initial scaffold creation assigned fresh schematic document/object IDs, ARC draft/title parameters and visible source/TBD notes, and cleared cached server-template associations. The imported managed project now retains its active ARC cloud identity. Embedded image data and other opaque streams are preserved. Each PCB receives a fresh document ID and file GUID, with its stored filename updated; its default board settings remain untouched apart from identity/filename.

Compound-file streams were reparsed after writing and compared with the intended streams. All project references resolve. Electrical-object counts were checked as zero. Desktop save/compile remains a separate acceptance check; project opening and cloud schematic/PCB rendering were checked as recorded above. Scaffold creation did not change systems engineering decisions, approved pinouts or released layouts. The deferred carrier reference CAD was removed separately; its provenance and historical audit remain in `hardware/orin-carrier/README.md`.

Before ordering, complete circuit capture, component and mechanical choices, verification and the systems fab gate. Safety changes require their own tests and a second reviewer.
