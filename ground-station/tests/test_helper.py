"""helper.py: who may connect, what it sends, and that bad input can't stop its send loop.

python3 -m unittest discover -s ground-station/tests   (needs websockets, from requirements.txt)
"""

import asyncio
import logging
import os
import socket
import sys
import time
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import helper  # noqa: E402
from websockets.asyncio.client import connect  # noqa: E402
from websockets.exceptions import InvalidStatus  # noqa: E402

# The readiness probe connects without speaking HTTP; websockets logs that as an error.
logging.getLogger('websockets').setLevel(logging.CRITICAL)


def free_port():
    with socket.socket() as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


class Teleop(unittest.IsolatedAsyncioTestCase):

    async def test_values_a_packet_cannot_carry_are_dropped(self):
        h = helper.Helper()
        for speed, steer in ((1e40, 0.0), (0.0, -1e39), (float('inf'), 0.0), (float('nan'), 0.0)):
            await h.handle({'type': 'teleop', 'speed': speed, 'steer': steer})
            self.assertEqual(h.teleop, (0.0, 0.0))
            self.assertIsNone(h.last_teleop)
        await h.handle({'type': 'teleop', 'speed': 2.5, 'steer': -0.1})
        self.assertEqual(h.teleop, (2.5, -0.1))
        h.sock.close()

    def test_send_never_raises_on_an_unpackable_value(self):
        car = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        car.bind(('127.0.0.1', 0))
        h = helper.Helper()
        h.target = car.getsockname()
        try:
            h.send(helper.TELEOP, 1e40, 0.0)  # used to raise OverflowError and end the loop
            h.send(helper.HEARTBEAT)
            data, _ = car.recvfrom(64)
            self.assertEqual(helper.PACKET.unpack(data)[1], helper.HEARTBEAT)
        finally:
            car.close()
            h.sock.close()


class Flood(unittest.TestCase):

    def test_receive_reads_at_most_max_reads_per_tick(self):
        car = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        car.bind(('127.0.0.1', 0))
        h = helper.Helper()
        h.target = car.getsockname()
        try:
            h.send(helper.HELLO)  # binds the helper's socket
            _, helper_addr = car.recvfrom(64)
            junk = helper.PACKET.pack(helper.MAGIC, helper.ACK, h.session ^ 1, 0, 0.0, 0.0, 0.0)
            for _ in range(3 * helper.MAX_READS):
                car.sendto(junk, helper_addr)
            time.sleep(0.05)
            start = time.monotonic()
            h.receive(time.monotonic())
            self.assertLess(time.monotonic() - start, 0.05)
            left = 0
            while True:  # what receive() left for the next tick
                try:
                    h.sock.recvfrom(64)
                    left += 1
                except BlockingIOError:
                    break
            self.assertEqual(left, 2 * helper.MAX_READS)
        finally:
            car.close()
            h.sock.close()


class Origin(unittest.IsolatedAsyncioTestCase):

    async def asyncSetUp(self):
        self.port = free_port()
        self.task = asyncio.create_task(helper.main_async(self.port, 47380))
        for _ in range(50):
            try:
                with socket.create_connection(('127.0.0.1', self.port), timeout=0.1):
                    break
            except OSError:
                await asyncio.sleep(0.05)

    async def asyncTearDown(self):
        self.task.cancel()
        try:
            await self.task
        except asyncio.CancelledError:
            pass

    async def test_the_ground_station_page_may_connect(self):
        for origin in ('http://localhost:47380', 'http://127.0.0.1:47380'):
            async with connect('ws://127.0.0.1:%d' % self.port, origin=origin) as ws:
                await ws.send('{"type": "hb", "seq": 0}')
                msg = await asyncio.wait_for(ws.recv(), 2)
                self.assertIn('"type": "sent"', msg)

    async def test_other_websites_may_not(self):
        for origin in ('https://evil.example', 'http://localhost:8000', 'null', None):
            with self.subTest(origin=origin):
                with self.assertRaises(InvalidStatus) as e:
                    async with connect('ws://127.0.0.1:%d' % self.port, origin=origin):
                        pass
                self.assertEqual(e.exception.response.status_code, 403)


if __name__ == '__main__':
    unittest.main()
