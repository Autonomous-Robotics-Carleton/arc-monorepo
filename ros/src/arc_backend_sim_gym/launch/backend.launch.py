"""Starts the F1TENTH gym simulator backend of the car interface (ADR-0028,
ADR-0032), in interface mode (ADR-0047).

Included by arc_bringup's platform.launch.py, whose car:= it sees; on its own:

    ros2 launch arc_backend_sim_gym backend.launch.py car:=twin real_time_factor:=0

Other settings (the map, the start pose, the TBC vehicle values) are the
node's parameters: see arc_backend_sim_gym/node.py.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def generate_launch_description():
    return LaunchDescription([
        # sensor profile to render (ADR-0032): the gym has the LiDAR only
        DeclareLaunchArgument('sensors', default_value='lidar'),
        # the car configuration, config/cars/<car>.yaml; platform.launch.py's when included
        DeclareLaunchArgument('car', default_value='twin'),
        # 1 runs at real time, 0 as fast as it can
        DeclareLaunchArgument('real_time_factor', default_value='1.0'),
        Node(
            package='arc_backend_sim_gym', executable='backend', output='screen',
            parameters=[{
                'sensors': LaunchConfiguration('sensors'),
                'car': LaunchConfiguration('car'),
                'real_time_factor': ParameterValue(
                    LaunchConfiguration('real_time_factor'), value_type=float),
            }]),
    ])
