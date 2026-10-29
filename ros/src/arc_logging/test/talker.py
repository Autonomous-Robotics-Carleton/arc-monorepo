"""A stand-in for a node in the logging test: publishes its count on a topic
at 20 Hz once something subscribes. With --crash-after N it kills itself
(SIGKILL, as the kernel's out-of-memory killer would) after N messages.

    talker.py TOPIC [--crash-after N]
"""

import argparse
import os
import signal

import rclpy
from rclpy.executors import ExternalShutdownException
from rclpy.node import Node
from std_msgs.msg import String


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('topic')
    parser.add_argument('--crash-after', type=int)
    args = parser.parse_args()

    rclpy.init()
    node = Node('talker_' + args.topic.replace('/', '_'))
    pub = node.create_publisher(String, args.topic, 10)
    sent = 0

    def tick():
        nonlocal sent
        if pub.get_subscription_count() == 0:
            return  # until the recorder has found the topic
        pub.publish(String(data=str(sent)))
        sent += 1
        if sent % 20 == 0:
            print(f'{args.topic}: {sent} sent', flush=True)
        if sent == args.crash_after:
            print(f'{args.topic}: crashing', flush=True)
            os.kill(os.getpid(), signal.SIGKILL)

    node.create_timer(0.05, tick)
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, ExternalShutdownException):
        pass


if __name__ == '__main__':
    main()
