"""Tests for the fake motor controllers. Run from the repo root:
python3 -m unittest discover -s tools/sim/tests -t tools/sim"""

import tempfile
import unittest
from pathlib import Path

import sim
from arcsim import controller, scenario
from arcsim.fleet import build
from arcsim.transport import MemoryTransport

from .helpers import MS, VERSION, decode, dialect, of_type, run

CORNERS = controller.CORNERS


class Rates(unittest.TestCase):
    def test_status_1khz_link_status_1hz(self):
        frames = decode(run(duration_ms=3000)["FL"])
        self.assertAlmostEqual(len(of_type(frames, "ARC_MOTOR_STATUS")), 3000, delta=2)
        self.assertEqual(len(of_type(frames, "ARC_LINK_STATUS")), 3)

    def test_frame_fits_the_icd_size(self):
        # ICD-controller-telemetry: 59 bytes on the wire, 47-byte payload.
        # MAVLink 2 drops trailing zero bytes, so a frame can be shorter.
        parser = dialect.MAVLink(None)
        sizes = {len(d) for _, d in run(duration_ms=200)["FR"].writes
                 if parser.parse_buffer(d)[0].get_type() == "ARC_MOTOR_STATUS"}
        self.assertLessEqual(max(sizes), 59)
        self.assertGreaterEqual(min(sizes), 59 - 3)

    def test_corner_field_and_clean_state(self):
        links = run(duration_ms=50)
        for i, name in enumerate(CORNERS):
            msgs = [m for _, m in of_type(decode(links[name]), "ARC_MOTOR_STATUS")]
            self.assertTrue(all(m.corner == i and m.state == 0 and m.fault == 0 for m in msgs))

    def test_controllers_are_not_in_phase(self):
        first = {n: t.writes[0][0] for n, t in run(duration_ms=20).items()}
        self.assertGreater(len(set(first.values())), 1)


class Silence(unittest.TestCase):
    def test_silent_corner_stops_sending_and_others_do_not(self):
        links = run([{"t_ms": 100, "corner": "FL", "action": "silence", "duration_ms": 20}])
        fl = [t for t, m in of_type(decode(links["FL"]), "ARC_MOTOR_STATUS")]
        gaps = [b - a for a, b in zip(fl, fl[1:])]
        self.assertAlmostEqual(max(gaps), 21 * MS, delta=2 * MS)
        for other in ("FR", "RL", "RR"):
            t = [t for t, m in of_type(decode(links[other]), "ARC_MOTOR_STATUS")]
            self.assertLessEqual(max(b - a for a, b in zip(t, t[1:])), MS)

    def test_a_hung_controller_leaves_no_sequence_gap(self):
        # silence is a controller that stopped, not a lossy link
        frames = decode(run([{"t_ms": 100, "corner": "FL", "action": "silence",
                              "duration_ms": 20}])["FL"])
        seqs = [m.get_header().seq for _, m in frames]
        self.assertTrue(all((b - a) & 0xFF == 1 for a, b in zip(seqs, seqs[1:])))

    def test_stop_for_good_and_resume(self):
        frames = decode(run([{"t_ms": 100, "corner": "RR", "action": "silence"},
                             {"t_ms": 300, "corner": "RR", "action": "resume"}])["RR"])
        t = [t for t, m in of_type(frames, "ARC_MOTOR_STATUS")]
        self.assertFalse([x for x in t if 100 * MS <= x < 300 * MS])
        self.assertTrue([x for x in t if x >= 300 * MS])


class LinkFaults(unittest.TestCase):
    def test_dropped_frames_show_as_a_sequence_gap(self):
        frames = decode(run([{"t_ms": 100, "corner": "FL", "action": "drop", "count": 3}])["FL"])
        seqs = [m.get_header().seq for _, m in frames]
        jumps = [(b - a) & 0xFF for a, b in zip(seqs, seqs[1:]) if (b - a) & 0xFF != 1]
        self.assertEqual(jumps, [4])  # 3 missing frames

    def test_corrupted_frame_fails_the_crc_and_the_next_is_fine(self):
        frames = decode(run([{"t_ms": 100, "corner": "FL", "action": "corrupt", "count": 1}],
                            duration_ms=200)["FL"])
        self.assertEqual(len([m for _, m in frames if m.get_type() == "BAD_DATA"]), 1)
        self.assertIn(frames[-1][1].get_type(), ("ARC_MOTOR_STATUS", "ARC_LINK_STATUS"))

    def test_version_mismatch_is_in_link_status(self):
        links = run([{"t_ms": 0, "corner": "FR", "action": "version", "value": VERSION + 1}],
                    duration_ms=2500)
        ok = {m.protocol_version for _, m in of_type(decode(links["FL"]), "ARC_LINK_STATUS")}
        wrong = {m.protocol_version for _, m in of_type(decode(links["FR"]), "ARC_LINK_STATUS")}
        self.assertEqual(ok, {VERSION})
        self.assertEqual(wrong, {VERSION + 1})


