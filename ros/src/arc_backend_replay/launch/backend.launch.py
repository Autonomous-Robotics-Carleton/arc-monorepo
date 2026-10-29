"""Starts the replay backend of the car interface (ADR-0028): plays a rosbag2
MCAP log back as the car.

    ros2 launch arc_bringup platform.launch.py target:=replay log:=<log> rate:=2.0

`log:=` is a .mcap file or a bag directory; `rate:=` plays it faster (2.0) or
slower (0.5) than it was recorded; `start_paused:=true` waits for
/replay_player/resume.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    return LaunchDescription([
        # accepted for a uniform interface; a log has the sensors it was recorded with
        DeclareLaunchArgument('sensors', default_value='lidar'),
        DeclareLaunchArgument('log', default_value='',
                              description='The log to play: a .mcap file or a bag directory'),
        DeclareLaunchArgument('rate', default_value='1.0',
                              description='Playback speed: 1.0 as recorded, 2.0 twice as fast'),
        DeclareLaunchArgument('start_paused', default_value='false',
                              description='Wait for /replay_player/resume before playing'),
        Node(package='arc_backend_replay', executable='backend', output='screen',
             parameters=[{
                 'log': ParameterValue(LaunchConfiguration('log'), value_type=str),
                 'rate': ParameterValue(LaunchConfiguration('rate'), value_type=float),
                 'start_paused': ParameterValue(LaunchConfiguration('start_paused'),
                                                value_type=bool),
             }]),
    ])
