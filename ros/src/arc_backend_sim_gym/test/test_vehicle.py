"""The gym's vehicle parameters and the wheel kinematics, on the nominal car
(config/cars/nominal.yaml), without the gym or ROS."""

import math

import pytest
from arc_car_config import load, top_speed

from arc_backend_sim_gym import vehicle


@pytest.fixture
def car():
    return load('nominal')


def test_params_come_from_the_car_configuration(car):
    p = vehicle.gym_params(car, vehicle.TBC)
    assert p['lf'] + p['lr'] == pytest.approx(car['geometry']['wheelbase'])
    assert p['lr'] == pytest.approx(car['geometry']['wheelbase'] * vehicle.TBC['cg_from_rear_axle'])
    assert p['s_max'] == car['steering']['max_angle'] == -p['s_min']
    assert p['v_max'] == pytest.approx(top_speed(car)) == pytest.approx(-p['v_min'])
    assert p['m'] == vehicle.TBC['mass']


def test_steering_turns_lock_to_lock_in_adr_0019s_time(car):
    p = vehicle.gym_params(car, vehicle.TBC)
    assert (p['s_max'] - p['s_min']) / p['sv_max'] == pytest.approx(vehicle.LOCK_TO_LOCK_TIME)


def test_top_speed_is_the_erpm_cap(car):
    # the slower axle reaches the controllers' eRPM cap at the top speed
    v = top_speed(car)
    slower = 'fl' if car['drivetrain']['gear_ratio_front'] >= car['drivetrain']['gear_ratio_rear'] else 'rl'
    assert vehicle.erpm(v, slower, car) == pytest.approx(car['drivetrain']['erpm_max'])


def test_erpm_follows_gearing_and_poles(car):
    d = car['drivetrain']
    # one wheel revolution per second
    speed = math.pi * car['wheels']['rolling_diameter']
    assert vehicle.erpm(speed, 'rr', car) == pytest.approx(60 * d['gear_ratio_rear'] * d['motor_pole_pairs'])
    assert vehicle.erpm(-speed, 'fl', car) == pytest.approx(-60 * d['gear_ratio_front'] * d['motor_pole_pairs'])
    assert vehicle.wheel_angle_rate(speed, car) == pytest.approx(2 * math.pi)


def test_straight_line_every_wheel_rolls_at_the_cars_speed(car):
    speeds = vehicle.wheel_speeds(2.0, 0.0, 0.0, 0.0, vehicle.wheel_positions(car))
    assert speeds == {c: pytest.approx(2.0) for c in vehicle.CORNERS}


def test_left_turn_without_slip(car):
    # base_link moves straight ahead (no sideslip at the rear axle) and the
    # front axle's centre along the steering angle
    g = car['geometry']
    v, steer = 2.0, 0.3
    yaw_rate = v * math.tan(steer) / g['wheelbase']
    s = vehicle.wheel_speeds(v, 0.0, yaw_rate, steer, vehicle.wheel_positions(car))
    # the inside (left) rear wheel is slower, by yaw rate x track
    assert s['rr'] - s['rl'] == pytest.approx(yaw_rate * g['track_rear'])
    assert (s['rl'] + s['rr']) / 2 == pytest.approx(v)
    assert (s['fl'] + s['fr']) / 2 == pytest.approx(v / math.cos(steer))


def test_reference_point(car):
    p = vehicle.gym_params(car, vehicle.TBC)
    assert vehicle.reference_offset(p, 'st') == p['lr']  # the gym's st state is at the CG
    assert vehicle.reference_offset(p, 'ks') == 0.0  # its ks state at the rear axle


@pytest.mark.parametrize('accel', [1.0, 4.0])
def test_setpoint_ramps_at_the_commands_accel(accel):
    dt, sp = 0.01, 0.0
    for _ in range(50):
        sp = vehicle.speed_setpoint(sp, 10.0, accel, 9.0, dt)
    assert sp == pytest.approx(accel * 0.5)


def test_setpoint_reaches_the_speed_without_overshoot():
    sp = 0.0
    for _ in range(1000):
        sp = vehicle.speed_setpoint(sp, 1.0, 2.0, 9.0, 0.01)
    assert sp == 1.0


def test_setpoint_accel_is_capped_by_the_drivetrain():
    # the zero command's accel is the largest float: as hard as allowed
    sp = vehicle.speed_setpoint(3.0, 0.0, 3.4e38, 9.0, 0.1)
    assert sp == pytest.approx(3.0 - 0.9)


def test_setpoint_holds_with_no_accel():
    assert vehicle.speed_setpoint(1.5, 3.0, 0.0, 9.0, 0.1) == 1.5
    assert vehicle.speed_setpoint(1.5, 3.0, -2.0, 9.0, 0.1) == 1.5
