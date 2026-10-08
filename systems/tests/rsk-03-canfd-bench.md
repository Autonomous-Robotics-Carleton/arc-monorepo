# TEST: RSK-03 CAN-FD telemetry bus bench test

> [!NOTE]
> **Retired.** ADR-0034 replaced the drive CAN-FD telemetry buses with a UART per motor controller, so this bench test no longer gates any layout. Kept for the record and in case CAN-FD returns.

- **Retires:** RSK-03
- **Verifies:** ADR-0011, ADR-0013, ICD-corner-connector rev D, SYS-24 (bus load ≤ 70%)
- **Must pass before:** VESC fork layout
- **Owner:** TBD
- **Status:** Not started

## Objective

Show that one CAN-FD telemetry bus, built the way it will be on the car, runs at 5 Mbit/s with ~60% load and zero errors while a motor runs at full load beside it. Find the stub length and termination limits.

## Step 0: simulation (before buying anything)

1. Model the bus in LTspice: transceiver IBIS models (standard and SIC), harness as lossy transmission lines (~120 Ω differential, lengths from the CAD layout), JST-GH connectors as short impedance discontinuities, split termination at both ends.
2. Sweep the near-node stub length (1 cm daisy-chain, 5, 10, 20, 30 cm) and termination variants at 5 and 8 Mbit/s.
3. Record ringing amplitude and settling time against the data-phase sample point. Use the results to pick the bench sweep points.

## Equipment

| Role | Item |
| --- | --- |
| Sync board stand-in | NUCLEO-H723ZG + SO-8 CAN FD transceiver breakout |
| VESC stand-ins (×2) | MCP2518FD breakout + SO-8 transceiver, on any small MCU over SPI |
| Transceivers | Standard 5 Mbit/s CAN FD (TJA1051T/3 class) and SIC (TJA1462 / TCAN1462 class) |
| Harness | JST-GH 10-pin per ICD rev D, twisted pairs, lengths from the CAD layout; daisy-chain and stub variants |
| Termination | Split 2 × 60 Ω + ~4.7 nF at the sync end and the far end; plain 120 Ω for comparison |
| Error logging | USB CAN-FD analyzer (PEAK PCAN-USB FD or a CANable-class FD adapter) |
| Signal | Oscilloscope ≥ 200 MHz, CAN_H − CAN_L (differential probe or math channel) |
| Noise source | Stock VESC 6 driving a Castle 1010 under load (brake or dyno), phase leads routed along the harness |
| Ground shift | Adjustable DC offset or series resistance between node grounds |

## Configuration (all FD nodes identical)

- 40 MHz CAN clock
- Arbitration 1 Mbit/s, data 5 Mbit/s (swept 2 / 5 / 8)
- Transmitter delay compensation on
- Sample points per CiA 601-3 guidance; record the exact values used
- Traffic: each VESC stand-in sends 2 × 64-byte frames at 1 kHz (~60% bus load); the sync node sends a periodic sync frame
- Every payload carries a sequence counter and its own CRC

## Procedure

1. **Baseline:** daisy-chain, split termination, standard transceivers, 5 Mbit/s, motor off. 10 minutes.
2. **Sweep** one variable at a time, 10 minutes each: data rate; stub length (if not daisy-chained); termination variant; standard vs SIC transceiver.
3. **Stress** the best configuration:
   - motor at full load beside the harness
   - ground offset stepped to ±2 V and ±5 V
   - transceivers heated to ~85 °C
   - connectors wiggled or on a shaker
4. **Soak** the chosen configuration for ≥ 1 h (≥ 14 million frames) with the motor at full load.

## Record at every step

Error frames (analyzer), controller TEC/REC and bus-off events, sequence-counter gaps, payload CRC failures, scope capture of the differential signal at the data-phase sample point.

| Step | Rate | Topology / stub | Termination | Transceiver | Motor | Error frames | Lost frames | Max TEC/REC | Ringing settled before sample point? | Pass |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 5 Mbit/s | daisy-chain | split | standard | off | | | | | |

## Pass criteria

- Zero error frames and zero lost frames over the ≥ 1 h soak at 5 Mbit/s with the motor at full load.
- No bus-off; TEC/REC stay at 0.
- Differential signal settled before the data-phase sample point, with margin, on the scope.
- Stub limit and termination recorded and written into ICD-corner-connector.

## If it fails

Work down the ladder in ADR-0013: SIC transceivers → shorter stub or tuned termination → one FD bus per VESC (extra MCP2518FD on the sync board). Record the result and update RSK-03 and ADR-0013.
