"""The gym backend's node: the F1TENTH gym behind the car interface
(ICD-ros2-msgs), in interface mode (ADR-0047).

Interface mode is a shortcut: the simulator drives the car interface directly,
with no car backend, sync MCU firmware or motor-controller models below it.
So nothing here enforces the safety envelope or the heartbeat watchdog
(SYS-22, SYS-04): those are the firmware's, and interface mode is to compile
its safety.c in once that's implemented (it's a fail-safe stub today).

In: car/cmd/drive, from arc_control, as on the car.
Out, from the gym's single-track model:
  sensors/lidar/scan                  every LiDAR period (E-10)
  car/motor/{fl,fr,rl,rr}             wheel speed as eRPM, and the tachometer
  car/steering                        the steering angle and its rate
  sensors/imu/cg                      yaw rate and acceleration at the CG
  sensors/angle/wheel_{fl,fr,rl,rr}   each wheel's angle
  /clock                              the simulator's time (ADR-0049)
Sim only, never on the car (simulation.md, Simulator controls):
  sim/ground_truth, sim/collision, sim/reset
The rest of the car interface (suspension, knuckles, ride height, the front
IMU, cameras, power, safety state, faults) the gym doesn't model, so it isn't
published: an absent topic, not a made-up value.
"""

import math
import threading
import time

import rclpy
import rclpy.executors
from arc_car_config import config_hash, load
from arc_msgs.msg import AngleSample, DriveCommand, MotorStatus, SteeringStatus
from nav_msgs.msg import Odometry
from rclpy.node import Node
from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
from rclpy.time import Time
from rosgraph_msgs.msg import Clock
from sensor_msgs.msg import Imu, LaserScan
from std_msgs.msg import Bool
from std_srvs.srv import Trigger

from . import vehicle, world
from .sim import GymCar

# QoS profiles (ICD-ros2-msgs)
COMMANDS = QoSProfile(
    depth=1, reliability=ReliabilityPolicy.RELIABLE, durability=DurabilityPolicy.VOLATILE)
STATE = QoSProfile(
    depth=1, reliability=ReliabilityPolicy.RELIABLE, durability=DurabilityPolicy.TRANSIENT_LOCAL)


def samples(rate):
    """Keep the last 100 ms of a stream at `rate` Hz, at least one (TBC)."""
    return QoSProfile(
        depth=max(1, round(0.1 * rate)), reliability=ReliabilityPolicy.RELIABLE,
        durability=DurabilityPolicy.VOLATILE)


NAN = float('nan')  # a MotorStatus or SteeringStatus field the gym doesn't model
STEERING_MODE_POSITION = 10  # the moteus mode register (ICD-sync-link): tracking a position


