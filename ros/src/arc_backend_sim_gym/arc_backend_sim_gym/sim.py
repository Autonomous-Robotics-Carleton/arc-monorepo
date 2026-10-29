"""One car in the F1TENTH gym, stepped with the car interface's drive command
and read back at base_link. No ROS here: node.py publishes what this computes.

It drives the gym's Simulator, not its gymnasium environment: the environment
adds lap counting, start-pose sampling along a raceline and a renderer,
none of which a backend needs.
"""

import math
from dataclasses import dataclass, field

import numpy as np
from f1tenth_gym.envs.action import CarAction
from f1tenth_gym.envs.base_classes import Simulator
from f1tenth_gym.envs.dynamic_models import DynamicModel
from f1tenth_gym.envs.integrator import IntegratorType
from f1tenth_gym.envs.laser_models import ScanSimulator2D

from . import vehicle

G = 9.80665  # m/s^2: what an accelerometer at rest reads, upwards (REP-145)


@dataclass
class State:
    """The car after a step, at base_link, in the world's frame unless noted."""

    time: float = 0.0  # s since the start
    x: float = 0.0  # m
    y: float = 0.0
    yaw: float = 0.0  # rad
    vx: float = 0.0  # m/s, in base_link
    vy: float = 0.0
    yaw_rate: float = 0.0  # rad/s
    accel_cg: tuple = (0.0, 0.0, G)  # m/s^2, at the centre of gravity, in base_link's axes
    steer: float = 0.0  # rad, the single-track model's
    steer_rate: float = 0.0  # rad/s
    wheel_speed: dict = field(default_factory=dict)  # m/s per corner
    wheel_angle: dict = field(default_factory=dict)  # rad per corner, in [0, 2 pi)
    collision: bool = False


class GymCar:
    """A car on a track. Not thread-safe: one thread steps it."""

    def __init__(self, car, tbc, track, timestep, model='st', integrator='rk4', seed=12345):
        if model not in vehicle.MODELS:
            raise ValueError(f'model must be one of {vehicle.MODELS}, not {model!r}')
        self.car = car
        self.params = vehicle.gym_params(car, tbc)
        self.dt = timestep
        self.model = model
        self._ref = vehicle.reference_offset(self.params, model)
        self._wheels = vehicle.wheel_positions(car)
        self._sim = Simulator(
            self.params, 1, seed,
            action_type=CarAction(['speed', 'steering_angle'], params=self.params),
            integrator=IntegratorType.from_string(integrator),
            model=DynamicModel.from_string(model),
            time_step=timestep)
        self._sim.set_map(track)
        self._track = track
        self._lidar = None
        self.reset((0.0, 0.0, 0.0))

    def reset(self, pose):
        """Put base_link at pose (x, y, yaw), stopped, steering centred. Time
        carries on: it never goes back."""
        x, y, yaw = pose
        ref = np.array([[x + self._ref * math.cos(yaw), y + self._ref * math.sin(yaw), yaw]])
        self._sim.reset(ref)
        self._setpoint = 0.0
        self.state = State(
            time=self.state.time if hasattr(self, 'state') else 0.0, x=x, y=y, yaw=yaw,
            wheel_speed={c: 0.0 for c in vehicle.CORNERS},
            wheel_angle={c: 0.0 for c in vehicle.CORNERS})

    def step(self, speed, accel, steer):
        """Advance one timestep under a drive command (DriveCommand's fields)."""
        self._setpoint = vehicle.speed_setpoint(
            self._setpoint, speed, accel, self.params['a_max'], self.dt)
        self._sim.step(np.array([[steer, self._setpoint]]))
        if self._sim.collisions[0]:
            # The gym stops a car at a wall by zeroing its state from the
            # speed on, which zeroes its heading too: keep the heading, and
            # ramp the speed up again from rest
            agent = self._sim.agents[0]
            agent.state[4] = self.state.yaw
            self._sim.agent_poses[0, 2] = self.state.yaw
            self._setpoint = 0.0
        self._read()

    def _read(self):
        prev = self.state
        ref_x, ref_y, yaw = self._sim.agent_poses[0]
        steer = float(self._sim.agent_steerings[0])
        x = ref_x - self._ref * math.cos(yaw)
        y = ref_y - self._ref * math.sin(yaw)

        # velocities from the step's motion, whichever model made it
        dt = self.dt
        dyaw = math.remainder(yaw - prev.yaw, 2 * math.pi)
        yaw_rate = dyaw / dt
        mid = prev.yaw + dyaw / 2
        dx, dy = (x - prev.x) / dt, (y - prev.y) / dt
        vx = dx * math.cos(mid) + dy * math.sin(mid)
        vy = -dx * math.sin(mid) + dy * math.cos(mid)

        # acceleration at the centre of gravity, in the car's axes: the
        # change of its velocity plus yaw rate x velocity
        lr = self.params['lr']
        vy_cg, prev_vy_cg = vy + yaw_rate * lr, prev.vy + prev.yaw_rate * lr
        ax = (vx - prev.vx) / dt - yaw_rate * vy_cg
        ay = (vy_cg - prev_vy_cg) / dt + yaw_rate * vx

        speeds = vehicle.wheel_speeds(vx, vy, yaw_rate, steer, self._wheels)
        angles = {
            c: (prev.wheel_angle[c] + vehicle.wheel_angle_rate(speeds[c], self.car) * dt)
            % (2 * math.pi)
            for c in vehicle.CORNERS}
        self.state = State(
            time=prev.time + dt, x=x, y=y, yaw=yaw, vx=vx, vy=vy, yaw_rate=yaw_rate,
            accel_cg=(ax, ay, G), steer=steer, steer_rate=(steer - prev.steer) / dt,
            wheel_speed=speeds, wheel_angle=angles,
            collision=bool(self._sim.collisions[0]))

    def lidar(self, beams, fov, range_max, noise, offset):
        """A 2D scan from a LiDAR `offset` m ahead of base_link: ranges from
        the first beam (-fov / 2, to the right) anticlockwise, with Gaussian
        noise of standard deviation `noise` m. A beam with no wall within
        range_max reads +inf (REP-117)."""
        if self._lidar is None:
            self._lidar = ScanSimulator2D(beams, fov, max_range=range_max)
            self._lidar.set_map(self._track)
            self._rng = np.random.default_rng(seed=0)
        s = self.state
        pose = np.array([s.x + offset * math.cos(s.yaw), s.y + offset * math.sin(s.yaw), s.yaw])
        ranges = self._lidar.scan(pose, None)
        hit = ranges < range_max
        if noise > 0:
            ranges = ranges + self._rng.normal(0.0, noise, size=ranges.shape)
        return np.where(hit, ranges, np.inf)
