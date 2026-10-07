# ICD-sync-link

- **Revision:** draft outline (2026-10-07). What each side must agree on; the message catalogue and field layouts come once the encoding is chosen (ADR-0031, Proposed).
- **Status:** Draft. Needs an owner on each side and their sign-off.
- **Side A:** sync MCU firmware (`firmware/sync-mcu/`, the `sync_link` module; E-30) (owner TBD)
- **Side B:** the sync bridge on the Orin (`ros/`, the car backend of the car interface; ADR-0028) (owner TBD)
- **Traces to:** SYS-04, SYS-07, SYS-19, SYS-22, SYS-23, SYS-24, SYS-29, ADR-0021, ADR-0024, ADR-0028

Everything that passes between the sync MCU and the Orin, apart from time synchronisation, which uses standard PTP in software-timestamping mode with a PPS cross-check (ADR-0021). The same protocol runs on every target: the sync MCU's `native_sim` build speaks it to the bridge on a laptop (ADR-0028).

## Transport

| Item | Value |
| --- | --- |
| Link | Ethernet, sync MCU ↔ switch (E-06) ↔ Orin (ADR-0023, Proposed) |
| Protocol | UDP, both directions |
| Addresses and ports | TBD: fixed addresses on the car's internal network |
| Datagram size | ≤ one Ethernet frame (no IP fragmentation) |
| Encoding | TBD (ADR-0031, Proposed) |

## Every message carries

| Field | Why |
| --- | --- |
| Protocol version | Mismatched versions are rejected and logged, never guessed at |
| Message type | Which payload follows |
| Sequence number (per message type, per direction) | UDP can drop or reorder; gaps are counted and logged as faults (SYS-19) |
| Sample time, in sync-MCU nanoseconds | When the data was sampled, not sent (SYS-07, SYS-29, ADR-0021) |

## Sync MCU → Orin

| Message family | Content | Rate | Timing |
| --- | --- | --- | --- |
| Vehicle-state samples | IMUs, wheel, suspension and knuckle encoders, ride-height ToF | Each sensor's full useful rate (SYS-24) | Stamped at the pin; forwarded within ≤ 0.25 ms (ADR-0024) |
| VESC telemetry | Each corner's status frames from the telemetry buses | ≥ 1 kHz per corner (ADR-0011) | Stamped at sampling on the VESC (ADR-0024), mapped to sync-MCU time |
| Steering telemetry | moteus-c1 angle and torque | ≥ 1 kHz (ADR-0019) | As above |
| Camera triggers | Trigger time and frame sequence number per camera | Each frame | The Orin pairs frames with trigger times (ADR-0021) |
| Power | Battery V/I, rail currents, cell voltages | ≥ 100 Hz (SYS-32) | Path per ADR-0022 (Proposed) |
| Safety state | Envelope in force, watchdog state, e-stop status | On change, plus TBD Hz | — |
| Faults and events | Bus errors, sequence gaps, watchdog trips, envelope clamps, brownouts | On occurrence | Stamped when detected (SYS-19) |

## Orin → sync MCU

| Message family | Content | Rate | Handling |
| --- | --- | --- | --- |
| Drive and steering commands | Speed, acceleration, steering angle | TBD (control rate) | Passed through the safety envelope before any CAN command (SYS-22) |
| Operator heartbeat | Forwarded from the laptop's heartbeat stream (50–100 Hz, ADR-0015) | 50–100 Hz | Feeds the watchdog; lost heartbeat → braking (SYS-04) |
| Envelope settings | Session limits for speed, acceleration and steering | Per session | Default 3 m/s for new experiments (SYS-22) |

Commands are "latest wins": a dropped command is replaced by the next one, never retransmitted.

## Open issues

1. Encoding and message catalogue (ADR-0031).
2. Addresses and ports.
3. Heartbeat path: via the Orin (as above) or straight from the laptop to the sync MCU. Via the Orin means a hung Orin also stops the car, which is fail-safe.
4. Command rate and the safety-state rate.
5. Owners on both sides.
