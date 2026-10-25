"""Scenarios: timed events for the fake nodes, as JSON.

    {"events": [
      {"t_ms": 1000, "node": "orin", "action": "command", "speed": 3.0},
      {"t_ms": 2000, "corner": "FL", "action": "silence", "duration_ms": 20}
    ]}

Times are milliseconds. An event with "node": "orin" is for the fake Orin;
any other is for motor controllers, "corner" being FL, FR, RL, RR, a list of
them, or "all" (the default)."""

import json
from dataclasses import dataclass, field
from pathlib import Path

from . import controller, orin


@dataclass(order=True)
class Event:
    t_ns: int
    nodes: tuple = field(compare=False)  # node names
    action: str = field(compare=False)
    args: dict = field(compare=False, default_factory=dict)


def load_scenario(source):
    data = source if isinstance(source, dict) else json.loads(Path(source).read_text())
    events = []
    for i, e in enumerate(data["events"]):
        e = dict(e)
        action = e.pop("action", None)
        t_ms = e.pop("t_ms")
        node = e.pop("node", "controllers")
        if node == "orin":
            table, nodes = orin.ACTIONS, ("orin",)
            e.pop("corner", None)
        elif node == "controllers":
            table = controller.ACTIONS
            corner = e.pop("corner", "all")
            nodes = (controller.CORNERS if corner == "all"
                     else tuple([corner] if isinstance(corner, str) else corner))
            for n in nodes:
                if n not in controller.CORNERS:
                    raise ValueError(f"event {i}: unknown corner {n!r}")
        else:
            raise ValueError(f"event {i}: unknown node {node!r}")
        if action not in table:
            raise ValueError(f"event {i}: unknown {node} action {action!r}")
        missing = [k for k in table[action] if k not in e]
        if missing:
            raise ValueError(f"event {i}: {action} needs {', '.join(missing)}")
        if action == "state":
            controller.parse_state(e["bits"])  # validate now
        events.append(Event(int(t_ms * 1e6), nodes, action, e))
    return sorted(events)
