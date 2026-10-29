"""What the logger promises, checked against what it depends on: the
decisions on logging (ros/README.md), rosbag2's command line, bringup's
defaults, and the log rate in systems/. Written apart from test_recorder.py's
worked cases; these fail when either side drifts.
"""

import ast
from datetime import datetime, timedelta, timezone
import importlib.util
import itertools
import os
from pathlib import Path
import re
import subprocess

import pytest

from arc_logging.recorder import (
    check_exclude,
    log_root,
    plan,
    record_command,
    run_dir,
    run_name,
    run_record,
)

PACKAGE = Path(__file__).resolve().parents[1]
REPO = PACKAGE.parents[2]


def _module(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


# --- whether a run is recorded, and what the operator is told ---------------

@pytest.mark.parametrize('log,must_log,has_root,space', itertools.product(
    (False, True), (False, True), (False, True), (0.0, 99.9, 100.0, 1e6)))
def test_plan_over_every_case(tmp_path, log, must_log, has_root, space):
    root = tmp_path if has_root else None
    before = sorted(tmp_path.rglob('*'))
    p = plan(log, must_log, root, 'run', 100.0, free=lambda _: space)

    recording = log and has_root and space >= 100.0
    assert (p.output is not None) == recording
    if recording:
        assert p.output.parent == tmp_path and not p.output.exists()
        assert not p.warn and 'recording to' in p.message
    else:
        assert 'recording to' not in p.message
        # a car run that isn't recorded is always a warning, never silent
        if must_log:
            assert p.warn and 'NOT LOGGING' in p.message
        # asked to log and can't: a warning too, wherever it runs
        if log:
            assert p.warn and 'not recorded' in p.message
        if not log and not must_log:
            assert not p.warn
    # planning never touches the drive
    assert sorted(tmp_path.rglob('*')) == before


def test_log_root_order(tmp_path):
    a, b = str(tmp_path / 'a'), str(tmp_path / 'b')
    for must_log in (False, True):
        assert log_root(a, b, must_log) == Path(a)  # log_dir:= first
        assert log_root('', b, must_log) == Path(b)  # then $ARC_LOG_DIR
    assert log_root('', '', False) == Path.home() / 'arc_logs'
    assert log_root('', '', True) is None  # the car has no fallback


# --- the run's name, directory and record ------------------------------------

def test_run_names_sort_by_time():
    start = datetime(2026, 1, 1, tzinfo=timezone.utc)
    times = [start + timedelta(seconds=s) for s in (0, 1, 59, 60, 3599, 86399, 86400 * 400)]
    names = [run_name('car', t) for t in times]
    assert names == sorted(names)
    assert all(re.fullmatch(r'\d{8}T\d{6}Z_car', n) for n in names)
    # a local time is written in UTC
    est = timezone(timedelta(hours=-5))
    assert run_name('', datetime(2026, 10, 29, 9, 0, tzinfo=est)) == '20261029T140000Z'


def test_every_bringup_target_names_one_directory():
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    for target in bringup.TARGETS:
        name = run_name(target)
        assert os.sep not in name and name not in ('.', '..') and '=' not in name


def test_run_dir_never_reuses_a_directory(tmp_path):
    seen = set()
    for _ in range(5):
        d = run_dir(tmp_path, 'run')
        assert not d.exists() and d not in seen and d.parent == tmp_path
        d.mkdir()
        seen.add(d)
    (tmp_path / 'run-9').write_text('not a directory')
    assert run_dir(tmp_path, 'run-9') == tmp_path / 'run-9-1'


def test_run_record_survives_rosbag2s_custom_data():
    # rosbag2 splits each --custom-data pair at '=': every value from the
    # launch must come back whole, and an empty field is left out
    record = run_record('car', '', 'lidar', 'twin', 'f' * 64, '/camera/.*')
    cmd = record_command('/logs/run', record, 15)
    pairs = cmd[cmd.index('--custom-data') + 1:]
    assert dict(p.split('=', 1) for p in pairs) == {k: v for k, v in record.items() if v}
    assert all(p.count('=') == 1 for p in pairs)


def test_exclude_patterns_that_cant_be_recorded_are_refused():
    assert check_exclude('') is None
    assert check_exclude('/camera/.*|/lidar') is None
    assert check_exclude('a=b')  # rosbag2 would cut the record short
    assert check_exclude('(')  # not a regex


# --- the recorder's command line ---------------------------------------------

def test_record_command_is_an_argument_list():
    nasty = "/x'; rm -rf / #  $(touch y)"
    cmd = record_command(Path('/logs/run one'), {'target': 'car'}, 15.9, nasty)
    assert cmd[:3] == ['ros2', 'bag', 'record']
    assert all(isinstance(a, str) for a in cmd)
    # each value is one argument, passed as is: no shell ever sees it
    assert cmd[cmd.index('--output') + 1] == '/logs/run one'
    assert cmd[cmd.index('--exclude-regex') + 1] == nasty
    assert cmd[cmd.index('--max-bag-duration') + 1] == '15'
    assert cmd[cmd.index('--storage') + 1] == 'mcap'
    assert '--all-topics' in cmd and '--disable-keyboard-controls' in cmd
    assert '--exclude-regex' not in record_command('/logs/run', {}, 15)


def test_rosbag2_still_has_every_flag_used():
    """A rosbag2 update that renames or drops a flag fails here, not on the car."""
    help_text = subprocess.run(['ros2', 'bag', 'record', '--help'], capture_output=True,
                               text=True, check=True).stdout
    flags = {a for a in record_command('/x', {'k': 'v'}, 15, 'e') if a.startswith('--')}
    for flag in flags:
        assert re.search(rf'(^|\s|\[){re.escape(flag)}\b', help_text), flag


def test_node_starts_the_recorder_to_die_with_it():
    """The recorder gets SIGINT when the node dies (setpriv, no Python in the fork)."""
    node = (PACKAGE / 'arc_logging/node.py').read_text()
    assert "['setpriv', '--pdeathsig', 'INT', '--']" in node
    assert 'preexec_fn' not in node
    assert subprocess.run(['setpriv', '--help'], capture_output=True,
                          text=True).stdout.count('--pdeathsig') >= 1


# --- decisions that must not drift --------------------------------------------

def _calls(path):
    tree = ast.parse(path.read_text())
    for node in ast.walk(tree):
        if isinstance(node, ast.Call):
            f = node.func
            yield f.attr if isinstance(f, ast.Attribute) else getattr(f, 'id', '')


def test_logs_are_never_deleted():
    """ros/README.md: logs are never deleted automatically. No code here can."""
    deleting = {'rmtree', 'unlink', 'remove', 'rmdir', 'removedirs', 'truncate', 'system'}
    sources = [*PACKAGE.glob('arc_logging/*.py'), *PACKAGE.glob('launch/*.py'),
               PACKAGE / 'scripts/logger']
    for path in sources:
        assert not deleting & set(_calls(path)), path
        assert not re.search(r"\brm\b|'-delete'|shutil\.move", path.read_text()), path


def _node_defaults():
    tree = ast.parse((PACKAGE / 'arc_logging/node.py').read_text())
    defaults = {}
    for node in ast.walk(tree):
        if (isinstance(node, ast.Call) and getattr(node.func, 'attr', '') == 'declare_parameter'
                and len(node.args) == 2):
            defaults[node.args[0].value] = ast.literal_eval(node.args[1])
    return defaults


def test_launch_and_node_agree_on_every_default():
    launch = _module(PACKAGE / 'launch/logging.launch.py', 'logging_launch')
    node = _node_defaults()
    assert set(launch.ARGS) == set(node), 'a parameter the launch or the node lacks'
    for name, (default, _) in launch.ARGS.items():
        cast = launch.TYPES.get(name, str)
        value = default == 'true' if cast is bool else cast(default)
        assert value == node[name], f'{name}: launch {default!r}, node {node[name]!r}'


def test_free_space_holds_a_full_run():
    """min_free_gb covers a whole run at systems/'s log rate (SYS-09, SYS-03),
    and the running warning comes before the drive fills."""
    cost = (REPO / 'systems/bom/total-electrical-cost.md').read_text()
    m = re.search(r'\*\*(\d+) GB per 10-minute run\*\*', cost)
    assert m, 'the v1.0 log size moved: update this test'
    run_gb = float(m.group(1))
    defaults = _node_defaults()
    assert defaults['min_free_gb'] >= run_gb * 1.5, (defaults['min_free_gb'], run_gb)
    assert 0 < defaults['warn_free_gb'] < defaults['min_free_gb']
    rate = re.search(r'write about \*\*(\d+) MB/s\*\*', cost)
    assert rate and defaults['warn_free_gb'] * 1000 / float(rate.group(1)) >= 120, \
        'warn_free_gb gives the operator under 2 minutes'


def test_bringup_logs_the_car_and_only_the_car_by_default():
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    for target in bringup.TARGETS:
        for log in ('', 'true', 'false'):
            args = bringup.logging_args(target, 'gym', log)
            on_car = target == 'car'
            assert args['must_log'] == str(on_car).lower()
            assert args['log'] == (log or str(on_car).lower())
            assert args['sim'] == ('gym' if target == 'sim' else '')
