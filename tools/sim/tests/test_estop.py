"""Tests for the fake e-stop button and what it does to the controllers."""

import unittest

from .helpers import MS, decode, of_type, run

PRESS = {"t_ms": 100, "node": "estop", "action": "press"}


def release(t_ms):
    return {"t_ms": t_ms, "node": "estop", "action": "release"}


def statuses(links, corner="FL"):
    return of_type(decode(links[corner]), "ARC_MOTOR_STATUS")


class Input(unittest.TestCase):
    def test_press_and_release_set_the_firmware_input(self):
        links = run([PRESS, release(300)], estop=True, duration_ms=400)
        self.assertEqual(links["estop"].writes, [(100 * MS, b"\x01"), (300 * MS, b"\x00")])

    def test_a_second_press_sends_nothing(self):
        links = run([PRESS, dict(PRESS, t_ms=150)], estop=True, duration_ms=200)
        self.assertEqual(len(links["estop"].writes), 1)


class Controllers(unittest.TestCase):
    def test_every_corner_reports_estop_while_pressed(self):
        links = run([PRESS, release(1000)], estop=True, duration_ms=1500)
        for corner in ("FL", "FR", "RL", "RR"):
            s = statuses(links, corner)
            during = {m.state for t, m in s if 101 * MS < t < 1000 * MS}
            after = {m.state for t, m in s if t > 1001 * MS}
            self.assertEqual(during, {1}, corner)  # ARC_MOTOR_STATE_ESTOP
            self.assertEqual(after, {0}, corner)

    def test_the_motor_bus_is_cut_at_t_and_the_controllers_boot_on_release(self):
        links = run([PRESS, release(3000)], estop=True, duration_ms=3500,
                    estop_options={"cut_ms": 2400, "restart_ms": 200})
        times = [t for t, _ in statuses(links)]
        self.assertTrue([t for t in times if 2000 * MS < t < 2500 * MS])
        self.assertFalse([t for t in times if 2501 * MS < t < 3200 * MS])  # off, then booting
        self.assertTrue([t for t in times if t > 3201 * MS])
        after = [m for t, m in statuses(links) if t > 3201 * MS]
        self.assertEqual({(m.state, m.fault, m.erpm) for m in after}, {(0, 0, 0)})

    def test_a_short_press_cuts_nothing(self):
        links = run([PRESS, release(500)], estop=True, duration_ms=1000)
        times = [t for t, _ in statuses(links)]
        gaps = [b - a for a, b in zip(times, times[1:])]
        self.assertLess(max(gaps), 2 * MS)

    def test_aux_always_on_keeps_telemetry_through_the_cut(self):
        links = run([PRESS, release(3000)], estop=True, duration_ms=3500,
                    estop_options={"aux_always_on": True})
        times = [t for t, _ in statuses(links)]
        self.assertTrue([t for t in times if 2600 * MS < t < 2900 * MS])

    def test_a_restart_clears_a_fault(self):
        links = run([{"t_ms": 50, "corner": "FR", "action": "fault", "code": 7},
                     PRESS, release(3000)], estop=True, duration_ms=3500)
        before = {m.fault for t, m in statuses(links, "FR") if 60 * MS < t < 2000 * MS}
        after = {m.fault for t, m in statuses(links, "FR") if t > 3300 * MS}
        self.assertEqual((before, after), ({7}, {0}))


if __name__ == "__main__":
    unittest.main()
