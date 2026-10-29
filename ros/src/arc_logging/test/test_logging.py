"""A short run produces an MCAP log, in chunks, that plain rosbag2 reads
back, and an experiment that crashes mid-run doesn't stop it (SYS-09, SYS-21).

A platform node and an experiment publish; the experiment is killed after
half a second while the platform carries on. After shutdown the log must
hold both, the platform's messages from after the crash, and the run record.
While recording, arc_logging reports OK on /diagnostics.
"""

import os
from pathlib import Path
import sys
import tempfile
import unittest

from diagnostic_msgs.msg import DiagnosticStatus
from launch import LaunchDescription
from launch.actions import ExecuteProcess, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
import launch_testing
import launch_testing.actions
import pytest
from rclpy.serialization import deserialize_message
import rosbag2_py
from std_msgs.msg import String

sys.path.insert(0, os.path.dirname(__file__))
from diagnostics import wait_for_status  # noqa: E402

LOG_DIR = Path(tempfile.mkdtemp(prefix='arc_logging_test_'))
TALKER = os.path.join(os.path.dirname(__file__), 'talker.py')
PLATFORM = 'test/platform'
EXPERIMENT = 'test/experiment'
LEFT_OUT = 'test/left_out'  # matches log_exclude: never recorded
CRASH_AFTER = 10  # messages, half a second at 20 Hz
CHUNK_S = 2  # so a few seconds' run is split


def talker(topic, *args):
    return ExecuteProcess(cmd=[sys.executable, TALKER, topic, *args],
                          name=topic.replace('/', '_'), output='screen')


@pytest.mark.launch_test
def generate_test_description():
    logging = PathJoinSubstitution([
        FindPackageShare('arc_logging'), 'launch', 'logging.launch.py',
    ])
    experiment = talker(EXPERIMENT, '--crash-after', str(CRASH_AFTER))
    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(logging),
            launch_arguments={'log': 'true', 'log_dir': str(LOG_DIR),
                              'chunk_s': str(CHUNK_S), 'log_exclude': f'/{LEFT_OUT}$',
                              'target': 'sim', 'sim': 'gym', 'car': 'twin'}.items()),
        talker(PLATFORM),
        talker(LEFT_OUT),
        experiment,
        launch_testing.actions.ReadyToTest(),
    ]), {'experiment': experiment}


class TestRun(unittest.TestCase):

    def test_reports_recording(self):
        status = wait_for_status(DiagnosticStatus.OK, 'recording to')
        self.assertIsNotNone(status)
        self.assertIn(str(LOG_DIR), status.message)

    def test_platform_outlives_the_experiment(self, proc_output, proc_info, experiment):
        proc_info.assertWaitForShutdown(process=experiment, timeout=30)
        # three seconds of the platform after the experiment started
        proc_output.assertWaitFor(f'{PLATFORM}: 60 sent', timeout=30, stream='stdout')


def read_log():
    """Every message in the run's log, by topic, as (receive time, count)."""
    (run,) = LOG_DIR.iterdir()
    reader = rosbag2_py.SequentialReader()
    reader.open(rosbag2_py.StorageOptions(uri=str(run), storage_id='mcap'),
                rosbag2_py.ConverterOptions('', ''))
    messages = {}
    while reader.has_next():
        topic, data, t = reader.read_next()
        if topic in (f'/{PLATFORM}', f'/{EXPERIMENT}'):
            messages.setdefault(topic, []).append((t, int(deserialize_message(data, String).data)))
    return run, messages


@launch_testing.post_shutdown_test()
class TestLog(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        for name in proc_info.process_names():
            expected = -9 if EXPERIMENT.replace('/', '_') in name else 0
            self.assertEqual(proc_info[name].returncode, expected, name)

    def test_one_mcap_log_for_the_run_in_chunks(self):
        (run,) = LOG_DIR.iterdir()
        self.assertRegex(run.name, r'^\d{8}T\d{6}Z_sim$')
        info = rosbag2_py.Info().read_metadata(str(run), 'mcap')
        self.assertEqual(info.storage_identifier, 'mcap')
        # a few seconds' run in CHUNK_S chunks, every one listed in the metadata
        chunks = sorted(p.name for p in run.glob('*.mcap'))
        self.assertGreaterEqual(len(chunks), 2)
        self.assertEqual(sorted(Path(f).name for f in info.relative_file_paths), chunks)

    def test_log_holds_both_and_the_platform_after_the_crash(self):
        _, messages = read_log()
        experiment = messages[f'/{EXPERIMENT}']
        platform = messages[f'/{PLATFORM}']
        # everything the experiment sent before it died, in order
        self.assertEqual([n for _, n in experiment], list(range(CRASH_AFTER)))
        # the platform's messages in order, none lost, and logging went on after the crash
        counts = [n for _, n in platform]
        self.assertEqual(counts, list(range(len(counts))))
        crash = experiment[-1][0]
        self.assertGreaterEqual(sum(t > crash for t, _ in platform), 40)

    def test_run_record_is_in_the_log(self):
        run, _ = read_log()
        record = rosbag2_py.Info().read_metadata(str(run), 'mcap').custom_data
        self.assertEqual(record['target'], 'sim')
        self.assertEqual(record['sim'], 'gym')
        self.assertEqual(record['car'], 'twin')
        self.assertRegex(record['car_config'], r'^[0-9a-f]{64}$')
        self.assertEqual(record['log_exclude'], f'/{LEFT_OUT}$')

    def test_excluded_topic_is_left_out(self):
        run, _ = read_log()
        info = rosbag2_py.Info().read_metadata(str(run), 'mcap')
        topics = {t.topic_metadata.name for t in info.topics_with_message_count}
        self.assertIn(f'/{PLATFORM}', topics)
        self.assertNotIn(f'/{LEFT_OUT}', topics)
