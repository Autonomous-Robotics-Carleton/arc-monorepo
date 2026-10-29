"""The replay backend plays the sample log (scripts/make_sample_log.py) through
the car interface: every message on the car's topics, with their stamps, at
the log's timing scaled by `rate:=`; /clock on the log's time; nothing on the
platform's own topics; and commands sent to the car are ignored.

Run by CMakeLists.txt once per rate, with log:= the sample log.
"""

import sys
import time
import unittest

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest
import rclpy
from arc_msgs.msg import ControlMode, DriveCommand, MotorStatus, SafetyState, SteeringStatus
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from rosbag2_interfaces.srv import Resume
from rosgraph_msgs.msg import Clock
from sensor_msgs.msg import Imu

# What the sample log holds
START_NS = 1_800_000_000 * 10**9
SAMPLES = 20  # per topic, 50 ms apart
SPAN_S = (SAMPLES - 1) * 0.05
CAR_TOPICS = {
    '/sensors/imu/cg': Imu,
    '/car/motor/fl': MotorStatus,
    '/car/steering': SteeringStatus,
    '/car/safety_state': SafetyState,
}
STATE_TOPICS = ('/car/safety_state', '/control/mode')


def rate():
    args = dict(a.split(':=', 1) for a in sys.argv if ':=' in a)
    return float(args.get('rate', '1.0'))


@pytest.mark.launch_test
def generate_test_description():
    backend = PathJoinSubstitution([
        FindPackageShare('arc_backend_replay'), 'launch', 'backend.launch.py',
    ])
    return LaunchDescription([
        DeclareLaunchArgument('log'),
        DeclareLaunchArgument('rate'),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(backend),
            # paused, so the test subscribes before the first message
            launch_arguments={'log': LaunchConfiguration('log'),
                              'rate': LaunchConfiguration('rate'),
                              'start_paused': 'true'}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


def qos(topic):
    if topic == '/clock':
        return QoSProfile(depth=1, reliability=ReliabilityPolicy.BEST_EFFORT)  # ROS's clock QoS
    durability = (DurabilityPolicy.TRANSIENT_LOCAL if topic in STATE_TOPICS
                  else DurabilityPolicy.VOLATILE)
    return QoSProfile(depth=100, reliability=ReliabilityPolicy.RELIABLE, durability=durability)


class TestReplay(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()
        cls.node = rclpy.create_node('test_replay')
        cls.arrivals = {topic: [] for topic in [*CAR_TOPICS, '/control/mode', '/clock']}
        topics = {**CAR_TOPICS, '/control/mode': ControlMode, '/clock': Clock}
        cls.subs = [
            cls.node.create_subscription(
                msg_type, topic,
                lambda msg, topic=topic: cls.arrivals[topic].append((time.monotonic(), msg)),
                qos(topic))
            for topic, msg_type in topics.items()
        ]
        cls.drive_pub = cls.node.create_publisher(DriveCommand, '/car/cmd/drive', qos('drive'))

    @classmethod
    def tearDownClass(cls):
        cls.node.destroy_node()
        rclpy.shutdown()

    def spin_until(self, done, timeout):
        end = time.monotonic() + timeout
        while not done() and time.monotonic() < end:
            rclpy.spin_once(self.node, timeout_sec=0.05)
        return done()

    def test_replay(self, proc_output):
        proc_output.assertWaitFor('replay backend: playing', timeout=30)

        # the player's publishers are up and matched before it's resumed
        self.assertTrue(self.spin_until(
            lambda: all(s.get_publisher_count() > 0 for s in self.subs
                        if s.topic_name in CAR_TOPICS), timeout=20),
            'the player never published the car topics')
        resume = self.node.create_client(Resume, '/replay_player/resume')
        self.assertTrue(resume.wait_for_service(timeout_sec=20))
        rclpy.spin_until_future_complete(self.node, resume.call_async(Resume.Request()),
                                         timeout_sec=10)

        expected_s = SPAN_S / rate()
        self.assertTrue(self.spin_until(
            lambda: all(len(self.arrivals[t]) == SAMPLES for t in CAR_TOPICS),
            timeout=expected_s + 20),
            {t: len(self.arrivals[t]) for t in CAR_TOPICS})
        proc_output.assertWaitFor('replay backend: end of log', timeout=10)
        self.spin_until(lambda: False, timeout=0.5)  # anything that shouldn't come

        for topic in CAR_TOPICS:
            with self.subTest(topic=topic):
                arrivals = self.arrivals[topic]
                self.assertEqual(len(arrivals), SAMPLES)
                # the sampling stamps are the log's, in order
                stamps = [m.header.stamp.sec * 10**9 + m.header.stamp.nanosec
                          for _, m in arrivals]
                self.assertEqual(stamps, [START_NS + i * 50_000_000 for i in range(SAMPLES)])
                # the log's timing, scaled by the rate (loose: CI machines are busy)
                span = arrivals[-1][0] - arrivals[0][0]
                self.assertAlmostEqual(span, expected_s, delta=0.25 * expected_s + 0.05)

        # /clock follows the log's time, and stops with it
        clock = [c.clock.sec * 10**9 + c.clock.nanosec for _, c in self.arrivals['/clock']]
        self.assertTrue(clock, 'no /clock')
        self.assertEqual(clock, sorted(clock))
        self.assertGreaterEqual(clock[0], START_NS)
        self.assertLess(clock[-1], START_NS + 2 * 10**9, 'the clock ran on past the log')
        ticks = len(self.arrivals['/clock'])
        self.spin_until(lambda: False, timeout=0.5)
        self.assertEqual(len(self.arrivals['/clock']), ticks, 'the clock ran on past the log')

        # the platform's own topics in the log stay out
        self.assertEqual(self.arrivals['/control/mode'], [])
        drive_publishers = [i.node_name for i in
                            self.node.get_publishers_info_by_topic('/car/cmd/drive')]
        self.assertEqual(drive_publishers, ['test_replay'])

    def test_commands_are_ignored(self, proc_output):
        self.assertTrue(self.spin_until(
            lambda: self.drive_pub.get_subscription_count() > 0, timeout=20))
        self.drive_pub.publish(DriveCommand(speed=1.0, accel=1.0))
        proc_output.assertWaitFor('replay backend: ignoring car/cmd/drive', timeout=10)


@launch_testing.post_shutdown_test()
class TestShutdown(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)
