"""Starts the replay backend of the car interface (ADR-0028)."""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        # accepted for a uniform interface; real targets have their sensors
        DeclareLaunchArgument('sensors', default_value='lidar'),
        Node(package='arc_backend_replay', executable='backend', output='screen'),
    ])
