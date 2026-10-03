# ADR-0021: Time sync: µs-critical sensors timestamped in the sync MCU's clock; Orin synced in software

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-07, SYS-23, SYS-29, RSK-01, RSK-15
- **Supersedes:** ADR-0020

## Context

ADR-0020 relied on PTP hardware timestamping on the Orin. RSK-15 found the Orin NX doesn't support it: NVIDIA states PTP isn't supported on Orin NX, and its Ethernet (Realtek RTL8111) has no PTP clock. SYS-07 still requires ≤ 10 µs between sensors (LiDAR ≤ 1 ms).

SYS-07 is about sensors lining up with each other. It only depends on the Orin's clock for sensors the Orin timestamps itself.

## Decision

Every µs-critical sensor is timestamped in the sync MCU's clock:

| Sensor | Timestamped by | Alignment |
| --- | --- | --- |
| IMUs, wheel/suspension/knuckle encoders, ToF, VESC and steering telemetry, cell voltages | Sync MCU, at the pin or on frame receipt with the sender's sample counter | ~µs |
| Frame cameras | The sync MCU's **trigger time**, paired with the frame by sequence number; Orin arrival time ignored | ~µs |
| Event cameras (GenX320) | The sync MCU sends a periodic pulse (~100 Hz) to each sensor's **Trigger In**; the sensor timestamps each edge (1 µs resolution) into the event stream, mapping its clock onto the sync MCU's | ~µs |
| LiDAR (Hokuyo, Ethernet) | Orin on arrival, mapped to sync-MCU time | ≤ 1 ms |
| Operator commands, policy outputs (SYS-23) | Orin, mapped to sync-MCU time | ~ms |

- **The Orin's clock** follows the sync MCU's to ≤ 1 ms by software: PTP in software-timestamping mode, with the PPS line into an Orin GPIO as a cross-check.
- **Ethernet switch (E-06):** ordinary unmanaged switch; no PTP support needed.
- **Optional:** an Intel i210/i226 NIC on the carrier fork, if the Orin NX has a spare PCIe x1 lane (EE to check against the Antmicro design), wired point-to-point to the sync MCU. This gives hardware PTP (~1 µs) for the Orin's own clock. Not required for SYS-07.

## Consequences

- The camera pipeline pairs frames with trigger times by sequence number. The event pipeline converts sensor time to sync-MCU time using the trigger events.
- **Each GenX320 MIPI module must break out the sensor's Trigger In pin** to a connector, wired to a sync-board trigger output (RSK-01 vendor questions).
- The sync board's trigger outputs: 4 camera triggers (stereo, quad, two event cameras) plus spare, at sync-MCU precision.
- RSK-15 is resolved by design.
