#!/usr/bin/env python3
"""Ground station server: serves the page and relays the car's (or the sim's) bridge to it.

http://127.0.0.1:8080/           web/index.html
http://127.0.0.1:8080/track.png  the connected bridge's /track.png, proxied
ws://127.0.0.1:8080/ws           server <-> page messages, see protocol.md
ws://127.0.0.1:8080/scan         the connected bridge's /scan (binary LiDAR frames), relayed

Modes: `real` (startup) connects to the car's bridge at config car.host, retrying every 2 s.
`sim` starts ../simulator/run.sh --bridge and connects to its bridge on 127.0.0.1. The sim
starts with no track: the server asks the bridge to load the page's pick on every connect and
whenever the page picks another, and relays the bridge's `track_status` (loading, ready,
failed) to the page. While in sim mode the car is still probed so the page can show that one
was found; the server never switches mode on its own.

Sensor sockets (/scan) are relayed one to one: each page socket opens its own socket to the
bridge and closes it when the page closes, so the bridge (and the sim behind it) only produce
a sensor while some page is showing it.
"""

import argparse
import asyncio
import base64
import email.utils
import json
import logging
import os
import shlex
import signal
import subprocess
import sys
import urllib.error
import urllib.request
from http import HTTPStatus

from websockets.asyncio.client import connect
from websockets.asyncio.server import broadcast, serve
from websockets.datastructures import Headers
from websockets.exceptions import ConnectionClosed
from websockets.http11 import Response

HERE = os.path.dirname(os.path.abspath(__file__))
PAGE = os.path.join(HERE, 'web', 'index.html')
SIM_RUN = os.path.normpath(os.path.join(HERE, '..', 'simulator', 'run.sh'))
TRACKS = os.path.normpath(os.path.join(HERE, '..', 'simulator', 'src', 'arc_sim', 'tracks'))
CONTAINER = 'arc-sim'
RETRY = 2.0  # s between car connection attempts (and car probes in sim mode)
SIM_RETRY = 0.5  # s between sim bridge attempts while it starts
STOP_GRACE = 20.0  # s to wait for run.sh to stop the container before `docker stop`
SENSORS = ('/scan',)  # bridge websocket paths the page may open

log = logging.getLogger('server')


def load_config(path, car):
    with open(path) as f:
        cfg = json.load(f)
    cfg.setdefault('car', {}).setdefault('host', None)
    cfg['car'].setdefault('bridge_port', 8765)
    cfg['car'].setdefault('udp_port', 9870)
    sim = cfg.setdefault('sim', {})
    sim.setdefault('host', '127.0.0.1')
    sim.setdefault('bridge_port', 8765)
    sim.setdefault('udp_port', 9870)
    sim.setdefault('track', 'oval')  # the page's initial pick
    if car:
        cfg['car']['host'] = car
    return cfg


def http_get(url, timeout=2.0):
    """Body of a 200 response, or None. Blocking; call it through asyncio.to_thread."""
    try:
        with urllib.request.urlopen(url, timeout=timeout) as r:
            return r.read() if r.status == 200 else None
    except (OSError, urllib.error.URLError, ValueError):
        return None


def track_names():
    """Bundled sim tracks (simulator/src/arc_sim/tracks/*.yaml): the ones the bridge can load."""
    try:
        return sorted(f[:-5] for f in os.listdir(TRACKS) if f.endswith('.yaml'))
    except OSError:
        return []


def container_running():
    try:
        out = subprocess.run(['docker', 'ps', '-q', '-f', 'name=^%s$' % CONTAINER],
                             capture_output=True, text=True, timeout=10).stdout
        return bool(out.strip())
    except (OSError, subprocess.SubprocessError):
        return False


def docker_stop():
    try:
        subprocess.run(['docker', 'stop', CONTAINER], capture_output=True, timeout=30)
    except (OSError, subprocess.SubprocessError) as e:
        log.warning('docker stop %s failed: %s', CONTAINER, e)


def response(status, body, content_type):
    status = HTTPStatus(status)
    headers = Headers([
        ('Date', email.utils.formatdate(usegmt=True)),
        ('Connection', 'close'),
        ('Content-Length', str(len(body))),
        ('Content-Type', content_type),
        ('Cache-Control', 'no-store'),
    ])
    return Response(status.value, status.phrase, headers, body)


