"""The run's name, directory, record and the recorder's command line."""

from datetime import datetime, timezone

from arc_logging.recorder import log_root, record_command, run_dir, run_name, run_record

START = datetime(2026, 10, 29, 14, 5, 1, tzinfo=timezone.utc)


def test_run_name_sorts_by_start_time_and_says_where():
    assert run_name('sim', 'gym', START) == '20261029T140501Z_sim-gym'
    assert run_name('car', 'gym', START) == '20261029T140501Z_car'
    assert run_name('replay', 'webots', START) == '20261029T140501Z_replay'


def test_run_dir_never_reuses_a_directory(tmp_path):
    first = run_dir(tmp_path, 'run')
    assert first == tmp_path / 'run'
    first.mkdir()
    second = run_dir(tmp_path, 'run')
    assert second == tmp_path / 'run-1'
    second.mkdir()
    assert run_dir(tmp_path, 'run') == tmp_path / 'run-2'


def test_log_root_follows_the_environment(monkeypatch, tmp_path):
    monkeypatch.setenv('ARC_LOG_DIR', str(tmp_path))
    assert log_root() == tmp_path
    monkeypatch.delenv('ARC_LOG_DIR')
    assert log_root().name == 'arc_logs'


def test_run_record_names_the_simulator_only_in_simulation():
    assert run_record('sim', 'gym', 'lidar', 'twin', 'abc')['sim'] == 'gym'
    assert 'sim' not in run_record('car', 'gym', 'lidar', 'v1.0', 'abc')
    assert run_record('car', 'gym', 'all', 'v1.0', 'abc') == {
        'target': 'car', 'sensors': 'all', 'car': 'v1.0', 'car_config': 'abc'}


def test_record_command_logs_every_topic_to_mcap():
    cmd = record_command('/logs/run', {'target': 'sim', 'car': 'twin'})
    assert cmd[:3] == ['ros2', 'bag', 'record']
    assert '--all-topics' in cmd
    assert cmd[cmd.index('--storage') + 1] == 'mcap'
    assert cmd[cmd.index('--output') + 1] == '/logs/run'
    assert cmd[cmd.index('--node-name') + 1] == 'arc_logging'
    # nothing limits it to some topics
    assert not {'--topics', '--regex', '-e', '--exclude-regex', '--exclude-topics'} & set(cmd)
    # the record goes last: --custom-data takes every argument after it
    assert cmd[cmd.index('--custom-data') + 1:] == ['target=sim', 'car=twin']
