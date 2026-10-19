# ICD-power-rails

- **Revision:** A (2026-10-08)
- **Status:** Draft contract; ratings, connector contacts and sequencing values remain open. No layout release or sign-off recorded.
- **Side A:** power board / motor distribution, E-41 (owner TBD)
- **Side B:** carrier, sync, sensors, steering and drive loads (owner TBD)
- **Traces to:** SYS-03, SYS-05, SYS-14, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32, E-41, ADR-0019, ADR-0022, ADR-0034, ADR-0039, ADR-0041, RSK-06, RSK-11
- **Related work:** [#102](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/102); ratings / schematic depend on #96, stop hardware on #95

This interface defines what every load and the power board must agree on. It carries the existing functional requirements from [electrical BOM](../bom/electrical.md), [power budget](../budgets/power.csv) and [power scenarios](../budgets/power-scenarios.md). Parts recommendations are not confirmed current ratings. Nominal voltage, tolerance, ripple, steady/peak current, startup load, fuse and contact assignments must be released together for each branch.

## Inputs and outputs

| Branch / proposed net | Source → load | Voltage / rating basis | Required behavior and unresolved values |
| --- | --- | --- | --- |
| BAT_IN | E-40 → power board | 4S, 12.0–16.8 V design range; loaded sag from SYS-31 | Pack choice, connector, wire gauge, fuse, reverse-polarity protection and minimum loaded voltage TBD |
| WALL_IN | E-44 → power board | Planned 19 V input; supply power ≥ combined low-voltage peak demand | Ideal-diode OR-ing, no unintended charge path into pack and no rail reboot during handover; connector / rating TBD |
| DRIVE_BUS_SW | Power board → four E-50 | Motor-bus voltage; updated TBC aggregate pack estimate 36–43 A at 9 m/s in `budgets/power-scenarios.md`. Drive-branch current/rating TBD; pack peak includes LV loads | Rated distribution with delayed e-stop cut, opens when unpowered; supplied A50S input caps; switch / inrush / clamp placement unresolved (#113 / #114). No control-board signal path may carry this return current |
| STEER_BUS_SW | Power board → E-31 | Motor-bus voltage; budget peak is an estimate pending actuator design | Separate fuse / switch; cut at drive T + TBC 1 s under ADR-0019; current / regen path TBD |
| COMPUTE | Power board → E-02 and compute assembly (v1.0: the developer kit's barrel jack, ADR-0046) | Fixed regulated voltage within the frozen carrier's input range and the dev kit's 9–20 V (NVIDIA SP-11324-001 v1.3); proposed converter voltage TBC | Rate for NX operating mode, carrier, SSD, Wi-Fi and actual fan branch. Exact input voltage / current, tolerance, inrush and hold-up TBD; SYS-30 clean shutdown before normal rail removal |
| SENSOR | Power board → Hokuyo E-10; switch E-06 if selected here | Proposed 12 V branch; UST-10LX accepts 10–30 V (12/24 V nominal), up to 150 mA running / 450 mA startup | Include LiDAR startup demand; actual switch allocation and additional current, total branch/fuse/contact ratings TBD |
| LOGIC_5V | Power board → sync / applicable loads | Nominal 5 V from the functional BOM | Final load allocation, regulator, tolerance, ripple, protection / sequencing TBD |
| LOGIC_3V3 | Power board → sync / applicable loads | Nominal 3.3 V from the functional BOM | Final local-regulator ownership and loads TBD; quiet IMU / reference supplies and switching-noise budget must be specified |
| A50S_AUX (optional) | Always-on source → controller Aux input | Triforce's page lists the Aux input as 6–52 V; the connector illustration and ICD-corner-connector record 12–48 V. Resolve before choosing the source | Population, source, current, off-state/backfeed and survival through motor-bus cut TBC (#116) |

Camera/module supply allocation follows the carrier and sensor-module design. Camera procurement is outside the current component shortlist; connector and power-interface obligations still apply.

## Returns and connectors

Use the proposed return map in ADR-0041 only after design approval. Every power output includes an adequately rated return; multiple connector contacts / derating are **TBD** from actual current and the connector datasheet. A stacking-header ground or SPI/CAN reference is not a motor-current return. No A50S 5 V / 3.3 V output is paralleled with another controller or an ARC supply.

Physical connector IDs, pin numbers, contact current ratings, positive-lock retention and wire gauges remain **TBD**. Signal interfaces / logical net directions are in [power-sync-stack](power-sync-stack.md) and [carrier-sync](carrier-sync.md); they do not release a power-connector pinout.

## Operating-state contract

| State / event | Required behavior |
| --- | --- |
| Initial power / reset | Load power sequencing follows the frozen carrier and component requirements. Drive remains inhibited until safety / steering / power-on checks pass; power application alone does not authorize motion |
| Battery ↔ wall handover | Preserve low-voltage operation without Orin / sync / sensor resets (SYS-14 / SYS-31). Record transient and supply margins at the actual loads |
| Physical e-stop | Controlled braked stop, then the motor-bus cut at T = 2.4 s and steering at T + 1 s (TBC; ADR-0012, ADR-0019, ADR-0034). Keep required logic / logging alive; Aux survival is open |
| Latched fault / reset | Follow ADR-0039 / SYS-25: operator e-stop cycle, power-on checks, explicit re-arm and restart from zero/default envelope. Restoring bus power does not clear the latch |
| Lowest-cell low battery | Warn, request clean Orin shutdown, acknowledge the actual halted/power-removal condition, then remove compute power; thresholds and timeout are TBD under proposed ADR-0022. Independent pack-protection cutoff must not rely on one healthy MCU |
| Rail fuse / fault / undervoltage | Report source / state to sync, retain fault evidence and enter the specified safe state. Recovery / latch / reset policy and behavior with the reporting rail absent are TBD for the selected circuits |
| Off / USB / Aux only | No unintended pack drain, phantom powering or outputs energized through GPIO/protection diodes. Verify all permitted external-cable states |

## Monitoring and release evidence

Power / cell data and interrupts cross [power-sync-stack](power-sync-stack.md). Every cell must meet ≥100 Hz and ±10 mV under SYS-32; source timing, measurement error, thresholds and off-state drain require analysis / tests. Electrical errors remain visible under SYS-19.

Before sign-off, attach the per-rail load/rating sheet, schematic and connector map; verify handover, sag, load/inrush steps, power-on/off, low-battery shutdown, faults/backfeed, regen and stop states. Measure at the load rather than only the regulator. Define acceptable voltage/ripple/transient margins from the selected load datasheets. Stop hardware requires its own tests and a second reviewer.

Both sides' owners and approvals are pending.
