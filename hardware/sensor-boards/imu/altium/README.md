# ARC-Sensor-IMU-ISM330DHCX Altium scaffold

**Status: Draft organizational scaffold, 2026-10-25. Circuit capture and layout are not complete.**

[Open ARC-Sensor-IMU-ISM330DHCX in Altium 365](https://americas-carleton-university-autonomous-robotics-carle.365.altium.com/designs/2A72933B-BA15-4B7E-AB7B-3331F4D6727C#design). Cloud schematic and PCB rendering verified 2026-10-25; desktop project opening checked, with save/compile not tested. This remains a draft scaffold with no circuitry or layout.

One shared ISM330DHCXTR design for two installed boards, at the CG/body and front axle, with separate SPI chip-select and data-ready lines (ADR-0045).

## Design inputs

[IMU source](../README.md) is the starting point. The requirements, accepted ADRs and ICDs in `systems/` remain the source of truth. Native sheets contain source and open-item notes; they contain **zero electrical components, pins, wires or net labels**. The PCB contains **zero components, nets, pads, tracks, vias or pours**.

Supply/decoupling/support BOM, connector/protection, land pattern and assembly process, mounting, calibration/thermal/vibration qualification, outline and stackup remain TBD/TBC. The eight-conductor minimum harness is provisional.

## Placeholder PCB settings

The native empty PCB template includes a **default rectangular outline (4.5 x 2.6 inches), two-layer stack and default rules**. These are software-template placeholders only: **they are not chosen ARC dimensions, stackup, copper specifications, clearance limits or released design rules**. Replace them after the relevant mechanical, thermal, electrical and interface reviews. No fabrication or assembly output is provided or approved.

## Native documents

| File | Purpose |
| --- | --- |
| `IMU-Sensor.SchDoc` | Draft schematic sheet |
| `ARC-Sensor-IMU-ISM330DHCX.PcbDoc` | Draft PCB placeholder |

Open `ARC-Sensor-IMU-ISM330DHCX.PrjPcb` in Altium Designer. These files are source snapshots of the active ARC cloud project and retain its managed-project/workspace identity. `altium-sync.json` records the cloud URL and imported file hashes. The project has been opened in Altium Designer; desktop save/compile has not been tested. Review the native documents before continuing circuit capture; opening and rendering do not approve circuit or layout content.

## Template provenance and validation

The empty native source files are `Sheet1.SchDoc` and `PCB1.PcbDoc` from Altium's released project template, as cached locally by Altium Designer. The underlying Altium titleblock/embedded-logo graphics remain template assets; their presence does not imply Altium endorsement. They are not ARC-designed circuitry.

Initial scaffold creation assigned fresh schematic document/object IDs, ARC draft/title parameters and visible source/TBD notes, and cleared cached server-template associations. The imported managed project now retains its active ARC cloud identity. Embedded image data and other opaque streams are preserved. Each PCB receives a fresh document ID and file GUID, with its stored filename updated; its default board settings remain untouched apart from identity/filename.

Compound-file streams were reparsed after writing and compared with the intended streams. All project references resolve. Electrical-object counts were checked as zero. Desktop save/compile remains a separate acceptance check; project opening and cloud schematic/PCB rendering were checked as recorded above. Scaffold creation did not change systems engineering decisions, approved pinouts or released layouts. The deferred carrier reference CAD was removed separately; its provenance and historical audit remain in `hardware/orin-carrier/README.md`.

Before ordering, complete circuit capture, component and mechanical choices, verification and the systems fab gate. Safety changes require their own tests and a second reviewer.
