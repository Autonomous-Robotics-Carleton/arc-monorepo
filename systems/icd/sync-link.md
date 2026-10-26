# ICD-sync-link

- **Revision:** B (2026-10-26). The message catalogue is in [`sync-link.xml`](sync-link.xml), version 3 (MAVLink 2, ADR-0031); code is generated from it by `tools/gen-interfaces.sh`.
- **Status:** Message set reviewed 2026-10-26 by the owner of both sides (#73). Open: who the sync MCU accepts messages from (open issue 6).
- **Side A:** sync MCU firmware (`firmware/sync-mcu/`, the `sync_link` module; E-30) (owner: Shrikar Vempati, until reassigned)
- **Side B:** the sync bridge on the Orin (`ros/src/arc_backend_car/`, the car backend of the car interface; ADR-0028) (owner: Shrikar Vempati, until reassigned)
- **Traces to:** SYS-04, SYS-07, SYS-19, SYS-22, SYS-23, SYS-24, SYS-29, SYS-32, ADR-0021, ADR-0024, ADR-0028, ADR-0031, ADR-0034

Everything that passes between the sync MCU and the Orin, apart from time synchronisation, which uses standard PTP in software-timestamping mode with a PPS cross-check (ADR-0021). The same protocol runs on every target: the sync MCU's `native_sim` build speaks it to the bridge on a laptop (ADR-0028).

## Transport

| Item | Value |
| --- | --- |
| Link | Ethernet, sync MCU ↔ switch (E-06) ↔ Orin (ADR-0023, Proposed) |
| Protocol | UDP, both directions |
| Addresses and ports | UDP 52000 into the sync MCU, 52001 into the Orin (set 2026-10-25). Addresses in [ICD-network](network.md): the sync MCU 192.168.70.2, the Orin 192.168.70.1 (set 2026-10-25); the same in the twin; 127.0.0.1 for single-machine tests |
| Datagram size | ≤ one Ethernet frame (no IP fragmentation) |
| Encoding | MAVLink 2 with our own message set (ADR-0031) |
| MAVLink IDs | System 1; component 1 the sync MCU, 2 the Orin bridge (set 2026-10-26, #73). The motor controllers are components 11–14 on their own links (ICD-controller-telemetry) |

## Every message carries

| Field | Why |
| --- | --- |
| Protocol version | Carried in `ARC_LINK_STATUS` (1 Hz each way) as the schema's `<version>`; a mismatch is logged as a fault, and the sync MCU refuses drive commands, so the car stays stopped, until both sides match (decided 2026-10-08). This is a safety function: its own tests and a second reviewer. A changed message definition is also rejected per message by MAVLink's CRC_EXTRA |
| Message type | Which payload follows |
| Sequence number (MAVLink's, per link and direction) | UDP can drop or reorder; gaps are counted from the sequence numbers (not from the MAVLink library's receive-error counters), reported in `ARC_LINK_STATUS` as `rx_gaps` and logged as faults (SYS-19). Frames rejected by CRC or as unknown messages are `rx_bad` |
| Time, in sync-MCU nanoseconds | `time_ns`, the first field of every message: for samples, when the data was sampled, not sent (SYS-07, SYS-29, ADR-0021); for link status, commands and heartbeats, as each message defines in `sync-link.xml` |

## Sync MCU → Orin

| Message family | Content | Rate | Timing |
| --- | --- | --- | --- |
| Vehicle-state samples | IMUs, wheel, suspension and knuckle encoders, ride-height ToF | Each sensor's full useful rate (SYS-24) | Stamped at the pin; forwarded within ≤ 0.25 ms (ADR-0024) |
| VESC telemetry | Full coherent snapshots from each controller’s 2 Mbit/s UART; firmware endpoints remain stubs | ≥ 1 kHz per corner (ADR-0034) | Stamped at sampling on the VESC (ADR-0024), mapped to sync-MCU time |
| Steering telemetry | moteus-c1 angle and torque | ≥ 1 kHz (ADR-0019) | Reply built when the sync MCU's query arrives, stamped in sync-MCU time (ADR-0019, ADR-0024) |
| Camera triggers | Trigger time and frame sequence number per camera | Each frame | The Orin pairs frames with trigger times (ADR-0021) |
| Power | Battery V/I, cell voltages. Rail currents get their own message once ADR-0022 is decided (#73) | ≥ 100 Hz (SYS-32) | Path per ADR-0022 (Proposed) |
| Safety state | Envelope in force, watchdog state, e-stop status, the fault latch's state and cause (ADR-0039, ADR-0052) | On change, plus 50 Hz (set 2026-10-25) | — |
| Faults and events | Bus errors, sequence gaps, watchdog trips, envelope clamps, brownouts | On occurrence | Stamped when detected (SYS-19) |
| Link status | `ARC_LINK_STATUS`: protocol version, receive gaps and rejects | 1 Hz | Sent each way |

## Orin → sync MCU

| Message family | Content | Rate | Handling |
| --- | --- | --- | --- |
| Drive and steering commands | Speed, acceleration, steering angle | As the commander sends them, up to 500 Hz, the command bus's rate (set 2026-10-25) | Passed through the safety envelope before any CAN command (SYS-22) |
| Operator heartbeat | Forwarded by the Orin from the laptop's heartbeat stream (50–100 Hz, ADR-0015). Decided 2026-10-07: always via the Orin, so a hung Orin also stops the car | 50–100 Hz | Feeds the watchdog; lost heartbeat → braking (SYS-04) |
| Envelope settings | Session limits for speed, acceleration and steering | Per session | Defaults for new experiments: 3 m/s (SYS-22), 5 m/s² (TBC) and the full steering range; reverse under the same speed limit (ADR-0052) |
| Re-arm | The operator's go after a reset (ADR-0039) or a heartbeat stop (ADR-0052), forwarded by the Orin like the heartbeat: `ARC_REARM` | On the operator's action | After a fault stop, accepted only after an e-stop cycle and passing power-on checks; after a heartbeat stop, once heartbeats are arriving again (ADR-0052). The first commands after it are zero and the envelope returns to its 3 m/s default (SYS-22) |

Commands are "latest wins": a dropped command is replaced by the next one, never retransmitted.

## Open issues

1. ~~Message catalogue~~ **Reviewed (2026-10-26, #73):** one message per family above in `sync-link.xml`, version 3. Added: the fault latch's state and cause in `ARC_SAFETY_STATE`, a status bitmask in `ARC_ANGLE_SAMPLE` (ADR-0038), the moteus-c1's mode in `ARC_STEERING_STATUS`, and `ARC_REARM`. Rail currents wait for ADR-0022.
2. ~~Addresses~~ **Set (2026-10-25):** ports 52000 and 52001; addresses in [ICD-network](network.md).
3. ~~Heartbeat path~~ **Decided (2026-10-07):** via the Orin, never straight from the laptop. A hung Orin stops forwarding it, so the car stops (fail-safe).
4. ~~Command rate and the safety-state rate~~ **Set (2026-10-25):** commands up to 500 Hz; safety state on change plus 50 Hz.
5. ~~Owners on both sides~~ **Set (2026-10-25):** Shrikar Vempati on both sides until reassigned.
6. Who the sync MCU accepts messages from. The link shares the car's switch (E-06) with the service port (E-09), so a laptop plugged in there can send to UDP 52000: drive commands, envelope settings and operator heartbeats, which would keep the watchdog (SYS-04) fed while the laptop is the one commanding. Nothing here restricts the sender yet. Options include accepting only the Orin's address, MAVLink 2 message signing, or a network the service port can't reach; none is chosen. Whatever is chosen guards SYS-04 and SYS-22, so it is a safety function: its own tests and a second reviewer. **Deferred (2026-10-25):** decided before the car is assembled; until then nothing restricts the sender.
