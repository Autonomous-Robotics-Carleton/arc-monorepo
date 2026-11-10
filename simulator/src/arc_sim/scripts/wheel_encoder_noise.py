#!/usr/bin/env python3
"""The wheel encoders' noise node: /sim/wheel_joints (Gazebo, perfect) -> /encoders/wheel (what algorithms read).

The car has an AS5047P on each wheel (E-22): a 14-bit absolute magnetic encoder read over SPI. For every
Gazebo sample, each wheel's true continuous joint angle goes through, in order:

  nonlinearity + jitter -> wrap to [0, 2pi) -> quantize to 16384 steps (floor) -> back to rad

Nonlinearity is a fixed error as a function of angle: a 1x and a 2x per-revolution sine, each with a
phase drawn once per sensor at startup. Jitter is gaussian, fresh every sample. Both scale with
SENSOR_NOISE_SCALE (0 = none). With probability SENSOR_FAIL_PROB a sensor's sample is dropped, as the
firmware discards a read with its parity or error flag set; consumers see a gap in that sensor's stamps.

Each output message is one sensor's sample, the fields of ARC_ANGLE_SAMPLE (systems/icd/sync-link.xml):
header.stamp the sample time, name the sensor (wheel_fl, wheel_fr, wheel_rl, wheel_rr), position the
absolute angle. velocity and effort are empty: the encoder measures angle only. The sign is the joint's
own (axis +y on every wheel), so right wheels are not flipped.

Lazy, like imu_noise.py: it subscribes to /sim/wheel_joints only while /encoders/wheel has subscribers.
"""

import math
import os
import random

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState

CHECK_S = 0.1  # how often to compare subscriber counts; the delay before a toggle takes effect
DEPTH = 100  # 0.1 s at 1 kHz: room for a burst without dropping samples
OUT_DEPTH = 4 * DEPTH  # four messages per sample
STEPS = 16384  # 14 bits per revolution
TAU = 2 * math.pi
WHEELS = {  # Gazebo joint -> sensor name, in ARC_ANGLE_WHEEL_* order
    'front_left_wheel_joint': 'wheel_fl',
    'front_right_wheel_joint': 'wheel_fr',
    'rear_left_wheel_joint': 'wheel_rl',
    'rear_right_wheel_joint': 'wheel_rr',
}


def env_float(name, default):
    value = os.environ.get(name, '').strip()
    return float(value) if value else default


class WheelEncoderNoise(Node):

    def __init__(self):
        super().__init__('wheel_encoder_noise')
        scale = env_float('SENSOR_NOISE_SCALE', 1.0)
        self.fail_prob = env_float('SENSOR_FAIL_PROB', 0.1)
        self.jitter = math.radians(env_float('ENCODER_JITTER_DEG', 0.0)) * scale
        # Each term gets half, so the summed error stays within the datasheet's figure.
        self.nonlinearity = math.radians(env_float('ENCODER_NONLINEARITY_DEG', 0.0)) * scale / 2
        seed = os.environ.get('SENSOR_SEED', '').strip()
        self.rng = random.Random(int(seed) if seed else None)  # empty: random each run
        self.phases = {name: (self.rng.uniform(0, TAU), self.rng.uniform(0, TAU)) for name in WHEELS.values()}
        # Reliable, so any subscriber (ros2 topic echo, a bag, the bridge) can match it.
        self.pub = self.create_publisher(JointState, '/encoders/wheel', OUT_DEPTH)
        self.sub = None
        self.create_timer(CHECK_S, self.check)
        self.get_logger().info(f'noise scale {scale}, fail probability {self.fail_prob}, seed {seed or "random"}')

    def check(self):
        wanted = self.pub.get_subscription_count() > 0
        if wanted and self.sub is None:
            self.sub = self.create_subscription(JointState, '/sim/wheel_joints', self.on_joints, DEPTH)
            self.get_logger().info('/encoders/wheel has subscribers: subscribing to /sim/wheel_joints')
        elif not wanted and self.sub is not None:
            self.destroy_subscription(self.sub)
            self.sub = None
            self.get_logger().info('/encoders/wheel has no subscribers: unsubscribed from /sim/wheel_joints')

    def on_joints(self, msg):
        for joint, true_angle in zip(msg.name, msg.position):
            name = WHEELS.get(joint)
            if name is None:
                continue
            if self.rng.random() < self.fail_prob:
                continue  # failed read: firmware drops it
            p1, p2 = self.phases[name]
            angle = (true_angle
                     + self.nonlinearity * (math.sin(true_angle + p1) + math.sin(2 * true_angle + p2))
                     + self.rng.gauss(0.0, self.jitter))
            code = math.floor(angle % TAU / TAU * STEPS) % STEPS  # % STEPS: rounding can reach 2pi
            out = JointState()
            out.header.stamp = msg.header.stamp
            out.name = [name]
            out.position = [code * TAU / STEPS]
            self.pub.publish(out)


def main():
    rclpy.init()
    node = WheelEncoderNoise()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
