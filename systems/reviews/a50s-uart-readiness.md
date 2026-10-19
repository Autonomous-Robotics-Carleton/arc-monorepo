# A50S UART telemetry readiness

- **Date:** 2026-10-09
- **Owner:** kunshpreet
- **Status:** Active source review; firmware and harness qualification pending
- **Traces to:** SYS-07, SYS-19, SYS-24, SYS-25, SYS-29, E-30, E-50, ADR-0034, RSK-12

Upstream `67392fa` sets 2 Mbit/s UART, adds braking-state fields and documents receive gaps/version refusal. ADR-0034 makes UART the active drive telemetry transport. Neither endpoint implements streaming yet.

## Rate and PCB consequences

| Check | Source evidence / result | EE implication |
| --- | --- | --- |
| A50S baud rate | USART3 uses APB1 at 42 MHz. The stock serial configuration clears `CR1`, and the USARTv1 driver writes `BRR = PCLK1 / speed`. At 2 Mbit/s, `BRR = 21`; this is an exact nominal divisor with 16× oversampling. Clock tolerance and installed-harness operation remain unqualified | 2 Mbit/s is the active baseline under ADR-0034; changing the baud setting alone does not produce ARC telemetry |
| UART load, SYS-24 | The current `ARC_MOTOR_STATUS` payload is 47 bytes. An unsigned, full-length MAVLink 2 frame is 59 bytes; 8N1 uses 10 wire bits per byte. `59 × 10 / 2,000,000 = 295 µs`, or **29.5%** per controller at 1 kHz. A full-length unsigned `ARC_LINK_STATUS` is 30 bytes at 1 Hz, bringing the nominal controller-to-sync load to **29.515%**, before any additional traffic | Nominal bandwidth fits the ≤50% ceiling. This is a calculation for four separate UARTs, not measured acquisition/forwarding timing |
| A50S pin ownership | USART3 TX is **PB10**, RX is **PB11** | Preserve UART on these pads. Actual connector contacts remain unqualified |
| Motherboard pin tally | The upstream H723 example includes four controller UARTs plus debug; the current board definition has no released peripheral map | Allocate five UARTs, with the required spare provision; qualify DMA/interrupt ownership, sensor SPI, command CAN, steering and Ethernet together |
| UART electrical interface | The active ICD specifies 3.3 V logic and a reference conductor; ground-shift and supply/power-off behavior have not been qualified | No 5 V logic assumption or released connector. Grounding/isolation stays TBD under ADR-0041; four UART runs are now part of the operating harness budget |

Baud evidence: [A50S pin definitions](../../firmware/vesc/bldc/hwconf/teamtriforceuk/a50s_v23c/hw_a50s_v23c_core.h), [clock configuration](../../firmware/vesc/bldc/hwconf/mcuconf.h), [stock UART application](../../firmware/vesc/bldc/applications/app_uartcomm.c) and [serial driver](../../firmware/vesc/bldc/ChibiOS_3.0.5/os/hal/ports/STM32/LLD/USARTv1/serial_lld.c). The stock application uses VESC packet processing; it is not the proposed MAVLink status publisher. Its baud rate is configurable, so its compiled 115200 default is not a newly selected ARC operating rate.

## Implementation gaps

| Stage | Current repository implementation | Still required |
| --- | --- | --- |
| A50S acquisition/transmit | [`arc_telemetry_init()`](../../firmware/vesc/arc/src/telemetry.c) only returns `0`; its [header](../../firmware/vesc/arc/include/arc/telemetry.h) explicitly calls it a stub. Stock `HAL_USE_UART` and `STM32_UART_USE_USART3` are false; the serial driver owns USART3 | Coherent sampling, source timestamps, chosen transport driver, DMA/IRQ ownership and loaded motor-loop validation. A successful init return does not prove streaming |
| Sync MCU controller receive | [`controller_links_init()`](../../firmware/sync-mcu/app/src/controller_links.c) logs “not implemented yet” and returns `0`; the [`arc_sync` board](../../firmware/sync-mcu/boards/arc/arc_sync/arc_sync.dts) has no released peripheral/pin map | Four independent UART receivers/parsers, per-corner loss/reject counters, clock mapping and coordinated fault response |
| Sync-to-Orin path | [`sync_link.c`](../../firmware/sync-mcu/app/src/sync_link.c) exchanges link status under socket configuration and sleeps 10 ms per loop. [`arc_backend_car`](../../ros/src/arc_backend_car/src/backend.cpp) currently handles link-status discovery | Motor-status forwarding and ROS publication, protocol-mismatch command refusal, plus measurements against the ≤0.25 ms forwarding / ≤2 ms p99 end-to-end limits (ADR-0024 / SYS-29). Existing discovery is not evidence of those limits |
| Message schema and IDs | [`sync-link.xml`](../icd/sync-link.xml) defines UART motor status, including fault/state fields. `ARC_CORNER` is **0–3**, while command-bus controller IDs are **1–4** | Explicit corner-ID mapping, coherent sample-time semantics and controller-to-sync clock alignment remain TBD; do not use the command ID directly as the corner enum |
| Tests | [VESC host tests](../../firmware/vesc/arc/tests/test_stubs.c) check stub return values; the [sync-link test](../../firmware/sync-mcu/tests/sync_link/src/main.c) round-trips link status | These do not test UART baud/harness, motor telemetry, clock alignment, version refusal, stale-corner stopping or loaded four-controller telemetry |

The [firmware overview](../../firmware/README.md), [sync MCU README](../../firmware/sync-mcu/README.md), [VESC README](../../firmware/vesc/README.md) and [software architecture](../software/architecture.md) already describe the UART scaffold. Their transport direction now agrees with ADR-0034; their listed planned behavior is not implemented merely because it is documented. No schema change is needed for this transport decision. Future schema changes must regenerate both firmware and ROS outputs through `tools/gen-interfaces.sh`.

## Harness and release

Use four separate TX/reference and RX/reference pairs at the 2 Mbit/s baseline. The proposed budget includes one spare per run: **five conductors per A50S / 20 for all four, TBC**. Final returns, shielding, isolation/protection and pinouts remain TBD under ADR-0041; a UART reference must not become a motor-current path. Motor phases and Hall cables still bypass the motherboard sensor banks. See the [wire schedule](../bom/motherboard-wiring.md) and [qualification procedure](../tests/a50s-arrival.md).

Ground shift, DMA/driver ownership, timing, protocol identity and fault handling remain open. This review records source evidence and calculated capacity, not completed firmware or bench acceptance.
