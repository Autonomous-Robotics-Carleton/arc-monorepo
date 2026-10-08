# tools

Developer scripts that don't run on the car: CAN and log analysis, bench-test helpers, code generation from the ICDs.

| Script | What |
| --- | --- |
| `check-systems.mts` | Validates `systems/` (links, IDs, ADRs, verification); `nx check systems` |
| `systems-ids.mts` | Looks up `systems/` IDs |
| `vesc-upstream.sh` | Imports and updates the vendored VESC firmware (ADR-0033) |
| `gen-sync-link.sh` | Generates the sync-link MAVLink code from `systems/icd/sync-link.xml` into the firmware and the ROS car backend (ADR-0031); `--check` for CI |
