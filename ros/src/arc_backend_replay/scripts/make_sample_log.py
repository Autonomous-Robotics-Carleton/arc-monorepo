#!/usr/bin/env python3
"""Writes the replay backend's sample log, test/data/sample.mcap: one second of
made-up car data on the car interface's topics (ICD-ros2-msgs), for the replay
launch test. Every value and timestamp is fixed, so the file only changes when
this script or rosbag2 does.

    source ros/install/setup.bash   # arc_msgs, built
    python3 ros/src/arc_backend_replay/scripts/make_sample_log.py

The log also holds two topics the replay backend must not play, a drive
command and the control mode: the live platform publishes those.
"""

import math
import re
import shutil
import struct
import sys
import tempfile
from pathlib import Path

import rosbag2_py
from arc_msgs.msg import ControlMode, DriveCommand, MotorStatus, SafetyState, SteeringStatus
from rclpy.serialization import deserialize_message, serialize_message
from rosbag2_py._storage import QoS
from sensor_msgs.msg import Imu

OUT = Path(__file__).resolve().parent.parent / 'test' / 'data' / 'sample.mcap'
START_NS = 1_800_000_000 * 10**9  # an arbitrary fixed start
DURATION_NS = 10**9
RATE_HZ = 20  # every stream, so the log stays a few KB

# QoS profiles (ICD ros2-msgs); samples keep 100 ms of their stream, at least 1
SAMPLES = QoS(max(1, RATE_HZ // 10)).reliable().durability_volatile()
STATE = QoS(1).reliable().transient_local()
COMMANDS = QoS(1).reliable().durability_volatile()

# topic: (type, QoS, message for sample i at time t in s)
TOPICS = {
    '/sensors/imu/cg': ('sensor_msgs/msg/Imu', SAMPLES, lambda i, t: imu(t)),
    '/car/motor/fl': ('arc_msgs/msg/MotorStatus', SAMPLES, lambda i, t: motor(i)),
    '/car/steering': ('arc_msgs/msg/SteeringStatus', SAMPLES, lambda i, t: steering(t)),
    '/car/safety_state': ('arc_msgs/msg/SafetyState', STATE, lambda i, t: safety()),
    # not the car's: the replay backend leaves these out
    '/car/cmd/drive': ('arc_msgs/msg/DriveCommand', COMMANDS, lambda i, t: drive(t)),
    '/control/mode': ('arc_msgs/msg/ControlMode', STATE, lambda i, t: ControlMode()),
}


def imu(t):
    msg = Imu()
    msg.header.frame_id = 'imu_cg'
    msg.orientation_covariance[0] = -1.0  # no orientation (ICD ros2-msgs)
    msg.angular_velocity.z = 0.5 * math.sin(2 * math.pi * t)
    msg.linear_acceleration.z = 9.81
    return msg


def motor(i):
    msg = MotorStatus()
    msg.erpm = 50 * i
    msg.tachometer = i
    msg.voltage_in = 22.2
    return msg


def steering(t):
    msg = SteeringStatus()
    msg.angle = 0.1 * math.sin(2 * math.pi * t)
    return msg


def safety():
    msg = SafetyState()
    msg.watchdog = SafetyState.WATCHDOG_OK
    msg.latch = SafetyState.LATCH_RESET
    return msg


def drive(t):
    msg = DriveCommand()
    msg.speed = 1.0
    msg.accel = 1.0
    msg.steer = 0.1 * math.sin(2 * math.pi * t)
    return msg


# CDR (little endian, XCDR1), written here rather than by rclpy: rclpy leaves
# the alignment padding as whatever was in memory, so the file would change on
# every run. Covers the field types the messages above use.
PRIMITIVES = {'boolean': '?', 'octet': 'B', 'uint8': 'B', 'int8': 'b', 'uint16': 'H',
              'int16': 'h', 'uint32': 'I', 'int32': 'i', 'uint64': 'Q', 'int64': 'q',
              'float': 'f', 'double': 'd'}


def cdr(msg):
    out = bytearray()

    def put(fmt, value):
        out.extend(b'\0' * (-len(out) % struct.calcsize(fmt)))
        out.extend(struct.pack('<' + fmt, value))

    def field(field_type, value):
        array = re.fullmatch(r'(.+)\[(\d+)\]', field_type)
        if array:
            for item in value:
                field(array.group(1), item)
        elif field_type in PRIMITIVES:
            put(PRIMITIVES[field_type], value)
        elif field_type == 'string':
            put('I', len(value) + 1)
            out.extend(value.encode() + b'\0')
        elif '/' in field_type:
            message(value)
        else:
            raise ValueError(f'no CDR encoding here for {field_type}')

    def message(m):
        for name, field_type in m.get_fields_and_field_types().items():
            field(field_type, getattr(m, name))

    message(msg)
    data = b'\0\x01\0\0' + bytes(out)  # encapsulation: CDR, little endian
    # the same message as rclpy's own encoding, float32 rounding included
    assert deserialize_message(data, type(msg)) == deserialize_message(
        serialize_message(msg), type(msg)), type(msg)
    return data


def main():
    with tempfile.TemporaryDirectory() as tmp:
        bag = Path(tmp) / 'sample'
        writer = rosbag2_py.SequentialWriter()
        writer.open(rosbag2_py.StorageOptions(uri=str(bag), storage_id='mcap',
                                              storage_preset_profile='zstd_small'),
                    rosbag2_py.ConverterOptions('cdr', 'cdr'))
        for topic_id, (topic, (msg_type, qos, _)) in enumerate(TOPICS.items()):
            writer.create_topic(rosbag2_py.TopicMetadata(
                topic_id, topic, msg_type, 'cdr', [qos]))
        for i in range(DURATION_NS * RATE_HZ // 10**9):
            stamp_ns = START_NS + i * 10**9 // RATE_HZ
            for topic, (_, _, make) in TOPICS.items():
                msg = make(i, (stamp_ns - START_NS) / 1e9)
                if hasattr(msg, 'header'):
                    msg.header.stamp.sec, msg.header.stamp.nanosec = divmod(stamp_ns, 10**9)
                writer.write(topic, cdr(msg), stamp_ns, stamp_ns)
        del writer  # closes the file
        OUT.parent.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(bag / 'sample_0.mcap', OUT)
    print(f'wrote {OUT} ({OUT.stat().st_size} bytes)', file=sys.stderr)


if __name__ == '__main__':
    main()
