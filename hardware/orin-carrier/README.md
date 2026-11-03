# ARC deferred Orin carrier provenance and conversion audit

**CAD cleanup, 2026-10-25:** the deferred Antmicro KiCad reference and its native Altium conversion, images, templates, histories and generated outputs were removed from this checkout at the owner's request. Source provenance, licensing and the historical conversion findings remain below; tracked CAD can be recovered from Git history or the pinned upstream source.

This removes reference files, not the future design recorded in `systems/`: the custom carrier remains a v1.1 item, while v1.0 uses the Orin Nano developer kit's own carrier (ADR-0046). No ARC carrier schematic/layout is supplied here, and the historical reference had not received ARC power, four 22-pin CSI ports or sync/trigger adaptations.

The carrier remains separate from the combined [power/sync motherboard](../motherboard/README.md) under ADR-0042.

## Four-port CSI feasibility

**Checked 2026-10-09.** For SYS-08 / LIM-05, the Orin NX exposes **eight CSI data lanes and four differential clock pairs**, sufficient for **four simultaneous 2-lane physical ports**. NVIDIA's [Orin NX/Nano Product Design Guide](https://developer.nvidia.com/downloads/jetson-orin-nx-series-nano-series-design-guide), revision 1.5, chapter 10 / Table 10-3, also supports two 4-lane ports. Four 4-lane ports would require more lanes than the module exposes. These are dedicated CSI signals, not spare sync-MCU GPIO.

The following is a module-side feasibility map, **not a released camera allocation or 22-pin connector pinout**. Pin pairs are listed N/P at the module socket J15, following NVIDIA Table 10-1.

| Module CSI group | Clock pins N/P | Data lane 0 pins N/P | Data lane 1 pins N/P | Capacity |
| --- | --- | --- | --- | --- |
| CSI0 | 10 / 12 | 4 / 6 | 16 / 18 | One 2-lane port |
| CSI1 | 9 / 11 | 3 / 5 | 15 / 17 | One 2-lane port |
| CSI2 | 28 / 30 | 22 / 24 | 34 / 36 | One 2-lane port |
| CSI3 | 27 / 29 | 21 / 23 | 33 / 35 | One 2-lane port |

The historical pinned source netlist exposed all four clock pairs and eight data lanes through **two 50-pin connectors, J7/J8**, with configurable lane routing on the CSI sheet. It must be adapted to the four 22-pin connectors required by E-02; the removed Altium reference had not made that change. Follow NVIDIA's supported lane grouping and its CSI0_D1 / CSI1_D0 internal polarity-swap note; preserve the approved D-PHY routing constraints and confirm device-tree polarity settings.

**Camera allocation (ADR-0025):** CSI 1 serves the AR0234 stereo kit, CSI 2 the four OV9281 side/rear cameras, CSI 3 the forward GenX320 and CSI 4 the downward GenX320. Confirm the kits' host-side lane modes, both GenX320s' lane modes, aggregate bandwidth, trigger wiring and required JetPack drivers. A mandatory 4-lane kit would require revisiting this allocation. The stock quad kit exceeds SYS-24's 50% ceiling at its advertised full-resolution rates; four connectors do not establish a pass. Shared camera-I2C addresses, reset/reference clocks, rails and all-port capture/logging must be verified before pin freeze.

## Source

