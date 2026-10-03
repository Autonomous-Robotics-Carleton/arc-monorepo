# hardware

Board designs and mechanical exports. One folder per board, added when its design starts:

| Board | Handoff section |
| --- | --- |
| Power board (+ e-stop circuit) and bus clamp board | `systems/handoff/electrical.md` §1 |
| Sync MCU board | §2 |
| VESC 6.4 fork | §3 |
| Orin carrier (fork of Antmicro's baseboard) | §4 |
| Harness | §5 |

Mechanical CAD lives in Fusion. `mechanical/` here holds STEP and drawing exports, committed per release.

## Rules

- Nothing is fabbed, machined or bought until it passes `systems/reviews/fab-gate.md`.
- Fab outputs (gerbers, BOM, pick-and-place) are committed only at a release tag, e.g. `corner-board-r1`.
- Large binaries (STEP, gerbers, PDFs) go through git LFS.
