#!/usr/bin/env python3
"""The sim's bridge to the ground station: track files over HTTP, car state over a websocket.

http://<host>:<port>/track.json  the track's placement, or null with no track
http://<host>:<port>/track.png   the track's walls: opaque where occupied, faint where unknown
ws://<host>:<port>/ws            JSON at 30 Hz, the `state` message in ground-station/protocol.md;
                                 `track_status` on connect and on change; takes `load_track`

The sim starts with no track. `load_track` (from the ground station) loads a bundled track
while the sim runs: the old walls are removed and the new ones spawned through Gazebo's
UserCommands services, and the car is moved to the track's start pose. Only the newest
request is loaded; one arriving mid-load waits for the current one to finish.

estimate is the car's own odometry (/sim/odom), which starts at zero at the spawn pose and
doesn't notice a teleport. So it is taken relative to its value when the car was last placed,
then moved into the world frame by that placement. truth is Gazebo's actual pose
(/sim/ground_truth), which only the sim has. link is car_link's latest /sim/link/status.
The page itself is served by the ground station, not here.
"""

import asyncio
import io
import json
import math
import os
import subprocess
import tempfile
import threading
import time
from http import HTTPStatus

import numpy as np
import rclpy
import websockets
from nav_msgs.msg import Odometry
from PIL import Image
from rclpy.node import Node
from std_msgs.msg import String

from ament_index_python.packages import get_package_share_directory

from arc_sim import track as tracks

GZ_TIMEOUT = 3000  # ms per Gazebo service call
GZ_TRIES = 60  # attempts, 0.5 s apart, while Gazebo or the car are still starting


def pose2d(msg):
    p, q = msg.pose.pose.position, msg.pose.pose.orientation
    yaw = math.atan2(2 * (q.w * q.z + q.x * q.y), 1 - 2 * (q.y * q.y + q.z * q.z))
    return {'x': p.x, 'y': p.y, 'yaw': yaw}


class Bridge(Node):

    def __init__(self):
        super().__init__('bridge')
        self.port = self.declare_parameter('port', 8765).value
        self.rate = self.declare_parameter('rate', 30.0).value
        self.world = self.declare_parameter('world', 'empty').value
        self.wall_height = self.declare_parameter('wall_height', 0.3).value
        self.tracks_dir = os.path.join(get_package_share_directory('arc_sim'), 'tracks')
        self.start = (0.0, 0.0, 0.0)  # where the car was last placed, world frame
        self.odom0 = None  # raw odometry at that moment; None: take the next message
        self.raw_odom = None
        self.state = {'type': 'state', 'source': 'sim', 'estimate': None, 'truth': None, 'link': None}
        self.create_subscription(Odometry, '/sim/ground_truth', self.on_truth, 10)
        self.create_subscription(Odometry, '/sim/odom', self.on_odom, 10)
        self.create_subscription(String, '/sim/link/status', self.on_link, 10)

    def on_truth(self, msg):
        self.state['truth'] = pose2d(msg)

    def on_odom(self, msg):
        p = self.raw_odom = pose2d(msg)
        if self.odom0 is None:
            self.odom0 = p
        # Odometry since the car was placed, in the frame it was placed in, then into the world.
        o = self.odom0
        dx, dy = p['x'] - o['x'], p['y'] - o['y']
        c, s = math.cos(-o['yaw']), math.sin(-o['yaw'])
        rx, ry, ryaw = c * dx - s * dy, s * dx + c * dy, p['yaw'] - o['yaw']
        sx, sy, syaw = self.start
        c, s = math.cos(syaw), math.sin(syaw)
        self.state['estimate'] = {'x': sx + c * rx - s * ry, 'y': sy + s * rx + c * ry,
                                  'yaw': math.atan2(math.sin(ryaw + syaw), math.cos(ryaw + syaw))}

    def placed(self, start):
        """The car was just moved to `start`: restart the estimate there."""
        self.start = start
        self.odom0 = self.raw_odom

    def on_link(self, msg):
        try:
            self.state['link'] = json.loads(msg.data)
        except ValueError:
            self.get_logger().warn('Unparseable /sim/link/status: %r' % msg.data)


def gz_call(service, reqtype, req):
    """One Gazebo service call through the gz CLI. True if it answered `data: true`."""
    try:
        out = subprocess.run(
            ['gz', 'service', '-s', service, '--reqtype', reqtype, '--reptype', 'gz.msgs.Boolean',
             '--timeout', str(GZ_TIMEOUT), '--req', req],
            capture_output=True, text=True, timeout=GZ_TIMEOUT / 1000 + 5).stdout
    except (OSError, subprocess.SubprocessError):
        return False
    return 'data: true' in out


def gz_retry(service, reqtype, req):
    for _ in range(GZ_TRIES):
        if gz_call(service, reqtype, req):
            return True
        time.sleep(0.5)
    return False


