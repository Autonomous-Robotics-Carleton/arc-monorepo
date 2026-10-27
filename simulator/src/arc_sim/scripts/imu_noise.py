#!/usr/bin/env python3
"""The IMUs' noise node: /sim/imu/<name> (Gazebo, perfect) -> /imu/<name> (what algorithms and viewers read).

Every IMU in the URDF gets its own pair; the node finds them by listing /sim/imu/* topics, so a second
IMU only needs its sensor in the xacro and its entry in config/bridge.yaml.

The car's IMU (E-20, ISM330DHCX) is 6-axis: gyro and accelerometer only. Gazebo also reports its true
orientation, which no real IMU gives, so it is dropped here: the quaternion is zeroed and
orientation_covariance[0] set to -1, the sensor_msgs/Imu convention for "no orientation". Otherwise
samples pass through unchanged until a noise model is designed.

Lazy, like lidar_noise.py: it subscribes to /sim/imu/<name> only while /imu/<name> has subscribers.
"""

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu

CHECK_S = 0.1  # how often to compare subscriber counts; the delay before a toggle takes effect
DISCOVER_S = 1.0  # how often to look for new /sim/imu/* topics
DEPTH = 100  # 0.1 s at 1 kHz: room for a burst without dropping samples
SIM = '/sim/imu/'
TYPE = 'sensor_msgs/msg/Imu'


class ImuNoise(Node):

    def __init__(self):
        super().__init__('imu_noise')
        self.pubs = {}  # name -> publisher of /imu/<name>
        self.subs = {}  # name -> subscription to /sim/imu/<name>, only while wanted
        self.create_timer(DISCOVER_S, self.discover)
        self.create_timer(CHECK_S, self.check)

    def discover(self):
        for topic, types in self.get_topic_names_and_types():
            name = topic[len(SIM):]
            if topic.startswith(SIM) and name and '/' not in name and TYPE in types and name not in self.pubs:
                # Reliable, so any subscriber (ros2 topic echo, a bag, the bridge) can match it.
                self.pubs[name] = self.create_publisher(Imu, '/imu/' + name, DEPTH)
                self.get_logger().info(f'found {topic}: publishing /imu/{name}')

    def check(self):
        for name, pub in self.pubs.items():
            wanted = pub.get_subscription_count() > 0
            if wanted and name not in self.subs:
                self.subs[name] = self.create_subscription(
                    Imu, SIM + name, lambda msg, pub=pub: self.on_imu(msg, pub), DEPTH)
                self.get_logger().info(f'/imu/{name} has subscribers: subscribing to {SIM}{name}')
            elif not wanted and name in self.subs:
                self.destroy_subscription(self.subs.pop(name))
                self.get_logger().info(f'/imu/{name} has no subscribers: unsubscribed from {SIM}{name}')

    def on_imu(self, msg, pub):
        msg.orientation.x = msg.orientation.y = msg.orientation.z = msg.orientation.w = 0.0
        msg.orientation_covariance[0] = -1.0
        pub.publish(msg)  # no noise yet


def main():
    rclpy.init()
    node = ImuNoise()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
