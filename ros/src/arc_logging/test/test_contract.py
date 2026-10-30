"""What the logger promises, checked against what it depends on: the
decisions on logging (ros/README.md, ADR-0056), rosbag2's command line,
bringup's defaults, the image types in ICD-ros2-msgs and the log rate in
systems/. Written apart from test_recorder.py's
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
    FALLBACK_PROFILE,
    IMAGE_TYPES,
    log_root,
    plan,
    PROFILES,
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

FALLBACK_MIN = PROFILES[FALLBACK_PROFILE].min_free_gb


@pytest.mark.parametrize('log,must_log,has_root,profile,space', itertools.product(
    (False, True), (False, True), (False, True), tuple(PROFILES),
    (0.0, FALLBACK_MIN - 0.1, FALLBACK_MIN, 99.9, 100.0, 1e6)))
def test_plan_over_every_case(tmp_path, log, must_log, has_root, profile, space):
    root = tmp_path if has_root else None
    before = sorted(tmp_path.rglob('*'))
    p = plan(log, must_log, root, 'run', profile, 100.0, free=lambda _: space)

    fits = space >= 100.0
    falls_back = not fits and profile != FALLBACK_PROFILE and space >= FALLBACK_MIN
    recording = log and has_root and (fits or falls_back)
    assert (p.output is not None) == recording
    if recording:
        assert p.output.parent == tmp_path and not p.output.exists()
        assert 'recording to' in p.message
        # the profile asked for, unless it didn't fit: then the fallback, with a warning
        assert p.profile == (profile if fits else FALLBACK_PROFILE)
        assert p.warn == (not fits)
    else:
        assert p.profile == profile
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
    cmd = record_command(Path('/logs/run one'), {'target': 'car'}, 15.9, nasty,
                         ('a/msg/B', nasty))
    assert cmd[:3] == ['ros2', 'bag', 'record']
    assert all(isinstance(a, str) for a in cmd)
    # each value is one argument, passed as is: no shell ever sees it
    assert cmd[cmd.index('--output') + 1] == '/logs/run one'
    assert cmd[cmd.index('--exclude-regex') + 1] == nasty
    types = cmd.index('--exclude-topic-types')
    assert cmd[types + 1:cmd.index('--custom-data')] == ['a/msg/B', nasty]
    assert cmd[cmd.index('--max-bag-duration') + 1] == '15'
    assert cmd[cmd.index('--storage') + 1] == 'mcap'
    assert '--all-topics' in cmd and '--disable-keyboard-controls' in cmd
    assert '--exclude-regex' not in record_command('/logs/run', {}, 15)


def test_rosbag2_still_has_every_flag_used():
    """A rosbag2 update that renames or drops a flag fails here, not on the car."""
    help_text = subprocess.run(['ros2', 'bag', 'record', '--help'], capture_output=True,
                               text=True, check=True).stdout
    flags = {a for a in record_command('/x', {'k': 'v'}, 15, 'e', IMAGE_TYPES)
             if a.startswith('--')}
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


def _adr_0056():
    return (REPO / 'systems/adr/0056-logging-profiles.md').read_text()


def test_profiles_are_the_adrs():
    """ADR-0056's table: each profile's free space to start and to warn."""
    rows = {name: (float(start), float(warn)) for name, start, warn in re.findall(
        r'^\s*\| `(\w+)` \| [^|]+ \| ([\d.]+) GB[^|]*\| ([\d.]+) GB[^|]*\|$', _adr_0056(), re.M)}
    assert set(rows) == set(PROFILES), rows
    for name, profile in PROFILES.items():
        assert rows[name] == (profile.min_free_gb, profile.warn_free_gb), name
    assert FALLBACK_PROFILE == 'minimal'


def test_node_and_launch_default_to_the_profiles_own_space():
    """min_free_gb and warn_free_gb are overrides: by default each profile's own."""
    node = _node_defaults()
    assert node['log_profile'] == FALLBACK_PROFILE == 'minimal'
    assert node['min_free_gb'] < 0 and node['warn_free_gb'] < 0


