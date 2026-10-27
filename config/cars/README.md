# Car configuration

Every car differs slightly: its tyres, gearing, steering centre and range. Those numbers live here, one file per car, so the code is the same on every car (ADR-0047, ADR-0051). Changing a car's numbers is a PR, so every measurement is reviewed and the history says when it changed.

| File | What |
| --- | --- |
| `nominal.yaml` | The car as designed. Each value says where it comes from, or that it's a placeholder until the CAD block layout (#103) or a measurement sets it |
| `twin.yaml` | The digital twin's car: the design, unchanged |
| `<car>.yaml` | A physical car (none yet): `extends: nominal`, plus only what was measured on it |
| `schema.json` | What a car's merged values must contain, checked on every load and in CI |

## How a car file works

A car file names the file it `extends` and overrides only what differs. Mappings merge key by key; anything else (a number, a list) is replaced whole. So a car with worn tyres and a taller rear gear is:

```yaml
extends: nominal
name: v1-0
build: v1.0
wheels:
  rolling_diameter: 0.097  # measured 2026-11-12, after 3 h of running
drivetrain:
  gear_ratio_rear: 15.5    # 18/62 pinion
```

The merged result is what's used and what's hashed. SI units throughout (m, rad).

## What's in a car file, and what isn't

In: what differs between cars or has to be measured on each one, and that code needs now. That's the wheelbase and track, tyre diameter, gearing per axle, and the steering range and centre: what the sync MCU needs to split a command into wheel speeds and the car backend needs to turn eRPM into m/s.

Not yet:
- **Sensor positions.** None is placed until the CAD block layout (#103), and they may belong in the car's URDF (#105), built from the CAD, with only per-car calibration here.
- **Mass, centre of gravity and inertia.** Only the twin needs them, and its Gazebo model takes them per link in the URDF.

Out:
- **The CAN IDs.** They're 1–4 = FL, FR, RL, RR on every car (`systems/icd/can-command.dbc`), set in each controller.
- **Rates and timeouts.** They're design constants in the ICDs, the same on every car.
- **Anything worked out from other values,** so it can't disagree with them. The top speed, the envelope's ceiling, comes from the eRPM cap, the slower axle's gearing and the tyre (`top_speed()` in `arc_car_config`).

## Who reads it

- **ROS:** `ros2 launch arc_bringup platform.launch.py car:=<car>` (default `twin`) checks the file before anything starts and logs its hash. Nodes read values with `arc_car_config.load()`, and the run record (#152) takes `config_hash()`.
- **The sync MCU** (not yet): the values it needs, compiled into a header for that car's build. It reports the hash, and a mismatch with the Orin's refuses drive commands.

## Commands

```bash
# in the dev container
npx nx check car-config                          # every car file loads and validates
ros2 run arc_car_config car_config show twin     # the merged values, the hash and the top speed
# without a ROS install
PYTHONPATH=ros/src/arc_car_config python3 -m arc_car_config.cli check config/cars
```
