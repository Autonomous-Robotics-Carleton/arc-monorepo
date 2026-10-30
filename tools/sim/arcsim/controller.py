"""A fake A50S motor controller on its UART (ICD-controller-telemetry, ADR-0034):
ARC_MOTOR_STATUS at 1 kHz and ARC_LINK_STATUS at 1 Hz."""

import math

from .node import NS, MavNode

CORNERS = ("FL", "FR", "RL", "RR")  # ARC_CORNER 0..3
STATUS_PERIOD_NS = 1_000_000  # 1 kHz
LINK_PERIOD_NS = NS  # 1 Hz
STATE_BITS = {"ESTOP": 1, "COMMAND_TIMEOUT": 2}  # ARC_MOTOR_STATE

# Car constants from ADR-0034 / ADR-0026: 62 mm wheels, 13.5:1, 4-pole motor
# (2 pole pairs), Castle 1010 4400 kV.
WHEEL_CIRCUMFERENCE_M = math.pi * 0.062
GEAR_RATIO = 13.5
POLE_PAIRS = 2
MOTOR_KV = 4400.0

ACTIONS = {  # action: required arguments
    "speed": ("mps",), "silence": (), "resume": (), "drop": ("count",),
    "corrupt": ("count",), "state": ("bits",), "fault": ("code",),
    "version": ("value",),
}


def parse_state(bits):
    if isinstance(bits, int):
        return bits
    value = 0
    for name in ([bits] if isinstance(bits, str) else bits):
        value |= STATE_BITS[name.upper()]
    return value


