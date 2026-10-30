"""Shared by the tests."""

from arcsim import controller, scenario
from arcsim.dialect import load_dialect
from arcsim.fleet import build
from arcsim.transport import MemoryTransport

MS = 1_000_000
dialect, VERSION = load_dialect()


def run(events=(), *, duration_ms=1000, seed=1, corners=controller.CORNERS, orin=False,
        estop=False, estop_options=None):
    """Virtual-time run; returns {node name: MemoryTransport}."""
    links = {n: MemoryTransport() for n in corners}
    if orin:
        links["orin"] = MemoryTransport()
    if estop:
        links["estop"] = MemoryTransport()
    fleet = build({n: t for n, t in links.items() if n in controller.CORNERS}, seed=seed,
                  orin=links.get("orin"), estop=links.get("estop"), estop_options=estop_options,
                  events=scenario.load_scenario({"events": list(events)}))
    fleet.run(duration_ms * MS)
    return links


def decode(transport):
    """[(true time ns, message)] for every frame written, CRC-checked."""
    parser = dialect.MAVLink(None)
    parser.robust_parsing = True
    out = []
    for t, data in transport.writes:
        out += [(t, m) for m in (parser.parse_buffer(data) or [])]
    return out


def of_type(frames, name):
    return [(t, m) for t, m in frames if m.get_type() == name]
