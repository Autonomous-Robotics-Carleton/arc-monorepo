# Motor configurations

Each drive motor's detected configuration, kept so a corner swap is configuration only (SYS-13, ADR-0034). The controller stays on the deck, so when a corner (and its motor) moves, the controller it's now wired to has to load that motor's values.

**Template only: no motor has been detected yet.** `motors.csv` has its header and no rows.

## What's kept

One row per motor in [`motors.csv`](motors.csv), keyed by the label on the motor:

| Column | What | Where in VESC Tool (Motor Settings → FOC) |
| --- | --- | --- |
| `label` | The label on the motor, as written on it. The labelling scheme is TBD | |
| `resistance_mohm` | Motor resistance, mΩ | General: R |
| `inductance_uh` | Motor inductance, µH | General: L |
| `ld_lq_diff_uh` | Ld − Lq, µH (detection measures it with L) | General: Ld − Lq |
| `flux_linkage_mwb` | Flux linkage, mWb | General: λ |
| `hall_table` | The eight Hall table entries, space-separated, in order 0–7 | Hall Sensors: the table |
| `detected_on` | Date of the detection (YYYY-MM-DD) | |
| `controller` | Which controller ran it (its label or serial) | |
| `firmware` | Firmware version or commit the controller ran | |
| `notes` | Anything odd: a re-run, a rewound motor, a replaced Hall board | |

Only these values are kept, not a whole VESC Tool XML export: an export also carries the controller's current and voltage limits, which would overwrite that controller's own settings when loaded. (The eRPM limit is capped in our firmware whatever the configuration says, ADR-0034.)

## Recording a motor

With the motor's wheel off the ground:

1. Connect the controller wired to the motor to VESC Tool over USB.
2. Run motor detection: the FOC setup wizard, or the R, L and λ measurements on Motor Settings → FOC → General followed by Hall detection on FOC → Hall Sensors. It takes about a minute.
3. Write the motor configuration to the controller.
4. Copy the five values into a new row of `motors.csv` with the motor's label, and commit it in a PR.

A motor that's re-detected gets its row updated in a PR, not a second row.

## Loading a motor's configuration

When a corner is swapped (the swap procedure in [`systems/icd/corner-mechanical.md`](../../../systems/icd/corner-mechanical.md)):

1. Read the label on the motor and find its row in `motors.csv`.
2. Connect the controller that motor is now wired to over USB and read its motor configuration in VESC Tool.
3. In Motor Settings → FOC → General, enter resistance, inductance, Ld − Lq and flux linkage; in Hall Sensors, enter the Hall table.
4. Write the motor configuration.

No row for the motor, or a row that doesn't drive smoothly: run detection instead (*Recording a motor*) and record it.
