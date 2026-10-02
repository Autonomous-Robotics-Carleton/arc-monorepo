# Decision Records

Copy `0000-template.md` to the next number. Never edit an accepted ADR's decision; write a new one that supersedes it.

The ADRs marked *backfilled* record decisions made before this process existed. Their rationale is taken from `architecture.md` only. The options and criteria sections still need the reasoning from whoever made the call.

| ADR | Decision | Status |
| --- | --- | --- |
| [0001](0001-lidar-hokuyo.md) | Hokuyo UST-10LX over RPLidar A3 | Accepted (backfilled) |
| [0002](0002-separate-can-buses.md) | Classic CAN for motor control, separate CAN-FD bus for sensors | Superseded by 0009 |
| [0003](0003-corner-sensor-node.md) | Separate sensor MCU per corner, not on the motor controller | Superseded by 0006 |
| [0004](0004-corner-connectors.md) | Separate power and signal connectors at each corner; XT30(2+2) rejected | Accepted (pinout in ICD rev B) |
| [0005](0005-wifi-only-link.md) | Wi-Fi to the laptop is the only wireless link; no RC radio | Accepted (backfilled) |
| [0006](0006-no-corner-module.md) | No corner module in v1; VESC is the only corner board | Accepted |
| [0007](0007-ride-height-sensors.md) | Four corner ride-height sensors in v1, plus one at the optical flow sensor | Accepted |
| [0008](0008-corner-sensors-to-sync-board.md) | Wheel encoders and suspension pots wire directly to the sync board | Accepted |
| [0009](0009-single-classic-can-bus.md) | One classic CAN bus to the four corner VESCs | Accepted |

Decisions still to record: Orin NX vs Nano (needs SYS-08), PPS vs PTP time sync (the sync MCU is already on Ethernet), forking the Antmicro carrier, staggered transverse motors.
