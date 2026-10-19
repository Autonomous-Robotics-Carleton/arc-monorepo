# A50S arrival and UART qualification

- **Owner:** kunshpreet
- **Status:** Not run; hardware unavailable for this source audit
- **Verifies:** SYS-05, SYS-13, SYS-19, SYS-24, SYS-29; integration items from [#100](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/100)
- **Rigs:** R2 loaded motor bench; progressive vehicle tests only after the bench passes
- **Read first:** [UART readiness review](../reviews/a50s-uart-readiness.md), [corner connector](../icd/corner-connector.md), [UART ICD](../icd/controller-telemetry.md)

## Unpowered inspection

Verify the active UART assignment under ADR-0034: PB10 is USART3 TX and PB11 RX. Actual connector contact numbers remain TBD until measured.

1. Identify all six supplied units, hardware revision, serial and heatsink option; mark four installed / two spare candidates. Record photographs and supplied connector / capacitor part details.
2. With power disconnected and capacitors discharged, establish connector mating face / latch orientation and GND contacts. Record the physical circuit map before connecting the UART harness; verify supplier evidence or continuity to the actual TX/RX pins.
3. Check PB10 TX / PB11 RX contacts, reviewed reference returns, Hall inputs, ESTOP and SWD against the actual board. Mark unresolved access **TBD**; a firmware macro is not a pass.
4. Check installed CAN termination and the approved removal method for intermediate nodes; inspect supplied XT30 bulk capacitors and their specified placement.
5. Meter each Castle 1010's sensor port for a thermistor (about 10 kΩ to GND at room temperature) and record its type and contacts; if there is none, fit the E-28 NTCs to the cans. Set the sensor type and curve in the VESC configuration. Confirm phase / Hall wiring before operation; validate calibrated temperature and telemetry during powered bring-up.

## Powered bring-up and recovery

1. Use a secured bench fixture, approved bus protection / clamp and a fused current-limited supply. Supply setting / current limit are **TBD** from the reviewed bring-up plan; motor is initially disconnected. Record scope / PSU / USB ground connections so the test does not add an unintended return path.
2. Measure the published 5 V and 3.3 V outputs. Confirm circuit 9/10 versus 11/12 orientation, supply direction, startup / power-off behavior and permitted load. Do not parallel controller outputs with each other or the ARC rails. Resolve the Aux-range discrepancy and reverse-feed behavior before feeding Aux.
3. Back up configuration, serial / calibration records and current hardware target. Flash **one spare first** with the reviewed `a50s_v23c_12s` build. Verify normal boot, VESC Tool and SWD recovery before modifying the other units; record tool / firmware revisions. No irreversible erase or calibration overwrite is part of this checklist.
4. Confirm Hall inputs, motor/FET temperature and ESTOP behavior. Enable reviewed 2 Mbit/s 8N1 UART firmware only after DMA/driver ownership, grounding and pin/connector review. Verify actual baud and 3.3 V signal levels at both ends; retain USB/CAN/upload and SWD recovery.

## Loaded integration

- Confirm phase/Hall order, sensored FOC, bidirectional operation, current calibration, loaded speed under the retained 9 m/s / 75,000 eRPM cap and temperature / airflow in the final deck envelope (ADR-0034). Record gearing / tire size and the actual limits; a no-load spin does not pass.
- Qualify four simultaneous 2 Mbit/s 8N1 UART streams on the final harness: full ≥1 kHz status per controller, unsigned baseline load 29.515% before additional traffic, measured UART ≤50%, coherent sample stamps, controller-to-sync clock alignment and ≤2 ms p99 sampling-to-Orin-driver latency. Verify ≤0.25 ms sync forwarding separately. Retain the ≥1 h full-load soak from the telemetry review; include framing/CRC errors, sequence gaps, RX overrun and reviewed stale/version/corrupt-frame injections. Steering FD has its [separate procedure](rsk-03-canfd-bench.md).
- Verify ESTOP, command/telemetry loss, steering loss and latched reset under SYS-25 / ADR-0039.
- Confirm Hall / phase extensions, locking connectors, strain relief, separation from data wiring and cooled CAD clearances before harness release.

## Evidence record

| Check | Unit / revision | Evidence / measured value | Pass criterion / remaining item | Result / reviewer |
| --- | --- | --- | --- | --- |
| Connector orientation and rail contacts | TBD | TBD | Provisional numbers replaced by verified map | Not run |
| UART TX / RX / reference contacts | TBD | TBD | Verified 3.3 V map preserving Hall / ESTOP / SWD | Not run |
| UART baud / DMA / motor timing | TBD | TBD | 2 Mbit/s 8N1; driver ownership and motor-loop deadlines preserved | Not run |
| Hall / thermistor / calibration | TBD | TBD | Actual mapping and curves match firmware | Not run |
| Spare flash and SWD recovery | TBD | TBD | Correct target; recovery demonstrated; calibration preserved | Not run |
| Loaded FOC, speed and cooling | TBD | TBD | ADR-0034 / RSK-02 limits met | Not run |
| Four UART streams / grounding / clock mapping | TBD | TBD | SYS-07 / SYS-24 / SYS-29 and RX electrical margins pass | Not run |
| Stop / reset | TBD | TBD | Braked stop and reset criteria under SYS-05 / ADR-0039 | Not run |

kunshpreet records the measurements and signs the interface result. A separate safety reviewer remains **TBD**; naming the interface owner does not satisfy the second-reviewer rule.
