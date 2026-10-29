"""Logs the run to MCAP, in chunks, from the platform's start to its stop
(SYS-09, #152): every topic but camera images, or every topic with
log_profile:=full (ADR-0056). Started by arc_bringup's platform.launch.py,
which decides whether a run is logged by default.

    ros2 launch arc_logging logging.launch.py log:=true [log_profile:=full] [log_dir:=DIR] \
        [target:=… car:=…]

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

from arc_logging.recorder import PROFILES

ARGS = {
    'log': ('false', 'Record this run'),
    'must_log': ('false', 'Warn if this run is not logged; no default log directory'),
    'log_dir': ('', 'Where runs are logged (default: $ARC_LOG_DIR, else ~/arc_logs '
                    'unless must_log)'),
    'log_profile': ('minimal', 'Which topics to record (ADR-0056): minimal, every topic but '
                               'camera images; full, every topic'),
    'min_free_gb': ('-1.0', 'Free space needed to start logging, GB; negative: the '
                            'profile\'s (TBC: minimal 20, full 270)'),
    'warn_free_gb': ('-1.0', 'Free space below which a recording run warns, GB; negative: '
                             'the profile\'s (TBC: minimal 10, full 60)'),
    'chunk_s': ('15', 'TBC: length of each log chunk, s: the most a crash or power loss loses'),
    'start_timeout_s': ('5.0', 'TBC: how long the recorder may take to start recording, s, '
                               'before the operator is warned'),
    'log_exclude': ('', 'Regex of topics not to record (rosbag2 --exclude-regex); '
                        'empty records every topic'),
    'target': ('', 'Run record: the target'),
    'sim': ('', 'Run record: the simulator, if any'),
    'sensors': ('', 'Run record: the sensor profile'),
    'car': ('twin', 'Run record: the car configuration'),
}
TYPES = {'log': bool, 'must_log': bool, 'min_free_gb': float, 'warn_free_gb': float,
         'chunk_s': int, 'start_timeout_s': float}


def generate_launch_description():
    return LaunchDescription([
        *(DeclareLaunchArgument(name, default_value=default, description=description,
                                **({'choices': tuple(PROFILES)} if name == 'log_profile' else {}))
          for name, (default, description) in ARGS.items()),
        Node(
            package='arc_logging', executable='logger', output='screen',
            parameters=[{name: ParameterValue(LaunchConfiguration(name),
                                              value_type=TYPES.get(name, str))
                         for name in ARGS}],
            # time for the recorder to write out its cache and close the file
            sigterm_timeout='30'),
    ])
