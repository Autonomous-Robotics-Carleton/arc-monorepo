# ICD-carrier-sync

- **Revision:** B (2026-10-13)
- **Status:** Draft logical interface; carrier revision, module contacts, GPIO mapping and connector pinout TBD
- **Side A:** custom Orin carrier / Orin platform, E-02 (owner TBD)
- **Side B:** sync MCU board / firmware, E-30 (owner TBD)
- **Traces to:** SYS-05, SYS-07, SYS-08, SYS-19, SYS-24, SYS-29, SYS-30, ADR-0021, ADR-0022, ADR-0023, ADR-0025, ADR-0031, ADR-0039, RSK-01
- **Related work:** [#102](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/102); physical release depends on #98 / #101

The carrier is the Antmicro fork under ADR-0025 (v1.1; v1.0 is below). Freeze its revision before assigning module / connector contacts. This interface controls timing, Ethernet and the proposed shutdown handshake; CSI camera data, motor commands and the power-board stack have separate interfaces. This document does not choose camera modules or approve a device tree.

## Signal and transport inventory

| Signal / link | Direction | Recorded requirement / proposed use | Remaining electrical assignment |
| --- | --- | --- | --- |
| Ethernet | Sync MAC/PHY ↔ E-06 switch ↔ carrier / Orin | Sync UDP/MAVLink data under ADR-0031; software time mapping under ADR-0021. Separate Ethernet connections to the common switch, not an assumed direct GPIO/header connection | PHY/magnetics/connector, switch power and addresses. Physical routing / cable length TBD; encoding in sync-link |
| PPS_SYNC | Sync → carrier GPIO → Orin timestamp capture | PPS cross-check of software clock alignment; Orin mapping ≤1 ms under ADR-0021. Store observed edges in the timing evidence | Module/carrier GPIO, voltage domain / translator, polarity, pulse width, termination/filtering and device tree TBD |
| FRAME_TRIGGER_STEREO | Sync → carrier routing → stereo module trigger | Sync timestamp/sequence paired to camera frame; part of four trigger outputs plus spare | Exact module input, level / polarity / pulse width, shared stereo triggering and cable contacts TBD |
| FRAME_TRIGGER_QUAD | Sync → carrier routing → quad kit trigger | Same acquisition-time/sequence contract | Exact kit trigger access / synchronization, levels and contacts TBD |
| EVENT_SYNC_FRONT | Sync → carrier routing → forward GenX320 Trigger In | Periodic pulse (~100 Hz) timestamped in sync-MCU time (ADR-0021) | Module Trigger In breakout, voltage, polarity, pulse width, contacts and driver support TBD; RSK-01 |
| EVENT_SYNC_DOWN | Sync → carrier routing → downward GenX320 Trigger In | Same contract as the forward event camera; lighting timing TBC | Module Trigger In breakout, translator, connector and ground-optics timing TBD; RSK-01 / RSK-07 |
| TRIGGER_SPARE | Sync → reserved contact | Spare in the sync I/O tally | No load or pin assignment selected |
| ORIN_SHUTDOWN_REQ | Sync → reviewed carrier input | Proposed ADR-0022 low-battery shutdown request, plus platform message | Confirm actual module/carrier power-button or equivalent input and allowed levels; driver, polarity and duration TBD |
| ORIN_HALTED / POWER_REMOVE_OK | Reviewed carrier/platform output → power board; sync observes if required | Proposed confirmation before normal compute-rail removal under SYS-30 | Confirm actual signal and whether it means halt / shutdown request / power removal permission. Module contact, translation, route and timeout TBD |
| Signal reference / shield | Per actual carrier / sync connection | Follows proposed ADR-0041 return map; no motor current through this interface | Connector reference contacts and shield/chassis bonds TBC after grounding review |

Physical pin numbers / connector MPNs are **TBD**. GPIO-level translation must match the frozen module/carrier guide and each sensor module; do not connect sync's 3.3 V outputs to an unverified carrier or camera input. Resolve load capacitance, edge rate, protection and both-side power-off behavior before routing.

## v1.0: the developer kit carrier

v1.0 uses the Orin Nano developer kit's own carrier (P3768) instead of E-02 (ADR-0046). The same logical signals apply:

- **Ethernet:** the dev kit's GbE port to the switch (E-06).
- **PPS_SYNC, ORIN_SHUTDOWN_REQ and the hotspot button and LED:** 40-pin header GPIOs (3.3 V, TBC from NVIDIA's carrier specification); pins and device tree TBD.
- **Camera triggers:** from the motherboard straight to the stereo kit's and the B0223's trigger inputs, not through the carrier.
- **ORIN_HALTED / POWER_REMOVE_OK:** which dev kit signal carries it is TBD (40-pin header or the J14 button header).
- **Power:** the motherboard's compute rail into the dev kit's barrel jack, 9–20 V (NVIDIA SP-11324-001 v1.3). By default the kit powers on when the rail comes up; a J14 jumper changes that (TBC whether v1.0 wants it).

## Timing and platform contract

- Sync records trigger acquisition time and sequence before forwarding to Orin. Orin arrival time does not replace trigger time. The camera pipeline pairs each frame to the correct sequence; missing edges/frames and time-map drift are logged. The frame cameras use frame triggers; the two GenX320s get a periodic pulse on Trigger In (ADR-0021).
- Use the sync-MCU time domain. Orin follows in software to ≤1 ms, with PPS as cross-check; camera/vehicle-state alignment remains governed by SYS-07 and end-to-end sample delivery by SYS-29. Exact GPIO capture and translator delays must be measured and accounted for.
- Optional Intel NIC / point-to-point Ethernet remains governed by ADR-0021 and proposed ADR-0023. This draft does not select it or silently reroute the baseline through a direct Ethernet header.
- UDP ports, MAVLink messages, schema version and link counters remain in [sync-link](sync-link.md) / `sync-link.xml`. No encoding change or generated-code edit is made here.
- GPIOs start in documented safe states, do not back-power an unpowered peer and never grant drive permission. The e-stop path remains separate; shutdown/reboot/network recovery cannot clear the ADR-0039 fault latch.

## Verification and release

Freeze the carrier source revision and attach a logical-net-to-MCU/module/contact map, translator/protection schematic and device-tree mapping. Confirm the four camera groups' trigger breakouts and drivers under RSK-01. Measure trigger/PPS latency/jitter and frame/sequence pairing; test loaded Ethernet, source timestamps and SYS-29 latency; test lost PPS, missing triggers, unplug/replug, asymmetric power, low-battery request and actual halted/power-removal handshake.

Assignments can become baselined only after electrical and platform sides approve the concrete connector and measured behavior. Camera-driver evidence / physical pin release remain outside this draft's completion claim.
