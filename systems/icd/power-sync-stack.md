# ICD-power-sync-stack

- **Revision:** B (2026-10-09)
- **Status:** Draft internal motherboard interface; clock/pin map and monitor topology TBD. No stacking header in the current plan (ADR-0042)
- **Side A:** motherboard power section, E-41 / E-46 (owner TBD)
- **Side B:** motherboard sync MCU section / firmware, E-30 (owner TBD)
- **Traces to:** SYS-05, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32, ADR-0022, ADR-0039, ADR-0041, ADR-0042
- **Related work:** [#102](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/102); physical release depends on #96 / #98

Under ADR-0042, power and sync share one [motherboard PCB](../../hardware/motherboard/README.md), with the Jetson carrier separate. This file retains its existing name for links and defines the internal signal inventory; it does not release MCU pins or a connector pinout. A stacking header is needed only if a later reviewed packaging change splits the motherboard. Monitor-IC transport and two-tier shutdown remain **Proposed** in ADR-0022. Power ratings are in [power-rails](power-rails.md); ground/reference rules are proposed in ADR-0041.

## Signal inventory

| Logical signal / group | Direction | Electrical contract | Remaining decision |
| --- | --- | --- | --- |
| LOGIC_5V / LOGIC_3V3 / LOGIC_RETURN | Power → sync | Supply and rated return; prevent logic return paths carrying motor current | Whether the sync locally derives 3.3 V, current / return capacity / sequence TBD |
| PWR_I2C_SCL / PWR_I2C_SDA | Sync master ↔ power monitors | Dedicated bus, separate from ToF I2C. Proposed 3.3 V open-drain domain; pull-ups must be allocated once and cannot back-power an unpowered side | Monitor MPNs / addresses, speed, total capacitance, pull-ups, bus recovery and off-state isolation TBD |
| CELL_SPI_SCK / MOSI / MISO / CS (conditional) | Sync master ↔ cell monitor | **Candidate-specific alternative** if the LTC6810 SPI/isoSPI proposal is retained; exact voltage / transport / isolation from selected monitor design | ADR-0022's one-I2C example does not define a transport for this candidate. Choose an I2C-capable monitor or explicitly review a separate cell-monitor SPI interface before pin freeze; no choice made here |
| PWR_ALERT_1…N / PWR_FAULT_1…N | Power → sync | Edges timestamped in sync clock; source must remain identifiable in telemetry | N, source per line, active polarity and level translation TBD. Open-drain aggregation is permitted only after verifying every source; never join push-pull outputs |
| ESTOP_STATUS | Power / stop circuit → sync | Observe physical stop state with defined reset/unpowered behavior | Level, polarity, pull and input conditioning TBD from #95 |
| PWR_GOOD | Power → sync | Qualified status for startup / reset checks; absence or invalid state cannot grant motion | Which rail(s), delay, logic level and reset behavior TBD |
| ORIN_SHUTDOWN_REQ path | Sync → carrier (routing traverses the motherboard) | Low-battery request under proposed ADR-0022; exact carrier input / driver and levels resolved in carrier-sync | Route / voltage / polarity / pulse duration TBD; do not assume a raw module input accepts sync voltage |
| ORIN_HALTED / POWER_REMOVE_OK path | Carrier/platform → power; sync observes if required | Must represent the actual condition allowing safe compute-power removal; retain evidence if handshake fails | Module/platform signal choice, translation, timeout and independent undervoltage backstop TBD; never label a generic GPIO as proof of halt |
| BAT_V_AIN / BAT_I_AIN (rework fallback) | Power → sync ADC | Conditioned analog fallback under proposed ADR-0022; referenced to approved analog/logic return, limited to selected ADC input ratings | Divider/shunt scaling, filters, protection, source impedance and ADC pins TBD; default DNP until reviewed |
| Spare MCU I/O / test access | Reserved | No circuit assigned | Count / location TBD from the complete pin map and test plan |

There is **no inter-board power/sync connector** in the current plan. MCU pins and test points remain **TBD**. Motor phases, motor-bus current and CAN trunks are outside this low-voltage monitor contract; combining boards does not make logic returns suitable for motor current.

## Data and failure handling

- Read and log battery V/I, individual cell voltages, selected branch currents / voltages, alert/fault sources, power-good and e-stop state. Addresses, units, scaling, sample-ready/read timing and stale thresholds are released with the selected monitor and firmware mapping. Cell acquisition meets SYS-32; the bus and other links meet SYS-24.
- Monitor conversion/read timing must be recorded; polling completion is not automatically the measurement instant. Unavailable, stale or impossible data must be distinguishable from a valid zero. Log bus recovery, CRC/status faults and rail loss under SYS-19.
- The sync-link schema currently carries battery/cell data; rail-current fields are still open. This ICD does not change XML or generated code. Schema changes require owner review, regeneration and matching firmware/Orin updates.
- Digital conversion in the power section does not eliminate return-current noise, local ground offset or the need for logic margins on the combined PCB. Resolve return paths / any isolation and unpowered-side behavior under ADR-0041.
- Sync reset or I2C/SPI failure must not silently disable the independent pack-protection / stop circuits. Proposed hardware backstop thresholds, hysteresis and timeout remain TBD under ADR-0022; no new cutoff circuit is selected here.

## Sequencing and sign-off

The power and sync designs must define which side supplies pull-ups, what each signal does before its rail is ready, when interrupts become valid, and how inputs are protected when either functional section is reset or unpowered. Shutdown request / handshake and the compute switch follow [power-rails](power-rails.md); driving stays latched off under ADR-0039 until valid checks and re-arm.

Review internal connectivity / test access, monitor data path, logic limits, pin mux, rail/current budget and reset / partial-power behavior in both sections. Any future board split also requires connector/contact and unplug-behavior review. Test cell accuracy/rate, source timing, fault identification, ground shift, bus-stuck recovery, shutdown handshake and power loss. A draft logical inventory does not release the motherboard layout or a future header pinout.
