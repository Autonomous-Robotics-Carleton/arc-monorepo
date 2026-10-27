"""Starts the platform on one target (ADR-0028), and in simulation on one
simulator (ADR-0032).

    ros2 launch arc_bringup platform.launch.py target:=sim sim:=gazebo|webots|gym
    ros2 launch arc_bringup platform.launch.py target:=replay
    ros2 launch arc_bringup platform.launch.py target:=car car:=<car>

The platform nodes are the same on every target; only the backend that
implements the car interface (arc_msgs) changes. `sensors:=` picks the
sensor profile a simulator renders (ADR-0032); real targets ignore it.
`car:=` picks the car configuration (config/cars/<car>.yaml, ADR-0051),
checked before anything starts.
Platform nodes are added here as they're written.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, LogInfo, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.substitutions import FindPackageShare

from arc_car_config import config_hash, load, top_speed

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


def car_config(context):
    car = LaunchConfiguration('car').perform(context)
    values = load(car)  # a missing or invalid car file stops the launch here
    return [LogInfo(msg=f'car {car} ({values["build"]}): config {config_hash(values)[:12]}, '
                        f'top speed {top_speed(values):.1f} m/s')]


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
        DeclareLaunchArgument(
            'car', default_value='twin',
            description='Car configuration: config/cars/<car>.yaml'),
        OpaqueFunction(function=car_config),
        OpaqueFunction(function=include_backend),
    ])
