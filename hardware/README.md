# hardware

Board designs and mechanical exports. One folder per board, added when its design starts; none has started yet, so `mechanical/` is the only folder so far.

| Board | Handoff section |
| --- | --- |
| Power board (+ e-stop circuit) and bus clamp board | `systems/handoff/electrical.md` §1 |
| Sync MCU board | §2 |
| Motor controllers | §3: off-the-shelf A50S, so no board here (ADR-0034) |
| Orin carrier (fork of Antmicro's baseboard) | §4 |
| Harness | §5 |

Mechanical CAD lives in Fusion. `mechanical/` here holds STEP and drawing exports, committed per release.

## Rules

- Nothing is fabbed, machined or bought until it passes `systems/reviews/fab-gate.md`.
- Fab outputs (gerbers, BOM, pick-and-place) are committed only at a release tag, e.g. `power-board-r1`.
- Large binaries (STEP, PDFs, zips: zip your gerbers) go through git LFS.
