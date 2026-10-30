"""Runs the fake nodes together on one clock."""

import time

from .controller import CORNERS, FakeController
from .canbus import FakeCommandBus
from .dialect import SCHEMA, load_dialect
from .estop import FakeEstop
from .orin import FakeOrin
from .transport import CaptureTransport, open_transport


class Fleet:
    def __init__(self, nodes, events=()):
        self.nodes = nodes  # name -> node: "FL".."RR", "orin", "can", "estop"
        self.events = list(events)

    def _apply_due(self, now_ns):
        while self.events and self.events[0].t_ns <= now_ns:
            ev = self.events.pop(0)
            for name in ev.nodes:
                if name in self.nodes:
                    self.nodes[name].apply(ev.action, _now_ns=ev.t_ns, **ev.args)

    def run(self, duration_ns, realtime=False):
        """Virtual time jumps from due time to due time (fast, deterministic);
        real time sleeps until each is due."""
        start = time.monotonic_ns()
        while True:
            due = min([n.next_due_ns() for n in self.nodes.values()]
                      + ([self.events[0].t_ns] if self.events else []))
            if due > duration_ns:
                break
            if realtime:
                wait = (start + due - time.monotonic_ns()) / 1e9
                if wait > 0:
                    time.sleep(wait)
            self._apply_due(due)
            for n in self.nodes.values():
                n.tick(due)
                n.poll_rx(due)
        for n in self.nodes.values():
            n.transport.close()


def build(links=None, *, orin=None, can=None, estop=None, seed=0, captures=None, events=(),
          schema=SCHEMA, estop_options=None, **orin_options):
    """links: corner name -> transport or spec; orin: transport or spec;
    captures: corner name -> file (each corner's bytes, for golden frames);
    can: the command bus, a transport or spec; estop: the e-stop input, a
    transport or spec."""
    dialect, version = load_dialect(schema)

    def transport(spec):
        return spec if hasattr(spec, "write") else open_transport(spec)

    nodes = {}
    for name, spec in (links or {}).items():
        nodes[name] = FakeController(dialect, version, CORNERS.index(name),
                                     transport(spec), seed=seed)
    for name, path in (captures or {}).items():
        nodes[name] = FakeController(dialect, version, CORNERS.index(name),
                                     CaptureTransport(path), seed=seed)
    if orin is not None:
        nodes["orin"] = FakeOrin(dialect, version, transport(orin), seed=seed,
                                 **orin_options)
    controllers = {n: v for n, v in nodes.items() if n in CORNERS}
    if can is not None:
        nodes["can"] = FakeCommandBus(transport(can), controllers=controllers)
    if estop is not None:
        nodes["estop"] = FakeEstop(transport(estop), controllers=controllers,
                                   **(estop_options or {}))
    return Fleet(nodes, events)
