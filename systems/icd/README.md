# Interface Control Documents

One file per interface. An ICD names an owner on **each** side. Changes need both owners' approval on the PR.

| ICD | Side A | Side B | Format | Status |
| --- | --- | --- | --- | --- |
| [corner-connector](corner-connector.md) | Corner (motor, gearbox) | Chassis harness / motor controllers on the deck / sync board / e-stop | Markdown | Draft (rev E) |
| [corner-mechanical](corner-mechanical.md) | Corner (motor, gearbox) | Chassis | Markdown + CAD | Draft (outline) |
| [can-command](can-command.dbc) | Sync MCU | Motor controllers (classic CAN, ADR-0009) | `can-command.dbc`: stock VESC frames | Draft |
| [controller-telemetry](controller-telemetry.md) | Motor controllers | Sync MCU (one UART each, ADR-0034) | Markdown; MAVLink 2, the ARC message set in `sync-link.xml` | Draft |
| power-rails | Power board | Every load | Markdown (voltage, current, sequencing, fusing) | TBD |
| power-sync-stack | Power board | Sync board | Markdown (header pinout) | TBD |
| carrier-sync | Orin carrier | Sync board | Markdown (triggers, PPS/PTP, Ethernet) | TBD |
| [sync-link](sync-link.md) | Sync MCU firmware | Sync bridge on the Orin | UDP; MAVLink 2 (ADR-0031) | Draft (outline) |
| ros2-msgs | Sync MCU bridge | ROS 2 stack | `.msg` files | TBD |
| battery-pack | Car (battery bay, power input, per-cell monitor) | Charging station | Markdown (pack form factor, main connector, balance connector (4S JST-XH 5-pin assumed, TBC) which the car also uses for cell monitoring, chemistry and cell count) | TBD |
| [deck-grid](deck-grid.md) | Chassis | Sensor mounts | Markdown + CAD | Draft (outline) |

The `.dbc` and MAVLink `.xml` files are the single source for their interfaces; code is generated from them by `tools/gen-interfaces.sh` (cantools, mavgen), never hand-written, and CI fails if it's stale.