class CornerState(unittest.TestCase):
    def test_estop_and_command_timeout_bits(self):
        links = run([{"t_ms": 100, "corner": "RL", "action": "state", "bits": ["ESTOP"]},
                     {"t_ms": 200, "corner": "RL", "action": "state",
                      "bits": ["ESTOP", "COMMAND_TIMEOUT"]}])
        got = [(t, m.state) for t, m in of_type(decode(links["RL"]), "ARC_MOTOR_STATUS")]
        self.assertEqual({s for t, s in got if t < 100 * MS}, {0})
        self.assertEqual({s for t, s in got if 100 * MS <= t < 200 * MS}, {1})
        self.assertEqual({s for t, s in got if t >= 200 * MS}, {3})

    def test_fault_code(self):
        links = run([{"t_ms": 100, "corner": "FL", "action": "fault", "code": 7}])
        got = [(t, m.fault) for t, m in of_type(decode(links["FL"]), "ARC_MOTOR_STATUS")]
        self.assertEqual({f for t, f in got if t >= 100 * MS}, {7})
        self.assertEqual({f for t, f in got if t < 100 * MS}, {0})


class Model(unittest.TestCase):
    def test_speed_target_gives_the_adr_erpm(self):
        # ADR-0034: 9 m/s is ~75k eRPM
        links = run([{"t_ms": 0, "action": "speed", "mps": 9.0}], duration_ms=1500)
        m = of_type(decode(links["FL"]), "ARC_MOTOR_STATUS")[-1][1]
        self.assertAlmostEqual(m.erpm, 75000, delta=1500)
        self.assertGreater(m.duty, 0.4)
        self.assertLess(m.duty, 0.7)

    def test_controller_clock_is_offset_and_drifts(self):
        frames = of_type(decode(run(duration_ms=2000)["FL"]), "ARC_MOTOR_STATUS")
        (t0, a), (t1, b) = frames[0], frames[-1]
        self.assertGreater(a.time_ns, 10**9)  # not the sync MCU's time
        self.assertGreater(b.time_ns, a.time_ns)
        self.assertNotEqual(b.time_ns - a.time_ns, t1 - t0)  # drift

    def test_same_seed_same_bytes(self):
        a = run(duration_ms=300, seed=5)["FL"].stream
        b = run(duration_ms=300, seed=5)["FL"].stream
        c = run(duration_ms=300, seed=6)["FL"].stream
        self.assertEqual(a, b)
        self.assertNotEqual(a, c)


class Receiving(unittest.TestCase):
    def test_sees_the_sync_mcus_link_status_and_counts_a_gap(self):
        t = MemoryTransport()
        ctrl = build({"FL": t}, seed=1).nodes["FL"]
        mcu = dialect.MAVLink(None, srcSystem=1, srcComponent=1)
        frames = []
        for i in range(3):
            msg = dialect.MAVLink_arc_link_status_message(time_ns=i, protocol_version=VERSION,
                                                          rx_gaps=0, rx_bad=0)
            frames.append(bytes(msg.pack(mcu)))
            mcu.seq += 1
        t.inbox = frames[0] + frames[2]  # the middle one is lost
        ctrl.poll_rx(0)
        self.assertEqual(ctrl.peer_version, VERSION)
        self.assertEqual(ctrl.rx_gaps, 1)


class Scenarios(unittest.TestCase):
    def test_rejects_bad_scenarios(self):
        for bad in ({"t_ms": 0, "action": "explode"},
                    {"t_ms": 0, "action": "drop"},
                    {"t_ms": 0, "corner": "XX", "action": "silence"},
                    {"t_ms": 0, "action": "state", "bits": ["NOPE"]},
                    {"t_ms": 0, "node": "orin", "action": "silence"},
                    {"t_ms": 0, "node": "moon", "action": "silence"}):
            with self.assertRaises((ValueError, KeyError)):
                scenario.load_scenario({"events": [bad]})

    def test_example_scenarios_load(self):
        for f in (Path(sim.__file__).parent / "scenarios").glob("*.json"):
            self.assertTrue(scenario.load_scenario(f), f)

    def test_capture_files(self):
        with tempfile.TemporaryDirectory() as d:
            sim.main(["--capture", d, "--duration", "0.2"])
            sizes = [(Path(d) / f"{n}.bin").stat().st_size for n in CORNERS]
            self.assertTrue(all(s > 200 * 50 for s in sizes))


if __name__ == "__main__":
    unittest.main()
