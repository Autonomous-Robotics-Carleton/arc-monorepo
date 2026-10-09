# vesc

Firmware for the four motor controllers: off-the-shelf Team Triforce A50S V2.3c boards on the lower deck (ADR-0034), built from upstream's `a50s_v23c_12s` target. Flash it with VESC Tool over USB; per-unit current calibration lives in the board's EEPROM and survives reflashing.

It's stock VESC firmware plus four additions:

- **E-stop brake routine** (ADR-0012): reads ESTOP on a spare input (PPM, TBC) with the MCU's pull-down, ramps brake current to a target deceleration, and ignores CAN commands while the e-stop is active. This is a safety function: it needs its own test cases, and changes get extra review.
- **Command timeout** (ADR-0035): no command on the CAN bus for 150 ms (TBC) brakes on the same ramp as the e-stop, replacing upstream's timeout (1000 ms, then 0 A: coasting). Also a safety function.
- **UART telemetry** (ADR-0024, ADR-0034, ICD-controller-telemetry): MAVLink 2 `ARC_MOTOR_STATUS` at 1 kHz, including a `state` field that reports braking on e-stop or command timeout (ADR-0035), and `ARC_LINK_STATUS` at 1 Hz (message set in `systems/icd/sync-link.xml`), on the A50S's UART (USART3, PB10/PB11) to the sync MCU at 2 Mbit/s (exact with 16× oversampling from its 42 MHz clock); motor commands stay on the classic CAN bus. Each frame is built from the latest motor-control values at send time, with `time_ns` in the controller's clock. The telemetry thread runs at high thread priority; motor control runs in the ADC interrupt, which preempts every thread, and RSK-12 tracks what's left of the risk. UART transfers are to be DMA-driven, which needs upstream's serial driver off USART3. The MAVLink code isn't generated into this firmware yet.
- **Speed limit** (ADR-0034): maximum eRPM ~75k (TBC), which keeps ≥ 10 control-loop samples per electrical cycle; stability up to it is checked on rig R2 (RSK-18).

**Don't touch the USB, CAN or firmware-upload code.** The running firmware receives uploads, so a build that breaks them can only be recovered over SWD (RSK-20).

## Upstream and updates (ADR-0033)

Upstream `vedderb/bldc` is vendored into `bldc/` as a plain snapshot, at `release_7_00` (SQ-3). `UPSTREAM` records the repository, ref and commit it's based on. `bldc/` is unmodified so far: our additions are developed as modules in `arc/`, and the edits that join them to the build will be ordinary commits in `bldc/`.

```bash
tools/vesc-upstream.sh import <ref>   # first import: done, at release_7_00 (SQ-3)
tools/vesc-upstream.sh update <ref>   # bring in upstream's changes, three-way, in a PR of its own
```

The script stages its result but doesn't commit it; on conflicts it lists the files and exits 2. It skips lispBM's tests, test reports, REPL, docs, examples, benchmarks, videos and mascot on every import and update. `VESC_UPSTREAM_URL` overrides the upstream repository.

**Licence:** everything under `firmware/vesc/` is GPL-3.0, under upstream's licence; the rest of the repo is MIT. Don't copy code from here into other parts of the repo.

## Layout

| Path | What |
| --- | --- |
| `bldc/` | Upstream VESC firmware (unmodified so far; our edits will be ordinary commits) |
| `arc/` | Our additions (e-stop, UART telemetry, speed limit; the command timeout to come) as modules with host tests. Stubs so far, fail-safe; each joins the firmware build in `bldc/` as it's implemented |
| `UPSTREAM` | The upstream commit `bldc/` is based on |

## Build and test

In the dev container (GCC 7 2018-q2, the compiler upstream expects):

```bash
npx nx build vesc   # bldc: make fw_a50s_v23c_12s -> bldc/build/a50s_v23c_12s/*.bin
npx nx test vesc    # arc/: host tests with CMake and CTest
```

Upstream's Makefile warns that the ARM SDK isn't in `bldc/tools`: ignore it. The container's GCC is on `PATH`; don't run `make arm_sdk_install`, which downloads a second copy.

Flash the `.bin` with VESC Tool over USB.
