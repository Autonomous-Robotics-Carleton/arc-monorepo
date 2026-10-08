"""Starts the Webots simulator backend of the car interface (ADR-0028)."""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch_ros.actions import Node


def generate_launch_description():
    return LaunchDescription([
        # sensor profile to render (ADR-0032); unused until the simulator is wired up
        DeclareLaunchArgument('sensors', default_value='lidar'),
        Node(package='arc_backend_sim_webots', executable='backend', output='screen'),
    ])
