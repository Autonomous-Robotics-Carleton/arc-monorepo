"""The base of every fake node: a MAVLink sender and a receiver on a transport."""

import collections
import random

NS = 1_000_000_000


class MavNode:
    """Subclasses set their own schedule (next_due_ns, tick) and actions
    (apply). Time passed in is true simulation time in ns."""

    def __init__(self, dialect, transport, *, name, system_id, component_id, seed=0):
        self.m = dialect
        self.name = name
        self.transport = transport
        self.rng = random.Random(f"{seed}/{name}")
        self.mav = dialect.MAVLink(None, srcSystem=system_id, srcComponent=component_id)
        self.parser = dialect.MAVLink(None)
        self.parser.robust_parsing = True
        self.to_corrupt = 0
        # what the firmware sent us
        self.rx = collections.defaultdict(lambda: collections.deque(maxlen=1000))
        self.rx_count = collections.Counter()
        self.rx_gaps = 0
        self.rx_bad = 0
        self._rx_seq = None

    def _pack(self, msg):
        buf = bytearray(msg.pack(self.mav))
        self.mav.seq = (self.mav.seq + 1) & 0xFF
        return buf

    def _emit(self, buf, t_ns):
        if self.to_corrupt:
            self.to_corrupt -= 1
            buf[len(buf) // 2] ^= 0x5A  # breaks the CRC; the length stays valid
        self.transport.write(bytes(buf), t_ns)

    def on_message(self, msg, now_ns):
        """Hook: a valid message from the firmware."""

    def poll_rx(self, now_ns=0):
        data = self.transport.read()
        for msg in self.parser.parse_buffer(data) or []:
            if msg.get_type() == "BAD_DATA":
                self.rx_bad += 1
                continue
            seq = msg.get_header().seq
            if self._rx_seq is not None:
                self.rx_gaps += (seq - self._rx_seq - 1) & 0xFF
            self._rx_seq = seq
            name = msg.get_type()
            self.rx[name].append((now_ns, msg))
            self.rx_count[name] += 1
            self.on_message(msg, now_ns)

    def next_due_ns(self):
        raise NotImplementedError

    def tick(self, now_ns):
        raise NotImplementedError

    def apply(self, action, **args):
        raise NotImplementedError
