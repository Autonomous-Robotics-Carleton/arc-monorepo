# ADR-0017: Sync MCU firmware runs on Zephyr

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-07, SYS-19, SYS-22, SYS-24, SYS-25, ADR-0011, ADR-0016

> [!NOTE]
> The Context below is from 2026-10-03. ADR-0034 later removed the two CAN-FD telemetry buses: telemetry comes over a UART per motor controller (ICD-controller-telemetry), and the only CAN-FD bus left is the steering's (ADR-0019). The decision, Zephyr, stands.

## Context

The STM32H723 sync MCU does the car's hard real-time work:
- timestamps every sensor at the pin
- triggers the cameras
- runs the heartbeat watchdog and the safety envelope
- drives the command CAN bus and both CAN-FD telemetry buses
- streams everything to the Orin over Ethernet

It needs an RTOS.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **Zephyr** | STM32H7 support; Ethernet stack with PTP (relevant to the open PPS vs PTP decision); CAN-FD and SPI-with-DMA drivers; device tree; runs tests on a PC in CI; micro-ROS support | Steeper learning curve than vendor tooling |
| FreeRTOS + ST HAL / CubeMX | Simple, familiar, good vendor tooling | Ethernet/PTP and CAN-FD stacks assembled by hand; less built-in test support |
| ChibiOS | Same RTOS as the VESC firmware, so shared knowledge | Smaller ecosystem for Ethernet/PTP |

## Decision

Zephyr on the sync MCU.

## Consequences

- Real-time layering: VESCs on ChibiOS (stock VESC firmware), sync MCU on Zephyr, Orin on PREEMPT_RT Linux (ADR-0016). Safety functions (watchdog, envelope, any-corner stop) live on the sync MCU and VESCs only.
- Sync firmware gets CI with simulated (PC-hosted) tests from the start.
- PTP is available natively if the time-sync decision goes that way.
