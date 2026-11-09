"""server.py: only this machine's ground station page may use it.

python3 -m unittest discover -s ground-station/tests   (needs websockets, from requirements.txt)
"""

import asyncio
import http.client
import json
import logging
import os
import socket
import sys
import unittest

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..'))

import server  # noqa: E402
from websockets.asyncio.client import connect  # noqa: E402
from websockets.exceptions import InvalidStatus  # noqa: E402

# The readiness probe connects without speaking HTTP; websockets logs that as an error.
logging.getLogger('websockets').setLevel(logging.CRITICAL)


def free_port():
    with socket.socket() as s:
        s.bind(('127.0.0.1', 0))
        return s.getsockname()[1]


def get(port, host):
    c = http.client.HTTPConnection('127.0.0.1', port, timeout=2)
    try:
        c.request('GET', '/', headers={'Host': host})
        return c.getresponse().status
    finally:
        c.close()


class Access(unittest.IsolatedAsyncioTestCase):

    async def asyncSetUp(self):
        self.port = free_port()
        cfg = server.load_config(os.path.join(os.path.dirname(server.__file__), 'config.json'), None)
        self.task = asyncio.create_task(server.Server(cfg, self.port).run('127.0.0.1', self.port))
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

    async def test_page_is_served_to_its_own_host_names(self):
        for host in ('localhost:%d' % self.port, '127.0.0.1:%d' % self.port):
            self.assertEqual(await asyncio.to_thread(get, self.port, host), 200)

    async def test_other_host_names_are_refused(self):  # DNS rebinding
        for host in ('evil.example:%d' % self.port, 'localhost', '127.0.0.1:1'):
            with self.subTest(host=host):
                self.assertEqual(await asyncio.to_thread(get, self.port, host), 421)

    async def test_page_origin_gets_the_link(self):
        async with connect('ws://localhost:%d/ws' % self.port, origin='http://localhost:%d' % self.port) as ws:
            msg = json.loads(await asyncio.wait_for(ws.recv(), 2))
            self.assertEqual(msg['type'], 'link')

    async def test_other_origins_are_refused(self):
        for path in ('/ws', '/scan', '/imu/cg', '/encoders/wheel'):
            for origin in ('https://evil.example', 'http://localhost:1', None):
                with self.subTest(path=path, origin=origin):
                    with self.assertRaises(InvalidStatus) as e:
                        async with connect('ws://localhost:%d%s' % (self.port, path), origin=origin):
                            pass
                    self.assertEqual(e.exception.response.status_code, 403)


if __name__ == '__main__':
    unittest.main()
