"""Logs the run: every topic to one MCAP log, from the platform's start to its
stop (SYS-09, #152). Started by arc_bringup's platform.launch.py.

    ros2 launch arc_logging logging.launch.py target:=sim sim:=gym car:=twin [log_dir:=DIR]

The recorder is its own process, so an experiment that crashes or hangs
can't take it down (SYS-21). It doesn't hold up publishers either: with the
ICD's keep-last profiles a slow subscriber only loses samples, and the
recorder's callbacks only copy into its cache, which a separate thread
writes to disk. On shutdown launch stops it with SIGINT, and it writes out
its cache and closes the MCAP file.
"""

from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    ExecuteProcess,
    LogInfo,
    OpaqueFunction,
    RegisterEventHandler,
)
from launch.event_handlers import OnProcessExit
from launch.substitutions import LaunchConfiguration

from arc_car_config import config_hash, load
from arc_logging import log_root, record_command, run_dir, run_name, run_record


def recorder(context):
    def arg(name):
        return LaunchConfiguration(name).perform(context)

    target, sim, car = arg('target'), arg('sim'), arg('car')
    output = run_dir(arg('log_dir') or log_root(), run_name(target, sim))
    output.parent.mkdir(parents=True, exist_ok=True)

    # Part 2 of #152: the full run record (versions, envelope, profile, who
    # drove) is added to this as each piece exists
    record = run_record(target, sim, arg('sensors'), car, config_hash(load(car)))

    process = ExecuteProcess(cmd=record_command(output, record), name='arc_logging',
                             output='screen')
    return [
        LogInfo(msg=f'arc_logging: logging this run to {output}'),
        process,
        # The platform keeps running without it (SYS-21), but says so: the
        # rest of the run isn't logged
        RegisterEventHandler(OnProcessExit(
            target_action=process,
            on_exit=lambda event, _: [LogInfo(
                msg=f'arc_logging: the recorder stopped (exit code {event.returncode}); '
                    f'{output} holds the run up to here')])),
    ]


def generate_launch_description():
    return LaunchDescription([
        DeclareLaunchArgument('target', default_value='sim'),
        DeclareLaunchArgument('sim', default_value='gym'),
        DeclareLaunchArgument('sensors', default_value='lidar'),
        DeclareLaunchArgument('car', default_value='twin'),
        DeclareLaunchArgument(
            'log_dir', default_value='',
            description='Where runs are logged (default: $ARC_LOG_DIR, else ~/arc_logs)'),
        OpaqueFunction(function=recorder),
    ])
