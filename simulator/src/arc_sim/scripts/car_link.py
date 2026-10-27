#!/usr/bin/env python3
"""The sim's stand-in for the car's receiving end: heartbeat watchdog and teleop receiver.

Implements car_link in ground-station/protocol.md. The ground station's helper sends
29-byte UDP packets (`<4sBIIdff`: magic, type, session, seq, t, speed, steer) to port 9870:
heartbeats and teleop, each at 50 Hz. Algorithms drive through /sim/auto/cmd_vel.

Sessions: each helper run picks a random session ID and sends `hello` until car_link acks it.
car_link accepts heartbeats and teleop only from the session it last acked. A hello with a new
ID replaces the session (the newest ground station wins): sequence numbers start over and the
old session's teleop is forgotten, so the car stays stopped until the new one heartbeats. Packets
from any other session are rejected, and the sender is told why (`no-session` when car_link has
none, e.g. after a restart, so the helper says hello again; `replaced` when another session took
over, so it stops), at most REJECT_EVERY apart per sender.

- No heartbeat for heartbeat_timeout: every source is overridden and the car stops.
- No teleop for teleop_timeout (heartbeat alive): teleop counts as zero speed.
- Teleop wins over /sim/auto/cmd_vel while fresh; both are gated by the heartbeat.
- Speed moves toward its target at no more than `decel` m/s^2, so a stop is a ramp,
  not a wall. Curvature holds its last value while braking, so the car keeps its line.

Publishes /sim/cmd_vel at `rate` Hz and /sim/link/status (JSON) at 10 Hz. Both timers run
on the wall clock (this node doesn't use sim time), so they still fire if sim time stalls.
"""

import json
import math
import socket
import struct
import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node
from std_msgs.msg import String

PACKET = struct.Struct('<4sBIIdff')
MAGIC = b'ARC0'
HEARTBEAT, TELEOP, HELLO, ACK, REJECT = 1, 2, 3, 4, 5
# REJECT's reason, carried in its seq field
NO_SESSION, REPLACED = 1, 2
REASONS = {NO_SESSION: 'no-session', REPLACED: 'replaced'}
REJECT_EVERY = 0.2  # s between rejects to one sender


def newer(seq, last):
    """Wrap-aware: seq is ahead of last by less than half the u32 range."""
    return last is None or 0 < (seq - last) % 2**32 < 2**31


