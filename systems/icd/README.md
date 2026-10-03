# Interface Control Documents

One file per interface. An ICD names an owner on **each** side. Changes need both owners' approval on the PR.

| ICD | Side A | Side B | Format | Status |
| --- | --- | --- | --- | --- |
| [corner-connector](corner-connector.md) | Corner VESC board | Chassis harness / sync board / e-stop | Markdown | Proposed (rev D) |
| corner-mechanical | Corner (motor, gearbox, VESC) | Chassis | Markdown + CAD | TBD |
| can-command | Sync MCU | Corner VESCs (classic CAN, ADR-0009) | `can-command.dbc` | TBD |
| can-telemetry | Corner VESCs | Sync MCU (2 × CAN-FD, ADR-0011) | `can-telemetry.dbc` | TBD |
| power-rails | Power board | Every load | Markdown (voltage, current, sequencing, fusing) | TBD |
| power-sync-stack | Power board | Sync board | Markdown (header pinout) | TBD |
| carrier-sync | Orin carrier | Sync board | Markdown (triggers, PPS/PTP, Ethernet) | TBD |
| ros2-msgs | Sync MCU bridge | ROS 2 stack | `.msg` files | TBD |
| battery-pack | Car (battery bay, power input, per-cell monitor) | Charging station | Markdown (pack form factor, main connector, balance connector (4S JST-XH 5-pin assumed, TBC) which the car also uses for cell monitoring, chemistry and cell count) | TBD |
| deck-grid | Chassis | Sensor mounts | Markdown + CAD | TBD |

The `.dbc` files are the single source for CAN; firmware headers get generated from them (e.g. with `cantools`), never hand-written.
