"""Turns one IMU's raw samples into what the page shows: attitude, derived values, one point per step.

The car's IMU is 6-axis (gyro + accelerometer, no orientation), so attitude is estimated here, on the
ground station, from every sample the bridge sends (protocol.md, "/imu/<name>"):

- Complementary filter. The gyro, integrated, gives roll, pitch and yaw: smooth, but it drifts. Gravity
  in the accelerometer gives absolute roll and pitch: noisy, and wrong while the car accelerates. Each
  sample nudges roll and pitch toward the accelerometer's, with time constant TAU, and only while the
  measured acceleration is close to 1 g (so cornering and braking don't tilt the estimate).
- Yaw has no absolute reference, so it is relative: it starts at zero and drifts with the gyro's bias.
  reset_yaw() zeroes it.

report() returns the step's summary and starts the next step: means of gyro and accelerometer, peak
|a| in g, whether any sample reached the part's range, the gyro bias learned while stopped, the received
sample rate and the age of the newest sample.
"""

import math
import struct
import time
from collections import deque

SAMPLE = struct.Struct('<dffffff')  # stamp, gyro x y z (rad/s), accel x y z (m/s^2)
COUNT = struct.Struct('<I')
G = 9.80665
TAU = 1.0  # s, complementary filter time constant (TBC)
TILT_GATE = 0.1  # trust the accelerometer's tilt only while | |a| - g | < this fraction of g
MAX_DT = 0.1  # s; a longer gap (or a stamp going backwards) restarts the integration
STILL_GYRO = 0.05  # rad/s; below this on every axis, with |a| near g, the car counts as stopped
STILL_ACCEL = 0.05  # fraction of g
BIAS_TAU = 2.0  # s, how quickly the stopped-gyro mean follows
SAT = 0.98  # fraction of full scale that counts as saturated


def wrap(a):
    return math.atan2(math.sin(a), math.cos(a))


def unpack(frame):
    """Samples in one bridge frame: [(stamp, gx, gy, gz, ax, ay, az), ...]."""
    (n,) = COUNT.unpack_from(frame)
    if len(frame) != COUNT.size + n * SAMPLE.size:
        raise ValueError('imu frame: %d bytes for %d samples' % (len(frame), n))
    return list(SAMPLE.iter_unpack(memoryview(frame)[COUNT.size:]))


class ImuFilter:

    def __init__(self, accel_range=16 * G, gyro_range=math.radians(2000)):
        self.accel_range = accel_range
        self.gyro_range = gyro_range
        self.roll = self.pitch = self.yaw = None
        self.stamp = None
        self.bias = None
        self.stopped = False
        self.arrivals = deque()  # (wall time, samples) per frame over the last second
        self.last_arrival = None
        self.new_step()

    def new_step(self):
        self.n = 0
        self.gsum = [0.0, 0.0, 0.0]
        self.asum = [0.0, 0.0, 0.0]
        self.peak = 0.0
        self.sat = False

    def reset_yaw(self):
        if self.yaw is not None:
            self.yaw = 0.0

    def feed(self, frame, now=None):
        now = time.monotonic() if now is None else now
        samples = unpack(frame)
        self.arrivals.append((now, len(samples)))
        self.last_arrival = now
        for s in samples:
            self.sample(*s)

    def sample(self, stamp, gx, gy, gz, ax, ay, az):
        a = math.sqrt(ax * ax + ay * ay + az * az)
        # Tilt from gravity: at rest the accelerometer reads +1 g along the car's up axis.
        acc_roll = math.atan2(ay, az)
        acc_pitch = math.atan2(-ax, math.sqrt(ay * ay + az * az))
        dt = stamp - self.stamp if self.stamp is not None else None
        self.stamp = stamp
        if self.roll is None or dt is None or not 0 < dt <= MAX_DT:
            if self.roll is None:
                self.roll, self.pitch, self.yaw = acc_roll, acc_pitch, 0.0
        else:
            # Body rates to Euler angle rates (ZYX), then integrate.
            r, p = self.roll, self.pitch
            cp = math.cos(p) if abs(math.cos(p)) > 1e-3 else math.copysign(1e-3, math.cos(p))
            self.roll += (gx + math.sin(r) * math.tan(p) * gy + math.cos(r) * math.tan(p) * gz) * dt
            self.pitch += (math.cos(r) * gy - math.sin(r) * gz) * dt
            self.yaw = wrap(self.yaw + (math.sin(r) / cp * gy + math.cos(r) / cp * gz) * dt)
            if abs(a - G) < TILT_GATE * G:
                k = dt / (TAU + dt)
                self.roll = wrap(self.roll + k * wrap(acc_roll - self.roll))
                self.pitch += k * (acc_pitch - self.pitch)
            # Gyro bias: the mean reading while the car is still.
            self.stopped = max(abs(gx), abs(gy), abs(gz)) < STILL_GYRO and abs(a - G) < STILL_ACCEL * G
            if self.stopped:
                if self.bias is None:
                    self.bias = [gx, gy, gz]
                else:
                    k = dt / (BIAS_TAU + dt)
                    self.bias = [b + k * (v - b) for b, v in zip(self.bias, (gx, gy, gz))]
        self.n += 1
        for i, v in enumerate((gx, gy, gz)):
            self.gsum[i] += v
        for i, v in enumerate((ax, ay, az)):
            self.asum[i] += v
        self.peak = max(self.peak, a / G)
        if (max(abs(ax), abs(ay), abs(az)) >= SAT * self.accel_range
                or max(abs(gx), abs(gy), abs(gz)) >= SAT * self.gyro_range):
            self.sat = True

    def report(self, now=None):
        """The step's summary (JSON-ready), then start the next step."""
        now = time.monotonic() if now is None else now
        while self.arrivals and self.arrivals[0][0] < now - 1.0:
            self.arrivals.popleft()
        n = self.n
        msg = {
            'type': 'imu',
            'rate': sum(k for _, k in self.arrivals),  # samples received in the last second
            'age': None if self.last_arrival is None else now - self.last_arrival,
            'n': n,
            'roll': self.roll, 'pitch': self.pitch, 'yaw': self.yaw,
            'gyro': [v / n for v in self.gsum] if n else None,
            'accel': [v / n for v in self.asum] if n else None,
            'peak_g': self.peak if n else None,
            'sat': self.sat,
            'bias': self.bias,
            'stopped': self.stopped,
        }
        self.new_step()
        return msg
