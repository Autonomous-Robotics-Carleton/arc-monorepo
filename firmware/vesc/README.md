# vesc

Firmware for the VESC 6.4 fork at each corner (ADR-0011).

It's stock VESC firmware plus two additions:

- **E-stop brake routine** (ADR-0012): reads ESTOP_EN on a GPIO, ramps brake current to a target deceleration, and ignores CAN commands while the e-stop is active. This is a safety function: it needs its own test cases, and changes get extra review.
- **CAN-FD telemetry** (ADR-0011, ADR-0024): an SPI CAN-FD controller carries telemetry; motor commands stay on the classic CAN bus. Each frame is built from the latest motor-control values at send time and stamped with the sample counter then. The telemetry thread runs at high thread priority; motor control is protected because it runs in the ADC interrupt (RSK-12). SPI transfers are DMA-driven.

## Upstream and updates (ADR-0033)

Upstream `vedderb/bldc` is vendored into `bldc/` as a plain snapshot. `UPSTREAM` records the repository, ref and commit it's based on. Our changes are ordinary commits in `bldc/`.

```bash
tools/vesc-upstream.sh import <ref>   # first import (ref: SQ-3 in systems/software/setup-plan.md)
tools/vesc-upstream.sh update <ref>   # bring in upstream's changes, three-way, in a PR of its own
```

**Licence:** everything under `firmware/vesc/` is GPL-3.0, under upstream's licence; the rest of the repo is MIT. Don't copy code from here into other parts of the repo.

Nothing is imported yet.