class GymBackend(Node):

    def __init__(self):
        super().__init__('sim_gym_backend')
        p = self._read_params()

        self.car_name = p['car']
        self.car = load(self.car_name)
        tbc = {name: p[name] for name in vehicle.TBC}
        self.track_name, track, self.start_pose = self._world(p)
        self.sim = GymCar(
            self.car, tbc, track, p['timestep'], model=p['model'], integrator=p['integrator'])
        self.dt = p['timestep']
        self.rtf = p['real_time_factor']
        self.sample_every = self._steps(p['sample_period'], 'sample_period')
        self.lidar_every = self._steps(1.0 / p['lidar_rate'], '1 / lidar_rate')
        self.lidar = dict(
            beams=p['lidar_beams'], fov=p['lidar_fov'], range_max=p['lidar_range_max'],
            noise=p['lidar_noise'], offset=p['lidar_x'])
        if p['sensors'] != 'lidar':
            self.get_logger().warn(
                f"sensors:={p['sensors']}: the gym has no cameras, it renders the LiDAR only")

        # The first step compiles the gym's numba functions (seconds without
        # a cache): done now, so the startup line below means ready
        self.sim.step(0.0, 0.0, 0.0)
        self.sim.lidar(**self.lidar)
        self.sim.reset(self.start_pose)

        self._lock = threading.Lock()
        self._command = (0.0, 0.0, 0.0)  # speed, accel, steer: held still until one arrives
        self._reset = False
        self._tacho = {c: 0.0 for c in vehicle.CORNERS}
        self._collided = None
        # The simulator's clock starts at the wall clock's time, so nodes not
        # on use_sim_time see about the same time while it runs at real time
        self._epoch_ns = time.time_ns()

        sample_rate = 1.0 / p['sample_period']
        self.scan_pub = self.create_publisher(LaserScan, 'sensors/lidar/scan', samples(p['lidar_rate']))
        self.motor_pubs = {
            c: self.create_publisher(MotorStatus, f'car/motor/{c}', samples(sample_rate))
            for c in vehicle.CORNERS}
        self.wheel_pubs = {
            c: self.create_publisher(AngleSample, f'sensors/angle/wheel_{c}', samples(sample_rate))
            for c in vehicle.CORNERS}
        self.steering_pub = self.create_publisher(SteeringStatus, 'car/steering', samples(sample_rate))
        self.imu_pub = self.create_publisher(Imu, 'sensors/imu/cg', samples(sample_rate))
        self.clock_pub = self.create_publisher(Clock, '/clock', QoSProfile(depth=1))
        self.truth_pub = self.create_publisher(Odometry, 'sim/ground_truth', samples(sample_rate))
        self.collision_pub = self.create_publisher(Bool, 'sim/collision', STATE)
        self.create_subscription(DriveCommand, 'car/cmd/drive', self._on_drive, COMMANDS)
        self.create_service(Trigger, 'sim/reset', self._on_reset)

        self._running = True
        self.failed = False
        self._thread = threading.Thread(target=self._run, name='gym', daemon=True)
        self._thread.start()

        speed = self.sim.params['v_max']
        # Logged last: the launch tests wait for this line
        self.get_logger().info(
            f'sim (gym) backend: interface mode (a shortcut: no firmware below it, no safety '
            f'envelope), car {self.car_name} ({config_hash(self.car)[:12]}, top speed '
            f'{speed:.1f} m/s), track {self.track_name}, {p["model"]} model at '
            f'{1.0 / self.dt:.0f} Hz, real-time factor {self.rtf:g}')

    def _read_params(self):
        declare = self.declare_parameter
        p = {
            'car': declare('car', 'twin').value,  # config/cars/<car>.yaml
            'sensors': declare('sensors', 'lidar').value,
            # the world: the built-in oval, or a map_server map's YAML file
            'map': declare('map', 'oval').value,
            # the oval, a made-up test track: centreline straights and turn
            # radius, and the width between its walls (m)
            'oval_straight': declare('oval_straight', 10.0).value,
            'oval_radius': declare('oval_radius', 3.0).value,
            'oval_width': declare('oval_width', 2.0).value,
            'map_resolution': declare('map_resolution', 0.05).value,  # m per cell, the oval's
            'start_pose': list(declare('start_pose', [0.0, 0.0, 0.0]).value),  # base_link x, y, yaw
            'timestep': declare('timestep', 0.001).value,  # s: simulation.md's step (TBC)
            # how often the sample topics go out (s): 200 Hz, not the car's
            # >= 1 kHz, so the gym keeps up with real time in Python (TBC)
            'sample_period': declare('sample_period', 0.005).value,
            # 1 runs at real time; 0 as fast as it can (the fast profile)
            'real_time_factor': declare('real_time_factor', 1.0).value,
            'model': declare('model', 'st').value,  # the gym's: st (single track) or ks (kinematic)
            'integrator': declare('integrator', 'rk4').value,
            # the LiDAR (E-10: UST-10LX, 40 Hz, 0.25 deg over 270 deg, 10 m)
            'lidar_rate': declare('lidar_rate', 40.0).value,
            'lidar_beams': declare('lidar_beams', 1081).value,
            'lidar_fov': declare('lidar_fov', math.radians(270.0)).value,
            'lidar_range_max': declare('lidar_range_max', 10.0).value,
            'lidar_noise': declare('lidar_noise', 0.01).value,  # m, standard deviation: the gym's (TBC)
            # m ahead of base_link: TBD until the block layout (#103) places it
            'lidar_x': declare('lidar_x', 0.0).value,
        }
        for name, default in vehicle.TBC.items():
            p[name] = declare(name, default).value
        if p['timestep'] <= 0 or p['real_time_factor'] < 0:
            raise ValueError('timestep must be positive and real_time_factor not negative')
        return p

    def _world(self, p):
        if p['map'] == 'oval':
            grid, origin = world.oval_grid(
                p['oval_straight'], p['oval_radius'], p['oval_width'], p['map_resolution'])
            resolution = p['map_resolution']
        else:
            grid, origin, resolution = world.load_map_yaml(p['map'])
        name = p['map']
        return name, world.make_track(grid, origin, resolution, name), tuple(p['start_pose'])

    def _steps(self, period, what):
        steps = max(1, round(period / self.dt))
        if not math.isclose(steps * self.dt, period, rel_tol=1e-6):
            self.get_logger().warn(
                f'{what} ({period:g} s) is not a whole number of timesteps: '
                f'using {steps * self.dt:g} s')
        return steps

    # Commands and sim controls, on the executor's thread

    def _on_drive(self, msg):
        cmd = (msg.speed, msg.accel, msg.steer)
        if not all(math.isfinite(v) for v in cmd):
            self.get_logger().warn(
                'gym: dropping a drive command with a NaN or infinite field',
                throttle_duration_sec=1.0)
            return
        with self._lock:
            self._command = cmd  # the latest wins, as on the sync link

    def _on_reset(self, request, response):
        with self._lock:
            self._reset = True
        response.success = True
        response.message = f'back to {self.start_pose}, stopped'
        return response

    # The simulation, on its own thread

    def _run(self):
        try:
            self._loop()
        except Exception as e:  # noqa: B902
            if not self.context.ok():
                return  # shutting down: a publish raced it
            # never a backend that's up with no simulation behind it
            self.get_logger().fatal(f'gym: the simulation stopped: {e!r}')
            self.failed = True
            rclpy.try_shutdown()

    def _loop(self):
        wall0 = time.monotonic()
        step = 0
        behind_since = None
        while self._running and rclpy.ok():
            if self.rtf > 0:
                lead = step * self.dt / self.rtf - (time.monotonic() - wall0)
                if lead > 0:
                    time.sleep(lead)
                elif lead < -0.1:
                    # more than 100 ms behind: run on from here rather than race
                    if behind_since is None:
                        behind_since = step
                        self.get_logger().warn(
                            'gym: running slower than the real-time factor asks')
                    wall0 = time.monotonic() - step * self.dt / self.rtf
            with self._lock:
                reset, self._reset = self._reset, False
                if reset:
                    self._command = (0.0, 0.0, 0.0)
                speed, accel, steer = self._command
            if reset:
                self.sim.reset(self.start_pose)
                self._tacho = {c: 0.0 for c in vehicle.CORNERS}
            self.sim.step(speed, accel, steer)
            step += 1
            self._integrate_tacho()
            if step % self.sample_every == 0:
                self._publish_samples()
            if step % self.lidar_every == 0:
                self._publish_scan()

    def _stamp(self):
        return Time(nanoseconds=self._epoch_ns + round(self.sim.state.time * 1e9)).to_msg()

    def _integrate_tacho(self):
        s = self.sim.state
        for c in vehicle.CORNERS:
            erpm = vehicle.erpm(s.wheel_speed[c], c, self.car)
            self._tacho[c] += erpm / 60.0 * vehicle.TACHO_STEPS_PER_EREV * self.dt

    def _publish_samples(self):
        s = self.sim.state
        stamp = self._stamp()
        self.clock_pub.publish(Clock(clock=stamp))

        for c in vehicle.CORNERS:
            m = MotorStatus()
            m.header.stamp = stamp
            m.erpm = round(vehicle.erpm(s.wheel_speed[c], c, self.car))
            m.tachometer = round(self._tacho[c])
            m.current_q = m.current_d = m.current_in = m.duty = NAN
            m.voltage_in = m.temp_fet = m.temp_motor = NAN
            self.motor_pubs[c].publish(m)

            a = AngleSample()
            a.header.stamp = stamp
            a.header.frame_id = f'wheel_{c}'
            a.angle = s.wheel_angle[c]
            self.wheel_pubs[c].publish(a)

        st = SteeringStatus()
        st.header.stamp = stamp
        st.angle = s.steer
        st.velocity = s.steer_rate
        st.torque = st.temp = NAN
        st.mode = STEERING_MODE_POSITION
        self.steering_pub.publish(st)

        imu = Imu()
        imu.header.stamp = stamp
        imu.header.frame_id = 'imu_cg'
        imu.orientation_covariance[0] = -1.0  # no orientation (ICD-ros2-msgs)
        imu.angular_velocity.z = s.yaw_rate
        imu.linear_acceleration.x, imu.linear_acceleration.y, imu.linear_acceleration.z = s.accel_cg
        self.imu_pub.publish(imu)

        truth = Odometry()
        truth.header.stamp = stamp
        truth.header.frame_id = 'map'
        truth.child_frame_id = 'base_link'
        truth.pose.pose.position.x = s.x
        truth.pose.pose.position.y = s.y
        truth.pose.pose.orientation.z = math.sin(s.yaw / 2)
        truth.pose.pose.orientation.w = math.cos(s.yaw / 2)
        truth.twist.twist.linear.x = s.vx
        truth.twist.twist.linear.y = s.vy
        truth.twist.twist.angular.z = s.yaw_rate
        self.truth_pub.publish(truth)

        if s.collision != self._collided:
            self._collided = s.collision
            self.collision_pub.publish(Bool(data=s.collision))
            if s.collision:
                self.get_logger().warn(f'gym: collision at ({s.x:.2f}, {s.y:.2f})')

    def _publish_scan(self):
        ranges = self.sim.lidar(**self.lidar)
        scan = LaserScan()
        scan.header.stamp = self._stamp()
        scan.header.frame_id = 'lidar'
        fov, beams = self.lidar['fov'], self.lidar['beams']
        scan.angle_min = -fov / 2
        scan.angle_max = fov / 2
        scan.angle_increment = fov / (beams - 1)
        scan.time_increment = 0.0  # the gym takes the whole scan at once
        scan.scan_time = self.lidar_every * self.dt
        scan.range_min = 0.0  # the gym has none; the UST's isn't recorded (TBD)
        scan.range_max = self.lidar['range_max']
        scan.ranges = ranges.astype(float).tolist()
        self.scan_pub.publish(scan)

    def stop(self):
        self._running = False
        self._thread.join(timeout=1.0)


def main(args=None):
    rclpy.init(args=args)
    node = GymBackend()
    try:
        rclpy.spin(node)
    except (KeyboardInterrupt, rclpy.executors.ExternalShutdownException):
        pass
    finally:
        node.stop()
        node.destroy_node()
        rclpy.try_shutdown()
    return 1 if node.failed else 0
