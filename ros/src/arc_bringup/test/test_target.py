"""The platform comes up on a target: the target's backend and the platform
nodes start and stay up. Logging follows the target's default: the car's run
is recorded (into a temporary directory), the others are not. It also checks
the default car configuration (twin): if that fails to load, the launch stops
before anything starts.

Run once per backend by CMakeLists.txt: target:=sim with each sim:=, then
target:=replay and target:=car.
"""

import sys
import tempfile
import unittest

import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare


@pytest.mark.launch_test
def generate_test_description():
    platform = PathJoinSubstitution([
        FindPackageShare('arc_bringup'), 'launch', 'platform.launch.py',
    ])
    return LaunchDescription([
        DeclareLaunchArgument('target'),
        DeclareLaunchArgument('sim', default_value='gym'),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(platform),
            launch_arguments={'target': LaunchConfiguration('target'),
                              'sim': LaunchConfiguration('sim'),
                              'log_dir': tempfile.mkdtemp(prefix='arc_bringup_test_')}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


def expected_backend():
    """The name the chosen backend logs, from this test's launch arguments."""
    args = dict(a.split(':=', 1) for a in sys.argv if ':=' in a)
    target = args.get('target', 'sim')
    return f"sim ({args.get('sim', 'gym')}) backend" if target == 'sim' else f'{target} backend'


class TestBackendStarts(unittest.TestCase):

    def test_chosen_backend_reports_in(self, proc_output):
        proc_output.assertWaitFor(f'{expected_backend()}: not implemented yet', timeout=30)

    def test_arc_control_reports_in(self, proc_output):
        proc_output.assertWaitFor('arc_control: operator mode, waiting for teleop', timeout=30)

    def test_arc_logging_follows_the_targets_default(self, proc_output):
        on_car = dict(a.split(':=', 1) for a in sys.argv if ':=' in a).get('target') == 'car'
        expected = ('[arc_logging_recorder]: Recording...' if on_car
                    else 'arc_logging: not logging (log:=false)')
        proc_output.assertWaitFor(expected, timeout=30)


@launch_testing.post_shutdown_test()
class TestShutdown(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)
