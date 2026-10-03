# ADR-0015: Ground link: team router, laptop gateway, button hotspot, wired service port

- **Status:** Accepted
- **Date:** 2026-10-03
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-11, SYS-26, SYS-27, SYS-28, ADR-0005, RSK-09, RSK-14
- **Builds on:** ADR-0005 (Wi-Fi is the only wireless link; ExpressLRS considered on 2026-10-03 and not adopted)

## Context

Manual driving, telemetry, SSH and log transfer all run over Wi-Fi to a laptop. The operator's laptop must keep its own internet while connected to the car (SYS-26). The car must be reachable whether or not any outside network exists (SYS-27). The track is in a school, where the building Wi-Fi typically needs a user login or device registration, isolates clients from each other, and is crowded on 5 GHz.

## Options

| Option | SYS-26 laptop internet | SYS-27 reachable without outside network | Notes |
| --- | --- | --- | --- |
| Car joins school Wi-Fi | Yes | No | Login and registration, client isolation, uncontrolled latency |
| Car hotspot only | No: the laptop's one radio leaves school Wi-Fi | Yes | |
| Second radio on the car for internet | Yes | Yes | School network likely won't accept the car; extra hardware |
| **Team router + laptop wired + laptop gateway for the car** | **Yes** | **Yes** | Chosen |

## Decision

**Networks**

| Network | Members | Carries | Internet |
| --- | --- | --- | --- |
| Team network (router) | Car (Wi-Fi, 6 GHz), operator laptop (Ethernet), other laptops | Teleop, heartbeat, telemetry, SSH, logs, camera streams | None of its own |
| Laptop's own Wi-Fi | Laptop only | The laptop's internet | Yes |

**Rules**

1. **The team network gives a default gateway only to the car.** Everyone else gets addresses but no gateway or DNS, so laptops keep using their own Wi-Fi for internet (SYS-26). Implemented with a per-host DHCP option on the OpenWrt router.
2. **The car's gateway is the ground-station laptop**, which shares its internet onto the team network (IP forwarding + one NAT rule; a script for Linux and macOS). Windows' built-in internet sharing runs its own DHCP and conflicts with the router, so Windows laptops use the backup.
3. **Backup internet:** a phone tethered to the router's USB port. The router then gives the car its gateway itself.
4. **Control traffic has priority.** Teleop and the heartbeat are a fixed-rate UDP stream (50–100 Hz, sequence numbers), marked DSCP EF and mapped to the router's WMM voice queue. Bulk transfers (updates, log pulls) run only while the car is parked; the sync board knows when it is.
5. **Fixed addresses** for the car on every link.
6. **Car Wi-Fi power saving off.**

**Reaching the car with no outside network (SYS-27)**

| Path | How |
| --- | --- |
| Team router | Normal operation |
| **Button hotspot** | A momentary button on the car (with LED), wired to a GPIO on the Orin carrier's expansion header. Press: the car's M.2 card leaves the team network and becomes a 5 GHz hotspot with a fixed name and address, with no gateway handed out. Press again: back to the team network |
| **Wired service port** | A spare port on the car's Ethernet switch, exposed on the chassis, with a fixed address and no gateway |

**Equipment**

| Item | Choice |
| --- | --- |
| Team router | **GL.iNet Flint 3 (GL-BE9300), $209.99:** Wi-Fi 7 tri-band incl. 6 GHz, 5 × 2.5 GbE, USB 3.0 (tethering), OpenWrt. Mounted high at the track. Chosen for a clean 6 GHz channel in a crowded school, wired ports, and tethering |
| Travel router (later, optional) | GL.iNet Slate 7 (GL-BE3600), $159.99: USB-C powered for away events; no 6 GHz |
| Car Wi-Fi card | **MediaTek MT7922 (Filogic 330), M.2 2230 key E:** Wi-Fi 6E client on 6 GHz; hotspot (AP) on 5 GHz, one band at a time, no 6 GHz AP. Linux driver in mainline from kernel 5.16; JetPack 7.2.1 uses 6.8 (ADR-0016), so RSK-14 is closed. **Fallback:** Intel AX210 (works on JetPack's kernel; hotspot limited to 2.4 GHz) |

## Consequences

- The driving link runs on 6 GHz, which lowers interference and delay spikes (RSK-09) at the cost of range. That's acceptable indoors with the router high and in line of sight.
- The car has internet only when a sharing laptop or the router's phone link is present. It never needs internet to drive.
- The Orin carrier exposes one GPIO plus an LED output for the hotspot button. The Ethernet switch gives up one port to the chassis service port.
- Ground equipment becomes part of the system: router configuration (DHCP options, WMM, fixed addresses) and the laptop gateway script go into version control.
