# sim

Fake nodes around the sync MCU firmware, so it can be tested without the car. The design, and what is still missing, are in [DESIGN.md](DESIGN.md).

The fake nodes, both speaking the ARC MAVLink 2 message set generated from `systems/icd/sync-link.xml` at start-up (so they can't drift from the firmware's schema):

| Node | Plays | Interface | Spec |
| --- | --- | --- | --- |
| `FL` `FR` `RL` `RR` | One A50S motor controller each: `ARC_MOTOR_STATUS` at 1 kHz, `ARC_LINK_STATUS` at 1 Hz | One UART per controller | ICD-controller-telemetry (draft) |
| `can` | The four controllers' side of the classic CAN command bus: receives `SET_CURRENT`, `SET_CURRENT_BRAKE`, `SET_RPM`, sends `STATUS_1` at 50 Hz, times out after 150 ms without a command | UDP, through the firmware's simulation-only bridge | can-command.dbc, ADR-0035 |
| `estop` | The e-stop button on the car: sets the firmware's e-stop input, and every controller reports `ESTOP`, ignores the command bus and brakes; at T (2.4 s) the motor bus is cut and the controllers go silent, then boot about 200 ms after release (ADR-0012, ADR-0034, ADR-0039) | UDP, one byte, through the firmware's simulation-only bridge (`sim_estop.c`) | ADR-0039 |
| `orin` | The Orin's bridge: `ARC_OPERATOR_HEARTBEAT` at 50 Hz, `ARC_DRIVE_COMMAND` at 500 Hz (repeating the latest output between 100 Hz controller updates), `ARC_ENVELOPE_SET`, `ARC_LINK_STATUS` at 1 Hz | UDP | ICD-sync-link (draft; the rates and addresses in it are proposals) |

Not a safety function. The motor model is plausible, not calibrated.

## Use

Needs `pymavlink` (in the dev container). From the repo root:

```bash
python3 -m unittest discover -s tools/sim/tests -t tools/sim     # 37 tests, under a second

# no firmware: write each corner's byte stream to a file (virtual time, instant)
tools/sim/sim.py --capture /tmp/frames --duration 2 \
  --scenario tools/sim/scenarios/one-corner-silent.json

# against firmware: a UART per corner, and the sync link
tools/sim/sim.py \
  --link FL=/dev/pts/3 --link FR=tcp:127.0.0.1:5001 --link RL=... --link RR=... \
  --orin udp:52001:192.168.70.2:52000 --can udp:52101:127.0.0.1:52100 --scenario tools/sim/scenarios/one-corner-silent.json
```

