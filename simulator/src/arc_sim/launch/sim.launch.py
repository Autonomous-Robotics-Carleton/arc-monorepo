"""Start Gazebo with the racecar, bridge its topics to ROS 2, and start car_link.

ros2 launch arc_sim sim.launch.py [world:=<path>] [gui:=false] [bridge:=false]

The world starts with no track. bridge:=true adds the ground-station bridge (track files and
car state on port 8765), which loads a track into the running sim when the ground station
asks for one (see scripts/bridge.py and arc_sim/track.py).
"""

import os
import re

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (AppendEnvironmentVariable, DeclareLaunchArgument, IncludeLaunchDescription,
                            OpaqueFunction)
from launch.conditions import IfCondition, UnlessCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import Command, LaunchConfiguration, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue


def world_actions(context):
    """Everything that depends on the world file: Gazebo itself, the spawn, the bridge."""
    share = get_package_share_directory('arc_sim')
    gz_launch = os.path.join(get_package_share_directory('ros_gz_sim'), 'launch', 'gz_sim.launch.py')
    world = LaunchConfiguration('world').perform(context)
    world_name = re.search(r'<world\s+name="([^"]+)"', open(world).read()).group(1)
    gui = LaunchConfiguration('gui')
    bridge = LaunchConfiguration('bridge')

    return [
        # Headless server (-s) unless gui:=true. -r starts the simulation running.
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch),
            launch_arguments={'gz_args': '-s -r ' + world}.items(),
            condition=UnlessCondition(gui),
        ),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(gz_launch),
            launch_arguments={'gz_args': '-r %s --gui-config %s' % (
                world, os.path.join(share, 'config', 'gui.config'))}.items(),
            condition=IfCondition(gui),
        ),

        Node(
            package='ros_gz_sim',
            executable='create',
            arguments=['-topic', 'robot_description', '-name', 'racecar',
                       '-z', '0.01'],
        ),

        Node(
            package='arc_sim',
            executable='bridge.py',
            parameters=[{'world': world_name,
                         'wall_height': float(LaunchConfiguration('wall_height').perform(context))}],
            condition=IfCondition(bridge),
        ),
    ]


def generate_launch_description():
    share = get_package_share_directory('arc_sim')

    robot_description = ParameterValue(
        Command(['xacro ', PathJoinSubstitution([share, 'urdf', 'racecar.urdf.xacro'])]),
        value_type=str,
    )

    return LaunchDescription([
        DeclareLaunchArgument('world', default_value=os.path.join(share, 'worlds', 'empty.sdf')),
        DeclareLaunchArgument('wall_height', default_value='0.3', description='Track wall height, metres'),
        DeclareLaunchArgument('gui', default_value='false', description='Open the Gazebo 3D GUI (needs a display)'),
        DeclareLaunchArgument('bridge', default_value='false',
                              description='Serve track files and car state to the ground station on port 8765'),

        # Lets Gazebo resolve package://arc_sim/... mesh URIs.
        AppendEnvironmentVariable('GZ_SIM_RESOURCE_PATH', os.path.dirname(share)),

        OpaqueFunction(function=world_actions),

        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description, 'use_sim_time': True}],
        ),

        Node(
            package='ros_gz_bridge',
            executable='parameter_bridge',
            parameters=[{
                'config_file': os.path.join(share, 'config', 'bridge.yaml'),
                'use_sim_time': True,
            }],
        ),

        # The car's receiving end: heartbeat gate, UDP teleop and /sim/auto/cmd_vel -> /sim/cmd_vel.
        Node(
            package='arc_sim',
            executable='car_link.py',
        ),
    ])
