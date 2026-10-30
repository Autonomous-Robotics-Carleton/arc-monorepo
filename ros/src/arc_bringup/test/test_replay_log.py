"""A replay's log (target:=replay log:=true, #205) holds only what the live
platform produces, and its run record points back to the replayed log.

The test writes a small source log with a run record, replays it paused (so
the player's publishers stay up for the whole test) and records the run.
The recorder must never subscribe to the replayed topics or the player's
/clock, but does record /diagnostics; after shutdown, the replay's log names
its source and the source's car.
"""

import os
from pathlib import Path
import re
import tempfile
import time
import unittest

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest
import rosbag2_py
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from rclpy.serialization import serialize_message
from sensor_msgs.msg import Imu

WORK = Path(tempfile.mkdtemp(prefix='arc_replay_log_test_'))
SOURCE = WORK / 'source'
LOGS = WORK / 'logs'
SOURCE_RECORD = {'target': 'car', 'car': 'v1.0', 'car_config': 'abc123', 'log_profile': 'minimal'}
REPLAYED = ('/sensors/imu/cg', '/sensors/imu/front')


def write_source():
    writer = rosbag2_py.SequentialWriter()
    writer.open(rosbag2_py.StorageOptions(uri=str(SOURCE), storage_id='mcap',
                                          custom_data=SOURCE_RECORD),
                rosbag2_py.ConverterOptions('cdr', 'cdr'))
    for i, topic in enumerate(REPLAYED):
        writer.create_topic(rosbag2_py.TopicMetadata(i, topic, 'sensor_msgs/msg/Imu', 'cdr'))
    for n in range(10):
        for topic in REPLAYED:
            writer.write(topic, serialize_message(Imu()), 1_800_000_000 * 10**9 + n * 10**8)
    del writer


@pytest.mark.launch_test
def generate_test_description():
    write_source()
    platform = PathJoinSubstitution([
        FindPackageShare('arc_bringup'), 'launch', 'platform.launch.py',
    ])
    return LaunchDescription([
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(platform),
            launch_arguments={'target': 'replay', 'bag': str(SOURCE), 'start_paused': 'true',
                              'log': 'true', 'log_dir': str(LOGS)}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


def subscribed(proc_output):
    """The topics the recorder has subscribed to so far."""
    text = ''.join(o.text.decode() for o in proc_output)
    return set(re.findall(r"Subscribed to topic '([^']+)'", text))


class TestReplayLog(unittest.TestCase):

    def test_records_the_platform_and_not_the_replay(self, proc_output):
        proc_output.assertWaitFor('replay backend: playing', timeout=30)
        proc_output.assertWaitFor(re.compile(r'\[arc_logging_recorder\]: Recording\.\.\.'),
                                  timeout=30)
        proc_output.assertWaitFor("Subscribed to topic '/diagnostics'", timeout=30)
        # time for discovery to find the player's publishers, had they not been left out
        time.sleep(4.0)
        topics = subscribed(proc_output)
        for topic in (*REPLAYED, '/clock'):
            self.assertNotIn(topic, topics)
        self.assertIn('/diagnostics', topics)


@launch_testing.post_shutdown_test()
class TestReplayLogRecord(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)

    def test_the_log_names_its_source_and_holds_no_replayed_topic(self):
        runs = [p for p in LOGS.iterdir() if p.is_dir()]
        self.assertEqual(len(runs), 1, os.listdir(LOGS))
        metadata = rosbag2_py.Info().read_metadata(str(runs[0]), 'mcap')
        record = dict(metadata.custom_data)
        self.assertEqual(record['target'], 'replay')
        self.assertRegex(record['ros_version'], r'^([0-9a-f]{12}(-dirty)?|unknown)$')
        self.assertEqual(record['source_log'], str(SOURCE))
        for key, value in SOURCE_RECORD.items():
            self.assertEqual(record[f'source_{key}'], value)
        logged = {t.topic_metadata.name for t in metadata.topics_with_message_count}
        self.assertFalse(logged & {*REPLAYED, '/clock'}, logged)
