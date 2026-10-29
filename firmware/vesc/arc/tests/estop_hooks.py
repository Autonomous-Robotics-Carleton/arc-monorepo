"""The e-stop routine's hooks in upstream VESC firmware are all still there
(ADR-0012): an upstream update (tools/vesc-upstream.sh) that drops one would
otherwise still build, and the e-stop would silently stop braking or stop
blocking commands.

    python3 estop_hooks.py <firmware/vesc/bldc>
"""

from pathlib import Path
import re
import sys

BLDC = Path(sys.argv[1])
failures = []


def body(path, signature):
    """The body of the C function whose definition is `signature`(...); its
    return type may be left out."""
    src = (BLDC / path).read_text()
    m = re.search(r'\n(?:\w+ )?' + re.escape(signature) + r'\([^)]*\)\s*\{', src)
    if not m:
        failures.append(f'{path}: {signature}() not found')
        return ''
    depth, i = 1, m.end()
    while depth:
        depth += {'{': 1, '}': -1}.get(src[i], 0)
        i += 1
    return src[m.end():i]


# Every motor command is ignored while the routine brakes: the gate is the
# last word in mc_interface_try_input() ...
try_input = body('motor/mc_interface.c', 'mc_interface_try_input')
gate = try_input.find('arc_estop_input_blocked()')
if gate < 0 or 'retval = 1' not in try_input[gate:]:
    failures.append('mc_interface_try_input() no longer returns 1 while arc_estop_input_blocked()')
elif try_input.rfind('retval =', 0, len(try_input)) > try_input.find('retval = 1', gate):
    failures.append('mc_interface_try_input() sets retval after the e-stop gate')

# ... and every function that drives the motor asks it.
for name in ['set_duty', 'set_duty_noramp', 'set_pid_speed', 'set_pid_pos', 'set_current',
             'set_brake_current', 'set_handbrake', 'set_openloop_current',
             'set_openloop_phase', 'set_openloop_duty', 'set_openloop_duty_phase',
             'release_motor', 'set_current_rel', 'set_brake_current_rel',
             'set_handbrake_rel']:
    b = body('motor/mc_interface.c', f'mc_interface_{name}')
    if b and 'mc_interface_try_input()' not in b and not re.search(r'mc_interface_set_\w+\(', b):
        failures.append(f'mc_interface_{name}() drives the motor without mc_interface_try_input()')

# A command after a release hands control back: every command resets the timeout.
if 'arc_estop_note_command()' not in body('timeout.c', 'timeout_reset'):
    failures.append('timeout_reset() no longer calls arc_estop_note_command()')

# The routine starts after the motor is set up, before anything can send commands.
main = body('main.c', 'main')
order = [main.find(s) for s in ('mc_interface_init(', 'arc_estop_start()', 'commands_init()')]
if -1 in order or order != sorted(order):
    failures.append('main(): arc_estop_start() must come after mc_interface_init() and '
                    'before commands_init()')

# Our code is built into the firmware.
if not re.search(r'^include \.\./arc/arc\.mk$', (BLDC / 'make/fw.mk').read_text(), re.M):
    failures.append('make/fw.mk no longer includes ../arc/arc.mk')

if failures:
    print('\n'.join(failures), file=sys.stderr)
    sys.exit(1)
print('every e-stop hook is in place')
