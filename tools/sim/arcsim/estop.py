"""The e-stop button on the car (E-32) and what it does to the drive side.

Pressed: the sync MCU's e-stop status input goes active (one byte over UDP to
the firmware's simulation-only bridge, sim_estop.c), and every motor
controller sees ESTOP: it reports the ESTOP state bit, ignores the command
bus and brakes (ADR-0012). At T the motor bus is cut and the controllers lose
power (ADR-0034), unless their Aux input is on an always-on rail (an open
question in handoff/electrical.md). Released: the controllers boot again.

The reset that this drives is the firmware's (fault_latch.c, ADR-0039)."""

from .node import NS

ACTIONS = {"press": (), "release": ()}  # action: required arguments

MS = NS // 1000
NEVER = 1 << 62


class FakeEstop:
    def __init__(self, transport, controllers=None, *, cut_ms=2400, restart_ms=200,
                 aux_always_on=False):
        self.name = "estop"
        self.transport = transport
        self.controllers = controllers or {}  # corner name -> FakeController
        self.cut_ns = cut_ms * MS  # T (ADR-0034, TBC)
        self.restart_ns = restart_ms * MS  # the controllers booting: this tool's guess
        self.aux_always_on = aux_always_on
        self.pressed = False
        self.cut_at = None  # when the motor bus will be cut
        self.cut = False  # the motor bus is cut

    def next_due_ns(self):
        return self.cut_at if self.cut_at is not None else NEVER

    def apply(self, action, **a):
        now = a["_now_ns"]
        if action == "press":
            if self.pressed:
                return
            self.pressed = True
            self.transport.write(b"\x01", now)
            for c in self.controllers.values():
                c.estop(True)
            if not self.aux_always_on:
                self.cut_at = now + self.cut_ns
        elif action == "release":
            if not self.pressed:
                return
            self.pressed = False
            self.cut_at = None
            self.transport.write(b"\x00", now)
            for c in self.controllers.values():
                c.estop(False)
                if self.cut:
                    c.power_on(now, self.restart_ns)
            self.cut = False
        else:
            raise ValueError(f"unknown action {action!r}")

    def tick(self, now_ns):
        if self.cut_at is not None and now_ns >= self.cut_at:
            self.cut_at = None
            self.cut = True
            for c in self.controllers.values():
                c.power_off()

    def poll_rx(self, now_ns=0):
        self.transport.read()  # nothing comes back