def _space(profile, rate_mb_s, run_s):
    """min_free_gb holds a run at the rate, x 1.5; warn_free_gb gives >= 2 min."""
    run_gb = rate_mb_s * run_s / 1000
    assert profile.min_free_gb >= run_gb * 1.5, (profile, run_gb)
    assert 0 < profile.warn_free_gb < profile.min_free_gb
    assert profile.warn_free_gb * 1000 / rate_mb_s >= 120, \
        f'warn_free_gb gives the operator under 2 minutes at {rate_mb_s} MB/s'


def test_full_space_holds_a_full_run():
    """full covers a whole run at systems/'s log rate (SYS-09, SYS-03)."""
    cost = (REPO / 'systems/bom/total-electrical-cost.md').read_text()
    m = re.search(r'\*\*(\d+) GB per 10-minute run\*\*', cost)
    assert m, 'the v1.0 log size moved: update this test'
    rate = re.search(r'write about \*\*(\d+) MB/s\*\*', cost)
    assert rate, 'the v1.0 log rate moved: update this test'
    assert abs(float(rate.group(1)) * 600 / 1000 - float(m.group(1))) < 1
    _space(PROFILES['full'], float(rate.group(1)), 600)
    assert PROFILES['full'].exclude_types == ()


def test_minimal_space_holds_a_full_run():
    """minimal covers a whole run at ADR-0056's upper estimate of its rate."""
    m = re.search(r'\| `minimal` \| ≤ (\d+) MB/s', _adr_0056())
    assert m, "ADR-0056's minimal rate moved: update this test"
    _space(PROFILES['minimal'], float(m.group(1)), 600)


def _icd_types():
    icd = (REPO / 'systems/icd/ros2-msgs.md').read_text()
    return {f'{pkg}/msg/{name}' for pkg, name in re.findall(r'`(\w+_msgs)/(\w+)`', icd)}


def test_minimal_leaves_out_every_image_in_the_icd_and_nothing_else():
    types = _icd_types()
    assert 'sensor_msgs/msg/Image' in types and 'sensor_msgs/msg/LaserScan' in types
    left_out = set(PROFILES['minimal'].exclude_types)
    for t in types:
        assert (t in left_out) == t.endswith('Image'), t
    # only images: the measurements, commands and state stay in every profile
    assert all('Image' in t or 'image_transport' in t or 'Packet' in t for t in left_out)
    assert set(IMAGE_TYPES) == left_out


def test_rosbag2_takes_the_image_types():
    """Each is a package/msg/Type name, as --exclude-topic-types matches them."""
    for t in IMAGE_TYPES:
        assert re.fullmatch(r'[a-z][a-z0-9_]*/msg/[A-Z][A-Za-z0-9]*', t), t


def test_bringup_logs_the_car_and_only_the_car_by_default():
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    for target in bringup.TARGETS:
        for log in ('', 'true', 'false'):
            args = bringup.logging_args(target, 'gym', log)
            on_car = target == 'car'
            assert args['must_log'] == str(on_car).lower()
            assert args['log'] == (log or str(on_car).lower())
            assert args['sim'] == ('gym' if target == 'sim' else '')


def _replay_backend_topics():
    """The replay player's topic pattern and its named car topics, from its source."""
    source = (REPO / 'ros/src/arc_backend_replay/src/backend.cpp').read_text()
    pattern = re.search(r'kCarTopics\[\]\s*=\s*((?:\s*"[^"]*")+);', source)
    names = re.search(r'kCarTopicNames = \{(.*?)\};', source, re.S)
    assert pattern and names, 'backend.cpp changed: update this test'
    return (re.compile(''.join(re.findall(r'"([^"]*)"', pattern.group(1)))),
            re.findall(r'"([^"]*)"', names.group(1)))


