# ADR-0031: The sync link uses MAVLink 2 with our own message set

- **Status:** Accepted. In practice mavgen generates C only (`tools/gen-interfaces.sh`): the Orin bridge, in C++, uses the same C headers, and nothing in Python uses the message set yet. Since ADR-0034 the same schema also encodes the controller telemetry links (ICD-controller-telemetry)
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-07, SYS-19, SYS-24, SYS-29, ADR-0017, ADR-0024, ADR-0028, ICD-sync-link

## Context

ICD-sync-link fixes what crosses between the sync MCU and the Orin, but not how it's encoded. The encoding must:

- be cheap and predictable on the MCU: several thousand messages per second, each forwarded within ≤ 0.25 ms (ADR-0024);
- carry a version, a message type, a sequence number and a sample time in every message (ICD-sync-link);
- be generated from one schema into C for Zephyr and C++ or Python for the Orin, never hand-written (ICD README);
- let the message set grow without breaking older builds where possible;
- be readable in a packet capture when something goes wrong.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Our own fixed-layout binary, generated from a schema we define | Fastest possible on the MCU; exactly what we need | We write and maintain the generator, the framing and the tooling |
| **B. MAVLink 2 with our own message set (dialect)** | Fixed-layout messages, cheap and predictable on an MCU; schema in XML with an existing generator for C, C++ and Python (`mavgen`); sequence numbers, framing and CRC built in; extension fields let messages grow compatibly; Wireshark dissector; widely used on robots | MAVLink's own conventions (system and component IDs) to carry along; payloads ≤ 255 bytes; no nested messages |
| C. Protocol Buffers (nanopb on the MCU) | Familiar; flexible evolution; tooling everywhere | Variable-length encoding costs more CPU per message on the MCU, and its timing depends on the values; no framing or sequence numbers of its own |
| D. CBOR via zcbor (built into Zephyr) | Already in Zephyr; schema (CDDL) code generation | Variable-length like C; less familiar; thinner tooling on the Orin side |
| E. micro-ROS (DDS-XRCE) on the MCU | The MCU publishes ROS 2 topics directly; no custom bridge | An agent on the Orin adds a hop and jitter against the 0.25 ms budget; heavy on MCU memory; ties the MCU to ROS |

## Criteria

1. Predictable, cheap encoding on the MCU (ADR-0024).
2. Generated from one schema for both sides, with existing tools.
3. Sequence numbers and framing, for loss detection (SYS-19).
4. Debuggable on the wire.
5. Can grow without breaking deployed builds.

## Decision

- **Option B:** MAVLink 2 with an ARC message set.
- **Where the schema lives:** `systems/icd/sync-link.xml`, next to ICD-sync-link.
- **Generated code:** `mavgen` produces the C headers for the sync MCU and the C++/Python for the Orin bridge. CI checks they match the schema.
- **The message catalogue** in ICD-sync-link is written as this XML.

## Consequences

- **Every message gets:** a sequence number and CRC from MAVLink itself, plus a sample-time field (`time_ns`, sync-MCU clock) in each of our messages.
- **Big payloads** (e.g. a full VESC status) must fit in 255 bytes, or be split.
- **System and component IDs** become fixed values for the sync MCU and the bridge.
- **To verify:** encode cost on the STM32H723 at the full message rate, before any other code depends on the format.
- **If B is rejected,** A is the fallback: the same fixed-layout properties, at the cost of writing our own generator.
- **Reopen if:** the 255-byte limit or MAVLink's conventions get in the way of the message set.
