# ARC Car — Systems Engineering

This folder is the source of truth for the car's requirements, interfaces, decisions, budgets and risks.
`architecture.md` is the overview that links into it; when the two disagree, the files here win.

| Folder / file | What lives there | Format |
| --- | --- | --- |
| `requirements/` | Mission, system requirements, subsystem requirements | Markdown tables with IDs |
| `icd/` | One interface control document per interface | Markdown tables; `.dbc` for CAN, MAVLink `.xml` for the sync link, `.msg` for ROS 2 |
| `adr/` | Architecture decision records | One Markdown file per decision |
| `budgets/` | Mass/CG, power per rail, bus load, latency, cost | CSV (one file per budget) |
| `bom/` | Functional BOM: what each part must do and what it connects to | Markdown tables |
| `verification/` | Verification plan: method, stage, rig and pass criterion for every requirement | Markdown |
| `tests/` | Test procedures that verify requirements or retire risks, with results tables | Markdown |
| `mechanical/` | Mechanical layout brief, envelopes, early mechanical decisions | Markdown (+ STEP exports per release) |
| `software/` | Software architecture (components, layers, targets) and the repo setup plan | Markdown |
| `handoff/` | Per-discipline handoff: what's decided, where it lives, what's open | Markdown |
| `risks.md` | Ranked risks, each paired with a spike test | Markdown table |
| `reviews/` | Gate checklists and review records | Markdown |
| `architecture.md` | Narrative overview of the whole car | Markdown |
| `topology.html` | Interface topology diagram, sync board I/O and stop sequence (source of the shared topology page) | HTML |

## IDs

| Prefix | Meaning | Example |
| --- | --- | --- |
| `SYS-nn` | System requirement | SYS-05 |
| `<SUB>-nn` | Subsystem requirement (`DRV`, `PWR`, `CMP`, `SNS`, `SAF`, `MEC`, `SW`) | PWR-03 |
| `ICD-<name>` | Interface control document | ICD-corner-connector |
| `ADR-nnnn` | Decision record | ADR-0002 |
| `RSK-nn` | Risk | RSK-01 |
| `LIM-nn` | Accepted limit (hardware deliberately limiting software) | LIM-01 |
| `E-nn` | Electrical BOM item (`bom/electrical.md`) | E-01 |

IDs are never reused. A deleted requirement keeps its row with status `Deleted`.

## Rules

1. **Every number has a parent.** Each subsystem requirement and each spec value in an ICD traces to a `SYS` requirement. A value with no parent is either gold-plating or a missing requirement; flag it, don't hide it.
2. **Every requirement has a verification method:** Test (T), Analysis (A), Inspection (I) or Demonstration (D), plus a link to the test once it exists.
3. **Changes go through PRs.** A PR that changes a requirement, ICD or budget links the ADR or issue that justifies it. ICD changes need sign-off from the owners on both sides.
4. **Unknown values are written as `TBD` (no value yet) or `TBC` (value proposed, not confirmed)**, never as "about" or "around".
5. **Gate before spending.** Nothing is fabbed, machined or bought over the team's spending threshold until it passes `reviews/fab-gate.md`.
6. **Failures get a record (FRACAS).** Every hardware or firmware failure during a run gets an issue with the log, root cause and fix (SYS-18). Nothing is written off as a fluke.
7. **Releases are tags:** `sys-v1-srr` (requirements baselined), then one tag per gated build (e.g. `power-board-r1`).

## Status values

`Draft` → `Proposed` → `Baselined` → (`Changed` via PR) / `Deleted`