class FakeController(MavNode):
    """One A50S. Frames carry the controller's own clock, which has an offset
    and a drift against true time (the sync MCU has to align them: ICD open
    issue 1)."""

    def __init__(self, dialect, schema_version, corner, transport, *, seed=0,
                 system_id=1, component_id=None, clock_offset_ns=None,
                 drift_ppm=None):
        # MAVLink IDs on this link are TBD in the ICD; component = corner + 1
        # is this tool's assumption
        super().__init__(dialect, transport, name=CORNERS[corner], system_id=system_id,
                         component_id=component_id or corner + 1, seed=seed)
        self.corner = corner
        self.version = schema_version
        self.clock_offset_ns = (clock_offset_ns if clock_offset_ns is not None
                                else self.rng.randrange(10**9, 10**13))
        self.drift_ppm = (drift_ppm if drift_ppm is not None
                          else self.rng.uniform(-50, 50))
        # controllers aren't synchronised with each other
        self.next_status_ns = self.rng.randrange(STATUS_PERIOD_NS)
        self.next_link_ns = self.next_status_ns
        self.silent_until_ns = 0
        self.silent_forever = False
        self.to_drop = 0
        self.state = 0
        self.auto_state = 0  # set by the command bus: COMMAND_TIMEOUT
        self.estop_line = False  # ESTOP_EN active (estop.py): ignores the bus, brakes
        self.powered = True  # the motor bus; cut at T after an e-stop press
        self.fault = 0
        self.target_erpm = 0.0
        self.erpm = 0.0
        self.tachometer = 0
        self.temp_fet = 28.0
        self.temp_motor = 26.0
        self.last_ns = self.next_status_ns
        self.peer_version = None
        self.sent_status = 0

    def controller_time_ns(self, t_ns):
        return self.clock_offset_ns + int(t_ns * (1 + self.drift_ppm * 1e-6))

    def on_message(self, msg, now_ns):
        if msg.get_type() == "ARC_LINK_STATUS":
            self.peer_version = msg.protocol_version

    def apply(self, action, **a):
        if action == "speed":
            self.target_erpm = (a["mps"] / WHEEL_CIRCUMFERENCE_M * 60
                                * GEAR_RATIO * POLE_PAIRS)
        elif action == "silence":
            if "duration_ms" in a:
                self.silent_until_ns = a["_now_ns"] + int(a["duration_ms"] * 1e6)
            else:
                self.silent_forever = True
        elif action == "resume":
            self.silent_until_ns = 0
            self.silent_forever = False
        elif action == "drop":
            self.to_drop += int(a["count"])
        elif action == "corrupt":
            self.to_corrupt += int(a["count"])
        elif action == "state":
            self.state = parse_state(a["bits"])
        elif action == "fault":
            self.fault = int(a["code"])
        elif action == "version":
            self.version = int(a["value"])
        else:
            raise ValueError(f"unknown action {action!r}")

    def silent(self, t_ns):
        return not self.powered or self.silent_forever or t_ns < self.silent_until_ns

    # --- the e-stop line and the motor bus (estop.py) ---
    def estop(self, active):
        self.estop_line = active
        if active:
            self.target_erpm = 0.0  # brakes on the e-stop ramp (ADR-0012)

    def power_off(self):
        self.powered = False

    def power_on(self, t_ns, boot_ns):
        """The controller boots: silent for boot_ns, then fresh: stopped, no
        fault, no command timeout."""
        self.powered = True
        self.silent_until_ns = t_ns + boot_ns
        self.erpm = self.target_erpm = 0.0
        self.fault = 0
        self.auto_state = 0

    # the motor: first-order lag to the target speed, plausible currents
    def _step_motor(self, dt_s):
        tau = 0.05
        new = self.erpm + (self.target_erpm - self.erpm) * min(1.0, dt_s / tau)
        accel = (new - self.erpm) / dt_s if dt_s > 0 else 0.0
        self.erpm = new
        self.tachometer += int(self.erpm / 60 * dt_s * 6 * POLE_PAIRS)
        iq = 0.4 + accel / 20000.0 + self.rng.gauss(0, 0.05)
        vin = 16.0 - 0.02 * abs(iq)
        duty = max(-1.0, min(1.0, (self.erpm / POLE_PAIRS) / (MOTOR_KV * vin)))
        iin = duty * iq
        self.temp_fet += (abs(iq) ** 2 * 0.0005 - (self.temp_fet - 28) * 0.00005) * dt_s * 1000
        self.temp_motor += (abs(iq) ** 2 * 0.0003 - (self.temp_motor - 26) * 0.00003) * dt_s * 1000
        return iq, iin, duty, vin

    def _status(self, t_ns):
        dt = max(0, t_ns - self.last_ns) / 1e9
        self.last_ns = t_ns
        iq, iin, duty, vin = self._step_motor(dt)
        msg = self.m.MAVLink_arc_motor_status_message(
            time_ns=self.controller_time_ns(t_ns), erpm=int(self.erpm),
            tachometer=self.tachometer, current_q=iq, current_d=0.0,
            current_in=iin, duty=duty, voltage_in=vin, temp_fet=self.temp_fet,
            temp_motor=self.temp_motor, corner=self.corner, fault=self.fault,
            state=self.state | self.auto_state | (STATE_BITS["ESTOP"] if self.estop_line else 0))
        buf = self._pack(msg)
        if self.to_drop:  # sequence number spent, frame never sent: a gap
            self.to_drop -= 1
            return
        self._emit(buf, t_ns)
        self.sent_status += 1

    def _link_status(self, t_ns):
        msg = self.m.MAVLink_arc_link_status_message(
            time_ns=self.controller_time_ns(t_ns), protocol_version=self.version,
            rx_gaps=self.rx_gaps, rx_bad=self.rx_bad)
        self._emit(self._pack(msg), t_ns)

    def next_due_ns(self):
        return min(self.next_status_ns, self.next_link_ns)

    def tick(self, now_ns):
        """Send whatever is due at or before now_ns."""
        while self.next_status_ns <= now_ns or self.next_link_ns <= now_ns:
            if self.next_status_ns <= self.next_link_ns:
                t = self.next_status_ns
                self.next_status_ns += STATUS_PERIOD_NS
                if self.silent(t):
                    self.last_ns = t
                else:
                    self._status(t)
            else:
                t = self.next_link_ns
                self.next_link_ns += LINK_PERIOD_NS
                if not self.silent(t):
                    self._link_status(t)
