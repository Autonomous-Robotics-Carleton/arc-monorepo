"""When a run that should be logged isn't, the operator is warned on
/diagnostics and nothing stops (#152). Run once per case by CMakeLists.txt:

    log_off        must_log, but log:=false was passed
    no_log_dir     must_log, and neither log_dir:= nor $ARC_LOG_DIR is set
    low_space      less free space than min_free_gb: nothing is recorded or deleted
    low_space_running  free space drops below warn_free_gb while recording
    recorder_dies  the recorder is killed once it is recording
    fails_to_start rosbag2 can't create the log directory, so the recorder exits
                   before it records anything
    bad_exclude    log_exclude can't be used: the whole run is recorded instead
"""

import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

from diagnostic_msgs.msg import DiagnosticStatus
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription, UnsetEnvironmentVariable
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import PathJoinSubstitution
from launch_ros.substitutions import FindPackageShare
import launch_testing
import launch_testing.actions
import launch_testing.asserts
import pytest

sys.path.insert(0, os.path.dirname(__file__))
from diagnostics import wait_for_status  # noqa: E402

LOG_DIR = Path(tempfile.mkdtemp(prefix='arc_logging_test_'))
CASE = dict(a.split(':=', 1) for a in sys.argv if ':=' in a).get('case', 'log_off')
CASES = {
    # launch arguments, and what the warning says
    'log_off': ({'log': 'false', 'must_log': 'true', 'log_dir': str(LOG_DIR)}, 'log:=false'),
    'no_log_dir': ({'log': 'true', 'must_log': 'true'}, '$ARC_LOG_DIR is not set'),
    'low_space': ({'log': 'true', 'log_dir': str(LOG_DIR), 'min_free_gb': '1e12'},
                  'Free space on the log drive'),
    'low_space_running': ({'log': 'true', 'log_dir': str(LOG_DIR), 'min_free_gb': '0',
                           'warn_free_gb': '1e12'}, 'LOW SPACE'),
    'recorder_dies': ({'log': 'true', 'log_dir': str(LOG_DIR), 'min_free_gb': '0',
                       'warn_free_gb': '0'}, 'RECORDER STOPPED'),
    # /proc can't hold directories, even for root; it reports no free space, hence min_free_gb 0
    'fails_to_start': ({'log': 'true', 'must_log': 'true', 'min_free_gb': '0',
                        'log_dir': f'/proc/arc_logging_test_{os.getpid()}'},
                       'the recorder failed to start (exit code 1)'),
    'bad_exclude': ({'log': 'true', 'log_dir': str(LOG_DIR), 'min_free_gb': '0',
                     'warn_free_gb': '0', 'log_exclude': '/a(?=b)'},
                    'every topic is recorded'),
}


@pytest.mark.launch_test
def generate_test_description():
    logging = PathJoinSubstitution([
        FindPackageShare('arc_logging'), 'launch', 'logging.launch.py',
    ])
    args, _ = CASES[CASE]
    return LaunchDescription([
        UnsetEnvironmentVariable('ARC_LOG_DIR'),
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(logging),
            launch_arguments={'target': 'car', 'car': 'twin', **args}.items()),
        launch_testing.actions.ReadyToTest(),
    ])


class TestWarning(unittest.TestCase):

    def test_operator_is_warned(self, proc_info):
        _, text = CASES[CASE]
        if CASE == 'recorder_dies':
            # "recording to" only once rosbag2 is recording into the run's directory
            self.assertIsNotNone(wait_for_status(DiagnosticStatus.OK, 'recording to'))
            self.assertEqual(len(list(LOG_DIR.iterdir())), 1)
            subprocess.run(['pkill', '-KILL', '-f', f'ros2 bag record .*{LOG_DIR}'], check=True)
        status = wait_for_status(DiagnosticStatus.WARN, text)
        self.assertIsNotNone(status, f'no WARN on /diagnostics saying "{text}"')
        # still running: an unlogged run is reported, never stopped
        self.assertIsNotNone(wait_for_status(DiagnosticStatus.WARN, text))


@launch_testing.post_shutdown_test()
class TestShutdown(unittest.TestCase):

    def test_exit_codes(self, proc_info):
        launch_testing.asserts.assertExitCodes(proc_info)

    def test_nothing_recorded_or_deleted(self):
        runs = list(LOG_DIR.iterdir())
        if CASE in ('recorder_dies', 'bad_exclude', 'low_space_running'):
            self.assertEqual(len(runs), 1)  # the run up to where the recorder died, or all of it
        else:
            self.assertEqual(runs, [])
