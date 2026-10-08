# tools

Developer scripts that don't run on the car: CAN and log analysis, bench-test helpers, code generation from the ICDs.

| Script | What |
| --- | --- |
| `check-systems.mts` | Validates `systems/` (links, IDs, ADRs, verification); `nx check systems` |
| `systems-ids.mts` | Looks up `systems/` IDs |
| `vesc-upstream.sh` | Imports and updates the vendored VESC firmware (ADR-0033) |
| `firmware-in-loop/` | Phase 6 smoke test: the sync MCU firmware on `native_sim` and the ROS car backend link up over UDP; `nx test firmware-in-loop` |
| `gen-interfaces.sh` | Generates code from the machine-readable ICDs: the sync-link MAVLink code from `systems/icd/sync-link.xml` (ADR-0031) and the command-bus CAN code from `systems/icd/can-command.dbc`; `--check` for CI |
