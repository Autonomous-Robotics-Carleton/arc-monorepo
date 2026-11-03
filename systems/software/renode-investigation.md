# Renode: running the real ARM sync-MCU firmware

- **Status:** Investigation done (2026-10-29), #178. The recommendation at the end is a **proposal for the owner**, not a decision.
- **Question:** can Renode run our real ARM build of the sync MCU firmware (ADR-0017), and do its STM32H7 models cover the peripherals the firmware will drive (E-30: Ethernet, FD-CAN, SPI, timers, GPIO)? [`simulation.md`](simulation.md) lists this as an open question.
- **Traces to:** SYS-33, ADR-0028, ADR-0047, ADR-0049
- **Reproduce:** [`tools/renode/`](../../tools/renode/) (a NUCLEO-H723ZG platform and a script that runs the firmware on it)

## Answer

**Yes for the NUCLEO-H723ZG build, with gaps.** Renode 1.17.0 boots the unmodified `nucleo_h723zg` ELF and prints every line the firmware logs at startup. Renode has no STM32H723 platform, so ours reuses its STM32H7 model (written for the H743/H753) with five changes in [`nucleo_h723zg.repl`](../../tools/renode/nucleo_h723zg.repl). On that platform, upstream Zephyr's own driver tests built for `nucleo_h723zg` pass for FD-CAN (all three controllers, classic and FD), Ethernet (TCP and UDP, between two emulated boards and to the host), PTP, GPIO and timers. SPI transfers complete, but nothing answers on the bus, and no models exist for our SPI sensors.

The `arc_sync` build also boots, and showed a real bug in it (below).

## What was run

All builds ran in the dev container's toolchain (Zephyr v4.4.2, Zephyr SDK 1.0.1, as pinned in `.devcontainer/Dockerfile` and `firmware/sync-mcu/west.yml`). Renode ran on the host, outside the container.

| Item | Version or source |
| --- | --- |
| Renode | 1.17.0 portable for Linux (`renode-1.17.0.linux-portable.tar.gz` from the GitHub release, sha256 `4ba7c68b59e2447f188ef4b4b112fcccd2802582c460beedceb63b84e5605a5f`); reports build `1.17.0+20260907gitf1dd1b4af`, .NET 8.0.12 |
| Our firmware | `firmware/sync-mcu/app` at this branch's base, built with `west build -b nucleo_h723zg` and `-b arc_sync`, as `nx build sync-mcu` does. Unmodified |
| Peripheral tests | Upstream Zephyr v4.4.2 tests and samples, built for `nucleo_h723zg` with no changes except where the table says |
| Starting platform | Renode's `platforms/boards/nucleo_h753zi.repl` and `platforms/cpus/stm32h7.repl` |

Runs used `renode --disable-gui --console`, Renode's logging UART analyzer on USART3, `emulation RunFor` for a fixed virtual time, and CPU hooks to read kernel variables where the firmware prints nothing.

## Results

