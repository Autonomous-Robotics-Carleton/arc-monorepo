"""For the launch tests: wait for arc_logging's status on /diagnostics."""

import time

from diagnostic_msgs.msg import DiagnosticArray
import rclpy


def wait_for_status(level, text, timeout=20.0):
    """arc_logging's first status at `level` whose message contains `text`, or None."""
    rclpy.init()
    node = rclpy.create_node('arc_logging_test_diagnostics')
    found = []

    def on_diagnostics(msg):
        found.extend(s for s in msg.status
                     if s.name == 'arc_logging' and s.level == level and text in s.message)

    node.create_subscription(DiagnosticArray, '/diagnostics', on_diagnostics, 10)
    try:
        end = time.monotonic() + timeout
        while not found and time.monotonic() < end:
            rclpy.spin_once(node, timeout_sec=0.1)
        return found[0] if found else None
    finally:
        node.destroy_node()
        rclpy.shutdown()
