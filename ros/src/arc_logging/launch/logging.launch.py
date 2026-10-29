"""Logs the run: every topic to MCAP, in chunks, from the platform's start to
its stop (SYS-09, #152). Started by arc_bringup's platform.launch.py, which
decides whether a run is logged by default.

    ros2 launch arc_logging logging.launch.py log:=true [log_dir:=DIR] [target:=… car:=…]

`log` records the run; `must_log` makes an unlogged run a warning and
requires the log directory to be set explicitly (log_dir:= or $ARC_LOG_DIR,
no ~/arc_logs fallback). The other fields are passed through into the run's
name and record. The arc_logging node runs the recorder and reports on
/diagnostics; see arc_logging/node.py.
"""

from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue

ARGS = {
    'log': ('false', 'Record this run'),
    'must_log': ('false', 'Warn if this run is not logged; no default log directory'),
    'log_dir': ('', 'Where runs are logged (default: $ARC_LOG_DIR, else ~/arc_logs '
                    'unless must_log)'),
    'min_free_gb': ('20.0', 'TBC: free space needed to start logging, GB: '
                            'log rate x the longest run (SYS-03) x a margin'),
    'chunk_s': ('60', 'TBC: length of each log chunk, s: the most a crash or power loss loses'),
    'log_exclude': ('', 'Regex of topics not to record (rosbag2 --exclude-regex); '
                        'empty records every topic'),
    'target': ('', 'Run record: the target'),
    'sim': ('', 'Run record: the simulator, if any'),
    'sensors': ('', 'Run record: the sensor profile'),
    'car': ('twin', 'Run record: the car configuration'),
}
TYPES = {'log': bool, 'must_log': bool, 'min_free_gb': float, 'chunk_s': int}


def generate_launch_description():
    return LaunchDescription([
        *(DeclareLaunchArgument(name, default_value=default, description=description)
          for name, (default, description) in ARGS.items()),
        Node(
            package='arc_logging', executable='logger', output='screen',
            parameters=[{name: ParameterValue(LaunchConfiguration(name),
                                              value_type=TYPES.get(name, str))
                         for name in ARGS}],
            # time for the recorder to write out its cache and close the file
            sigterm_timeout='30'),
    ])
