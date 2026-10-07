"""The platform comes up on a target: the target's backend starts and stays up.

Run once per target by CMakeLists.txt with target:=sim|replay|car.
"""

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
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(platform),
            launch_arguments={'target': LaunchConfiguration('target')}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


class TestBackendStarts(unittest.TestCase):

    def test_backend_reports_in(self, proc_output):
        proc_output.assertWaitFor('backend: not implemented yet', timeout=30)


@launch_testing.post_shutdown_test()
class TestShutdown(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)