def swap_walls(node, old, new, track):
    """Blocking: remove model `old` (if any), spawn `track`'s walls as `new` (if any), place the car.

    Each wall set gets a fresh model name: Gazebo removes an entity at the end of a step, so
    reusing the name right away could collide with the one being removed. Returns (the wall
    model now in the world or None, an error string or None).
    """
    w = '/world/%s/' % node.world
    if old and not gz_call(w + 'remove', 'gz.msgs.Entity', 'name: "%s", type: MODEL' % old):
        node.get_logger().warn("Couldn't remove %s; it may already be gone" % old)
    start = (0.0, 0.0, 0.0)
    if track:
        start = track.start
        with tempfile.NamedTemporaryFile('w', prefix='arc_walls_', suffix='.sdf', delete=False) as f:
            f.write('<?xml version="1.0"?>\n<sdf version="1.9">\n%s</sdf>\n'
                    % tracks.walls_model(track, node.wall_height, new))
        try:
            if not gz_retry(w + 'create', 'gz.msgs.EntityFactory', 'sdf_filename: "%s"' % f.name):
                return None, "couldn't spawn the walls"
        finally:
            os.unlink(f.name)
    else:
        new = None
    x, y, yaw = start
    pose = ('name: "racecar", position: {x: %f, y: %f, z: 0.01}, orientation: {z: %f, w: %f}'
            % (x, y, math.sin(yaw / 2), math.cos(yaw / 2)))
    if not gz_retry(w + 'set_pose', 'gz.msgs.Pose', pose):
        return new, "couldn't move the car to the start"
    node.placed(start)
    return new, None


def track_files(path):
    """(track.json, track.png) bodies. The png is an alpha mask the page tints."""
    if not path:
        return b'null', None
    t = tracks.load(path)
    alpha = np.where(t.occupied, 255, np.where(t.unknown, 40, 0)).astype(np.uint8)
    rgba = np.zeros(alpha.shape + (4,), np.uint8)
    rgba[..., 3] = alpha
    png = io.BytesIO()
    Image.fromarray(rgba, 'RGBA').save(png, 'PNG')
    meta = {'name': t.name, 'resolution': t.resolution, 'origin': t.origin,
            'width': alpha.shape[1], 'height': alpha.shape[0]}
    return json.dumps(meta).encode(), png.getvalue()


async def serve(node):
    files = {'json': b'null', 'png': None}
    status = {'type': 'track_status', 'name': None, 'status': 'none', 'error': None}
    walls = {'model': None, 'n': 0}  # current wall model's name, and a counter for fresh ones
    wanted = {'name': None}
    wake = asyncio.Event()
    clients = set()

    def set_status(**kw):
        status.update(kw)
        if clients:
            websockets.broadcast(clients, json.dumps(status))

    async def loader():
        """Loads the newest wanted track whenever it differs from the loaded one."""
        while True:
            await wake.wait()
            wake.clear()
            name = wanted['name']
            if name == status['name'] and status['status'] in ('ready', 'none'):
                continue
            set_status(name=name, status='loading', error=None)
            node.get_logger().info('Loading track: %s' % (name or 'none'))
            try:
                track = tracks.load(os.path.join(node.tracks_dir, name + '.yaml')) if name else None
                walls['n'] += 1
                new = 'track_walls_%d' % walls['n'] if track else None
                walls['model'], error = await asyncio.to_thread(swap_walls, node, walls['model'], new, track)
                files['json'], files['png'] = track_files(walls['model'] and track.yaml_path)
            except Exception as e:  # unreadable map
                error = str(e)
            if error:
                node.get_logger().error('Track %s: %s' % (name, error))
                set_status(status='failed', error=error)
            else:
                set_status(status='ready' if name else 'none')

    async def process_request(path, headers):
        if path == '/ws':
            return None  # continue the websocket handshake
        if path == '/track.json':
            return HTTPStatus.OK, [('Content-Type', 'application/json')], files['json']
        if path == '/track.png' and files['png']:
            return HTTPStatus.OK, [('Content-Type', 'image/png')], files['png']
        return HTTPStatus.NOT_FOUND, [], b'not found\n'

    async def handler(ws, path=None):
        clients.add(ws)
        try:
            await ws.send(json.dumps(status))
            async for raw in ws:
                try:
                    msg = json.loads(raw)
                except ValueError:
                    continue
                if isinstance(msg, dict) and msg.get('type') == 'load_track':
                    name = msg.get('name')
                    if name is not None and not os.path.isfile(os.path.join(node.tracks_dir, '%s.yaml' % name)):
                        set_status(name=name, status='failed', error='no such track')
                        continue
                    wanted['name'] = name
                    wake.set()
        except websockets.ConnectionClosed:
            pass
        finally:
            clients.discard(ws)

    asyncio.get_running_loop().create_task(loader())

    async with websockets.serve(handler, '0.0.0.0', node.port, process_request=process_request):
        node.get_logger().info('Bridge on port %d' % node.port)
        while rclpy.ok():
            if clients:
                websockets.broadcast(clients, json.dumps(node.state))
            await asyncio.sleep(1.0 / node.rate)


def main():
    rclpy.init()
    node = Bridge()
    threading.Thread(target=rclpy.spin, args=(node,), daemon=True).start()
    try:
        asyncio.run(serve(node))
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
