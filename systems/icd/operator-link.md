# ICD-operator-link

- **Revision:** A (2026-11-10). Draft, started from the prototype's [`ground-station/protocol.md`](../../ground-station/protocol.md) (v1, #228).
- **Status:** Draft (#144). What other ADRs and ICDs already decide is recorded here with its source; everything else is TBC and listed in the open issues. Nothing is implemented on the Orin yet (`arc_safety`, #150).
- **Side A:** the ground station on the operator's laptop: the process that sends the stream (`ground-station/helper.py` in the prototype) (owner: TBD, open issue 1)
- **Side B:** `arc_safety` on the Orin (#150), the operator link's receiver (owner: Shrikar Vempati, until reassigned)
- **Traces to:** SYS-04, SYS-11, SYS-19, SYS-22, SYS-23, SYS-26, SYS-28, ADR-0015, ADR-0031, ADR-0039, ADR-0052, ADR-0055

The control stream between the operator's laptop and the car's Orin: the heartbeat that keeps the car allowed to drive (SYS-04), teleop, the re-arm, the control-mode and envelope requests, and what the Orin answers. `arc_safety` turns it into the car interface's topics (ICD-ros2-msgs); the Orin forwards the heartbeat and the re-arm to the sync MCU (ICD-sync-link).

Not in this ICD:

- **Telemetry and sensor streams** to the ground station: the bridge (#234), on the car interface's topics.
- **Simulator controls** (tracks, reset, ground truth): the sim-only interface ([`software/simulation.md`](../software/simulation.md)). The ground station behaves the same against the twin and the car.
- **The laptop's own sockets** between the page, the helper and the server: they stay in `ground-station/protocol.md`.

## Already decided

| What | Decided in |
| --- | --- |
| A fixed-rate UDP stream at 50–100 Hz with sequence numbers, marked DSCP EF and mapped to the router's WMM voice queue | ADR-0015, rule 4 |
| Control traffic has priority; bulk transfers run only while the car is parked | SYS-28, ADR-0015 |
| The laptop is 192.168.71.20 and the Orin 192.168.71.10 on the team network; the hotspot puts the Orin at 192.168.72.1 | ICD-network |
| The heartbeat reaches the sync MCU only through the Orin, so a hung Orin also stops the car | ICD-sync-link (2026-10-07) |
| The Orin forwards each heartbeat and re-arm with the laptop's counter passed through (`counter`, u32) | ICD-sync-link (`ARC_OPERATOR_HEARTBEAT`, `ARC_REARM`); ICD-ros2-msgs (`Heartbeat`, `Rearm`) |
| `arc_safety` is the only publisher of `/car/cmd/heartbeat` and `/car/cmd/rearm`, and publishes teleop on `/control/teleop` | ICD-ros2-msgs |
| No heartbeat for 150 ms (TBC) at the sync MCU: the car brakes on the e-stop ramp and stays stopped until heartbeats are back and the operator re-arms | SYS-04, ADR-0052 |
| The sync MCU accepts a re-arm only while its watchdog is `NOT_ARMED` and its latch `RESET`; the ground station offers the re-arm on the same condition | ICD-sync-link, open issue 7 |
| Every e-stop press latches; driving resumes after the release, the power-on checks and the operator's re-arm | ADR-0055 |
| A request for operator mode is always accepted; experiment mode only while the experiment is publishing | ICD-ros2-msgs, "Choosing who drives" |
| No teleop for 200 ms (TBC) in operator mode: `arc_control` sends the zero command | ICD-ros2-msgs (`teleop_timeout`) |

## Transport

| Item | Value |
| --- | --- |
| Link | The team network's Wi-Fi (6 GHz) to the Orin, or the car's hotspot (ADR-0015) |
| Protocol | UDP, both directions. The Orin replies to the address and port a packet came from |
| Port | TBD, open issue 3 (the prototype's car_link uses 9870). Once set, it goes in ICD-network |
| Marking | DSCP EF (`IP_TOS = 0xB8`) on both sides (ADR-0015) |
| Rate | Heartbeat and teleop at 50 Hz (TBC, open issue 4), within ADR-0015's 50–100 Hz |
| Datagram size | ≤ one Ethernet frame (no IP fragmentation) |
| Encoding | TBD, open issue 2. The prototype's is below |

## Messages

The prototype has the first five. The rest are what the car needs and the prototype doesn't have yet.

### Laptop → Orin

| Message | Content | Rate | `arc_safety` does | In the prototype |
| --- | --- | --- | --- | --- |
| Hello | The session ID, random per ground-station run | Every 0.1 s until acked | Starts the session (open issue 6) and acks it | Yes (`hello`) |
| Heartbeat | Counter | 50 Hz (TBC) | Publishes `/car/cmd/heartbeat` with the counter, for each heartbeat accepted | Yes (`heartbeat`; its `seq` is the counter) |
| Teleop | Speed (m/s, + forward), steering angle (rad, + left) | 50 Hz (TBC) while driving | Publishes `/control/teleop` (`DriveCommand`), stamped on arrival (open issue 8 for `accel`) | Yes (`teleop`) |
| Re-arm | Counter | On the operator's action | Publishes `/car/cmd/rearm` with the counter | No: the prototype's page says `DRIVABLE`, not `ARMED` |
| Set mode | Operator or experiment | On the operator's action | Calls `/control/set_mode` and returns the answer | No |
| Envelope | Max speed, acceleration and steering | Per session | Publishes `/car/cmd/envelope`, if the ground station is the envelope's owner (open issue 9) | No |

### Orin → laptop

| Message | Content | Rate | In the prototype |
| --- | --- | --- | --- |
| Ack | The session it answers | Per hello | Yes |
| Reject | The session, and why: no session (the Orin restarted), or replaced (another session said hello since) | At most one per 0.2 s per sender | Yes |
| Safety state | `/car/safety_state`: the watchdog, the latch and its cause, the e-stop, the envelope in force | On change, plus a fixed rate (TBC) | No: the sim sends its own link status over the bridge |
| Control mode | `/control/mode`, and the answer to the latest set-mode request | On change | No |
| Link status | What `arc_safety` received: gaps, duplicates, rejects, the age of the last heartbeat and teleop | 1 Hz (TBC) | Partly: car_link's `/sim/link/status`, over the bridge |

Whether the Orin's answers travel on this link or on the bridge is open issue 5.

### The prototype's encoding

Little-endian, 29 bytes, Python `struct` format `<4sBIIdff`, both directions:

| Field | Type | Meaning |
| --- | --- | --- |
| magic | 4 bytes | `ARC0`; anything else is dropped |
| type | u8 | `1` heartbeat, `2` teleop, `3` hello (laptop → car); `4` ack, `5` reject (car → laptop) |
| session | u32 | The sender's session ID; replies carry the session they answer |
| seq | u32 | Per-type counter from the sender, from 0, wrapping at 2^32. Reject: the reason, `1` no session, `2` replaced. Ack: 0 |
| t | f64 | The sender's monotonic clock, s; for logging, never compared across machines |
| speed | f32 | Teleop: speed, m/s. Others: 0 |
| steer | f32 | Teleop: steering angle, rad. Others: 0 |

## Sessions

From the prototype:

- **The handshake:** before any heartbeat, the laptop sends hello every 0.1 s, and only while the page's own heartbeats are fresh, until the Orin acks its session.
- **One session at a time.** The Orin accepts heartbeat and teleop only from the session it last acked; anything else gets a reject.
- **A new hello replaces the session.** Sequence numbers start over and the old session's teleop is forgotten, so the car gets no teleop until the new session sends it. In the prototype the newest ground station wins; what the car does is open issue 6.
- **A replaced ground station stops.** It stops sending to that car and says so; only a different target or a restart starts a new handshake, so two ground stations never take the car back and forth.
- **A restarted Orin rejects with "no session"** and the laptop says hello again.

## Sequence numbers

- Each message type has its own counter from the sender.
- **Latest wins:** a packet whose counter isn't newer than the last accepted one of its type (wrap-aware) is dropped, so a late or duplicated packet never applies. Nothing is retransmitted.
- **Gaps are counted** from the counters and reported in the link status (TBC). Whether they are also logged as faults is open issue 10 (SYS-19).
- **The heartbeat's counter is passed through** to the sync link unchanged (ICD-sync-link), so a log shows which laptop heartbeat the sync MCU last saw.
- **A flood can't stall the receiver:** each side reads at most 64 packets per 20 ms tick (the prototype's limit, TBC), so it still runs its timers. Heartbeats lost in a flood stop the car.
- **Values a message can't carry are dropped,** and so is a teleop field that is NaN or infinite (as `arc_control` does, ICD-ros2-msgs).

## When the link goes quiet

| What stops | What happens | Where |
| --- | --- | --- |
| The page (browser tab closed or hung) | The laptop stops sending heartbeats 150 ms after the page's last one (the prototype's helper) | Laptop |
| The laptop, its Wi-Fi or the router | No heartbeats reach the Orin, so `arc_safety` has none to forward | — |
| `arc_safety` is stalled | It stops forwarding, tied to its own deadline (#150) | Orin |
| The Orin or the car backend | Nothing reaches the sync MCU | — |
| No heartbeat at the sync MCU for 150 ms (TBC) | The car brakes on the e-stop ramp and stays stopped until heartbeats are back and the operator re-arms | Sync MCU (SYS-04, ADR-0052) |
| Heartbeats arrive but no teleop for 200 ms (TBC) | In operator mode, `arc_control` sends the zero command | Orin (ICD-ros2-msgs) |

The Orin forwards heartbeats; it doesn't make its own. Whether `arc_safety` also needs a timeout of its own, for example to report a quiet link before the watchdog trips, is open issue 7.

In the prototype, car_link does both timeouts itself (0.15 s each) and its own 5 m/s² ramp, and never needs a re-arm. On the car those are the sync MCU's and `arc_control`'s (ADR-0047).

## Open issues

1. **Owners.** Side A has no owner yet; the prototype is Yufeng Liu's (#228). "Done when" for #144 needs both sides named.
2. **The encoding.** MAVLink 2, as the sync link (ADR-0031): one schema tool, generated code, and message signing (open issue 11). It would need a Python generator in `tools/gen-interfaces.sh`, since nothing in Python uses the message set yet. The prototype's fixed 29-byte struct is simpler and already tested. If not MAVLink 2, record why (#144).
3. **The port.** Then add it to ICD-network.
4. **The rate.** 50 Hz in the prototype; ADR-0015 allows up to 100 Hz. The 150 ms watchdog (TBC) tolerates about 6 lost heartbeats in a row at 50 Hz, 14 at 100 Hz. To be set with the link's measured gaps (rig R6), which also set `teleop_timeout` (ICD-ros2-msgs).
5. **Where the Orin's answers go:** on this link, so the safety state arrives with the heartbeat's priority, or on the bridge (#234) with the rest of the telemetry. The re-arm button depends on the safety state (ICD-sync-link, open issue 7).
6. **More than one ground station.** The prototype lets the newest hello win. Options: newest wins, the first session holds until it goes quiet, or only the operator's laptop (192.168.71.20) may connect.
7. **A timeout in `arc_safety`.** It forwards each heartbeat as it comes, so the sync MCU's watchdog is the only timeout on the heartbeat path. A separate one in `arc_safety` could report a quiet link, but must never keep the car alive.
8. **Teleop's acceleration limit.** `DriveCommand` has `accel`; the prototype's teleop doesn't. Either the laptop sends it, or `arc_safety` fills in a fixed value (the envelope's `max_accel`?).
9. **Who sets the envelope** for a session, and how: ICD-ros2-msgs, open issue 5. If it's the ground station, it goes through this link.
10. **Logging the link.** Gaps, rejects and session changes on the car's time base (SYS-19, SYS-23), so a heartbeat stop can be traced to the link.
11. **Who may send.** Packets aren't authenticated: anything that reaches the port can take the session with a hello, and then send heartbeats. Options include accepting only 192.168.71.20, MAVLink 2 message signing, or both. Like ICD-sync-link's open issue 6, this guards SYS-04, so it's a safety function: its own tests and a second reviewer.
12. **The teleop speed cap:** ICD-ros2-msgs, open issue 13, once the link's gaps are measured.
