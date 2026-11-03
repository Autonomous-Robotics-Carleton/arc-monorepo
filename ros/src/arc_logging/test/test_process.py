"""The logger as real processes, on a ROS domain of its own: what happens to
the log when the node is killed outright, which topics log_exclude and each
log_profile leave out, that a latched value from before the start is kept,
and that what's already on the log drive is never touched.
"""

import hashlib
import os
from pathlib import Path
import random
import signal
import subprocess
import sys
import threading
import time

import pytest
import rosbag2_py

from arc_logging.recorder import free_gb, PROFILES

TALKER = str(Path(__file__).with_name('talker.py'))
LATCHED = """
import sys, time, rclpy
from rclpy.qos import QoSProfile, DurabilityPolicy
from std_msgs.msg import String
rclpy.init()
node = rclpy.create_node('latched')
qos = QoSProfile(depth=1, durability=DurabilityPolicy.TRANSIENT_LOCAL)
pub = node.create_publisher(String, '/test/latched', qos)
pub.publish(String(data='set before the recorder'))
print('published', flush=True)
rclpy.spin(node)
"""
# A camera's topics, raw and through image_transport's compressed transport, and
# a measurement: published whether or not anything subscribes
CAMERA = """
import rclpy
from sensor_msgs.msg import CameraInfo, CompressedImage, Image
from std_msgs.msg import String
rclpy.init()
node = rclpy.create_node('camera')
pubs = [(node.create_publisher(Image, '/test/camera/image_raw', 10),
         Image(height=2, width=2, encoding='mono8', step=2, data=[0, 1, 2, 3])),
        (node.create_publisher(CompressedImage, '/test/camera/image_raw/compressed', 10),
         CompressedImage(format='jpeg', data=[1, 2, 3])),
        (node.create_publisher(CameraInfo, '/test/camera/camera_info', 10), CameraInfo()),
        (node.create_publisher(String, '/test/measurement', 10), String(data='m'))]
sent = 0
def tick():
    global sent
    for pub, msg in pubs:
        pub.publish(msg)
    sent += 1
    if sent % 20 == 0:
        print(f'{sent} sent', flush=True)
node.create_timer(0.05, tick)
rclpy.spin(node)
"""
CAMERA_TOPICS = {'/test/camera/image_raw', '/test/camera/image_raw/compressed',
                 '/test/camera/camera_info', '/test/measurement'}


@pytest.fixture
def env():
    """A ROS domain no other test shares."""
    return dict(os.environ, ROS_DOMAIN_ID=str(random.randint(100, 200)),
                ROS_AUTOMATIC_DISCOVERY_RANGE='LOCALHOST', PYTHONUNBUFFERED='1')


def start(cmd, env):
    """Runs cmd in a session of its own, collecting its output in .lines."""
    proc = subprocess.Popen(cmd, env=env, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                            text=True, start_new_session=True)
    proc.lines = []
    threading.Thread(target=lambda: proc.lines.extend(iter(proc.stdout.readline, '')),
                     daemon=True).start()
    return proc


def logger(env, log_dir, **params):
    params = {'log': True, 'log_dir': str(log_dir), 'min_free_gb': 0.0, 'warn_free_gb': 0.0,
              'chunk_s': 1, **params}
    args = []
    for k, v in params.items():
        args += ['-p', f'{k}:={str(v).lower() if isinstance(v, bool) else v}']
    # the executable itself, as launch runs it: `ros2 run` would put a process between
    # the test and the node, and a signal to it wouldn't reach the node
    prefix = subprocess.run(['ros2', 'pkg', 'prefix', 'arc_logging'], env=env, check=True,
                            capture_output=True, text=True).stdout.strip()
    return start([f'{prefix}/lib/arc_logging/logger', '--ros-args', *args], env)


def wait_for(proc, text, timeout=30):
    end = time.monotonic() + timeout
    while time.monotonic() < end:
        if any(text in line for line in list(proc.lines)):
            return
        time.sleep(0.05)
    raise AssertionError(f'no "{text}" in:\n' + ''.join(proc.lines[-30:]))


def recorder_running(log_dir):
    return subprocess.run(['pgrep', '-f', f'bag record .*{log_dir}'],
                          capture_output=True).returncode == 0


def stop(*procs):
    for p in procs:
        if p.poll() is None:
            os.killpg(p.pid, signal.SIGINT)
    for p in procs:
        try:
            p.wait(timeout=30)
        except subprocess.TimeoutExpired:
            os.killpg(p.pid, signal.SIGKILL)
            p.wait()


def read(run):
    reader = rosbag2_py.SequentialReader()
    reader.open(rosbag2_py.StorageOptions(uri=str(run), storage_id='mcap'),
                rosbag2_py.ConverterOptions('', ''))
    topics = {t.name for t in reader.get_all_topics_and_types()}
    messages = []
    while reader.has_next():
        topic, data, _ = reader.read_next()
        messages.append((topic, data))
    return topics, messages


def only_run(log_dir):
    runs = [p for p in log_dir.iterdir() if p.is_dir()]
    assert len(runs) == 1, runs
    return runs[0]


