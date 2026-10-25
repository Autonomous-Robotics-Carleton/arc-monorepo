# Interface Control Documents

One file per interface. An ICD names an owner on **each** side. Changes need both owners' approval on the PR.

| ICD | Side A | Side B | Format | Status |
| --- | --- | --- | --- | --- |
| [corner-connector](corner-connector.md) | Corner (motor, gearbox) | Chassis harness / motor controllers on the deck / sync board / e-stop | Markdown | Draft (rev F; approvals pending) |
| [corner-mechanical](corner-mechanical.md) | Corner (motor, gearbox) | Chassis | Markdown + CAD | Draft (outline) |
| [can-command](can-command.dbc) | Sync MCU | Motor controllers (classic CAN, ADR-0009) | `can-command.dbc`: stock VESC frames | Draft |
| [controller-telemetry](controller-telemetry.md) | Motor controllers | Sync MCU (four independent UARTs; ADR-0034) | Markdown; MAVLink 2, the ARC message set in `sync-link.xml` | Draft active UART interface; implementation / qualification pending |
| [power-rails](power-rails.md) | Power board | Every load | Markdown (voltage, current, sequencing, fusing) | Draft (rev A; approvals pending) |
| [power-sync-stack](power-sync-stack.md) | Motherboard power section | Motherboard sync section | Markdown (internal signals; ADR-0042) | Draft (rev B; approvals pending) |
| [carrier-sync](carrier-sync.md) | Orin carrier | Sync board | Markdown (triggers, PPS/PTP, Ethernet) | Draft (rev A; approvals pending) |
| [sync-link](sync-link.md) | Sync MCU firmware | Sync bridge on the Orin (`arc_backend_car`) | UDP; MAVLink 2 (ADR-0031), message set in `sync-link.xml` | Draft |
| [network](network.md) | The car (Orin, sync MCU, LiDAR, switch) | Ground equipment (team router, laptops; ADR-0015) | Markdown (networks, addresses, ports, rates) | Draft (rev A) |
| ros2-msgs | Backends (sim, replay, car; ADR-0028) | ROS 2 stack | `.msg` files | TBD |
| steering-bus | Sync MCU | Steering controller (moteus-c1, CAN-FD, ADR-0019) | Markdown (moteus register protocol, rates, timeout mode) | TBD |
| operator-link | Laptop | Orin (heartbeat, teleop; ADR-0015, SYS-04) | Markdown | TBD |
| sim-link | Simulator (device models) | Sync MCU firmware on `native_sim` only (ADR-0047; `software/simulation.md`) | MAVLink 2 or Zephyr device emulators, TBD | TBD |
| battery-pack | Car (battery bay, power input, per-cell monitor) | Charging station | Markdown (pack form factor, main connector, balance connector (4S JST-XH 5-pin assumed, TBC) which the car also uses for cell monitoring, chemistry and cell count) | TBD |
| [deck-grid](deck-grid.md) | Chassis | Sensor mounts | Markdown + CAD | Draft (outline) |

The `.dbc` and MAVLink `.xml` files are the single source for their interfaces; code is generated from them by `tools/gen-interfaces.sh` (cantools, mavgen), never hand-written, and CI fails if it's stale.

The three power/carrier drafts define logical interfaces; final monitor transport, pinouts, ratings and physical tests remain open.
