"""The eRPM limit against where it comes from: erpm_max in the car
configuration, ADR-0034's >= 10 loop samples per electrical cycle (RSK-18),
and SYS-01's top speed. Fails when either side changes without the other.

    python3 speed_limit_values.py <repo root>
"""

import math
from pathlib import Path
import re
import sys

ROOT = Path(sys.argv[1])
failures = []


def check(ok, message):
    if not ok:
        failures.append(message)


def find(path, pattern):
    """The first group of `pattern` in `path`; a missing one fails loudly."""
    m = re.search(pattern, (ROOT / path).read_text())
    if not m:
        sys.exit(f'{path}: no match for {pattern!r}: update speed_limit_values.py')
    return float(m.group(1))


def config(key):
    return find('config/cars/nominal.yaml', rf'\n\s*{key}: ([0-9.]+)')


limit = find('firmware/vesc/arc/include/arc/speed_limit.h',
             r'#define ARC_SPEED_LIMIT_ERPM_TBC ([0-9.]+)f')

# One value: the controllers' cap is the car configuration's.
check(limit == config('erpm_max'), f'limit {limit} != nominal.yaml erpm_max {config("erpm_max")}')

# ADR-0034: >= 10 control-loop samples per electrical cycle at the A50S's loop rate.
loop_khz = find('systems/adr/0034-off-the-shelf-motor-controllers.md', r'(\d+(?:\.\d+)?) kHz at its default')
samples = loop_khz * 1000 / (limit / 60)
check(samples >= 10, f'{samples:.1f} loop samples per electrical cycle at the limit, ADR-0034 needs >= 10')

# SYS-01: the top speed is still reachable at the limit, with the higher gearing.
sys01 = find('systems/requirements/system.md', r'\| SYS-01 \| Top speed on tile ≥ (\d+(?:\.\d+)?) m/s')
gear = max(config('gear_ratio_front'), config('gear_ratio_rear'))
top = limit / config('motor_pole_pairs') / gear / 60 * math.pi * config('rolling_diameter')
check(top >= sys01, f'{top:.2f} m/s at the limit, below SYS-01 {sys01} m/s')

# The hook in upstream's code, which an upstream update could drop: the limits
# are capped every time upstream recomputes them, on every timer tick.
src = (ROOT / 'firmware/vesc/bldc/motor/mc_interface.c').read_text()


def body(name):
    m = re.search(r'\n(?:static )?\w+ ' + name + r'\([^)]*\)\s*\{', src)
    if not m:
        failures.append(f'mc_interface.c: {name}() not found')
        return ''
    depth, i = 1, m.end()
    while depth:
        depth += {'{': 1, '}': -1}.get(src[i], 0)
        i += 1
    return src[m.end():i]


check('arc_speed_limit_apply(' in body('update_override_limits'),
      'update_override_limits() no longer calls arc_speed_limit_apply()')
check('update_override_limits(' in body('run_timer_tasks'),
      'run_timer_tasks() no longer calls update_override_limits()')

print(f'limit {limit:.0f} eRPM: {samples:.1f} samples per cycle, top speed {top:.2f} m/s '
      f'(SYS-01 {sys01} m/s)')
if failures:
    print('\n'.join(failures), file=sys.stderr)
    sys.exit(1)