def test_killed_node_still_closes_the_log(tmp_path, env):
    """SIGKILL the node (the OOM killer, a crash): the recorder still gets
    SIGINT from the kernel and closes a log rosbag2 can read."""
    talker = start([sys.executable, TALKER, 'test/platform'], env)
    node = logger(env, tmp_path)
    try:
        wait_for(node, 'recording to')
        wait_for(talker, '40 sent')  # two seconds of messages, in chunks
        node.send_signal(signal.SIGKILL)
        node.wait(timeout=10)
        end = time.monotonic() + 30
        while recorder_running(tmp_path) and time.monotonic() < end:
            time.sleep(0.2)
        assert not recorder_running(tmp_path), 'the recorder outlived the node'
    finally:
        stop(talker, node)
    run = only_run(tmp_path)
    assert (run / 'metadata.yaml').exists(), 'the recorder never closed the log'
    topics, messages = read(run)
    assert '/test/platform' in topics
    counts = [m for t, m in messages if t == '/test/platform']
    assert len(counts) >= 40
    assert len(list(run.glob('*.mcap'))) >= 2


def test_exclude_leaves_out_every_topic_containing_a_match(tmp_path, env):
    """rosbag2: 'Exclude topics ... containing provided regular expression'."""
    talkers = [start([sys.executable, TALKER, t], env)
               for t in ('test/kept', 'test/left_out', 'leftover')]
    node = logger(env, tmp_path, log_exclude='left')
    try:
        wait_for(node, 'recording to')
        wait_for(talkers[0], '20 sent')
        time.sleep(1.0)  # the others have been discovered too
    finally:
        stop(node, *talkers)
    topics, _ = read(only_run(tmp_path))
    assert '/test/kept' in topics
    assert '/test/left_out' not in topics and '/leftover' not in topics


def test_latched_value_from_before_the_start_is_logged(tmp_path, env):
    latched = start([sys.executable, '-c', LATCHED], env)
    node = None
    try:
        wait_for(latched, 'published')
        time.sleep(1.0)
        node = logger(env, tmp_path)
        wait_for(node, 'recording to')
        # discovery can take seconds; the value is delivered on subscribing
        wait_for(node, "Subscribed to topic '/test/latched'")
        time.sleep(1.0)
    finally:
        stop(*(p for p in (node, latched) if p))
    _, messages = read(only_run(tmp_path))
    assert any(t == '/test/latched' for t, _ in messages)


def test_nothing_already_on_the_drive_is_touched(tmp_path, env):
    old = tmp_path / '20250101T000000Z_car'
    old.mkdir()
    (old / 'metadata.yaml').write_text('an older run')
    (tmp_path / 'notes.txt').write_text('the operator\'s')

    def snapshot():
        return {p: (hashlib.sha256(p.read_bytes()).hexdigest() if p.is_file() else None,
                    p.stat().st_mtime_ns)
                for p in tmp_path.rglob('*') if not p.is_relative_to(new_runs())}

    def new_runs():
        runs = [p for p in tmp_path.iterdir() if p.is_dir() and p != old]
        return runs[0] if runs else tmp_path / 'none'

    before = snapshot()
    talker = start([sys.executable, TALKER, 'test/platform'], env)
    node = logger(env, tmp_path, target='car')
    try:
        wait_for(node, 'recording to')
        wait_for(talker, '20 sent')
    finally:
        stop(node, talker)
    assert new_runs().name.endswith('_car')
    after = snapshot()
    assert {p: v for p, v in after.items() if p in before} == before


@pytest.mark.parametrize('profile,kept', [
    ('minimal', {'/test/camera/camera_info', '/test/measurement'}),
    ('full', CAMERA_TOPICS),
])
def test_each_profile_logs_its_topics(tmp_path, env, profile, kept):
    """ADR-0056: minimal leaves out camera images, raw or compressed, by type."""
    camera = start([sys.executable, '-c', CAMERA], env)
    node = None
    try:
        wait_for(camera, '20 sent')
        node = logger(env, tmp_path, log_profile=profile)
        wait_for(node, 'recording to')
        for topic in kept:
            wait_for(node, f"Subscribed to topic '{topic}'")
        time.sleep(1.0)  # some messages on each
    finally:
        stop(*(p for p in (node, camera) if p))
    # never subscribed to, so a driver never encodes frames for the log
    subscribed = {topic for topic in CAMERA_TOPICS
                  if any(f"Subscribed to topic '{topic}'" in line for line in node.lines)}
    assert subscribed == kept
    run = only_run(tmp_path)
    topics, messages = read(run)
    assert topics & CAMERA_TOPICS == kept
    assert {t for t, _ in messages} & CAMERA_TOPICS == kept
    record = rosbag2_py.Info().read_metadata(str(run), 'mcap').custom_data
    assert record['log_profile'] == profile


def test_full_without_room_logs_minimal(tmp_path, env):
    """A full run that doesn't fit records minimal, warns, and says so in its record."""
    need = PROFILES['minimal'].min_free_gb
    if free_gb(tmp_path) < need:
        pytest.skip(f'needs {need:g} GB free where the test logs')
    camera = start([sys.executable, '-c', CAMERA], env)
    node = None
    try:
        wait_for(camera, '20 sent')
        node = logger(env, tmp_path, log_profile='full', min_free_gb=1e12)
        wait_for(node, 'NOT LOGGING full')
        wait_for(node, "Subscribed to topic '/test/measurement'")
        time.sleep(1.0)
    finally:
        stop(*(p for p in (node, camera) if p))
    assert not any("Subscribed to topic '/test/camera/image_raw" in line for line in node.lines)
    # a warning, not just a message: the operator asked for images and isn't getting them
    assert any('[WARN]' in line and 'NOT LOGGING full' in line for line in node.lines)
    run = only_run(tmp_path)
    topics, _ = read(run)
    assert '/test/measurement' in topics and '/test/camera/image_raw' not in topics
    assert rosbag2_py.Info().read_metadata(str(run), 'mcap').custom_data['log_profile'] == \
        'minimal'
