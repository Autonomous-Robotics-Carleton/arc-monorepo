#!/usr/bin/env python3
"""Fake nodes around the sync MCU firmware.

    sim.py --link FL=/dev/pts/3 --link FR=tcp:127.0.0.1:5001 ... \\
           --orin udp:52001:10.73.1.10:52000 --scenario scenarios/one-corner-silent.json

Needs pymavlink (it is in the dev container). Not a safety function; the
motor model is plausible, not calibrated. See README.md."""

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from arcsim.controller import CORNERS  # noqa: E402
from arcsim.fleet import build  # noqa: E402
from arcsim.scenario import load_scenario  # noqa: E402


def main(argv=None):
    p = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    p.add_argument("--link", action="append", default=[], metavar="CORNER=TARGET",
                   help="UART of a corner: a device or pseudo-terminal path, or tcp:HOST:PORT")
    p.add_argument("--orin", metavar="TARGET",
                   help="the sync link: udp:LOCAL_PORT:PEER_HOST:PEER_PORT (ICD: udp:52001:<sync MCU>:52000)")
    p.add_argument("--can", metavar="TARGET",
                   help="the command bus (needs the firmware's simulator bridge): "
                        "udp:52101:127.0.0.1:52100")
    p.add_argument("--estop", metavar="TARGET",
                   help="the e-stop input (needs the firmware's simulator bridge): "
                        "udp:52103:127.0.0.1:52102")
    p.add_argument("--capture", metavar="DIR",
                   help="write each corner's byte stream to DIR/<corner>.bin (virtual time)")
    p.add_argument("--scenario", help="JSON file of events")
    p.add_argument("--duration", type=float, default=10.0, help="seconds (default 10)")
    p.add_argument("--seed", type=int, default=0)
    p.add_argument("--speed", type=float, default=0.0, help="initial motor target speed in m/s")
    args = p.parse_args(argv)

    links = {}
    for item in args.link:
        name, _, target = item.partition("=")
        if name.upper() not in CORNERS or not target:
            p.error(f"--link wants FL|FR|RL|RR=TARGET, got {item!r}")
        links[name.upper()] = target
    captures = None
    if args.capture:
        Path(args.capture).mkdir(parents=True, exist_ok=True)
        captures = {n: str(Path(args.capture) / f"{n}.bin") for n in CORNERS}
    if not (links or captures or args.orin or args.can):
        p.error("give --link for each corner, --orin, --can, or --capture DIR")
    if links and captures:
        p.error("--link and --capture can't be combined")
    events = load_scenario(args.scenario) if args.scenario else []
    fleet = build(links, orin=args.orin, can=args.can, estop=args.estop, seed=args.seed,
                  captures=captures, events=events)
    if args.speed:
        for name, n in fleet.nodes.items():
            if name in CORNERS:
                n.apply("speed", mps=args.speed)
    fleet.run(int(args.duration * 1e9), realtime=not captures)
    bus = fleet.nodes.get("can")
    if bus:
        print("\n".join(bus.report()))
    orin = fleet.nodes.get("orin")
    if orin:
        states = orin.rx.get("ARC_SAFETY_STATE")
        last = states[-1][1] if states else None
        print(f"orin received: {dict(orin.rx_count)}")
        if last:
            print(f"orin last ARC_SAFETY_STATE: watchdog={last.watchdog} estop={last.estop} "
                  f"max_speed={last.max_speed:g} max_accel={last.max_accel:g} "
                  f"max_steer={last.max_steer:g}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
