"""The car as the gym simulates it.

The gym's vehicle parameters come from the car configuration (config/cars/,
ADR-0051) where it has them, from systems/ where that records them, and are
TBC otherwise. The wheel kinematics turn the gym's single-track state into
the car interface's per-corner values. Plain Python, no ROS or gym, so it's
tested on its own.
"""

import math

from arc_car_config import top_speed

# ADR-0019: lock to lock (about +-25 deg at the wheels) in at most 0.1 s. The
# gym's steering is rate-limited, so the model turns exactly that fast: the
# slowest actuator the requirement allows (TBC until the actuator is measured).
LOCK_TO_LOCK_TIME = 0.1  # s

# What nothing in systems/ or config/cars/ records yet. All TBC: the gym's own
# defaults (its F1TENTH Traxxas car), until budgets/mass.csv has a total and a
# centre of gravity, the CAD block layout (#103) the outline, and a
# system-identification run on the floor the tyre (simulation.md, Fidelity).
# Each one is a node parameter of the same name.
TBC = {
    'mass': 3.74,                        # kg
    'yaw_inertia': 0.04712,              # kg m^2, about the centre of gravity
    'cg_height': 0.074,                  # m
    'cg_from_rear_axle': 0.5192,         # fraction of the wheelbase (the gym's 0.17145 / 0.3302)
    'friction': 1.0489,                  # tyre on the floor
    'cornering_stiffness_front': 4.718,  # 1/rad, per unit of load
    'cornering_stiffness_rear': 5.4562,  # 1/rad
    'accel_max': 9.51,                   # m/s^2, the drivetrain's
    'v_switch': 7.319,                   # m/s: above it, acceleration falls off with speed
    'length': 0.58,                      # m, the outline the gym checks collisions with;
    'width': 0.31,                       # m, both inside SYS-02
}

# The gym's single-track models and where their state's (x, y) sits on the car
MODELS = ('st', 'ks')  # st: at the centre of gravity; ks: at the rear axle

CORNERS = ('fl', 'fr', 'rl', 'rr')

# One commutation step per sixth of an electrical revolution: how the VESC
# counts its tachometer (MotorStatus.tachometer)
TACHO_STEPS_PER_EREV = 6


def gym_params(car, tbc):
    """The gym's vehicle parameters for a car configuration and the TBC values."""
    wheelbase = car['geometry']['wheelbase']
    lr = wheelbase * tbc['cg_from_rear_axle']
    max_steer = car['steering']['max_angle']
    steer_rate = 2 * max_steer / LOCK_TO_LOCK_TIME
    # the eRPM cap through the slower axle (arc_car_config); reverse is
    # allowed (ADR-0052) and has the same cap
    v_max = top_speed(car)
    return {
        'mu': tbc['friction'],
        'C_Sf': tbc['cornering_stiffness_front'],
        'C_Sr': tbc['cornering_stiffness_rear'],
        'lf': wheelbase - lr,
        'lr': lr,
        'h': tbc['cg_height'],
        'm': tbc['mass'],
        'I': tbc['yaw_inertia'],
        's_min': -max_steer,
        's_max': max_steer,
        'sv_min': -steer_rate,
        'sv_max': steer_rate,
        'v_switch': tbc['v_switch'],
        'a_max': tbc['accel_max'],
        'v_min': -v_max,
        'v_max': v_max,
        'width': tbc['width'],
        'length': tbc['length'],
    }


def reference_offset(params, model):
    """How far ahead of base_link (the centre of the rear axle, ICD-ros2-msgs)
    the gym's state (x, y) is."""
    return params['lr'] if model == 'st' else 0.0


def wheel_positions(car):
    """Each wheel's contact point in base_link: x forward, y left (REP-103)."""
    g = car['geometry']
    return {
        'fl': (g['wheelbase'], g['track_front'] / 2),
        'fr': (g['wheelbase'], -g['track_front'] / 2),
        'rl': (0.0, g['track_rear'] / 2),
        'rr': (0.0, -g['track_rear'] / 2),
    }


def wheel_speeds(vx, vy, yaw_rate, steer, positions):
    """Each wheel's rolling speed (m/s, + forward) for base_link's velocity
    (vx, vy) and yaw rate. Both front wheels take the single-track model's
    steering angle: the linkage (Ackermann or not) isn't designed yet. No
    longitudinal slip: the gym doesn't model it."""
    speeds = {}
    for corner, (x, y) in positions.items():
        # the contact point's velocity, base_link's plus yaw rate x position
        px = vx - yaw_rate * y
        py = vy + yaw_rate * x
        heading = steer if corner in ('fl', 'fr') else 0.0
        speeds[corner] = px * math.cos(heading) + py * math.sin(heading)
    return speeds


def erpm(wheel_speed, corner, car):
    """A wheel's rolling speed as its motor's eRPM (the controller's unit,
    MotorStatus.erpm): through the axle's gear ratio and the pole pairs."""
    d = car['drivetrain']
    ratio = d['gear_ratio_front'] if corner in ('fl', 'fr') else d['gear_ratio_rear']
    wheel_rpm = wheel_speed / (math.pi * car['wheels']['rolling_diameter']) * 60.0
    return wheel_rpm * ratio * d['motor_pole_pairs']


def wheel_angle_rate(wheel_speed, car):
    """A wheel's rotation rate (rad/s) at its rolling speed."""
    return wheel_speed / (car['wheels']['rolling_diameter'] / 2)


def speed_setpoint(setpoint, speed, accel, accel_max, dt):
    """The speed the gym is asked for after one step: the commanded speed,
    reached no faster than the command's acceleration limit (DriveCommand
    .accel), itself capped at what the drivetrain can do. A limit of zero or
    less holds the setpoint where it is."""
    limit = min(max(accel, 0.0), accel_max) * dt
    return setpoint + min(max(speed - setpoint, -limit), limit)
