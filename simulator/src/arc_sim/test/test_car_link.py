"""car_link: the heartbeat stop still works under a UDP flood.

In the sim image: docker run --rm --entrypoint /entrypoint.sh arc-sim python3 -m pytest /sim_ws/src/arc_sim/test
"""

import os
import socket
import sys
import threading
import time

import pytest
import rclpy

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'scripts'))

import car_link  # noqa: E402


@pytest.fixture
def link():
    rclpy.init(args=['--ros-args', '-p', 'port:=0'])
    node = car_link.CarLink()
    yield node
    node.destroy_node()
    rclpy.shutdown()


def packet(kind, session, seq, speed=0.0, steer=0.0):
    return car_link.PACKET.pack(car_link.MAGIC, kind, session, seq, 0.0, speed, steer)


def test_flood_cannot_hold_off_the_heartbeat_stop(link):
    addr = ('127.0.0.1', link.sock.getsockname()[1])
    gs = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    session = 0x1234
    gs.sendto(packet(car_link.HELLO, session, 0), addr)
    time.sleep(0.02)
    link.tick()
    gs.sendto(packet(car_link.HEARTBEAT, session, 0), addr)
    gs.sendto(packet(car_link.TELEOP, session, 0, 2.0, 0.0), addr)
    time.sleep(0.02)
    link.tick()
    assert link.reason == 'ok'

    # The ground station goes quiet; six strangers flood car_link with packets of another session.
    stop = threading.Event()

    def flood():
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        junk = packet(car_link.HEARTBEAT, 0xBAD, 0)
        while not stop.is_set():
            try:
                s.sendto(junk, addr)
            except OSError:
                pass
        s.close()

    floods = [threading.Thread(target=flood) for _ in range(6)]
    for t in floods:
        t.start()
    try:
        slowest = 0.0
        end = time.monotonic() + 1.0
        while time.monotonic() < end:
            start = time.monotonic()
            link.tick()
            slowest = max(slowest, time.monotonic() - start)
            time.sleep(0.02)
    finally:
        stop.set()
        for t in floods:
            t.join()
        gs.close()
    assert slowest < 0.1, 'a tick took %.3f s under the flood' % slowest
    assert link.reason == 'heartbeat'
    assert link.speed == pytest.approx(0.0)  # and ramped all the way down during it


def test_reads_at_most_max_reads_per_tick(link):
    addr = ('127.0.0.1', link.sock.getsockname()[1])
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    for _ in range(3 * car_link.MAX_READS):
        s.sendto(b'junk', addr)
    s.close()
    time.sleep(0.05)
    link.receive(time.monotonic())
    left = 0
    while True:
        try:
            link.sock.recvfrom(64)
            left += 1
        except BlockingIOError:
            break
    assert left == 2 * car_link.MAX_READS
