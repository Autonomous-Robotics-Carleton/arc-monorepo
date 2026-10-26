"""Tests for the fake Orin on the sync link."""

import unittest

from arcsim import scenario
from arcsim.fleet import build
from arcsim.transport import MemoryTransport

from .helpers import MS, VERSION, decode, dialect, of_type, run


def orin_run(events=(), duration_ms=1000, **kw):
    return decode(run(events, duration_ms=duration_ms, corners=(), orin=True, **kw)["orin"])


class Rates(unittest.TestCase):
    def test_heartbeat_50hz_command_500hz_link_status_1hz(self):
        frames = orin_run(duration_ms=2000)
        self.assertAlmostEqual(len(of_type(frames, "ARC_OPERATOR_HEARTBEAT")), 100, delta=1)
        self.assertAlmostEqual(len(of_type(frames, "ARC_DRIVE_COMMAND")), 1000, delta=2)
        # one at start, then every second: t = 0, 1 s, 2 s
        self.assertEqual(len(of_type(frames, "ARC_LINK_STATUS")), 3)

    def test_heartbeat_counter_counts_up(self):
        c = [m.counter for _, m in of_type(orin_run(duration_ms=200), "ARC_OPERATOR_HEARTBEAT")]
        self.assertEqual(c, list(range(1, len(c) + 1)))

    def test_commands_repeat_with_their_original_time_until_the_next_update(self):
        # ICD: 500 Hz on the link, 100 Hz from the controller: 5 sends per output
        cmds = [m for _, m in of_type(orin_run(duration_ms=300), "ARC_DRIVE_COMMAND")]
        stamps = [m.time_ns for m in cmds]
        runs = [stamps.count(s) for s in dict.fromkeys(stamps)]
        self.assertEqual(set(runs[:-1]), {5})
        self.assertEqual(len(set(stamps)), len(runs))

    def test_command_values(self):
        cmds = of_type(orin_run([{"t_ms": 100, "node": "orin", "action": "command",
                                  "speed": 2.5, "accel": 1.5, "steer": -0.1}]),
                       "ARC_DRIVE_COMMAND")
        before = {(m.speed, m.accel, m.steer) for t, m in cmds if t < 100 * MS}
        after = {(round(m.speed, 3), round(m.accel, 3), round(m.steer, 3))
                 for t, m in cmds if t >= 102 * MS}
        self.assertEqual(before, {(0.0, 0.0, 0.0)})
        self.assertEqual(after, {(2.5, 1.5, -0.1)})


class Faults(unittest.TestCase):
    def test_heartbeat_stops_and_commands_carry_on(self):
        frames = orin_run([{"t_ms": 200, "node": "orin", "action": "heartbeat_stop"}],
                          duration_ms=600)
        hb = [t for t, _ in of_type(frames, "ARC_OPERATOR_HEARTBEAT")]
        self.assertFalse([t for t in hb if t > 200 * MS])
        self.assertTrue([t for t, _ in of_type(frames, "ARC_DRIVE_COMMAND") if t > 500 * MS])

    def test_commands_stop_and_the_heartbeat_carries_on(self):
        # ICD open issue 6: commands gone, heartbeat still arriving
        frames = orin_run([{"t_ms": 200, "node": "orin", "action": "command_stop"}],
                          duration_ms=600)
        self.assertFalse([t for t, _ in of_type(frames, "ARC_DRIVE_COMMAND") if t > 202 * MS])
        self.assertTrue([t for t, _ in of_type(frames, "ARC_OPERATOR_HEARTBEAT") if t > 500 * MS])

    def test_heartbeat_resumes(self):
        frames = orin_run([{"t_ms": 100, "node": "orin", "action": "heartbeat_stop"},
                           {"t_ms": 300, "node": "orin", "action": "heartbeat_resume"}],
                          duration_ms=500)
        hb = [t for t, _ in of_type(frames, "ARC_OPERATOR_HEARTBEAT")]
        self.assertFalse([t for t in hb if 100 * MS < t < 300 * MS])
        self.assertTrue([t for t in hb if t >= 300 * MS])

    def test_envelope_is_sent_once(self):
        frames = of_type(orin_run([{"t_ms": 50, "node": "orin", "action": "envelope",
                                    "max_speed": 3.0, "max_accel": 2.0, "max_steer": 0.4}]),
                         "ARC_ENVELOPE_SET")
        self.assertEqual(len(frames), 1)
        m = frames[0][1]
        self.assertEqual((m.max_speed, m.max_accel), (3.0, 2.0))
        self.assertAlmostEqual(m.max_steer, 0.4, places=5)

    def test_rearm_is_sent_once_per_action(self):
        frames = of_type(orin_run([{"t_ms": 50, "node": "orin", "action": "rearm"},
                                   {"t_ms": 300, "node": "orin", "action": "rearm"}]),
                         "ARC_REARM")
        self.assertEqual([m.counter for _, m in frames], [1, 2])
        self.assertTrue(50 * MS <= frames[0][0] < 53 * MS)

    def test_dropped_datagrams_show_as_a_sequence_gap(self):
        frames = orin_run([{"t_ms": 100, "node": "orin", "action": "drop", "count": 2}],
                          duration_ms=300)
        seqs = [m.get_header().seq for _, m in frames]
        self.assertEqual([(b - a) & 0xFF for a, b in zip(seqs, seqs[1:])
                          if (b - a) & 0xFF != 1], [3])

    def test_corrupted_datagram_fails_the_crc(self):
        frames = orin_run([{"t_ms": 100, "node": "orin", "action": "corrupt", "count": 1}],
                          duration_ms=300)
        self.assertEqual(len([1 for _, m in frames if m.get_type() == "BAD_DATA"]), 1)

    def test_version_mismatch(self):
        frames = orin_run([{"t_ms": 0, "node": "orin", "action": "version",
                            "value": VERSION + 1}], duration_ms=1500)
        self.assertEqual({m.protocol_version for _, m in of_type(frames, "ARC_LINK_STATUS")},
                         {VERSION + 1})


class Receiving(unittest.TestCase):
    def test_follows_the_sync_mcus_clock_and_records_what_it_gets(self):
        t = MemoryTransport()
        orin = build(orin=t, seed=1).nodes["orin"]
        mcu = dialect.MAVLink(None, srcSystem=1, srcComponent=1)
        t.inbox = bytes(dialect.MAVLink_arc_safety_state_message(
            time_ns=5 * 10**12, max_speed=3.0, max_accel=2.0, max_steer=0.4,
            watchdog=0, estop=0).pack(mcu))
        orin.poll_rx(10 * MS)
        self.assertEqual(orin.rx_count["ARC_SAFETY_STATE"], 1)
        self.assertEqual(orin.mcu_time_ns(10 * MS), 5 * 10**12)
        self.assertEqual(orin.mcu_time_ns(30 * MS), 5 * 10**12 + 20 * MS)


class Scenarios(unittest.TestCase):
    def test_orin_actions_are_validated(self):
        for bad in ({"t_ms": 0, "node": "orin", "action": "envelope", "max_speed": 3},
                    {"t_ms": 0, "node": "orin", "action": "explode"}):
            with self.assertRaises(ValueError):
                scenario.load_scenario({"events": [bad]})


if __name__ == "__main__":
    unittest.main()
