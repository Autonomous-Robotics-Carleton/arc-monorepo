"""Whether and where a run is logged, its name and record, and the recorder's command line."""

from datetime import datetime, timezone
from pathlib import Path

from arc_logging.recorder import (
    check_exclude,
    free_gb,
    log_root,
    lost_messages,
    plan,
    record_command,
    run_dir,
    run_name,
    run_record,
    start_status,
    started,
)

START = datetime(2026, 10, 29, 14, 5, 1, tzinfo=timezone.utc)


def plenty(_):
    return 1000.0


def test_run_name_sorts_by_start_time_and_names_the_target():
    assert run_name('car', START) == '20261029T140501Z_car'
    assert run_name('sim', START) == '20261029T140501Z_sim'
    assert run_name('', START) == '20261029T140501Z'


def test_run_dir_never_reuses_a_directory(tmp_path):
    first = run_dir(tmp_path, 'run')
    assert first == tmp_path / 'run'
    first.mkdir()
    second = run_dir(tmp_path, 'run')
    assert second == tmp_path / 'run-1'
    second.mkdir()
    assert run_dir(tmp_path, 'run') == tmp_path / 'run-2'


def test_log_root_is_explicit_when_the_run_must_be_logged():
    assert log_root('/a', '/b', True) == Path('/a')
    assert log_root('', '/b', True) == Path('/b')
    assert log_root('', '', True) is None
    assert log_root('', '', False).name == 'arc_logs'


def test_plan_records_with_a_directory_and_space(tmp_path):
    p = plan(True, True, tmp_path, 'run', 20.0, plenty)
    assert p.output == tmp_path / 'run'
    assert not p.warn


def test_plan_warns_when_a_run_that_must_be_logged_is_not():
    off = plan(False, True, Path('/logs'), 'run', 20.0, plenty)
    assert off.output is None and off.warn and 'log:=false' in off.message
    no_dir = plan(True, True, None, 'run', 20.0, plenty)
    assert no_dir.output is None and no_dir.warn and '$ARC_LOG_DIR' in no_dir.message


def test_plan_is_quiet_when_not_logging_was_asked_for():
    off = plan(False, False, Path('/logs'), 'run', 20.0, plenty)
    assert off.output is None and not off.warn


def test_plan_does_not_record_on_low_space_and_never_deletes(tmp_path):
    old = tmp_path / 'old_run'
    old.mkdir()
    low = plan(True, False, tmp_path, 'run', 20.0, lambda _: 5.0)
    assert low.output is None and low.warn
    assert 'Free space' in low.message and 'never deleted' in low.message
    assert old.exists()


def test_free_gb_measures_where_the_directory_would_be(tmp_path):
    assert free_gb(tmp_path / 'not' / 'yet') > 0


def test_run_record_passes_its_fields_through():
    assert run_record('car', '', 'all', 'v1.0', 'abc') == {
        'target': 'car', 'sim': '', 'sensors': 'all', 'car': 'v1.0', 'car_config': 'abc',
        'log_exclude': ''}
    assert run_record('sim', 'gym', 'lidar', 'twin', 'abc')['sim'] == 'gym'


def test_run_record_says_what_the_log_left_out():
    record = run_record('car', '', 'all', 'v1.0', 'abc', '/sensors/camera/.*')
    assert record['log_exclude'] == '/sensors/camera/.*'
    cmd = record_command('/logs/run', record, 60, '/sensors/camera/.*')
    assert 'log_exclude=/sensors/camera/.*' in cmd[cmd.index('--custom-data') + 1:]


def test_record_command_logs_every_topic_to_mcap_in_chunks():
    cmd = record_command('/logs/run', {'target': 'car', 'sim': '', 'car': 'v1.0'}, 60)
    assert cmd[:3] == ['ros2', 'bag', 'record']
    assert '--all-topics' in cmd
    assert cmd[cmd.index('--storage') + 1] == 'mcap'
    assert cmd[cmd.index('--output') + 1] == '/logs/run'
    assert cmd[cmd.index('--max-bag-duration') + 1] == '60'
    # nothing limits it to some topics
    assert not {'--topics', '--regex', '-e', '--exclude-regex', '--exclude-topics'} & set(cmd)
    # the record goes last (--custom-data takes every argument after it); empty fields left out
    assert cmd[cmd.index('--custom-data') + 1:] == ['target=car', 'car=v1.0']


def test_record_command_excludes_topics_only_when_asked():
    record = {'target': 'car'}
    assert '--exclude-regex' not in record_command('/logs/run', record, 60, '')
    cmd = record_command('/logs/run', record, 60, '/sensors/camera/.*')
    assert cmd[cmd.index('--exclude-regex') + 1] == '/sensors/camera/.*'
    assert cmd.index('--exclude-regex') < cmd.index('--custom-data')


def test_check_exclude_refuses_what_rosbag2_cant_use_or_record():
    assert check_exclude('') is None
    assert check_exclude('/sensors/camera/.*|/sensors/ground_camera') is None
    assert '=' in check_exclude('/a(?=b)')
    assert 'regex' in check_exclude('/sensors/camera/[')


def test_lost_messages_reads_rosbag2s_reports():
    assert lost_messages(
        "[WARN] [r]: Messages lost on transport layer for topic '/x'. Total lost: 12") == 12
    assert lost_messages('[WARN] [rosbag2_cpp]: Total lost: 3') == 3
    assert lost_messages('[INFO] [r]: Recording...') is None



def test_started_only_once_rosbag2_records_into_the_directory(tmp_path):
    line = '[INFO] [1793293385.2] [arc_logging_recorder]: Recording...'
    assert not started(line, tmp_path / 'run')  # the directory isn't there yet
    (tmp_path / 'run').mkdir()
    assert started(line, tmp_path / 'run')
    assert not started("[INFO] [arc_logging_recorder]: Starting recording to 'run'",
                       tmp_path / 'run')


def test_start_status_never_says_recording():
    # starting: a warning only for a run that must be logged
    starting = 'starting the recorder: not recording yet'
    assert start_status(None, 1.0, 10.0, True) == (True, starting)
    assert start_status(None, 1.0, 10.0, False) == (False, starting)
    warn, message = start_status(1, 1.0, 10.0, False)
    assert warn and 'failed to start (exit code 1)' in message and 'not recorded' in message
    warn, message = start_status(None, 11.0, 10.0, False)
    assert warn and 'has not started' in message and 'start_timeout_s' in message
    for status in (start_status(None, 1.0, 10.0, True), start_status(1, 1.0, 10.0, True)):
        assert 'recording to' not in status[1]
