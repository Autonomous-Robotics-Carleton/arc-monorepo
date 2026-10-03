# sync-mcu

Zephyr firmware for the STM32H723 on the sync board (ADR-0017).

It owns the car's time base: µs-critical sensors are timestamped in this clock, and the Orin syncs to it in software (ADR-0021). It also runs the heartbeat watchdog and the safety envelope (SYS-04), drives the command CAN bus, both CAN-FD telemetry buses and the steering bus (ADR-0011, ADR-0019), and streams everything to the Orin over Ethernet.

Nothing is here yet. The app will be a west workspace application; Zephyr itself is pulled by the west manifest, not committed. Tests run on a PC-hosted target in CI from the start (ADR-0017).

Pin and I/O tally: `systems/bom/electrical.md`. Board handoff: `systems/handoff/electrical.md`.