A `--link` target is a device or pseudo-terminal path (what `native_sim` gives its UARTs), or `tcp:HOST:PORT` for a serial port that something else serves (QEMU's `-serial tcp::5001,server,nowait`). `--can` is the command bus through the `native_sim` firmware's bridge (UDP 52100 in, 52101 out). `--orin` is `udp:LOCAL_PORT:PEER_HOST:PEER_PORT`; the ICD ports are 52001 into the Orin and 52000 into the sync MCU. Real-time runs pace themselves against the clock; a reader that stops reading loses bytes, as a UART would, and `dropped` on the transport counts them.

## Against the firmware (native_sim)

`firmware/sync-mcu/app/boards/native_sim.overlay` gives the `native_sim` firmware four pseudo-terminal UARTs for the controllers. In the dev container, from the repo root:

```bash
tools/sim/firmware_in_loop.sh
```

It builds the firmware with a 100 ms silence window, starts it in real time, attaches a fake controller to each pseudo-terminal it prints, and checks the firmware's log and what the fake Orin received: all four healthy gives no stop; a corner silent for 300 ms, a fault code, `COMMAND_TIMEOUT` and `ESTOP` each stop the car and name the right corner and reason. The stop holds after the frames come back (ADR-0039). With the fake Orin: a command inside the envelope drives, one beyond it is clamped, a lost heartbeat brakes (and a returning heartbeat doesn't restart the car; a re-arm once it's back does, ADR-0052), commands that stop give a zero command, and a controller fault while driving brakes. The strict 5 ms and 20 ms limits are checked where time is injected: `firmware/sync-mcu/tests/controller_link`.

## On the car's addresses

`tools/sim/addresses_in_loop.sh` (on the host, needs Docker) puts the firmware and the fake Orin in two containers on a 192.168.70.0/24 network with no gateway, the firmware at 192.168.70.2 and the Orin at 192.168.70.1 (ICD-network, set 2026-10-25). It checks that each side reaches the other on its address and port, that the network has no default route, and that reserving port 52001 keeps it out of the ephemeral range while an explicit bind still works. `ARC_TEST_ORIN_IP=192.168.70.21` puts the Orin at another address, to show the checks fail.

## Scenarios

JSON, times in milliseconds. An event is for the fake Orin with `"node": "orin"`; any other is for motor controllers, `corner` being `FL`, `FR`, `RL`, `RR`, a list, or `"all"`.

```json
{"events": [
  {"t_ms": 500,  "node": "orin", "action": "command", "speed": 3.0},
  {"t_ms": 3000, "corner": "FL", "action": "silence", "duration_ms": 20}
]}
```

| Node | Action | Arguments | What the sync MCU should see |
| --- | --- | --- | --- |
| controller | `speed` | `mps` | The motor follows a target speed |
| controller | `silence` | optional `duration_ms` | A hung controller: no frames, and no gap in the sequence numbers |
| controller | `resume` | | Ends a silence |
| controller | `drop` | `count` | A lossy link: frames never sent, so the sequence numbers skip |
| controller | `corrupt` | `count` | Frames that fail the CRC |
| controller | `state` | `bits`: `ESTOP`, `COMMAND_TIMEOUT` or a number | The controller braking on its own (SYS-25) |
| controller | `fault` | `code` | A non-zero fault code (SYS-25) |
| controller, orin | `version` | `value` | A protocol version mismatch in `ARC_LINK_STATUS` |
| orin | `command` | `speed`, `accel`, `steer` (each optional) | A new controller output |
| orin | `command_stop`, `command_resume` | | Drive commands stop while the heartbeat goes on (ICD open issue 6) |
| orin | `heartbeat_stop`, `heartbeat_resume` | | The watchdog's trigger (SYS-04) |
| orin | `rearm` | | One `ARC_REARM`. The firmware drives only after one, sent while heartbeats arrive and once the fault latch is reset (ADR-0039, ADR-0052) |
| estop | `press`, `release` | | The e-stop button. The firmware boots latched, so every Orin scenario starts with a press held 3 s (T is 2.4 s), a release and a re-arm |
| orin | `envelope` | `max_speed`, `max_accel`, `max_steer` | One `ARC_ENVELOPE_SET` |
| orin | `drop`, `corrupt` | `count` | As for controllers, on the sync link |

## What it models

- Controller frames are 56 to 59 bytes. MAVLink 2 drops trailing zero bytes, so a frame with `corner`, `fault` and `state` at zero is 56 bytes; the ICD's 59 is the largest.
- Each controller stamps its frames with its own clock, offset and drifting against the sync MCU's, so the clock-alignment question (ICD-controller-telemetry open issue 1) can be tested. Controllers are not in phase with each other.
- The fake Orin follows the sync MCU's clock from what it receives. Until the first message arrives its timestamps use its own clock, since the Orin's clock isn't synced to the sync MCU's yet (ICD-sync-link).
- Both keep what the firmware sends them (`rx`, `rx_count`, `rx_gaps`, `rx_bad`); nothing asserts on it yet.
- A fixed `--seed` gives identical bytes.

## Assumptions

- MAVLink system and component IDs on the controller link are TBD in the ICD; the tool uses system 1 and component corner + 1. On the sync link it uses the ICD's TBC values: system 1, component 2 for the Orin.
- Only `ARC_LINK_STATUS` is defined from the sync MCU to a controller. Clock alignment messages are TBD.
- UART timing (2 Mbit/s) is not modelled: a pseudo-terminal or socket has no such limit.
