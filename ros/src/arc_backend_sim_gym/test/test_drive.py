"""Closed loop through the car interface: an experiment drives the twin a few
metres down the oval's straight on the gym, through arc_control.

The test is the experiment (ADR-0048): it reads the LiDAR, steers to keep the
car between the walls, publishes on control/experiment and asks arc_control
for control. arc_control forwards its commands on car/cmd/drive, as on the
car, and the gym moves. The car starts off the centreline, heading for the
left wall, so it only gets the distance without hitting a wall if the loop
works. Ground truth comes from the gym's sim-only topics.
"""

import math
import time
import unittest

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest
import rclpy
from arc_car_config import load
from arc_msgs.msg import ControlMode, DriveCommand, MotorStatus
from arc_msgs.srv import SetControlMode
from launch import LaunchDescription
from launch_ros.actions import Node
from nav_msgs.msg import Odometry
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from sensor_msgs.msg import LaserScan
from std_msgs.msg import Bool

START = [0.0, 0.4, 0.15]  # base_link x, y (m), yaw (rad): 0.6 m from the left wall
DISTANCE = 3.0  # m to drive
SPEED = 1.0  # m/s
ACCEL = 1.0  # m/s^2
STEER_GAIN = 0.5  # rad per m of difference between the diagonal ranges

COMMANDS = QoSProfile(
    depth=1, reliability=ReliabilityPolicy.RELIABLE, durability=DurabilityPolicy.VOLATILE)
STATE = QoSProfile(
    depth=1, reliability=ReliabilityPolicy.RELIABLE, durability=DurabilityPolicy.TRANSIENT_LOCAL)


@pytest.mark.launch_test
def generate_test_description():
    return LaunchDescription([
        Node(package='arc_backend_sim_gym', executable='backend', output='screen',
             parameters=[{'start_pose': START}]),
        Node(package='arc_control', executable='control', output='screen'),
        launch_testing.actions.ReadyToTest(),
    ])


def range_at(scan, angle):
    return scan.ranges[round((angle - scan.angle_min) / scan.angle_increment)]


class Experiment:
    """Keeps the car between the walls: steers away from the nearer one, seen
    on the beams 45 degrees either side of ahead."""

    def __init__(self, node):
        self.node = node
        self.pub = node.create_publisher(DriveCommand, 'control/experiment', COMMANDS)
        node.create_subscription(LaserScan, 'sensors/lidar/scan', self.on_scan, 10)
        self.commands = 0

    def on_scan(self, scan):
        left, right = range_at(scan, math.pi / 4), range_at(scan, -math.pi / 4)
        cmd = DriveCommand()
        cmd.header.stamp = self.node.get_clock().now().to_msg()
        cmd.speed = SPEED
        cmd.accel = ACCEL
        cmd.steer = max(-0.4, min(0.4, STEER_GAIN * (left - right)))
        self.pub.publish(cmd)
        self.commands += 1


class TestClosedLoop(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        rclpy.init()

    @classmethod
    def tearDownClass(cls):
        rclpy.shutdown()

    def test_experiment_drives_a_few_metres(self, proc_output):
        # the backend's startup line comes once the gym has compiled
        proc_output.assertWaitFor('sim (gym) backend:', timeout=60)
        proc_output.assertWaitFor('arc_control: operator mode', timeout=30)

        node = rclpy.create_node('test_experiment')
        experiment = Experiment(node)
        poses, collided, modes, motor = [], [], [], []
        node.create_subscription(
            Odometry, 'sim/ground_truth',
            lambda m: poses.append((m.pose.pose.position.x, m.pose.pose.position.y,
                                    m.twist.twist.linear.x)), 100)
        node.create_subscription(Bool, 'sim/collision', lambda m: collided.append(m.data), STATE)
        node.create_subscription(ControlMode, 'control/mode', lambda m: modes.append(m.mode), STATE)
        node.create_subscription(MotorStatus, 'car/motor/rl', lambda m: motor.append(m.erpm), 100)
        set_mode = node.create_client(SetControlMode, 'control/set_mode')

        travelled, request, accepted = 0.0, None, False
        deadline = time.monotonic() + 60
        try:
            while time.monotonic() < deadline and travelled < DISTANCE:
                rclpy.spin_once(node, timeout_sec=0.01)
                # ask for control once commands are flowing, until granted
                if not accepted and experiment.commands > 0:
                    if request is not None and request.done():
                        accepted = request.result().accepted
                        request = None
                    if not accepted and request is None and set_mode.service_is_ready():
                        request = set_mode.call_async(
                            SetControlMode.Request(mode=ControlMode.MODE_EXPERIMENT))
                if len(poses) > 1:
                    (x0, y0, _), (x1, y1, _) = poses[-2], poses[-1]
                    travelled += math.hypot(x1 - x0, y1 - y0)
                    del poses[:-1]
        finally:
            node.destroy_node()

        self.assertTrue(accepted, 'arc_control never gave the experiment control')
        self.assertEqual(modes[-1], ControlMode.MODE_EXPERIMENT)
        self.assertGreaterEqual(travelled, DISTANCE, 'the car did not get the distance in 60 s')
        self.assertNotIn(True, collided, 'the car hit a wall')
        x, y, speed = poses[-1]
        # it found the centre between the walls (y = 0) and is going down the straight
        self.assertLess(abs(y), 0.25)
        self.assertAlmostEqual(speed, SPEED, delta=0.1)
        # the measurements agree with the motion: the rear wheel's eRPM is the speed
        car = load('twin')
        d = car['drivetrain']
        expected = speed / (math.pi * car['wheels']['rolling_diameter']) * 60 \
            * d['gear_ratio_rear'] * d['motor_pole_pairs']
        self.assertAlmostEqual(motor[-1], expected, delta=0.1 * expected)


@launch_testing.post_shutdown_test()
class TestShutdown(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)
