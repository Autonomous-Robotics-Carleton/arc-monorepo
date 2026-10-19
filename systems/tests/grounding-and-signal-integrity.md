# Ground-return and signal-integrity checks

- **Owner:** kunshpreet
- **Status:** Not run
- **Verifies:** SYS-19, SYS-24, SYS-31; RSK-03 / RSK-12; review evidence for ADR-0041 and issues #94 / #115
- **Rigs:** R2 motor bench and final harness; FD checks also use the [RSK-03 procedure](rsk-03-canfd-bench.md)

## Before power

Document all rated power returns, local SPI references, CAN-reference paths, stacking-header returns, Aux feeds, USB cables, scope/PSU earth, shield bonds and conductive mounts. Compare continuity against the return schematic with capacitors discharged. Record every extra conductive connection, its purpose and whether it creates a parallel motor-current path. A galvanic-isolation claim requires the complete boundary, not just an isolator part number.

Measure actual branch resistance with a method appropriate to the small resistance (lead resistance accounted for; Kelvin measurement if needed). Calculate expected DC drop at the peak current from the power scenario. The resistance, transient allowance and acceptance margin are **TBD** until the actual harness / selected receiver limits are known.

## Loaded checks

1. Use a reviewed secured load fixture, protected motor bus and differential / isolated measurement equipment where needed. Record the test instrument grounds; do not let the measurement setup bypass an intended isolation boundary or introduce a second return path.
2. Measure ground offset / switching transients between each A50S-local domain and sync ground during startup, acceleration, sustained load, regen and stop. Capture CANH/CANL relative to the local receiver reference and the differential waveform; verify the exact transceiver's specified operating limits with a documented margin. Capture all four UART TX/RX signals relative to the receiving endpoint ground; verify 3.3 V high/low and timing/noise margins, reference current and unpowered-node behavior.
3. Run classic commands, all four UART status links at 2 Mbit/s 8N1 and steering FD at its 1 / 5 Mbit/s BRS baseline, with every sensor and Ethernet active. Record UART framing/CRC/sequence errors, RX overrun, CAN TEC/REC/bus-off, timestamps and measured load/latency. Qualify the actual installed harness and coherent ≥1 kHz streams, not only nominal baud.
4. Repeat with the final harness / shield / mounting configuration and permitted wall, battery and Aux states. Verify unpowered-node behavior and absence of backfeed. Controlled loss-of-return / crossing faults require the reviewed fault-injection setup; do not disconnect a loaded power return by hand.
5. If margins fail, revise the return topology or complete the isolation alternative under ADR-0041. Repeat affected tests at the selected UART and steering operating rates. Never add a thin ground wire without reevaluating current paths.

## Results

| Link / power state | Return path / resistance | Ground offset and transient capture | Receiver logic / common-mode / timing margin | Errors, load, latency | Decision / reviewer |
| --- | --- | --- | --- | --- | --- |
| Classic commands | TBD | TBD | TBD | TBD | Not run |
| Four UART links (each controller independently) | TBD | TBD | TBD | TBD | Not run |
| Steering FD | TBD | TBD | TBD | TBD | Not run |
| Power-sync monitoring / shutdown | TBD | TBD | TBD | TBD | Not run |
| Wall / battery handover and stop / Aux states | TBD | TBD | TBD | TBD | Not run |

Pass: the approved return/isolation drawing matches assembly; every endpoint has documented electrical/timing margin, required telemetry and error logging pass, and no rail reset or unintended return/backfeed occurs under the verified states. Hardware findings and any changes affecting stop functions require the safety review in the fab gate.