def test_a_replays_log_leaves_out_what_the_player_publishes_and_only_that():
    """#205: the replay player's topics and its /clock are left out of a replay's
    log; /diagnostics and the platform's own topics are recorded."""
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    replayed = re.compile(bringup.REPLAYED_TOPICS)
    player, names = _replay_backend_topics()
    assert len(names) >= 30
    # rosbag2's --exclude-regex leaves out a topic the pattern matches anywhere in
    left_out = [t for t in names if replayed.search(t)]
    assert set(left_out) == set(names) - {'/diagnostics'}
    assert replayed.search('/clock')
    for topic in ('/diagnostics', '/control/mode', '/car/cmd/drive', '/car/cmd/heartbeat',
                  '/car/cmd/envelope', '/tf', '/estimate/odom', '/rosout', '/experiment/x'):
        assert not replayed.search(topic), topic
    # the same pattern as the player's, but for /diagnostics and /clock
    for topic in [*names, '/sensors/new/thing', '/car/motor/xx', '/car/steering2', '/clock']:
        if topic not in ('/diagnostics', '/clock'):
            assert bool(replayed.search(topic)) == bool(player.fullmatch(topic)), topic


def test_bringup_logs_a_replay_without_the_replayed_topics_and_names_its_source():
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    args = bringup.logging_args('replay', 'gym', 'true', '', 'run.mcap')
    assert args['log_exclude'] == bringup.REPLAYED_TOPICS and args['source_log'] == 'run.mcap'
    # an exclude of the run's own still applies, with the replayed topics
    args = bringup.logging_args('replay', 'gym', 'true', '/camera|/lidar', 'run.mcap')
    combined = re.compile(args['log_exclude'])
    assert check_exclude(args['log_exclude']) is None
    for topic in ('/camera', '/lidar', '/sensors/imu/cg', '/clock'):
        assert combined.search(topic), topic
    assert not combined.search('/control/mode')
    # other targets: the exclude as given, no source
    for target in ('sim', 'car'):
        args = bringup.logging_args(target, 'gym', '', '/camera', 'run.mcap')
        assert args['log_exclude'] == '/camera' and args['source_log'] == ''


def test_bringup_passes_the_ros_version_to_logging():
    """#160: bringup reads arc_version's stamp and hands it to the run record."""
    logging = _module(PACKAGE / 'launch/logging.launch.py', 'logging_launch')
    assert 'ros_version' in logging.ARGS
    source = (REPO / 'ros/src/arc_bringup/launch/platform.launch.py').read_text()
    assert "args['ros_version'] = ros_version()" in source
    node = (PACKAGE / 'arc_logging/node.py').read_text()
    assert "declare_parameter('ros_version'" in node


def test_bringup_passes_bag_to_logging_and_logging_takes_source_log():
    from launch.actions import DeclareLaunchArgument
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    logging = _module(PACKAGE / 'launch/logging.launch.py', 'logging_launch')
    declared = {e.name for e in bringup.generate_launch_description().entities
                if isinstance(e, DeclareLaunchArgument)}
    assert 'bag' in declared
    assert 'source_log' in logging.ARGS
    source = (REPO / 'ros/src/arc_bringup/launch/platform.launch.py').read_text()
    assert "arg('bag')" in source


def test_bringup_passes_the_profile_on_with_the_same_choices():
    from launch import LaunchContext
    from launch.actions import DeclareLaunchArgument
    from launch.utilities import perform_substitutions
    bringup = _module(REPO / 'ros/src/arc_bringup/launch/platform.launch.py', 'platform_launch')
    logging = _module(PACKAGE / 'launch/logging.launch.py', 'logging_launch')
    for launch in (bringup, logging):
        args = {e.name: e for e in launch.generate_launch_description().entities
                if isinstance(e, DeclareLaunchArgument)}
        arg = args['log_profile']
        assert tuple(arg.choices) == tuple(PROFILES)
        assert perform_substitutions(LaunchContext(), arg.default_value) == FALLBACK_PROFILE
    source = (REPO / 'ros/src/arc_bringup/launch/platform.launch.py').read_text()
    assert re.search(r"for name in \([^)]*'log_profile'", source)
