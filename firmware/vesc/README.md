# vesc

Firmware for the four motor controllers: off-the-shelf Team Triforce A50S V2.3c boards on the lower deck (ADR-0034), built from upstream's `a50s_v23c_12s` target. Flash it with VESC Tool over USB; per-unit current calibration lives in the board's EEPROM and survives reflashing.

It's stock VESC firmware plus three additions:

- **E-stop brake routine** (ADR-0012): reads ESTOP on a spare input (PPM, TBC) with the MCU's pull-down, ramps brake current to a target deceleration, and ignores CAN commands while the e-stop is active. This is a safety function: it needs its own test cases, and changes get extra review.
- **UART telemetry** (ADR-0024, ADR-0034): full status at 1 kHz over a UART to the sync MCU; motor commands stay on the classic CAN bus. Each frame is built from the latest motor-control values at send time and stamped with the sample counter then. The telemetry thread runs at high thread priority; motor control is protected because it runs in the ADC interrupt (RSK-12). UART transfers are DMA-driven.
- **Speed limit** (ADR-0034): maximum eRPM ~75k (TBC), the range where the A50S's control loop is stable (RSK-18).

**Don't touch the USB, CAN or firmware-upload code.** The running firmware receives uploads, so a build that breaks them can only be recovered over SWD (RSK-20).

## Upstream and updates (ADR-0033)

Upstream `vedderb/bldc` is vendored into `bldc/` as a plain snapshot, at `release_7_00` (SQ-3). `UPSTREAM` records the repository, ref and commit it's based on. Our changes are ordinary commits in `bldc/`.

```bash
tools/vesc-upstream.sh import <ref>   # first import: done, at release_7_00 (SQ-3)
tools/vesc-upstream.sh update <ref>   # bring in upstream's changes, three-way, in a PR of its own
```

**Licence:** everything under `firmware/vesc/` is GPL-3.0, under upstream's licence; the rest of the repo is MIT. Don't copy code from here into other parts of the repo.

## Layout

| Path | What |
| --- | --- |
| `bldc/` | Upstream VESC firmware, plus our edits as ordinary commits |
| `arc/` | Our additions (e-stop, UART telemetry, speed limit) as modules with host tests. Stubs so far, fail-safe; each joins the firmware build in `bldc/` as it's implemented |
| `UPSTREAM` | The upstream commit `bldc/` is based on |

## Build and test

In the dev container (GCC 7 2018-q2, the compiler upstream expects):

```bash
npx nx build vesc   # bldc: make fw_a50s_v23c_12s -> bldc/build/a50s_v23c_12s/*.bin
npx nx test vesc    # arc/: host tests with CMake and CTest
```

Flash the `.bin` with VESC Tool over USB.
