"""Starts the platform on one target (ADR-0028).

    ros2 launch arc_bringup platform.launch.py target:=sim|replay|car

The platform nodes are the same on every target; only the backend that
implements the car interface (arc_msgs) changes. Platform nodes are added
here as they're written.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration, PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare

TARGETS = ('sim', 'replay', 'car')


def generate_launch_description():
    target = LaunchConfiguration('target')
    backend = PathJoinSubstitution([
        FindPackageShare(['arc_backend_', target]), 'launch', 'backend.launch.py',
    ])
    return LaunchDescription([
        DeclareLaunchArgument(
            'target', default_value='sim', choices=TARGETS,
            description='Which backend implements the car interface'),
        IncludeLaunchDescription(PythonLaunchDescriptionSource(backend)),
    ])
