"""Starts the platform on one target (ADR-0028), and in simulation on one
simulator (ADR-0032).

    ros2 launch arc_bringup platform.launch.py target:=sim sim:=gazebo|webots|gym
    ros2 launch arc_bringup platform.launch.py target:=replay
    ros2 launch arc_bringup platform.launch.py target:=car car:=<car>

The platform nodes are the same on every target; only the backend that
implements the car interface (arc_msgs) changes. `sensors:=` picks the
sensor profile a simulator renders (ADR-0032); real targets ignore it.
`car:=` picks the car configuration (config/cars/<car>.yaml, ADR-0051),
checked before anything starts. `log:=` records the run to MCAP
(arc_logging): on by default on the car, where an unlogged run is a warning
and $ARC_LOG_DIR (or `log_dir:=`) must be set; off elsewhere.
`log_profile:=` picks what's recorded (ADR-0056): every topic but camera
images by default, every topic with `full`. A replay's log (target:=replay
log:=true) leaves out what the player replays, so it holds only what the
live platform produced, and its run record points back to `bag:=` (#205).
The workspace's git version (arc_version) is printed at start and goes into
the run record (#160).
Platform nodes are added here as they're written.
"""

from pathlib import Path

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, LogInfo, OpaqueFunction
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.substitutions import FindPackageShare

from arc_car_config import config_hash, load, top_speed

TARGETS = ('sim', 'replay', 'car')
SIMS = ('gazebo', 'webots', 'gym')
SENSOR_PROFILES = ('lidar', 'stereo', 'all')
# What the replay player publishes from the log: the car's topics
# (arc_backend_replay's kCarTopics) and /clock. A replay's log leaves them out
# (#205). /diagnostics stays: the live platform publishes there too.
REPLAYED_TOPICS = r'^/(sensors/.+|car/(motor/.+|steering|power|safety_state|faults|link)|clock)$'


def ros_version():
    """The git commit this workspace was built from, '-dirty' if it had changes (#160)."""
    share = Path(get_package_share_directory('arc_version'))
    return (share / 'version.txt').read_text().strip()


def backend_package(target, sim):
    return f'arc_backend_sim_{sim}' if target == 'sim' else f'arc_backend_{target}'


def include_backend(context):
    target = LaunchConfiguration('target').perform(context)
    sim = LaunchConfiguration('sim').perform(context)
    share = FindPackageShare(backend_package(target, sim)).perform(context)
    return [IncludeLaunchDescription(
        PythonLaunchDescriptionSource(f'{share}/launch/backend.launch.py'),
        launch_arguments={'sensors': LaunchConfiguration('sensors')}.items())]


def logging_args(target, sim, log, log_exclude='', bag=''):
    """What arc_logging is told for this target: it never looks at the target itself."""
    on_car = target == 'car'
    replay = target == 'replay'
    exclude = log_exclude
    if replay:
        exclude = f'(?:{log_exclude})|{REPLAYED_TOPICS}' if log_exclude else REPLAYED_TOPICS
    return {
        'log': log or str(on_car).lower(),  # every car run is logged unless log:=false
        'must_log': str(on_car).lower(),
        'sim': sim if target == 'sim' else '',
        'log_exclude': exclude,
        'source_log': bag if replay else '',
    }


def include_logging(context):
    def arg(name):
        return LaunchConfiguration(name).perform(context)

    share = FindPackageShare('arc_logging').perform(context)
    args = {name: arg(name)
            for name in ('target', 'sensors', 'car', 'log_profile', 'log_dir', 'log_exclude')}
    args.update(logging_args(arg('target'), arg('sim'), arg('log'), arg('log_exclude'),
                             arg('bag')))
    args['ros_version'] = ros_version()
    return [IncludeLaunchDescription(
        PythonLaunchDescriptionSource(f'{share}/launch/logging.launch.py'),
        launch_arguments=args.items())]


def car_config(context):
    car = LaunchConfiguration('car').perform(context)
    values = load(car)  # a missing or invalid car file stops the launch here
    return [LogInfo(msg=f'car {car} ({values["build"]}): config {config_hash(values)[:12]}, '
                        f'top speed {top_speed(values):.1f} m/s'),
            LogInfo(msg=f'software: ros {ros_version()}')]


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
        DeclareLaunchArgument(
            'log', default_value='', choices=('', 'true', 'false'),
            description='Record the run to MCAP (default: true on target:=car, else false)'),
        DeclareLaunchArgument(
            'log_profile', default_value='minimal', choices=('minimal', 'full'),
            description='What a logged run records (ADR-0056): minimal, every topic but '
                        'camera images; full, every topic'),
        DeclareLaunchArgument(
            'log_dir', default_value='',
            description='Where runs are logged (default: $ARC_LOG_DIR, which the car '
                        'requires; elsewhere ~/arc_logs if it is unset)'),
        DeclareLaunchArgument(
            'bag', default_value='',
            description='The log to play, with target:=replay: a .mcap file or a bag directory'),
        DeclareLaunchArgument(
            'log_exclude', default_value='',
            description='Regex of topics not to record (rosbag2 --exclude-regex; '
                        'default: record every topic)'),
        OpaqueFunction(function=car_config),
        # every topic of the run to MCAP (#152), and whether it's being logged on /diagnostics
        OpaqueFunction(function=include_logging),
        OpaqueFunction(function=include_backend),
        # who drives: the operator or the experiment (#151)
        Node(package='arc_control', executable='control', output='screen'),
    ])