class CarLink(Node):

    def __init__(self):
        super().__init__('car_link')
        self.port = self.declare_parameter('port', 9870).value
        self.hb_timeout = self.declare_parameter('heartbeat_timeout', 0.15).value
        self.teleop_timeout = self.declare_parameter('teleop_timeout', 0.15).value
        self.decel = self.declare_parameter('decel', 5.0).value
        self.wheelbase = self.declare_parameter('wheelbase', 0.3302).value
        self.steer_limit = self.declare_parameter('steering_limit', 0.4189).value
        rate = self.declare_parameter('rate', 50.0).value

        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.sock.bind(('0.0.0.0', self.port))
        self.sock.setblocking(False)

        self.session = None  # the acked session's ID; None until the first hello
        self.seq = {HEARTBEAT: None, TELEOP: None}
        self.rejected = 0  # packets rejected since start
        self.rejected_reason = None  # the latest rejection's reason
        self.last_reject = {}  # sender address -> monotonic time of the last reject sent to it
        self.hb_time = None      # monotonic receipt time of the last accepted packet
        self.teleop_time = None
        self.teleop = (0.0, 0.0)  # speed, steer
        self.auto_time = None
        self.auto = Twist()

        self.speed = 0.0
        self.curvature = 0.0
        self.reason = 'idle'
        self.last_tick = time.monotonic()

        self.pub = self.create_publisher(Twist, '/sim/cmd_vel', 10)
        self.status_pub = self.create_publisher(String, '/sim/link/status', 10)
        self.create_subscription(Twist, '/sim/auto/cmd_vel', self.on_auto, 10)
        self.create_timer(1.0 / rate, self.tick)
        self.create_timer(0.1, self.publish_status)
        self.get_logger().info('car_link listening on UDP %d' % self.port)

    def on_auto(self, msg):
        self.auto_time = time.monotonic()
        self.auto = msg

    def reply(self, kind, session, code, addr):
        try:
            self.sock.sendto(PACKET.pack(MAGIC, kind, session, code, time.monotonic(), 0.0, 0.0), addr)
        except OSError as e:
            self.get_logger().warn('reply to %s:%d failed: %s' % (addr[0], addr[1], e), throttle_duration_sec=5.0)

    def reject(self, session, code, addr, now):
        self.rejected += 1
        self.rejected_reason = REASONS[code]
        if now - self.last_reject.get(addr, -math.inf) >= REJECT_EVERY:
            self.last_reject[addr] = now
            self.reply(REJECT, session, code, addr)
            self.get_logger().warn('Rejecting packets from %s:%d (session %08x): %s'
                                   % (addr[0], addr[1], session, REASONS[code]), throttle_duration_sec=2.0)

    def hello(self, session, addr):
        if session != self.session:
            if self.session is not None:
                self.get_logger().warn('Session %08x replaced by %08x from %s:%d' % (self.session, session, *addr))
            else:
                self.get_logger().info('Session %08x from %s:%d' % (session, *addr))
            self.session = session
            self.seq = {HEARTBEAT: None, TELEOP: None}
            # Forget the old session's commands: the car stays stopped until this one heartbeats.
            self.hb_time = self.teleop_time = None
            self.teleop = (0.0, 0.0)
            self.last_reject.clear()
        # A repeated hello for the current session only gets another ack: its seqs carry on.
        self.reply(ACK, session, 0, addr)

    def receive(self, now):
        """Drain every pending packet; keep only valid, newer ones from the current session."""
        while True:
            try:
                data, addr = self.sock.recvfrom(64)
            except BlockingIOError:
                return
            if len(data) != PACKET.size:
                continue
            magic, kind, session, seq, _t, speed, steer = PACKET.unpack(data)
            if magic != MAGIC:
                continue
            if kind == HELLO:
                self.hello(session, addr)
                continue
            if kind not in self.seq:
                continue
            if session != self.session:
                self.reject(session, NO_SESSION if self.session is None else REPLACED, addr, now)
                continue
            if not newer(seq, self.seq[kind]):
                continue
            if not (math.isfinite(speed) and math.isfinite(steer)):
                continue
            self.seq[kind] = seq
            if kind == HEARTBEAT:
                self.hb_time = now
            else:
                self.teleop_time = now
                self.teleop = (speed, steer)

    def fresh(self, stamp, timeout, now):
        return stamp is not None and now - stamp <= timeout

    def target(self, now):
        """(reason, target speed, curvature or None to hold the last one)."""
        if self.hb_time is None and self.teleop_time is None:
            return 'idle', 0.0, None
        if not self.fresh(self.hb_time, self.hb_timeout, now):
            return 'heartbeat', 0.0, None
        if self.fresh(self.teleop_time, self.teleop_timeout, now):
            speed, steer = self.teleop
            steer = max(-self.steer_limit, min(self.steer_limit, steer))
            return 'ok', speed, math.tan(steer) / self.wheelbase
        # An algorithm that goes silent is stopped like teleop (protocol.md gives it no timeout).
        if self.fresh(self.auto_time, self.teleop_timeout, now):
            v, w = self.auto.linear.x, self.auto.angular.z
            return 'ok', v, (w / v if abs(v) > 1e-3 else None)
        return 'teleop', 0.0, None

    def tick(self):
        now = time.monotonic()
        dt = min(now - self.last_tick, 0.1)
        self.last_tick = now
        self.receive(now)

        reason, target, curvature = self.target(now)
        if reason != self.reason:
            if reason == 'ok':
                self.get_logger().info('Link ok: driving')
            elif self.reason == 'ok':
                self.get_logger().warn('Stopping: %s' % {
                    'heartbeat': 'no heartbeat for %.2f s' % self.hb_timeout,
                    'teleop': 'no teleop for %.2f s' % self.teleop_timeout,
                    'idle': 'nothing received'}[reason])
            self.reason = reason
        if curvature is not None:
            self.curvature = curvature

        step = self.decel * dt
        self.speed += max(-step, min(step, target - self.speed))
        msg = Twist()
        msg.linear.x = self.speed
        msg.angular.z = self.speed * self.curvature
        self.pub.publish(msg)

    def publish_status(self):
        now = time.monotonic()

        def age(stamp):
            return None if stamp is None else round(now - stamp, 3)

        self.status_pub.publish(String(data=json.dumps({
            'heartbeat_age': age(self.hb_time),
            'teleop_age': age(self.teleop_time),
            'stopping': self.reason != 'ok',
            'reason': self.reason,
            'session': None if self.session is None else '%08x' % self.session,
            'rejected': self.rejected,
            'rejected_reason': self.rejected_reason,
        })))


def main():
    rclpy.init()
    node = CarLink()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.try_shutdown()


if __name__ == '__main__':
    main()
