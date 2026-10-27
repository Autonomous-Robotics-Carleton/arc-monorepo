# ADR-0054: The car configuration's values: written by hand in nominal.yaml, and only what code needs now

- **Status:** Accepted (2026-10-27)
- **Date:** 2026-10-27
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-22, SYS-33, ADR-0047, ADR-0048, ADR-0051
- **Amends:** ADR-0051 (the defaults are written by hand, not generated; what a car file holds)

## Context

ADR-0051 put each car's numbers in a file in the repo and said the defaults would be **generated from `systems/`** (the budgets, the BOM, the ICDs), with measured values written over them. Implementing it (#143) showed three problems:

1. **`systems/` doesn't hold the numbers yet.** Most of the geometry waits on the CAD block layout (#103): the wheelbase is "~330–350 mm (start)", the track only the width box, the mass and every position TBD. A generator would mostly copy TBDs.
2. **A generator would read prose.** The values sit in Markdown tables and sentences, so it would break whenever a page is reworded, and it would keep the same number in two places.
3. **ADR-0051's examples include values that aren't per car.** The motor controllers' CAN IDs are 1–4 = FL, FR, RL, RR on every car (`can-command.dbc`, #73), and code is generated from that file. Sensor positions and mass properties have no reader yet, and may belong in the car's URDF (#105).

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. A generator reads `systems/` and writes the twin's file (ADR-0051, #174) | A design change in `systems/` reaches the twin by itself | Copies TBDs today; parses prose; the number lives in two places |
| **B. The design values written by hand in one file, `nominal.yaml`, each with its source; every car file extends it** | One place for each number; each value says where it came from or that it's a placeholder; a car's file shows only what was measured | A design change means editing `nominal.yaml` as well as the page that justifies it |

## Decision

**Option B.**

1. **`config/cars/nominal.yaml` holds the car as designed.** Each value names its source in `systems/`, or is marked a placeholder until the block layout or a measurement sets it. There's no generator (#174 is closed).
2. **Car files layer.** A car's file names the file it `extends` and overrides only what differs: mappings merge key by key, anything else is replaced. The twin's file extends `nominal` unchanged; a physical car's file holds its measurements. The merged values are what's validated (`schema.json`), used and hashed.
3. **What a car file holds:** what differs between cars or is measured on each, and that code needs now: the wheelbase and track, the tyre's rolling diameter, the gearing per axle with the motor's pole pairs and the eRPM cap, and the steering range and centre.
4. **What it doesn't:**
   - **The CAN IDs.** They're fixed by `can-command.dbc` on every car and set in each controller.
   - **Rates and timeouts.** They're design constants in the ICDs.
   - **Anything worked out from other values,** so it can't disagree with them. The top speed, the envelope's ceiling (SYS-22), comes from the eRPM cap, the slower axle's gearing and the tyre.
5. **Not yet: sensor positions and mass properties.** They wait for the block layout (#103) and the car's URDF (#105). Sensor mounts and per-link inertia usually live in the URDF, built from the CAD, with only per-car calibration in the car file. That's decided with the URDF.

ADR-0051's other items stand: one file per car in the repo, the sync MCU's values compiled into its build, the Orin reading the same file, and a hash on both sides.

## Consequences

- **Implemented in #202:** `config/cars/` (`nominal.yaml`, `twin.yaml`, `schema.json`), the `arc_car_config` loader, `car:=` on the platform launch, and `nx check car-config`.
- **A design change is two edits:** the page in `systems/` that justifies it, and `nominal.yaml`. The value's comment points at the page.
- **The twin follows the design only as far as `nominal.yaml` does.** Placeholders stay placeholders until someone replaces them.
- **ICD-ros2-msgs open issue 3** (sensor frames as static transforms) waits on #105, not on the car configuration.
- **Reopen if:** `systems/` gains machine-readable design values (a parameters table the checker can read), so a generator would read data, not prose; or sensor positions turn out to need a per-car home before the URDF exists.
