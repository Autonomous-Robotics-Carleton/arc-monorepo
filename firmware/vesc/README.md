# vesc

Firmware for the VESC 6.4 fork at each corner (ADR-0011).

It's stock VESC firmware plus two additions:

- **E-stop brake routine** (ADR-0012): reads ESTOP_EN on a GPIO, ramps brake current to a target deceleration, and ignores CAN commands while the e-stop is active. This is a safety function: it needs its own test cases, and changes get extra review.
- **CAN-FD telemetry** (ADR-0011): an SPI CAN-FD controller carries telemetry; motor commands stay on the classic CAN bus. It must not disturb motor control (RSK-12).

Nothing is here yet. Still to decide: whether the fork of the upstream VESC firmware lives here as a submodule or as its own repo.