- Repository: https://github.com/antmicro/jetson-orin-baseboard
- Pinned commit: `34f0e7f5fbca441191e20ed55d93cf8614e12bed`
- Board title-block revision: `1.3.4`, dated `2025-03-12`
- License: Apache-2.0; preserved in [upstream/LICENSE](upstream/LICENSE)
- Main source project: [jetson-orin-baseboard.kicad_pro at the pinned source](https://github.com/antmicro/jetson-orin-baseboard/blob/34f0e7f5fbca441191e20ed55d93cf8614e12bed/jetson-orin-baseboard.kicad_pro)
- Main schematic: [jetson-orin-baseboard.kicad_sch at the pinned source](https://github.com/antmicro/jetson-orin-baseboard/blob/34f0e7f5fbca441191e20ed55d93cf8614e12bed/jetson-orin-baseboard.kicad_sch)
- PCB: [jetson-orin-baseboard.kicad_pcb at the pinned source](https://github.com/antmicro/jetson-orin-baseboard/blob/34f0e7f5fbca441191e20ed55d93cf8614e12bed/jetson-orin-baseboard.kicad_pcb)
- Historical file SHA-256 inventory: [upstream-lock.json](upstream-lock.json); this records the former snapshot, not files currently present

The retained [upstream overview](upstream/README.md) and [license](upstream/LICENSE) document the original reference. The overview photo links to the pinned upstream source rather than a local image. The historical source inventory retains the original hashes; its removed CAD/image paths are audit entries. Ignored `.nx/` working copies and audit exports are outside this scoped cleanup.

## Historical conversion status

**Native conversion completed on 2026-10-09; electrical equivalence failed verification.** Altium Designer Professional **26.7.1**, using the official **KiCad Importer 1.0.66.3329**, generated and saved the native project. Both its root schematic and routed PCB were opened in Altium.

The removed `altium/` tree contained the native project, **ten `.SchDoc` sheets**, one **`.PcbDoc`**, one **`.SchLib`**, 21 imported PNGs, ten schematic templates and local generated logs/history/output files. The removed `upstream/` CAD consisted of the KiCad project, ten schematic sheets, PCB, rules and library tables, plus its local photo. No local carrier project remains to open. The findings below describe the 2026-10-09 conversion and remain useful if the future carrier work resumes.

> [!WARNING]
> **The removed conversion failed electrical equivalence and was not fabrication-ready.** The compiled Altium schematic export merged **GND with +3V3**, and **USBC0_CC1 with USB2_D_N**. If restored, repair and recheck the conversion against the pinned KiCad netlist before using schematic-to-PCB updates or making ARC changes. Component counts alone did not reveal these defects.

Conversion verification found all **669 source schematic component designators** present, plus eleven KiCad power-flag designators. Repeated J15/U6 designators correspond to multipart symbols. All ten schematic files could be read as native Altium documents. The PCB had **681 components, 799 assigned nets and 1,345 vias**, matching the corresponding source counts; the source's 800 net entries include its unassigned net. Its saved copper stack retains **F.Cu, In1.Cu through In6.Cu and B.Cu**, including In4.Cu.

The compiled Protel project netlist has **640 exported nets**. The KiCad source has **791**, including **143 single-pin nets** which Altium's export settings omit. Comparing pin membership rather than net names found **580 of 648 source multi-pin nets** identical. The export also omits **246 connections belonging to source multi-pin nets**, in addition to the single-pin omissions. This records an export/comparison failure; it does not establish the cause of each difference. The full local comparison is in `.nx/ee-carrier-netlist-comparison.json`; the reproducible Altium export is `Design → Netlist For Project → Protel`.

The wizard's analysis reported **zero errors** but **489 project, 4,062 schematic and 319,940 PCB warnings**, including unsupported `uuid`, `stroke`, `exclude_from_sim`, teardrop and solder-mask settings. These counts describe parsing, not a clean compiled design. Altium also reported compiler findings for off-grid objects, unconnected pins and electrical pin/port types, plus incomplete sheet-number cross-references. The native PCB has **zero differential-pair objects and zero imported 3D models**; net classes are present, but differential pairs and rule/geometry equivalence still need review. Its aggregate track-record count includes converted drawing/fill geometry and is not a validated routed-track comparison.

If the deferred work resumes, recover the source at the pinned commit before using Altium's [KiCad Import Wizard](https://www.altium.com/documentation/altium-designer/design-tools-interfacing/kicad-import). The original settings below are historical instructions, not evidence that these local paths still exist. Restore a byte-identical source snapshot separately from any new conversion output; the previous importer expanded the project into its root schematic and PCB, then loaded all child sheets.

Recorded historical import settings (paths were removed during cleanup):

1. After restarting Altium, select **File → Import Wizard → KiCad Design Files**.
2. Add `upstream/jetson-orin-baseboard.kicad_pro`. Confirm the output preview includes the root schematic, all nine child sheets and the PCB. If the project is not expanded automatically, add the root `.kicad_sch` and `.kicad_pcb` explicitly. The source contains embedded symbols/footprints; both external library tables are empty.
3. Enable error, warning and event logging. Set the output directory to `hardware/orin-carrier/altium` in this checkout. In schematic options, retain KiCad colors and recognize power ports; **Force passive pin types was disabled** to preserve source pin types. The PCB font mapping was **Default Altium Font** and testpoint-name overrides were left blank.
4. Preserve all eight copper layers in their source order: F.Cu, In1.Cu, In2.Cu, In3.Cu, In4.Cu, In5.Cu, In6.Cu, B.Cu. Check In4.Cu explicitly rather than dropping it because of its `jumper` tag.
5. Finish the wizard, save the native project and preserve its import logs. The wizard creates the `Imported jetson-orin-baseboard/` subdirectory. The outline maps to **Keep Out Layer**, masks to **Top/Bottom Solder Mask**, silkscreen to **Top/Bottom Overlay**, and paste to **Top/Bottom Paste**; remaining user/fabrication/courtyard layers map to mechanical layers. Native board-shape and rule equivalence are not qualified by that mapping alone.

The upstream README describes KiCad 9.x, while the pinned CAD files carry KiCad 8.0 generator tags. The installed KiCad 9 CLI reads them and exported the complete schematic netlist successfully: **10 sheets, 669 components and 791 nets**. The intermediate source netlist is retained locally in `.nx/ee-carrier-source-netlist.xml`.

The source PCB contains **681 footprints**, **800 net entries including the unassigned net**, **12,461 track segments**, **1,345 vias** and **eight copper layers**. In4.Cu is marked `jumper` in the source; confirm its mapping explicitly in Altium. Schematic component/net counts differ from PCB counts, so compare against the corresponding source domain rather than forcing them to match.

KiCad source DRC/parity check completed with **zero unconnected items and zero schematic/PCB parity issues**. Its **221 findings** comprise one intersecting-zone finding, three silkscreen-edge clearances, 199 footprint-library findings, eight silkscreen-over-copper findings and ten silkscreen overlaps. They are preserved as an upstream baseline; no corrections or clean-DRC claim is made. The report and comparison inventory are retained locally in `.nx/ee-carrier-source-drc.json` and `.nx/ee-carrier-source-baseline.json`.

3D model paths reference external `${ANT3DMDL}` assets that are absent from this clone. Missing 3D models must be recorded/resolved for mechanical review; do not assume the PCB import supplies them.

## Acceptance required if carrier work resumes

1. Open the native Altium project, every schematic sheet and the PCB; record errors/warnings and confirm hierarchy/connectivity.
2. Compare component designators/values, all pin-to-net connections, footprints, pad numbers, net names, tracks/vias, board outline, copper layers/stack and rule constraints with the pinned source. Counts alone cannot establish equivalence.
3. Inspect differential pairs, length/skew constraints, copper pours, holes and unconnected nets. Resolve importer losses before making ARC changes.
4. Preserve third-party attribution; record the Altium version, import settings and verified source hashes.
5. Complete ARC adaptations and the systems fab gate before ordering. This imported reference does not release an ARC board for manufacture.
