# tools

Developer scripts that don't run on the car (the `.mts` scripts need Node ≥ 22.18, which runs TypeScript directly): the `systems/` checker and ID lookup, interface code generation, VESC firmware vendoring, the firmware-in-the-loop test and the split check.

| Script | What |
| --- | --- |
| `check-systems.mts` | Validates `systems/` (links, IDs, ADRs, verification); `nx check systems` |
| `systems-ids.mts` | Looks up `systems/` IDs as JSON, from the repo root: `node tools/systems-ids.mts SYS-04 ADR-0012`, a kind (`RSK`), or no arguments for all |
| [Altium source sync](altium-sync/README.md) | Imports the four active Altium 365 project checkouts or extracted source snapshots into `hardware/` for a later GitHub commit; `npx nx run altium-sync:sync` previews, `--apply` writes with recovery copies. Never stages, commits or pushes |
| `vesc-upstream.sh` | Imports and updates the vendored VESC firmware (ADR-0033): `import\|update <ref>`; stages the result without committing; exits 2 on conflicts |
| `firmware-in-loop/` | Phase 6 of `systems/software/setup-plan.md`: the sync MCU firmware on `native_sim` and the ROS car backend link up over UDP; `nx test firmware-in-loop` |
| `gen-interfaces.sh` | Generates code from the machine-readable ICDs: the sync-link MAVLink code from `systems/icd/sync-link.xml` (ADR-0031) and the command-bus CAN code from `systems/icd/can-command.dbc`, into the sync MCU firmware and (MAVLink only) the ROS car backend. Needs the dev container's pymavlink and cantools. `--check` is `nx check sync-mcu` and `nx check ros` |
| `split-check/` | The split check (ADR-0047): fails if code above the split branches on which target it's on. `nx check split-check`, on every PR; [below](#split-check) |

## Split check

ADR-0047: code above the split never asks which target it's on. Target names appear only in board files, device models and target configurations (plus the backends and bringup, which choose a target at launch). `split-check/split-check.mts` enforces it. CI runs it on every pull request, whatever it changes.

```bash
npx nx check split-check                         # every file git knows (tracked and new, not ignored)
node tools/split-check/split-check.mts <paths>   # only these files
npx nx test split-check                          # its own tests
```

It reads each line of C, C++, Python, TypeScript/JavaScript, CMake, Kconfig, shell, Dockerfiles and ROS launch files (`.launch.py`, `.launch.xml`, `.launch.yaml`), with comments and Python docstrings removed. It flags a line that is a branch condition and names a target:

- **Branch conditions:** `if`, `elif`, `while`, `switch`/`case`, `match`/`case`, comparisons (`==`, `!=`, `in`), conditional expressions, `strcmp`, `#if`/`#ifdef`, `IS_ENABLED`, CMake `if()` and generator expressions, Kconfig `depends on` and `if`, shell `[ ]`/`[[ ]]` and `case` patterns, and launch conditions (`IfCondition`, `LaunchConfigurationEquals`, `if=`, `unless:`). A table keyed by target name (`{'sim': a, 'car': b}[target]`) counts too.
- **Target names**, listed in `split-check/rules.mts`: the targets (`sim`, `devhw`, `car`), worlds (`gazebo`, `webots`, `gym`), shortcuts (`replay`, `interface`) and sync MCU boards (`native_sim`, `nucleo_h723zg`, `arc_sync`), as whole string literals or as bare words in shell, CMake and Kconfig; the less common ones inside identifiers too (`is_sim`, `inGazebo`; not `car`, an everyday word here, and not ROS's `use_sim_time`); and Zephyr's board, SoC and architecture symbols (`CONFIG_BOARD_*`, `CONFIG_SOC_*`, `CONFIG_ARCH_POSIX`). Firmware branches on a feature instead (`CONFIG_NET_SOCKETS`), which the board's configuration turns on.

Where target names are allowed is a list of path globs in `rules.mts` (`ALLOWED`), each with its reason: board files, device models (`tools/sim/`), configurations (`config/`), every `ros/src/arc_backend_*` package, `arc_bringup`, test code (`test/` and `tests/` folders, `test_*.py`, `*_test.*`: tests assert on target names rather than branch on them, and the code they test is still checked), and the parts of the repo that aren't code above the split (the websites, tools, upstream VESC code, generated code). Everything else is checked, so a new ROS package or firmware module is covered without changing the check.

**If it fails,** move the target-specific part below the split: into a board file, a device model, a target configuration or a backend, and let the code above it read a feature or a parameter instead of the target. A real exception that can't be fixed in the same change goes in `EXCEPTIONS` in `rules.mts`, with the follow-up issue that removes it; the check warns once an exception matches nothing.

It's line-based, so it can miss a condition split over several lines or a target name held in a variable first; reviewers still check for those.
