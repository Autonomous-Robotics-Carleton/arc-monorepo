"""Starts the platform on one target (ADR-0028), and in simulation on one
simulator (ADR-0032).

    ros2 launch arc_bringup platform.launch.py target:=sim sim:=gazebo|webots|gym
    ros2 launch arc_bringup platform.launch.py target:=replay
    ros2 launch arc_bringup platform.launch.py target:=car

The platform nodes are the same on every target; only the backend that
implements the car interface (arc_msgs) changes. `sensors:=` picks the
sensor profile a simulator renders (ADR-0032); real targets ignore it.
Platform nodes are added here as they're written.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.substitutions import FindPackageShare

TARGETS = ('sim', 'replay', 'car')
SIMS = ('gazebo', 'webots', 'gym')
SENSOR_PROFILES = ('lidar', 'stereo', 'all')


def backend_package(target, sim):
    return f'arc_backend_sim_{sim}' if target == 'sim' else f'arc_backend_{target}'


def include_backend(context):
    target = LaunchConfiguration('target').perform(context)
    sim = LaunchConfiguration('sim').perform(context)
    share = FindPackageShare(backend_package(target, sim)).perform(context)
    return [IncludeLaunchDescription(
        PythonLaunchDescriptionSource(f'{share}/launch/backend.launch.py'),
        launch_arguments={'sensors': LaunchConfiguration('sensors')}.items())]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument(
            'target', default_value='sim', choices=TARGETS,
            description='Which backend implements the car interface'),
        DeclareLaunchArgument(
            'sim', default_value='gym', choices=SIMS,
            description='Which simulator, when target:=sim'),
        DeclareLaunchArgument(
            'sensors', default_value='lidar', choices=SENSOR_PROFILES,
            description='Sensor profile a simulator renders'),
        OpaqueFunction(function=include_backend),
    ])
