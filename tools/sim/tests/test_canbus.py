"""The controllers' side of the command bus (arcsim/canbus.py)."""

import struct
import unittest

from arcsim import controller
from arcsim.canbus import COMMAND_TIMEOUT_NS, WIRE, FakeCommandBus
from arcsim.fleet import build
from arcsim.transport import MemoryTransport

from .helpers import MS, decode, of_type


def frame(packet, corner, value):
    """What the firmware's bridge sends: the VESC ID and a big-endian int32."""
    return WIRE.pack((packet << 8) | (corner + 1), 4, struct.pack(">i", value) + bytes(4))


def feed(bus, data, t_ns):
    bus.transport.inbox = data
    bus.poll_rx(t_ns)


class Commands(unittest.TestCase):
    def test_commands_are_decoded_per_corner(self):
        bus = FakeCommandBus(MemoryTransport())
        feed(bus, frame(3, 0, 2500) + frame(3, 3, -1000) + frame(1, 1, 0) + frame(2, 2, 10000), 5 * MS)
        self.assertEqual(bus.count[0], {"current": 0, "brake": 0, "rpm": 1})
        self.assertEqual(bus.last_value[0], 2500.0)
        self.assertEqual(bus.last_value[3], -1000.0)
        self.assertEqual(bus.count[1]["current"], 1)
        self.assertEqual(bus.last_value[2], 10.0)  # 10000 mA
        self.assertEqual(bus.first_ns[2]["brake"], 5 * MS)

    def test_first_brake_time_is_kept(self):
        bus = FakeCommandBus(MemoryTransport())
        feed(bus, frame(2, 0, 10000), 7 * MS)
        feed(bus, frame(2, 0, 10000), 9 * MS)
        self.assertEqual(bus.first_ns[0]["brake"], 7 * MS)
        self.assertEqual(bus.last_ns[0]["brake"], 9 * MS)

    def test_bad_frames_are_counted_and_ignored(self):
        bus = FakeCommandBus(MemoryTransport())
        feed(bus, WIRE.pack((7 << 8) | 1, 4, bytes(8)), 0)         # not a packet we take
        feed(bus, WIRE.pack((3 << 8) | 9, 4, bytes(8)), 0)         # no such controller
        feed(bus, WIRE.pack((3 << 8) | 1, 3, bytes(8)), 0)         # wrong length
        feed(bus, b"\x00" * 5, 0)                                  # a truncated datagram
        self.assertEqual(bus.bad, 4)
        self.assertEqual(bus.last_cmd_ns, [None] * 4)


class Status(unittest.TestCase):
    def test_status_1_at_50_hz_for_each_corner(self):
        bus = FakeCommandBus(MemoryTransport())
        for t in range(0, 1000 * MS, MS):
            bus.tick(t)
        frames = [WIRE.unpack(d) for _, d in bus.transport.writes]
        self.assertEqual(len(frames), 200)
        self.assertEqual({f[0] for f in frames}, {(9 << 8) | n for n in (1, 2, 3, 4)})
        self.assertTrue(all(f[1] == 8 for f in frames))


class CommandTimeout(unittest.TestCase):
    def setUp(self):
        self.ctl = {"FL": type("C", (), {"auto_state": 0, "target_erpm": 5000.0, "erpm": 0.0})()}
        self.bus = FakeCommandBus(MemoryTransport(), controllers=self.ctl)

    def test_no_timeout_before_the_first_command(self):
        for t in range(0, 1000 * MS, MS):
            self.bus.tick(t)
        self.assertEqual(self.ctl["FL"].auto_state, 0)

    def test_times_out_after_150_ms_and_brakes(self):
        feed(self.bus, frame(3, 0, 5000), 0)
        self.bus.tick(COMMAND_TIMEOUT_NS)
        self.assertEqual(self.ctl["FL"].auto_state, 0)  # exactly 150 ms is not yet
        self.bus.tick(COMMAND_TIMEOUT_NS + MS)
        self.assertEqual(self.ctl["FL"].auto_state, 2)
        self.assertEqual(self.ctl["FL"].target_erpm, 0.0)

    def test_commands_clear_the_timeout(self):
        feed(self.bus, frame(3, 0, 5000), 0)
        self.bus.tick(200 * MS)
        self.assertEqual(self.ctl["FL"].auto_state, 2)
        feed(self.bus, frame(3, 0, 5000), 210 * MS)
        self.assertEqual(self.ctl["FL"].auto_state, 0)

    def test_a_brake_command_stops_the_motor(self):
        feed(self.bus, frame(2, 0, 10000), 0)
        self.assertEqual(self.ctl["FL"].target_erpm, 0.0)

    def test_an_rpm_command_sets_the_target(self):
        feed(self.bus, frame(3, 0, 8000), 0)
        self.assertEqual(self.ctl["FL"].target_erpm, 8000.0)


class InTheFleet(unittest.TestCase):
    def test_the_timeout_shows_in_the_uart_telemetry(self):
        links = {n: MemoryTransport() for n in controller.CORNERS}
        can = MemoryTransport()
        can.inbox = frame(3, 0, 3000)  # one command at the start, then silence
        fleet = build(links, can=can, seed=1)
        fleet.run(400 * MS)
        states = [(t, m.state) for t, m in of_type(decode(links["FL"]), "ARC_MOTOR_STATUS")]
        self.assertEqual(states[10][1], 0)
        self.assertEqual(states[-1][1] & 2, 2)
        first = next(t for t, s in states if s & 2)
        self.assertAlmostEqual(first / MS, 150, delta=3)
        # the corners that were never commanded don't time out
        self.assertTrue(all(m.state == 0 for _, m in of_type(decode(links["FR"]), "ARC_MOTOR_STATUS")))


if __name__ == "__main__":
    unittest.main()
