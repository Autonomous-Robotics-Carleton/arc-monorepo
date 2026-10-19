# ARC motherboard

**Current plan, 2026-10-09:** one PCB combines the power circuitry and sync MCU circuitry. The Jetson carrier remains a separate PCB in [orin-carrier](../orin-carrier/README.md). Owner: **Kunshpreet Baweja** (`kunshpreet`). ADR-0042 records this packaging direction; a later split into two PCBs remains possible after review.

This folder is the working location for the combined motherboard design. Schematic capture and layout have not started here; the imported Antmicro Altium project remains the separate carrier reference.

| Motherboard section | Scope | Design inputs |
| --- | --- | --- |
| Power — E-41 / E-46 | Battery/wall input, protected LV rails, switching, voltage/current and cell monitoring, e-stop circuitry and shutdown sequencing | [Power rails](../../systems/icd/power-rails.md), [electrical handoff §1](../../systems/handoff/electrical.md#1-power-board--e-stop-circuit-and-bus-clamp-board) |
| Sync — E-30 | STM32H723, Ethernet PHY, sensor interfaces, camera triggers/PPS, watchdog, classic command CAN, four 2 Mbit/s A50S UARTs and separate steering FD | [Electrical handoff §2](../../systems/handoff/electrical.md#2-sync-mcu-board), [controller UART](../../systems/icd/controller-telemetry.md) |
| Internal power/sync interface | Monitor buses, alerts, logic supplies and shutdown signals connect within this PCB; no stacking header in the current plan | [Logical signal contract](../../systems/icd/power-sync-stack.md) |
| Carrier connection | Compute supply, triggers/PPS, shutdown request and acknowledgement; Ethernet follows the recorded network plan | [Carrier/sync interface](../../systems/icd/carrier-sync.md) |

The four bought A50S controllers stay separate and send telemetry over four 2 Mbit/s UARTs under ADR-0034. UART firmware, pin/DMA assignment, references and protection still need qualification. The bus clamp’s placement remains open.

Grounding/isolation, power-stage parts and sizing remain **TBD for later review**. Combining the circuits does not remove noise, return-current, power-loss or independent-stop requirements. Board envelope, copper stack, connectors, pin map, layout and verification remain TBD. Refer to the [parts list](../../systems/bom/pcb-parts-selection.md) for candidates; the sync and power component sections belong to this same PCB and must not be counted as two boards.

Native Altium files will live here once capture starts. Use Git LFS for large CAD binaries, preserve source attribution, and release fabrication outputs only after the [fab gate](../../systems/reviews/fab-gate.md).

The [proposed wiring diagram and conductor schedule](../../systems/bom/motherboard-wiring.md) organize connections for **ten angle, two IMU and five ToF boards**, all assembled in house under ADR-0045. All four wheel, four suspension and two front-knuckle angle locations remain selected. Our [custom remote sensor PCBs](../sensor-boards/README.md), including the [IMU](../sensor-boards/imu/README.md) and [corner-height](../sensor-boards/tof/README.md) designs, remain separate from the motherboard; their 2-layer layouts and connector details remain TBC. The **seventeen-run / 111-conductor sensor minimum TBC** excludes power, carrier and controller wiring. One motherboard is installed from v1.0 and one separate carrier from v1.1 (v1.0 uses the developer kit's carrier, ADR-0046), both assembled by JLC. The costing scenario shows two factory-populated boards per core design: one installed and one minimum-run surplus. Unused bare boards and core minimum-run surplus do not add a second installed sensor set.