class Server:

    def __init__(self, cfg):
        self.cfg = cfg
        self.pages = set()
        self.mode = 'real'
        self.connected = False
        self.car_found = False
        self.sim = 'stopped'
        self.udp = None
        self.bridge_url = None  # http://host:port of the connected bridge
        self.track_msg = None  # last track message, for pages that connect later
        self.track_status = None  # last bridge track_status message, likewise
        self.bridge_ws = None
        self.track = cfg['sim']['track']
        self.proc = None  # run.sh we started
        self.reused = False  # an arc-sim container we found running and are using
        self.bridge_task = None
        self.probe_task = None
        self.watch_task = None
        self.last_link = None
        self.switching = asyncio.Lock()
        self.tasks = set()

    # ---- page side ---------------------------------------------------------------

    def link(self):
        return {'type': 'link', 'mode': self.mode, 'connected': self.connected,
                'car_found': self.car_found, 'sim': self.sim, 'udp': self.udp}

    def send_link(self, force=False):
        msg = json.dumps(self.link())
        if msg != self.last_link or force:
            self.last_link = msg
            log.info('link %s', msg)
            broadcast(self.pages, msg)

    async def process_request(self, connection, request):
        path = request.path.split('?', 1)[0]
        if path == '/ws' or path in SENSORS:
            return None
        if path in ('/', '/index.html'):
            try:
                with open(PAGE, 'rb') as f:
                    return response(200, f.read(), 'text/html; charset=utf-8')
            except OSError:
                return connection.respond(HTTPStatus.NOT_FOUND, 'web/index.html not found\n')
        if path == '/track.png':
            body = await asyncio.to_thread(http_get, self.bridge_url + '/track.png') if self.bridge_url else None
            if body is None:
                return connection.respond(HTTPStatus.NOT_FOUND, 'no track\n')
            return response(200, body, 'image/png')
        return connection.respond(HTTPStatus.NOT_FOUND, 'not found\n')

    async def page(self, ws):
        path = ws.request.path.split('?', 1)[0]
        if path in SENSORS:
            return await self.sensor_relay(ws, path)
        self.pages.add(ws)
        try:
            await ws.send(json.dumps(self.link()))
            await ws.send(json.dumps({'type': 'tracks', 'names': track_names(), 'selected': self.track}))
            if self.connected and self.track_msg:
                await ws.send(self.track_msg)
            if self.connected and self.track_status:
                await ws.send(self.track_status)
            async for raw in ws:
                try:
                    msg = json.loads(raw)
                except ValueError:
                    continue
                if not isinstance(msg, dict):
                    continue
                if msg.get('type') == 'switch' and msg.get('mode') in ('sim', 'real'):
                    task = asyncio.create_task(self.switch(msg['mode']))
                    self.tasks.add(task)
                    task.add_done_callback(self.tasks.discard)
                elif msg.get('type') == 'sim':
                    track = msg.get('track')
                    if track in track_names():
                        self.track = track
                        log.info('sim track: %s', self.track)
                        await self.load_track()
                    else:
                        log.warning('unknown sim track %r; keeping %s', track, self.track)
        except ConnectionClosed:
            pass
        finally:
            self.pages.discard(ws)

    async def sensor_relay(self, page, path):
        """Relay one sensor socket from the bridge to one page socket, newest frame only.

        With no bridge connected, the page socket is closed right away; the page retries.
        """
        if not self.connected:
            await page.close(1013, 'no bridge')
            return
        host, port, _ = self.target()
        try:
            async with connect('ws://%s:%d%s' % (host, port, path), open_timeout=2,
                               max_size=2 ** 22) as up:
                latest = {'frame': None}
                fresh = asyncio.Event()

                async def pull():  # keeps only the newest frame, so a slow page skips frames
                    async for frame in up:
                        latest['frame'] = frame
                        fresh.set()

                async def push():
                    while True:
                        await fresh.wait()
                        fresh.clear()
                        await page.send(latest['frame'])

                async def drain():  # notices the page closing
                    async for _ in page:
                        pass

                tasks = [asyncio.create_task(f()) for f in (pull, push, drain)]
                try:
                    await asyncio.wait(tasks, return_when=asyncio.FIRST_COMPLETED)
                finally:
                    for t in tasks:
                        t.cancel()
                    await asyncio.gather(*tasks, return_exceptions=True)
        except (OSError, TimeoutError, ConnectionClosed) as e:
            log.info('%s relay: %s', path, e)
        finally:
            await page.close()

    # ---- mode switching ----------------------------------------------------------

    async def switch(self, mode):
        async with self.switching:  # one switch at a time; stopping the sim takes seconds
            await self._switch(mode)

    async def _switch(self, mode):
        if mode == self.mode:
            if mode == 'sim' and self.sim == 'stopped':
                await self.start_sim()  # sim exited; the page asked again
            return
        log.info('switch to %s', mode)
        self.mode = mode
        await self.cancel(self.bridge_task)
        self.set_disconnected()
        if mode == 'sim':
            self.probe_task = asyncio.create_task(self.probe_car())
            await self.start_sim()
        else:
            await self.cancel(self.probe_task)
            self.probe_task = None
            self.car_found = False
            self.send_link()
            await self.stop_sim()
        self.send_link()
        self.bridge_task = asyncio.create_task(self.bridge_loop())

    @staticmethod
    async def cancel(task):
        if task and not task.done():
            task.cancel()
            try:
                await task
            except (asyncio.CancelledError, Exception):
                pass

    # ---- simulator ---------------------------------------------------------------

    def sim_command(self):
        run = os.environ.get('ARC_GS_SIM_RUN', SIM_RUN)  # test hook: a stand-in for run.sh
        return [run, '--bridge']

    async def start_sim(self):
        if self.proc and self.proc.returncode is None:
            return
        self.sim = 'starting'
        self.send_link()
        if await asyncio.to_thread(container_running):
            sim = self.cfg['sim']
            answers = await asyncio.to_thread(
                http_get, 'http://%s:%d/track.json' % (sim['host'], sim['bridge_port']))
            log.info('an %s container is already running%s', CONTAINER,
                     '; its bridge answers, using it' if answers is not None
                     else "; its bridge doesn't answer yet, waiting for it")
            self.reused = True
            return
        cmd = self.sim_command()
        log.info('starting sim: %s', shlex.join(cmd))
        try:
            self.proc = await asyncio.create_subprocess_exec(*cmd, start_new_session=True)
        except OSError as e:
            log.error("couldn't start the sim: %s", e)
            self.sim = 'stopped'
            self.send_link()
            return
        self.reused = False
        await self.cancel(self.watch_task)
        self.watch_task = asyncio.create_task(self.watch_sim(self.proc))

    async def watch_sim(self, proc):
        code = await proc.wait()
        log.info('sim exited (%s)', code)
        if proc is self.proc:
            self.proc = None
            self.sim = 'stopped'
            if self.mode == 'sim':
                self.set_disconnected()
            self.send_link()

    async def stop_sim(self):
        proc, self.proc = self.proc, None
        await self.cancel(self.watch_task)
        if proc and proc.returncode is None:
            log.info('stopping sim')
            try:
                proc.send_signal(signal.SIGTERM)  # run.sh's trap stops the container
                await asyncio.wait_for(proc.wait(), STOP_GRACE)
            except (ProcessLookupError, asyncio.TimeoutError):
                pass
        if (proc or self.reused) and await asyncio.to_thread(container_running):
            log.info('container still running: docker stop %s', CONTAINER)
            await asyncio.to_thread(docker_stop)
        if proc and proc.returncode is None:
            try:
                os.killpg(proc.pid, signal.SIGKILL)
            except (ProcessLookupError, PermissionError):
                pass
            await proc.wait()
        self.reused = False
        self.sim = 'stopped'

    # ---- bridge ------------------------------------------------------------------

    def target(self):
        if self.mode == 'sim':
            s = self.cfg['sim']
            return s['host'], s['bridge_port'], s['udp_port']
        c = self.cfg['car']
        return c['host'], c['bridge_port'], c['udp_port']

    def set_disconnected(self):
        self.connected = False
        self.udp = None
        self.bridge_url = None
        self.bridge_ws = None
        self.track_status = None
        if self.mode == 'real':
            self.car_found = False
        if self.mode == 'sim' and self.sim == 'running':
            self.sim = 'starting' if (self.proc or self.reused) else 'stopped'

    async def bridge_loop(self):
        while True:
            host, port, udp_port = self.target()
            if self.mode == 'sim' and self.sim == 'stopped':
                await asyncio.sleep(SIM_RETRY)
                continue
            if host is None:  # car address is TBD: nothing to connect to
                await asyncio.sleep(RETRY)
                continue
            try:
                async with connect('ws://%s:%d/ws' % (host, port), open_timeout=2,
                                   max_size=2 ** 22) as ws:
                    base = 'http://%s:%d' % (host, port)
                    log.info('bridge connected: %s', base)
                    self.track_msg = await self.fetch_track(base)
                    self.bridge_url = base
                    self.bridge_ws = ws
                    self.connected = True
                    self.udp = {'host': host, 'port': udp_port}
                    if self.mode == 'real':
                        self.car_found = True
                    else:
                        self.sim = 'running'
                    self.send_link()
                    broadcast(self.pages, self.track_msg)
                    await self.load_track()
                    async for raw in ws:
                        if isinstance(raw, bytes):
                            continue
                        try:
                            kind = json.loads(raw).get('type')
                        except (ValueError, AttributeError):
                            continue
                        if kind == 'state':
                            broadcast(self.pages, raw)  # unchanged
                        elif kind == 'track_status':
                            await self.on_track_status(raw, base)
                log.info('bridge closed')
            except asyncio.CancelledError:
                raise
            except Exception as e:  # refused, timed out, dropped
                if self.connected:
                    log.info('bridge lost: %s', e)
            if self.connected:
                self.set_disconnected()
                self.send_link()
            if self.mode == 'sim' and self.reused and not await asyncio.to_thread(container_running):
                log.info('%s container is gone', CONTAINER)
                self.reused = False
                self.sim = 'stopped'
                self.send_link()
            await asyncio.sleep(RETRY if self.mode == 'real' else SIM_RETRY)

    async def fetch_track(self, base):
        """The bridge's track as a page `track` message."""
        raw = await asyncio.to_thread(http_get, base + '/track.json')
        try:
            meta = json.loads(raw) if raw is not None else None
        except ValueError:
            meta = None
        # The image rides in the message as base64: Firefox rejected it as a separate
        # HTTP request from this server, and this way it arrives with its metadata.
        png = await asyncio.to_thread(http_get, base + '/track.png') if meta else None
        return json.dumps({'type': 'track', 'meta': meta,
                           'png': base64.b64encode(png).decode() if png else None})

    async def load_track(self):
        """In sim mode, ask the connected bridge for the picked track. It ignores a repeat."""
        if self.mode != 'sim' or self.bridge_ws is None:
            return
        try:
            await self.bridge_ws.send(json.dumps({'type': 'load_track', 'name': self.track}))
        except ConnectionClosed:
            pass  # bridge_loop notices and reconnects, then asks again

    async def on_track_status(self, raw, base):
        """Relay the bridge's track_status; once a load finishes, send the page the new track."""
        try:
            status = json.loads(raw)
        except ValueError:
            return
        self.track_status = raw
        log.info('track %s: %s%s', status.get('name'), status.get('status'),
                 ' (%s)' % status['error'] if status.get('error') else '')
        broadcast(self.pages, raw)
        if status.get('status') in ('ready', 'none'):
            msg = await self.fetch_track(base)
            if msg != self.track_msg:
                self.track_msg = msg
                broadcast(self.pages, msg)

    async def probe_car(self):
        """In sim mode: does the car's bridge answer? Drives the page's "live car found"."""
        while True:
            c = self.cfg['car']
            found = False
            if c['host'] is not None:
                found = await asyncio.to_thread(
                    http_get, 'http://%s:%d/track.json' % (c['host'], c['bridge_port']), 1.5) is not None
            if found != self.car_found:
                self.car_found = found
                self.send_link()
            await asyncio.sleep(RETRY)

    # ---- main --------------------------------------------------------------------

    async def run(self, host, port):
        stop = asyncio.Event()
        loop = asyncio.get_running_loop()
        for sig in (signal.SIGINT, signal.SIGTERM):
            loop.add_signal_handler(sig, stop.set)
        async with serve(self.page, host, port, process_request=self.process_request):
            log.info('serving http://%s:%d/ (car: %s, sim track: %s)', host, port,
                     self.cfg['car']['host'] or 'TBD', self.track)
            self.bridge_task = asyncio.create_task(self.bridge_loop())
            await stop.wait()
            log.info('shutting down')
            async with self.switching:  # let a switch in progress finish, so its sim is known
                pass
            await self.cancel(self.bridge_task)
            await self.cancel(self.probe_task)
            if self.proc:  # only a sim we started
                await self.stop_sim()


def main():
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    p.add_argument('--config', default=os.path.join(HERE, 'config.json'))
    p.add_argument('--car', help="car's address (overrides config car.host)")
    p.add_argument('--port', type=int, default=8080)
    args = p.parse_args()
    logging.basicConfig(level=logging.INFO, format='%(asctime)s server: %(message)s', datefmt='%H:%M:%S')
    logging.getLogger('websockets').setLevel(logging.WARNING)
    cfg = load_config(args.config, args.car)
    asyncio.run(Server(cfg).run('127.0.0.1', args.port))


if __name__ == '__main__':
    sys.exit(main())
