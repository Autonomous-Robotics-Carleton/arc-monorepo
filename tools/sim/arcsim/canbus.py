"""The four controllers' side of the classic CAN command bus (can-command.dbc,
ADR-0009, ADR-0035): they receive SET_CURRENT, SET_CURRENT_BRAKE and SET_RPM
from the sync MCU and answer with STATUS_1 at 50 Hz.

The bus reaches the firmware as UDP datagrams (the simulation-only bridge,
firmware/sync-mcu/app/src/sim_can_bridge.c): 13 bytes, the 29-bit ID
(big-endian), the length, 8 data bytes.

Like the real controller firmware (ADR-0035), a controller that hears no
command for COMMAND_TIMEOUT_NS brakes on its own and reports COMMAND_TIMEOUT
in its UART telemetry; it clears the bit when commands return."""

import struct

from .controller import CORNERS

NS = 1_000_000_000
MS = 1_000_000
WIRE = struct.Struct(">IB8s")  # id, length, data
STATUS_PERIOD_NS = 20 * MS  # 50 Hz
COMMAND_TIMEOUT_NS = 150 * MS  # ADR-0035, TBC
COMMAND_TIMEOUT_BIT = 2  # ARC_MOTOR_STATE_COMMAND_TIMEOUT

PACKETS = {1: "current", 2: "brake", 3: "rpm", 9: "status"}  # VESC CAN_PACKET_*

ACTIONS = {}  # nothing to do from a scenario


class FakeCommandBus:
    def __init__(self, transport, controllers=None):
        self.name = "can"
        self.transport = transport
        self.controllers = controllers or {}  # corner name -> FakeController
        n = len(CORNERS)
        self.last_cmd_ns = [None] * n
        self.last_kind = [None] * n
        self.last_value = [0.0] * n
        self.timed_out = [False] * n
        self.count = [dict.fromkeys(("current", "brake", "rpm"), 0) for _ in range(n)]
        self.first_ns = [dict() for _ in range(n)]  # kind -> first time seen
        self.last_ns = [dict() for _ in range(n)]  # kind -> last time seen
        self.max_gap_ns = [0] * n  # longest time between two commands
        self.bad = 0
        self.next_status_ns = 0

    # --- Fleet node interface ---
    def next_due_ns(self):
        return self.next_status_ns

    def apply(self, action, **args):
        raise ValueError(f"unknown action {action!r}")

    def poll_rx(self, now_ns=0):
        data = self.transport.read()
        for i in range(0, len(data) - WIRE.size + 1, WIRE.size):
            self._frame(*WIRE.unpack_from(data, i), now_ns)
        if len(data) % WIRE.size:
            self.bad += 1

    def tick(self, now_ns):
        for corner, t in enumerate(self.last_cmd_ns):
            if t is not None and not self.timed_out[corner] and now_ns - t > COMMAND_TIMEOUT_NS:
                self.timed_out[corner] = True
                self._controller(corner, timed_out=True, brake=True)
        while self.next_status_ns <= now_ns:
            t = self.next_status_ns
            self.next_status_ns += STATUS_PERIOD_NS
            for corner in range(len(CORNERS)):
                self._status(corner, t)

    # --- the controllers ---
    def _controller(self, corner, *, timed_out=None, brake=False, rpm=None):
        c = self.controllers.get(CORNERS[corner])
        if c is None:
            return
        if timed_out is not None:
            c.auto_state = COMMAND_TIMEOUT_BIT if timed_out else 0
        if brake:
            c.target_erpm = 0.0
        if rpm is not None:
            c.target_erpm = rpm

    def _frame(self, can_id, length, data, now_ns):
        packet, ctrl = (can_id >> 8) & 0xFF, can_id & 0xFF
        kind = PACKETS.get(packet)
        corner = ctrl - 1
        if kind not in ("current", "brake", "rpm") or not 0 <= corner < len(CORNERS) or length != 4:
            self.bad += 1
            return
        (raw,) = struct.unpack(">i", data[:4])
        value = raw / 1000.0 if kind != "rpm" else float(raw)  # A, A, eRPM
        if self.last_cmd_ns[corner] is not None:
            self.max_gap_ns[corner] = max(self.max_gap_ns[corner], now_ns - self.last_cmd_ns[corner])
        self.last_cmd_ns[corner] = now_ns
        self.last_kind[corner] = kind
        self.last_value[corner] = value
        self.count[corner][kind] += 1
        self.first_ns[corner].setdefault(kind, now_ns)
        self.last_ns[corner][kind] = now_ns
        if self.timed_out[corner]:
            self.timed_out[corner] = False
            self._controller(corner, timed_out=False)
        if kind == "brake":
            self._controller(corner, brake=True)
        elif kind == "rpm":
            self._controller(corner, rpm=value)

    def _status(self, corner, t_ns):
        c = self.controllers.get(CORNERS[corner])
        erpm = int(c.erpm) if c else 0
        duty = int(max(-1.0, min(1.0, c.erpm / 1e5)) * 1000) if c else 0
        data = struct.pack(">ihh", erpm, 0, duty)  # eRPM, current x10, duty x1000
        self.transport.write(WIRE.pack((9 << 8) | (corner + 1), 8, data), t_ns)

    def report(self):
        lines = []
        for corner, name in enumerate(CORNERS):
            cnt = self.count[corner]
            first = self.first_ns[corner].get("brake")
            lines.append(f"bus {name}: current={cnt['current']} rpm={cnt['rpm']} brake={cnt['brake']}"
                         f" first_brake_ms={'-' if first is None else f'{first / MS:.1f}'}"
                         f" max_gap_ms={self.max_gap_ns[corner] / MS:.1f}")
        return lines
