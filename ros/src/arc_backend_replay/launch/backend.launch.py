"""Starts the replay backend of the car interface (ADR-0028)."""

from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        Node(package='arc_backend_replay', executable='backend', output='screen'),
    ])