| What | How it was tested | Result |
| --- | --- | --- |
| Boot, our `nucleo_h723zg` build | The firmware as built | Boots; logs the banner and every module's start line on USART3, then runs its main loop. Needed one platform fix: Zephyr's backup-SRAM driver waits on a PWR register bit Renode's stub didn't set |
| Kernel clock | A hook on the main loop's 100 ms sleep, 60 s of virtual time | SysTick ran at 550 MHz: Renode's RCC model works it out from the PLL registers the firmware writes. Kernel time drifted 742 µs behind virtual time over 60 s; the cause wasn't looked into |
| Repeatability, one machine | The same 60 s run three times, paced to the wall clock and as fast as possible | The same virtual timestamp at every one of the 600 wakeups. As fast as possible, the 60 s took 5.1 s of host time with the firmware idle |
| Boot, our `arc_sync` build | The firmware as built, on the same platform | Boots to the main loop with SysTick at 64 MHz (the skeleton's internal oscillator), then panics. A real bug, not the emulator: see below |
| FD-CAN1, loopback | `tests/drivers/can/api` (CAN FD on by default for this board) | 65 passed, 0 failed, 8 skipped (power management, transceiver, RTR, two too-low-bitrate cases) |
| FD-CAN3 | The same test with `zephyr,canbus` moved to FDCAN3 | Fails on Renode's H7 platform, which has no FDCAN3. Passes, with the same counts as FDCAN1, once FDCAN3 is added to the platform |
| FD-CAN between boards | `samples/drivers/can/counter` without loopback, two machines on a Renode CAN hub | Each board receives the other's counter every 0.5 s |
| Ethernet between boards | `samples/net/sockets/echo_server` and `echo_client`, two machines on a Renode switch, 10 s | 3,964 to 3,969 TCP and 41 to 46 UDP echoes over IPv4 and IPv6 in three runs, with no errors or warnings logged. The counts differ from run to run, so two connected machines don't repeat exactly. The client must start after the server is listening, or its first connection is refused |
| Ethernet to the host | `echo_server` (address moved off this machine's own subnet) on a host TAP interface | Ping from Linux 5 of 5; a UDP datagram from Linux `nc` echoed back |
| PTP | `samples/net/ptp`, one machine | The port becomes grandmaster and sends Sync and Follow_Up messages. Timestamps weren't checked against a second clock |
| GPIO | `samples/basic/blinky` | The board's green LED model (PB0) toggles every second |
| Timers | `samples/drivers/counter/alarm` on TIM2, with upstream's STM32H735G-DK overlay | Alarms fired 10% late (2 s at 2.2 s): Renode fixes the timer clock at 250 MHz, and the NUCLEO's is 275 MHz. With 275 MHz set in the platform, alarms at 2 s, 6 s, 14 s and 30 s, as set |
| SPI1 | `tests/drivers/spi/spi_loopback` (upstream has a NUCLEO-H723ZG overlay) | On Renode's H7 platform SPI1 isn't modelled, and the driver spins. With Renode's `STM32H7_SPI` model added at SPI1, 8 tests pass and 20 fail: nothing loops MOSI back to MISO, and a transfer takes no time (the timing test fails) |
| CAN to Linux SocketCAN | — | Not tested: this machine's kernel has no `vcan`. Renode has a `CreateSocketCANBridge` command for it |
| PWM | `tests/drivers/pwm/pwm_api` | Not tested: upstream has no NUCLEO-H723ZG configuration for it, and the build fails |

### The platform changes

All five are in [`tools/renode/nucleo_h723zg.repl`](../../tools/renode/nucleo_h723zg.repl), on top of Renode's NUCLEO-H753ZI:

1. **PWR:** CR2 reads back with the backup regulator enabled, so the backup-SRAM driver's wait finishes.
2. **Timer clocks:** 275 MHz instead of Renode's fixed 250 MHz, from the clock tree in Zephyr's NUCLEO-H723ZG devicetree.
3. **DWT cycle counter:** 550 MHz, the core clock.
4. **FDCAN3:** added; addresses and interrupts from Zephyr's `stm32h723.dtsi`.
5. **SPI1:** Renode's `STM32H7_SPI` model added at SPI1's address.

## What Renode doesn't cover yet

| Gap | Matters for |
| --- | --- |
| No STM32H723 platform upstream; ours is the H743/H753 model with changes, so other H723-only differences (memory sizes, TIM23 and TIM24, UART9 and USART10, OCTOSPI) aren't modelled | Anything that uses those parts |
| SPI2, SPI3, SPI5 and SPI6 aren't modelled. SPI mode, speed and timing aren't modelled on any SPI | The IMU and angle-sensor buses (E-20 to E-24); which SPI instances the car uses is TBD (`arc_sync.dts`) |
| No device models for our sensors: none for the AS5047P (E-22 to E-24) or a VL53L4 ToF (E-26). Renode has an LSM6DSO IMU model; whether it is close enough to the ISM330DHCX (E-20, E-21) is TBD | Testing the sensor drivers without writing models |
| Timer input capture and output compare to pins: not tested | Camera triggers and PPS (ADR-0021) |
| USART links to the motor controllers: the USART model is used for the console only; the other links weren't tested | Controller telemetry (ADR-0034) |
| The emulated PHY reports a 10 Mbit/s half-duplex link | Nothing found so far; noted in case link speed matters to a test |
| The `arc_sync` platform: its clocks run from the 64 MHz internal oscillator until the crystal is chosen, and its pins are TBD, so it has no platform file of its own yet | Running `arc_sync` builds |

## A bug found in the `arc_sync` build

The `arc_sync` firmware panics as soon as its main loop first sleeps. `app/prj.conf` turns on logging and asserts (`CONFIG_LOG=y`, `CONFIG_ASSERT=y`), and the board's defconfig turns off the console and serial, so the build has no log backend. Zephyr's logging thread asserts that at least one backend exists. The assert fails and the kernel halts. The ELF confirms it: its log-backend list is empty, and the `nucleo_h723zg` build's has one entry.

The same would happen on the real board. It's a firmware fix (no logging on `arc_sync` until its console exists, or another log backend), and the choice is the firmware owner's. It isn't made here.

## How this fits the twin

The twin runs the firmware on `native_sim` (ADR-0047), which exercises our code but not the STM32 drivers: [`simulation.md`](simulation.md)'s "Not exercised" column lists the FD-CAN, Ethernet, SPI and timer peripherals. Renode covers part of that column:

- **It runs the drivers that will ship.** The Ethernet, FD-CAN and timer drivers tested above are the STM32 drivers in the `nucleo_h723zg` build, not host substitutes.
- **It has its own virtual clock.** A one-machine run repeats exactly (two machines on a network didn't, above), and Renode ships a C client (`tools/external_control_client`) that advances time step by step (`renode_run_for`) and reaches GPIO, CAN, SPI and ADC peripherals from outside. That is the kind of control ADR-0049's lockstep needs. It wasn't tested here.
- **It doesn't replace `native_sim`.** The sensor and controller models planned for `native_sim` would need Renode versions, and the twin's network would need a bridge to Renode (the TAP above).

## Proposal for the owner

> [!IMPORTANT]
> A proposal, not a decision: what to use Renode for, if anything, is the owner's call.

- **Use Renode for driver-level CI of the `nucleo_h723zg` build** once #175 turns on Ethernet and FD-CAN in our application. Our firmware would then run its own sync link and CAN buses on the emulated board: two machines, or one plus the ROS car backend over a TAP. It needs no hardware, and the platform in `tools/renode/` already passes Zephyr's tests for both.
- **Keep `native_sim` as the twin's firmware target** (ADR-0047) for now. Renode in the twin's lockstep would need every device model rewritten for Renode and its clock driven by the simulator. That's worth an ADR of its own only if driver-level CI shows it pays off.
- **Fix the `arc_sync` logging panic** (firmware owner's choice of fix), and add an `arc_sync` platform once its pin map and crystal exist.
- **Offer the H723 changes upstream** (FDCAN3, timer clocks, the PWR bit) to Renode, so this platform shrinks to the board's own parts.

## Open questions

| Question | Blocks |
| --- | --- |
| Which of the proposal's uses, if any, to adopt; if CI, which runner image carries Renode (`.devcontainer/` or a separate one) | Renode in CI |
| Why kernel time drifts behind Renode's virtual time (742 µs in 60 s) | Timing results from Renode |
| Whether Renode's LSM6DSO model can stand in for the ISM330DHCX, or a model must be written; the same for the AS5047P and the ToF part (TBC, ADR-0007) | Sensor driver tests in Renode |
| Whether Renode's SocketCAN bridge carries CAN FD frames to the twin's motor-controller model | Mixing Renode firmware with the twin's device models |
| Whether a Renode TAP can join the twin's Docker network at the car's addresses (ICD-network) | Renode firmware in the twin's network |
