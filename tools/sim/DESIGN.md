# Simulator design

The goal: run the real sync MCU firmware (E-30, Zephyr) against a simulated car, so safety behaviour can be tested before any hardware exists. The firmware is the thing under test; everything it talks to is faked.

This is a proposal, not a decision of the repo's owners. It follows ADR-0028: Zephyr `native_sim` is the runner.

## What the firmware talks to

| Interface | Peer | Spec in the repo | Fake | State |
| --- | --- | --- | --- | --- |
| Controller UARTs, 4 | Motor controllers (A50S) | ICD-controller-telemetry (draft) | `FakeController` | Done, and running against the firmware (`firmware_in_loop.sh`) |
| Sync link, UDP | The Orin | ICD-sync-link (draft) | `FakeOrin` | Done: sends heartbeat, commands, envelope, link status. `sim.py` prints what it received (message counts, the last `ARC_SAFETY_STATE`) and `firmware_in_loop.sh` checks it |
| Command CAN bus | Controllers' CAN side: receive set-current, set-brake and set-RPM commands, send `STATUS_1` at about 50 Hz | `can-command.dbc` | `FakeCommandBus` | Done: over UDP through a simulation-only bridge on Zephyr's CAN loopback device (SocketCAN needs a `vcan` module and root). A controller that hears no command for 150 ms reports `COMMAND_TIMEOUT` on its UART |
| Steering CAN-FD | moteus-c1 | ADR-0019 only; the protocol is mjbots' and isn't in the repo | | Not started. Needs mjbots' protocol documentation |
| SPI sensors, 12 | 2 IMUs, 4 wheel, 4 suspension and 2 knuckle encoders | Only the sensor choices (BOM); no register-level spec, and the firmware drivers aren't written | | Not started |
| I2C ToF, 5 | Ride-height sensors | The part is TBD (E-26) | | Blocked |
| GPIO | E-stop in, LEDs, lighting modes, camera triggers and PPS out, low-battery signal out | BOM tally | | Not started: needs pins to drive and observe |
| Power-board header | Battery, cells, faults | ICD power-sync-stack not written; ADR-0022 proposed | | Blocked |
| Time | Orin PTP/PPS | ADR-0021 | | Out of scope |

Of the eight, three have specs that are complete enough to simulate faithfully today, and two of those are done. The rest wait either on specs the team hasn't written or on firmware that doesn't exist yet. A fake built ahead of its spec is a guess, so each one should be built when its interface is defined.

## How a fake reaches the firmware

The runner is `native_sim` (ADR-0028), decided 2026-10-17. The fakes are host programs that speak byte streams (`arcsim/transport.py`): a pseudo-terminal, a TCP serial port, a UDP socket. `native_sim` gives each controller UART a pseudo-terminal, and the sync link already runs over the host's sockets.

CAN and SPI are not done. Zephyr has emulators for both that `native_sim` can use (`can_loopback`, `spi_emul`, and a host SocketCAN driver). The simulator attaches to those, so no QEMU-specific bridge is needed. A sim-only driver layer backed by a serial channel would also run under QEMU, if that is wanted later; it is not planned now.

## Time

Two modes, both already in `Fleet.run`:

- **Virtual time**: the fakes jump from event to event. Instant and deterministic; the right place to check the 5 ms and 20 ms limits against captured byte streams.
- **Real time**: the fakes pace themselves against the wall clock. Fine for smoke tests, but Python and an emulator add jitter of several milliseconds, which is the size of the limits under test. Under QEMU use `-icount` so guest time follows executed instructions.

Checking a limit against firmware in the loop needs the guest's clock to drive the fakes. That is a lockstep protocol (the firmware, or the sim bridge, reports its time, and the fakes act relative to it). It is not built.

## Where the safety logic gets tested

The strongest tests need no simulator: the any-corner stop and the watchdog written as functions of (time, received frame), with the sequences produced here (`--capture` writes them as golden byte streams). The simulator then proves the firmware wires those functions up correctly.

## Milestones

