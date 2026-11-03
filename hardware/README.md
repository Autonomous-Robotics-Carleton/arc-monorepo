# hardware

Board designs and mechanical exports. One folder per board, added when its design starts. [Motherboard](motherboard/README.md) contains the combined power/sync design plan; [Orin carrier](orin-carrier/README.md) preserves the deferred design's source provenance and historical conversion audit; its reference CAD was removed; `mechanical/` holds mechanical exports.

| Board | Handoff section |
| --- | --- |
| [Motherboard](motherboard/README.md) — one PCB for power (+ e-stop) and sync MCU | `systems/handoff/electrical.md` §1–2; ADR-0042 |
| Bus clamp assembly — PCB placement TBD | §1 |
| Motor controllers | §3: off-the-shelf A50S, so no board here (ADR-0034) |
| [Orin carrier](orin-carrier/README.md) (fork of Antmicro's baseboard) | §4 |
| [Remote sensor PCBs](sensor-boards/README.md) — custom angle, IMU and height/scale designs; layers TBC | §2 / §5; ADR-0045; no corner MCU |
| Harness | §5; [wiring/count schedule](../systems/bom/motherboard-wiring.md) |

Mechanical CAD lives in Fusion. `mechanical/` here holds STEP and drawing exports, committed per release.

Use the [Altium source snapshot workflow](../tools/altium-sync/README.md) to import saved workspace source files into the four active board folders before a normal Git commit.

## Rules

- Nothing is fabbed, machined or bought until it passes `systems/reviews/fab-gate.md`.
- Fab outputs (gerbers, BOM, pick-and-place) are committed only at a release tag, e.g. `power-board-r1`.
- Large binaries (STEP, PDFs, zips: zip your gerbers) go through git LFS.
