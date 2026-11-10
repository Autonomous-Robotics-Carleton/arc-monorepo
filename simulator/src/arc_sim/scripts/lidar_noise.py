#!/usr/bin/env python3
"""The LiDAR's noise node: /sim/scan (Gazebo, perfect) -> /scan (what algorithms and viewers read).

Passes scans through unchanged until a noise model is designed. Lazy: it subscribes to /sim/scan
only while /scan has subscribers, so with nobody watching, ros_gz_bridge (also lazy) drops its
Gazebo subscription and Gazebo stops rendering the LiDAR. The node itself stays running; only its
subscription comes and goes.
"""

import rclpy
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import LaserScan

CHECK_S = 0.1  # how often to compare /scan's subscriber count; the delay before a toggle takes effect


class LidarNoise(Node):

    def __init__(self):
        super().__init__('lidar_noise')
        # Reliable, so any subscriber (ros2 topic echo, a bag, the bridge) can match it.
        self.pub = self.create_publisher(LaserScan, '/scan', 10)
        self.sub = None
        self.create_timer(CHECK_S, self.check)

    def check(self):
        wanted = self.pub.get_subscription_count() > 0
        if wanted and self.sub is None:
            self.sub = self.create_subscription(LaserScan, '/sim/scan', self.on_scan, qos_profile_sensor_data)
            self.get_logger().info('/scan has subscribers: subscribing to /sim/scan')
        elif not wanted and self.sub is not None:
            self.destroy_subscription(self.sub)
            self.sub = None
            self.get_logger().info('/scan has no subscribers: unsubscribed from /sim/scan')

    def on_scan(self, msg):
        self.pub.publish(msg)  # no noise yet


def main():
    rclpy.init()
    node = LidarNoise()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