| | What | Needs | State |
| --- | --- | --- | --- |
| M1 | Fake controllers (UART) | ICD-controller-telemetry | Done |
| M2 | Fake Orin (UDP) | ICD-sync-link | Done |
| M3a | Firmware: parse the four controller links and decide when to stop (`controller_link_core`, `controller_links`), with tests in virtual time | M1 | Done: 21 unit tests; `firmware_in_loop.sh` passes 5 scenarios, 3 runs in a row |
| M3b | `safety.c` (envelope clamp, heartbeat watchdog, drive decision) and `safety_task.c`, fed by the fake Orin's heartbeat, commands and envelope; the firmware answers with `ARC_SAFETY_STATE` | M2 | Done: 22 unit tests; `firmware_in_loop.sh` checks 5 Orin scenarios. Still missing: brake commands on the CAN bus (M4) |
| M4 | The command CAN bus: controllers receive commands, answer with `STATUS_1`, brake on `COMMAND_TIMEOUT` | The firmware's CAN code | Done: `can_command_core` (11 unit tests), `can_buses`, `sim_can_bridge`, `FakeCommandBus` (10 tests); `firmware_in_loop.sh` checks that a heartbeat loss and a corner fault put brake frames on all four controllers, and that drive and zero commands never leave a gap. Missing: the STATUS_1 watch (SYS-25, ~60 ms) and bus-off, and how the sync MCU commands the brake *ramp* (see below) |
| M5 | Steering controller | mjbots protocol | Not started |
| M6 | Sensors, e-stop, camera triggers | Sensor driver specs; pins | Not started |
| M7 | Lockstep with the guest clock; CI job | | Not started |

## Decisions for the owners

1. Where it lives: `tools/sim/` here, or under `firmware/sync-mcu/`, and whether `tools/sim/firmware_in_loop.sh` and the Python tests become Nx targets with a CI job.
2. The silence window: 5 ms (TBC in SYS-25) is what the unit tests check. A real-time run on a laptop can't hold it, so the integration run uses 100 ms. Whether a real-time check of 5 ms is wanted (on hardware, rig R1) is for the verification plan.
3. Who reviews the firmware: the any-corner stop is a safety function, and the repo requires a second reviewer.
4. What the sync MCU does at the stop (brake commands need the CAN bus), and the reset sequence of ADR-0039, neither of which exists yet.

## Addresses

`addresses_in_loop.sh` runs the sync link on the proposed 10.73.1.0/24 addresses across two containers. It does not check: the real switch, ARP conflicts, or the Orin's other interfaces (Wi-Fi, hotspot) overlapping the subnet. The firmware does not check who sent a datagram, so a message from the wrong address is accepted.

## Not verified

- The decision is made, logged, reported to the Orin and put on the command bus. On the bench it reaches all four fake controllers about 5 ms after a corner fault (1 run, laptop jitter included); SYS-25's 20 ms is not asserted, because a laptop can't hold it.
- The spec does not say how the sync MCU commands the brake. `can-command.dbc` has only the stock VESC frames (current, brake current, rpm), and the ramp lives in the controllers (ADR-0012). The firmware sends `SET_CURRENT_BRAKE` at `ARC_CAN_BRAKE_CURRENT_MA`, which has no value in the spec (TBD): with none set, it sends nothing on a stop and the controllers brake on their own 150 ms timeout, later than SYS-25's 20 ms. The simulator build uses 10 A as a placeholder. Likewise `ARC_CAN_ERPM_PER_MPS` (wheel, gear ratio and pole pairs are still changing: ADR-0026, 0034, 0040) is 0 on the car, so it sends 0 A; the simulator uses the fake controller's model.
- The command bus carries no STATUS_1 watch (SYS-25: ~60 ms) and no bus-off handling yet; the frames are counted and nothing acts on them.
- `watchdog` in `ARC_SAFETY_STATE` has no value for "no heartbeat yet"; the firmware reports ROLLING.
- The nucleo and arc_sync boards build but have no controller UARTs, so they use a stub.
- Real-time runs on a loaded machine can add jitter; the firmware-in-the-loop test uses a 100 ms window for that reason.
- The motor model is plausible, not calibrated.
