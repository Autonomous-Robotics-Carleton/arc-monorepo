# ICD-sync-link

- **Revision:** draft (2026-10-07). The message catalogue is drafted in [`sync-link.xml`](sync-link.xml) (MAVLink 2, ADR-0031); code is generated from it by `tools/gen-interfaces.sh`.
- **Status:** Draft. Needs an owner on each side and their sign-off.
- **Side A:** sync MCU firmware (`firmware/sync-mcu/`, the `sync_link` module; E-30) (owner TBD)
- **Side B:** the sync bridge on the Orin (`ros/src/arc_backend_car/`, the car backend of the car interface; ADR-0028) (owner TBD)
- **Traces to:** SYS-04, SYS-07, SYS-19, SYS-22, SYS-23, SYS-24, SYS-29, SYS-32, ADR-0021, ADR-0024, ADR-0028, ADR-0031, ADR-0034

Everything that passes between the sync MCU and the Orin, apart from time synchronisation, which uses standard PTP in software-timestamping mode with a PPS cross-check (ADR-0021). The same protocol runs on every target: the sync MCU's `native_sim` build speaks it to the bridge on a laptop (ADR-0028).

## Transport

| Item | Value |
| --- | --- |
| Link | Ethernet, sync MCU ↔ switch (E-06) ↔ Orin (ADR-0023, Proposed) |
| Protocol | UDP, both directions |
| Addresses and ports | Ports TBC: UDP 52000 into the sync MCU, 52001 into the Orin. Addresses TBD: fixed addresses on the car's internal network; 127.0.0.1 when the sync MCU runs as `native_sim` |
| Datagram size | ≤ one Ethernet frame (no IP fragmentation) |
| Encoding | MAVLink 2 with our own message set (ADR-0031) |
| MAVLink IDs | System 1; component 1 the sync MCU, 2 the Orin bridge (TBC; what the code uses) |

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
| VESC telemetry | Each controller's status frames from its UART link | ≥ 1 kHz per corner (ADR-0034) | Stamped at sampling on the VESC (ADR-0024), mapped to sync-MCU time |
| Steering telemetry | moteus-c1 angle and torque | ≥ 1 kHz (ADR-0019) | Reply built when the sync MCU's query arrives, stamped in sync-MCU time (ADR-0019, ADR-0024) |
| Camera triggers | Trigger time and frame sequence number per camera | Each frame | The Orin pairs frames with trigger times (ADR-0021) |
| Power | Battery V/I, cell voltages (rail currents TBD: not in the message set yet) | ≥ 100 Hz (SYS-32) | Path per ADR-0022 (Proposed) |
| Safety state | Envelope in force, watchdog state, e-stop status | On change, plus TBD Hz | — |
| Faults and events | Bus errors, sequence gaps, watchdog trips, envelope clamps, brownouts | On occurrence | Stamped when detected (SYS-19) |
| Link status | `ARC_LINK_STATUS`: protocol version, receive gaps and rejects | 1 Hz | Sent each way |

## Orin → sync MCU

| Message family | Content | Rate | Handling |
| --- | --- | --- | --- |
| Drive and steering commands | Speed, acceleration, steering angle | TBD (control rate) | Passed through the safety envelope before any CAN command (SYS-22) |
| Operator heartbeat | Forwarded by the Orin from the laptop's heartbeat stream (50–100 Hz, ADR-0015). Decided 2026-10-07: always via the Orin, so a hung Orin also stops the car | 50–100 Hz | Feeds the watchdog; lost heartbeat → braking (SYS-04) |
| Envelope settings | Session limits for speed, acceleration and steering | Per session | Default 3 m/s for new experiments (SYS-22) |

Commands are "latest wins": a dropped command is replaced by the next one, never retransmitted.

## Open issues

1. ~~Message catalogue~~ **Drafted** in `sync-link.xml`: one message per family above, except vehicle-state samples (IMU, angle and ride-height messages). Field lists to review with both owners.
2. Addresses: the car's fixed addresses. Ports proposed (TBC): 52000 and 52001.
3. ~~Heartbeat path~~ **Decided (2026-10-07):** via the Orin, never straight from the laptop. A hung Orin stops forwarding it, so the car stops (fail-safe).
4. Command rate and the safety-state rate.
5. Owners on both sides.
