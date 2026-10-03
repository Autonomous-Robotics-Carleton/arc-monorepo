# firmware

Code that runs on the car's microcontrollers.

| Folder | Target | RTOS | Decided in |
| --- | --- | --- | --- |
| `sync-mcu/` | STM32H723 on the sync board: time base, sensor timestamping, camera triggers, watchdog, safety envelope, CAN buses, Ethernet to the Orin | Zephyr | ADR-0017, ADR-0021 |
| `vesc/` | VESC 6.4 fork at each corner: stock VESC firmware plus the e-stop brake routine and CAN-FD telemetry | ChibiOS | ADR-0011, ADR-0012 |

The steering controller (moteus-c1) runs mjbots' firmware, not ours. Its configuration (timeout mode, limits, belt ratio) will be kept here too (ADR-0019).

Safety functions live only on the sync MCU and the VESCs, never on the Orin (ADR-0017). Changes to them get extra review.
