"""When a run that should be logged isn't, the operator is warned on
/diagnostics and nothing stops (#152). Run once per case by CMakeLists.txt:

    log_off        must_log, but log:=false was passed
    no_log_dir     must_log, and neither log_dir:= nor $ARC_LOG_DIR is set
    low_space      less free space than min_free_gb: nothing is recorded or deleted
    recorder_dies  the recorder is killed mid-run
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
    'recorder_dies': ({'log': 'true', 'log_dir': str(LOG_DIR)}, 'RECORDER STOPPED'),
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
            self.assertIsNotNone(wait_for_status(DiagnosticStatus.OK, 'recording to'))
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
        if CASE == 'recorder_dies':
            self.assertEqual(len(runs), 1)  # the run up to where the recorder died
        else:
            self.assertEqual(runs, [])
