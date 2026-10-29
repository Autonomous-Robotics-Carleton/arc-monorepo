"""The arc_logging node: runs rosbag2's recorder for the run, watches it, and
tells the operator whether the run is being logged.

The recorder is a child process in its own session, so it outlives anything
an experiment does (SYS-21) and only this node signals it. If this node dies,
the kernel sends the recorder SIGINT and it still closes its file. At
shutdown the node forwards SIGINT and waits for the recorder to write out its
cache.

The run counts as recorded only once the recorder's own output shows rosbag2
recording ("Recording...", with the output directory created); until then
the status says the recorder is starting.

Its status goes on /diagnostics (ICD-ros2-msgs) and to the console: OK while
recording (or when not logging was asked for), WARN when the run isn't being
logged and should be (including while the recorder starts), when the recorder
fails to start or hasn't started within start_timeout_s, when it stops
mid-run, when rosbag2 reports
lost messages, when free space drops below min_free_gb, or when log_exclude
can't be used (the run is then recorded whole). The platform never stops for
any of these.
"""

import ctypes
import os
import signal
import subprocess
import sys
import threading
import time

from diagnostic_msgs.msg import DiagnosticArray, DiagnosticStatus, KeyValue
import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node

from arc_car_config import config_hash, load
from arc_logging.recorder import (
    check_exclude,
    free_gb,
    log_root,
    lost_messages,
    plan,
    record_command,
    run_name,
    run_record,
    start_status,
    started,
)

PR_SET_PDEATHSIG = 1


def _die_with_parent():
    """In the recorder, before exec: SIGINT it if this node dies, so it closes its file."""
    ctypes.CDLL('libc.so.6', use_errno=True).prctl(PR_SET_PDEATHSIG, signal.SIGINT)


class Logger(Node):

    def __init__(self):
        super().__init__('arc_logging')
        log = self.declare_parameter('log', False).value
        must_log = self.declare_parameter('must_log', False).value
        log_dir = self.declare_parameter('log_dir', '').value
        # TBC: the log rate (TBD, E-04) x the longest run (SYS-03) x a margin
        self.min_free_gb = self.declare_parameter('min_free_gb', 20.0).value
        # TBC: the most a crash or power loss may lose
        self.chunk_s = self.declare_parameter('chunk_s', 60).value
        # TBC: how long rosbag2 may take to start recording before the operator is warned
        self.start_timeout_s = self.declare_parameter('start_timeout_s', 10.0).value
        # topics not to record (rosbag2's --exclude-regex); empty records everything
        self.exclude = self.declare_parameter('log_exclude', '').value
        self.exclude_problem = check_exclude(self.exclude)
        if self.exclude_problem:
            # record more, never less, than was asked for
            self.exclude_problem = (f'log_exclude {self.exclude!r} ignored, every topic is '
                                    f'recorded: {self.exclude_problem}')
            self.exclude = ''
        # the run's fields, passed through into its name and record
        target = self.declare_parameter('target', '').value
        sim = self.declare_parameter('sim', '').value
        sensors = self.declare_parameter('sensors', '').value
        car = self.declare_parameter('car', 'twin').value
        record = run_record(target, sim, sensors, car, config_hash(load(car)), self.exclude)

        self.root = log_root(log_dir, os.environ.get('ARC_LOG_DIR', ''), must_log)
        self.plan = plan(log, must_log, self.root, run_name(target), self.min_free_gb)
        self.must_log = must_log
        self.proc = None
        self.recording = threading.Event()  # set once rosbag2 shows it is recording
        self.lost = 0
        self.stopping = False
        self.last = None
        if self.plan.output:
            self.start_recorder(record)

        self.diagnostics = self.create_publisher(DiagnosticArray, '/diagnostics', 10)
        # The /diagnostics rate (1 Hz, TBC in ICD-ros2-msgs)
        self.create_timer(1.0, self.report)
        self.report()

    def start_recorder(self, record):
        # rosbag2 creates the directory and its parents; if it can't, it fails to start
        self.started_at = time.monotonic()
        env = dict(os.environ, RCUTILS_LOGGING_BUFFERED_STREAM='1')
        self.proc = subprocess.Popen(
            record_command(self.plan.output, record, self.chunk_s, self.exclude),
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, bufsize=1, env=env,
            start_new_session=True, preexec_fn=_die_with_parent)
        self.reader = threading.Thread(target=self.read_recorder, daemon=True)
        self.reader.start()

    def read_recorder(self):
        """Pass the recorder's output through, and note when it starts and what it loses."""
        for line in self.proc.stdout:
            sys.stderr.write(line)
            sys.stderr.flush()
            if not self.recording.is_set() and started(line, self.plan.output):
                self.recording.set()
            lost = lost_messages(line)
            if lost:
                self.lost = max(self.lost, lost)

    def status(self):
        if self.proc is None:
            return self.plan.warn, self.plan.message
        code = self.proc.poll()
        if code is not None:
            self.reader.join()  # it has exited: read all it said before deciding
        if not self.recording.is_set():
            return start_status(code, time.monotonic() - self.started_at,
                                self.start_timeout_s, self.must_log)
        if code is not None:
            return True, (f'RECORDER STOPPED (exit code {code}): the rest of this run is not '
                          f'recorded; {self.plan.output} holds it up to here')
        warn, message = False, self.plan.message
        if self.exclude_problem:
            warn, message = True, f'{message}; {self.exclude_problem}'
        if self.lost:
            warn, message = True, f'{message}; rosbag2 reports {self.lost} messages lost'
        space = free_gb(self.root)
        if space < self.min_free_gb:
            warn, message = True, (
                f'{message}; LOW SPACE: {space:.1f} GB free, below min_free_gb '
                f'({self.min_free_gb:g} GB). Free space on the log drive (logs are never '
                'deleted automatically)')
        return warn, message

    def report(self):
        if self.stopping:
            return
        warn, message = self.status()
        if (warn, message) != self.last:
            # one call site per severity: rclpy won't change it between calls
            if warn:
                self.get_logger().warning(f'arc_logging: {message}')
            else:
                self.get_logger().info(f'arc_logging: {message}')
            self.last = (warn, message)
        values = {
            'log_dir': str(self.plan.output or self.root or ''),
            'min_free_gb': f'{self.min_free_gb:g}',
            'chunk_s': str(self.chunk_s),
            'log_exclude': self.exclude,
            'messages_lost': str(self.lost),
        }
        msg = DiagnosticArray()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.status.append(DiagnosticStatus(
            level=DiagnosticStatus.WARN if warn else DiagnosticStatus.OK,
            name='arc_logging', message=message,
            values=[KeyValue(key=k, value=v) for k, v in values.items()]))
        self.diagnostics.publish(msg)

    def stop_recorder(self):
        """SIGINT the recorder and wait for it to write out its cache and close the file."""
        self.stopping = True
        if self.proc and self.proc.poll() is None:
            self.proc.send_signal(signal.SIGINT)
            self.proc.wait()
            self.reader.join()


def main():
    rclpy.init()
    node = Logger()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass
    finally:
        node.stop_recorder()


if __name__ == '__main__':
    main()
