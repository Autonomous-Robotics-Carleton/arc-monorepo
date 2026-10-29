"""The e-stop routine's constants against where they come from: the car
configuration and the ADRs. Fails when either side changes without the other.

    python3 estop_values.py <repo root>
"""

import math
from pathlib import Path
import re
import sys

ROOT = Path(sys.argv[1])
HEADER = ROOT / 'firmware/vesc/arc/include/arc/estop.h'
failures = []


def check(ok, message):
    if not ok:
        failures.append(message)


def find(path, pattern):
    """The first group of `pattern` in `path`; a missing one fails loudly."""
    m = re.search(pattern, (ROOT / path).read_text())
    if not m:
        sys.exit(f'{path}: no match for {pattern!r}: update estop_values.py')
    return m.group(1)


def define(name):
    return float(find(HEADER.relative_to(ROOT), rf'#define {name} ([0-9.]+)f?'))


def config(key):
    return float(find('config/cars/nominal.yaml', rf'\n\s*{key}: ([0-9.]+)'))


diameter = define('ARC_ESTOP_WHEEL_DIAMETER_M_TBC')
gear = define('ARC_ESTOP_GEAR_RATIO_TBC')
kv = define('ARC_ESTOP_MOTOR_KV_TBC')
mass = define('ARC_ESTOP_CAR_MASS_KG_TBC')
decel = define('ARC_ESTOP_DECEL_MPS2_TBC')
ramp_s = define('ARC_ESTOP_RAMP_S_TBC')

# The car configuration: same tyre; the brake current is sized for the lower
# gearing, which needs the more current.
check(diameter == config('rolling_diameter'),
      f'wheel diameter {diameter} != nominal.yaml rolling_diameter {config("rolling_diameter")}')
check(gear <= min(config('gear_ratio_front'), config('gear_ratio_rear')),
      f'gear ratio {gear} above the lower of nominal.yaml front/rear')

# ADR-0026: the Castle 1010-4400kV.
check(kv == float(find('systems/adr/0026-staggered-transverse-motors.md', r'1010-(\d+)kV')),
      f'motor kV {kv} != ADR-0026')
# ADR-0012: ~5 m/s^2 and ~150 ms.
adr12 = 'systems/adr/0012-estop-controlled-braking.md'
check(decel == float(find(adr12, r'~(\d+(?:\.\d+)?) m/s² deceleration')),
      f'deceleration {decel} != ADR-0012')
check(ramp_s * 1000 == float(find(adr12, r'~(\d+) ms brake ramp')), f'ramp {ramp_s} != ADR-0012')
# ADR-0044: the brake is sized for the upper end of the mass range.
low, high = (float(x) for x in re.search(
    r'TBC (\d+(?:\.\d+)?)–(\d+(?:\.\d+)?) kg',
    (ROOT / 'systems/adr/0044-ee-sizing-after-custom-corners.md').read_text()).groups())
check(mass >= high, f'mass {mass} below ADR-0044 upper bound {high}')

# The power board cuts motor power at T: the brake must have stopped the car
# from the highest speed the eRPM cap allows by then (ADR-0012, ADR-0034).
erpm_max = config('erpm_max')
top = (erpm_max / config('motor_pole_pairs') / min(config('gear_ratio_front'),
       config('gear_ratio_rear')) / 60 * math.pi * config('rolling_diameter'))
stop_s = top / decel + ramp_s / 2  # the ramp costs half its length at full deceleration
t_cut = float(find(adr12, r'T TBC (\d+(?:\.\d+)?) s'))
check(stop_s < t_cut, f'a stop from {top:.1f} m/s takes {stop_s:.2f} s, not inside T {t_cut} s')
print(f'stop from the cap ({top:.1f} m/s): {stop_s:.2f} s, T {t_cut} s, margin {t_cut - stop_s:.2f} s')

if failures:
    print('\n'.join(failures), file=sys.stderr)
    sys.exit(1)
print('estop values match config/cars/nominal.yaml and the ADRs')
