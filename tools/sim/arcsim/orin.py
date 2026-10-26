"""A fake Orin on the sync link (ICD-sync-link): the operator heartbeat, the
operator's re-arm, drive commands, envelope settings and link status, UDP,
MAVLink 2.

Rates are the ICD's: ARC_DRIVE_COMMAND every 2 ms (500 Hz, the command bus
rate) carrying the latest controller output, which updates at the control
rate (SYS-08: >= 100 Hz); between updates the command repeats with its
original time_ns. The operator heartbeat is 50 to 100 Hz."""

from .node import NS, MavNode

LINK_PERIOD_NS = NS  # ARC_LINK_STATUS, 1 Hz

ACTIONS = {  # action: required arguments
    "heartbeat_stop": (), "heartbeat_resume": (), "rearm": (),
    "command": (), "command_stop": (), "command_resume": (),
    "envelope": ("max_speed", "max_accel", "max_steer"),
    "drop": ("count",), "corrupt": ("count",), "version": ("value",),
}


class FakeOrin(MavNode):
    def __init__(self, dialect, schema_version, transport, *, seed=0,
                 heartbeat_hz=50, command_hz=500, controller_hz=100):
        # MAVLink IDs: system 1, component 2 the Orin bridge (ICD, TBC)
        super().__init__(dialect, transport, name="orin", system_id=1,
                         component_id=2, seed=seed)
        self.version = schema_version
        self.heartbeat_period = NS // heartbeat_hz
        self.command_period = NS // command_hz
        self.controller_period = NS // controller_hz
        self.next_heartbeat_ns = 0
        self.next_command_ns = 0
        self.next_controller_ns = 0
        self.next_link_ns = 0
        self.heartbeat_on = True
        self.command_on = True
        self.counter = 0
        self.target = (0.0, 0.0, 0.0)  # speed, accel, steer
        self.command_time_ns = 0
        self.pending_envelope = None
        self.pending_rearm = False
        self.rearms = 0
        self.to_drop = 0
        # sync-MCU time = ours + offset, learned from what the MCU sends; the
        # Orin's clock isn't synced yet (ICD), so until then it is our own
        self.mcu_offset_ns = 0

    def mcu_time_ns(self, t_ns):
        return t_ns + self.mcu_offset_ns

    def on_message(self, msg, now_ns):
        t = getattr(msg, "time_ns", None)
        if t:
            self.mcu_offset_ns = t - now_ns

    def apply(self, action, **a):
        if action == "heartbeat_stop":
            self.heartbeat_on = False
        elif action == "heartbeat_resume":
            self.heartbeat_on = True
        elif action == "rearm":
            self.pending_rearm = True
        elif action == "command":
            self.target = (float(a.get("speed", self.target[0])),
                           float(a.get("accel", self.target[1])),
                           float(a.get("steer", self.target[2])))
        elif action == "command_stop":
            self.command_on = False
        elif action == "command_resume":
            self.command_on = True
        elif action == "envelope":
            self.pending_envelope = (float(a["max_speed"]), float(a["max_accel"]),
                                     float(a["max_steer"]))
        elif action == "drop":
            self.to_drop += int(a["count"])
        elif action == "corrupt":
            self.to_corrupt += int(a["count"])
        elif action == "version":
            self.version = int(a["value"])
        else:
            raise ValueError(f"unknown action {action!r}")

    def _send(self, msg, t_ns):
        buf = self._pack(msg)
        if self.to_drop:  # sequence number spent, datagram never sent
            self.to_drop -= 1
            return
        self._emit(buf, t_ns)

    def next_due_ns(self):
        return min(self.next_heartbeat_ns, self.next_command_ns,
                   self.next_controller_ns, self.next_link_ns)

    def tick(self, now_ns):
        while self.next_due_ns() <= now_ns:
            t = self.next_due_ns()
            m = self.m
            if self.next_controller_ns == t:
                # the control loop produced a new output: stamp it
                self.next_controller_ns += self.controller_period
                self.command_time_ns = self.mcu_time_ns(t)
            elif self.next_link_ns == t:
                self.next_link_ns += LINK_PERIOD_NS
                self._send(m.MAVLink_arc_link_status_message(
                    time_ns=self.mcu_time_ns(t), protocol_version=self.version,
                    rx_gaps=self.rx_gaps, rx_bad=self.rx_bad), t)
            elif self.next_heartbeat_ns == t:
                self.next_heartbeat_ns += self.heartbeat_period
                if self.heartbeat_on:
                    self.counter += 1
                    self._send(m.MAVLink_arc_operator_heartbeat_message(
                        time_ns=self.mcu_time_ns(t), counter=self.counter), t)
            else:
                self.next_command_ns += self.command_period
                if self.pending_rearm:
                    self.pending_rearm = False
                    self.rearms += 1
                    self._send(m.MAVLink_arc_rearm_message(
                        time_ns=self.mcu_time_ns(t), counter=self.rearms), t)
                if self.pending_envelope:
                    s, a, st = self.pending_envelope
                    self.pending_envelope = None
                    self._send(m.MAVLink_arc_envelope_set_message(
                        time_ns=self.mcu_time_ns(t), max_speed=s, max_accel=a,
                        max_steer=st), t)
                if self.command_on:
                    speed, accel, steer = self.target
                    self._send(m.MAVLink_arc_drive_command_message(
                        time_ns=self.command_time_ns, speed=speed, accel=accel,
                        steer=steer), t)
