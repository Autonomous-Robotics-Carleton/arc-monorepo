"""Turns the wheel encoders' raw angle samples into what the page shows: measured speed per wheel, one point per step.

The car's wheel encoders (E-22) are absolute: each sample is a wheel's angle in [0, 2pi), not a count, and
a failed read is simply missing (a gap in that sensor's stamps). From every sample the bridge sends
(protocol.md, "/encoders/wheel"), per sensor:

- Unwrapping. Consecutive angles are joined by the shortest jump (|delta| <= pi), so a wheel turning less
  than half a revolution between reads is followed correctly; at 1 kHz that is up to 3140 rad/s.
- Measured speed (rad/s) over a step: the sum of the step's angle changes over the sum of their time
  changes. A dropped read just makes one interval longer. A gap over MAX_DT (or a stamp going backwards)
  restarts the sensor instead, as the IMU's integration does: neither speed nor drops are counted across it.
- Dropped share over the last WINDOW s of stamps: the sample period is the median spacing of consecutive
  stamps (all sensors pooled; with most reads succeeding, the median is one period), each interval counts
  round(dt / period) expected samples, and the share is (expected - received) / expected.

report() returns the step's summary and starts the next step: per sensor the measured speed, samples
received in the last second, the dropped share, and the age of the newest batch.
"""

import math
import struct
import time
from collections import deque

SAMPLE = struct.Struct('<dBxxxf')  # stamp, sensor index, padding, angle (rad)
COUNT = struct.Struct('<I')
TAU = 2 * math.pi
MAX_DT = 0.1  # s; a longer gap (or a stamp going backwards) restarts that sensor
WINDOW = 1.0  # s of stamps the dropped share and the period cover


def unpack(frame):
    """Samples in one bridge frame: [(stamp, index, angle), ...]."""
    (n,) = COUNT.unpack_from(frame)
    if len(frame) != COUNT.size + n * SAMPLE.size:
        raise ValueError('encoder frame: %d bytes for %d samples' % (len(frame), n))
    return list(SAMPLE.iter_unpack(memoryview(frame)[COUNT.size:]))


class Sensor:

    def __init__(self):
        self.stamp = self.angle = None
        self.gaps = deque()  # (stamp, dt) of each interval over the last WINDOW s of stamps
        self.arrivals = deque()  # (wall time, samples) per frame over the last second
        self.new_step()

    def new_step(self):
        self.dangle = 0.0
        self.dt = 0.0

    def sample(self, stamp, angle):
        dt = stamp - self.stamp if self.stamp is not None else None
        if dt is not None and 0 < dt <= MAX_DT:
            self.dangle += (angle - self.angle + math.pi) % TAU - math.pi  # shortest jump, in [-pi, pi)
            self.dt += dt
            self.gaps.append((stamp, dt))
        elif dt is not None:
            self.gaps.clear()  # a restart: the window starts over
        self.stamp, self.angle = stamp, angle
        while self.gaps and self.gaps[0][0] < stamp - WINDOW:
            self.gaps.popleft()


class WheelEncoders:

    def __init__(self, names):
        self.names = list(names)  # in frame index order
        self.sensors = [Sensor() for _ in self.names]
        self.last_arrival = None

    def feed(self, frame, now=None):
        now = time.monotonic() if now is None else now
        counts = [0] * len(self.sensors)
        for stamp, i, angle in unpack(frame):
            if i < len(self.sensors):
                self.sensors[i].sample(stamp, angle)
                counts[i] += 1
        for s, k in zip(self.sensors, counts):
            s.arrivals.append((now, k))
        self.last_arrival = now

    def period(self):
        """The sample period: median spacing of consecutive stamps over the window, all sensors. None before any."""
        dts = sorted(dt for s in self.sensors for _, dt in s.gaps)
        return dts[len(dts) // 2] if dts else None

    def report(self, now=None):
        """The step's summary (JSON-ready), then start the next step."""
        now = time.monotonic() if now is None else now
        period = self.period()
        speed, rate, dropped = [], [], []
        for s in self.sensors:
            while s.arrivals and s.arrivals[0][0] < now - 1.0:
                s.arrivals.popleft()
            speed.append(s.dangle / s.dt if s.dt > 0 else None)
            rate.append(sum(k for _, k in s.arrivals))  # samples received in the last second
            expected = sum(max(1, round(dt / period)) for _, dt in s.gaps) if period else 0
            dropped.append((expected - len(s.gaps)) / expected if expected else None)
            s.new_step()
        return {
            'type': 'encoders',
            'names': self.names,
            'speed': speed,
            'rate': rate,
            'dropped': dropped,
            'period': period,
            'age': None if self.last_arrival is None else now - self.last_arrival,
        }
