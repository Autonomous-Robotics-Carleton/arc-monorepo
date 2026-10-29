"""The twin's car in the gym, without ROS: on the built-in oval, starting at
the bottom straight's start, heading +x, with walls 1 m to either side."""

import math

import numpy as np
import pytest
from arc_car_config import load

from arc_backend_sim_gym import vehicle, world
from arc_backend_sim_gym.sim import G, GymCar

DT = 0.001
STRAIGHT, RADIUS, WIDTH = 10.0, 3.0, 2.0
# the LiDAR geometry of E-10
BEAMS, FOV, RANGE = 1081, math.radians(270.0), 10.0


@pytest.fixture(scope='module')
def track():
    grid, origin = world.oval_grid(STRAIGHT, RADIUS, WIDTH, 0.05)
    return world.make_track(grid, origin, 0.05, 'oval')


@pytest.fixture(params=['st', 'ks'])
def car(request, track):
    return GymCar(load('twin'), vehicle.TBC, track, DT, model=request.param)


def lag(car):
    """The gym's speed controller's time constant (its pid_accl, accelerating
    forward): the car follows the ramped setpoint this far behind."""
    return car.params['v_max'] / (10 * car.params['a_max'])


def drive(car, seconds, speed, accel=2.0, steer=0.0):
    for _ in range(round(seconds / DT)):
        car.step(speed, accel, steer)
    return car.state


def test_oval_is_free_along_its_centreline_and_walled_beside_it():
    grid, origin = world.oval_grid(STRAIGHT, RADIUS, WIDTH, 0.05)
    x, y = world.oval_centreline(STRAIGHT, RADIUS)
    cols = ((x - origin[0]) / 0.05).astype(int)
    rows = ((y - origin[1]) / 0.05).astype(int)
    assert np.all(grid[rows, cols] == world.FREE)
    # 1.2 m to the right of the start: past the wall
    assert grid[int((-1.2 - origin[1]) / 0.05), int((0.0 - origin[0]) / 0.05)] == world.OCCUPIED


def test_map_server_map_loads_the_right_way_up(tmp_path):
    from PIL import Image

    # 4 x 3 cells, all free but the top-left one in the image: that's the
    # highest row, y = 2, x = 0, once the image is flipped into the gym's layout
    pixels = np.full((3, 4), 254, dtype=np.uint8)
    pixels[0, 0] = 0
    Image.fromarray(pixels).save(tmp_path / 'room.png')
    (tmp_path / 'room.yaml').write_text(
        'image: room.png\nresolution: 0.5\norigin: [-1.0, -0.5, 0.0]\nnegate: 0\n'
        'occupied_thresh: 0.65\nfree_thresh: 0.196\n')
    grid, origin, resolution = world.load_map_yaml(tmp_path / 'room.yaml')
    assert (origin, resolution) == ((-1.0, -0.5, 0.0), 0.5)
    assert grid.shape == (3, 4)
    assert grid[2, 0] == world.OCCUPIED
    assert np.count_nonzero(grid == world.OCCUPIED) == 1


def test_starts_at_rest(car):
    s = car.state
    assert (s.x, s.y, s.yaw, s.vx) == (0.0, 0.0, 0.0, 0.0)


def test_drives_straight_at_the_commanded_speed(car):
    s = drive(car, 3.0, 1.0)
    assert s.vx == pytest.approx(1.0, abs=0.01)
    assert s.vy == pytest.approx(0.0, abs=1e-6)
    assert s.y == pytest.approx(0.0, abs=1e-6)
    # 0.5 s of ramp at 2 m/s^2 covers 0.25 m, then 2.5 s at 1 m/s, less the lag
    assert s.x == pytest.approx(2.75 - 1.0 * lag(car), abs=0.02)
    assert not s.collision
    for c in vehicle.CORNERS:
        assert s.wheel_speed[c] == pytest.approx(1.0, abs=0.01)


def test_accelerates_no_faster_than_the_command_allows(car):
    s = drive(car, 1.0, 5.0, accel=1.0)
    assert s.vx == pytest.approx(1.0 * (1.0 - lag(car)), abs=0.02)
    assert s.accel_cg[0] == pytest.approx(1.0, abs=0.1)
    assert s.accel_cg[2] == G


def test_turns_left_at_the_single_track_yaw_rate(car):
    steer = 0.2
    s = drive(car, 1.0, 1.0, steer=steer)  # a quarter of the way round a 1.7 m radius
    wheelbase = car.car['geometry']['wheelbase']
    assert s.steer == pytest.approx(steer, abs=0.01)
    assert s.yaw_rate == pytest.approx(s.vx * math.tan(steer) / wheelbase, rel=0.1)
    assert s.yaw > 0 and s.y > 0
    # inside wheels slower; the centripetal acceleration points left
    assert s.wheel_speed['rl'] < s.wheel_speed['rr']
    assert s.accel_cg[1] == pytest.approx(s.vx * s.yaw_rate, rel=0.1)


def test_steering_is_limited_to_the_cars_range(car):
    s = drive(car, 0.5, 0.5, steer=1.0)
    assert s.steer == pytest.approx(car.car['steering']['max_angle'], abs=0.01)


def test_lidar_sees_the_walls(car):
    ranges = car.lidar(BEAMS, FOV, RANGE, 0.0, 0.0)
    assert ranges.shape == (BEAMS,)

    def at(angle):
        return ranges[round((angle + FOV / 2) / (FOV / (BEAMS - 1)))]

    # the walls are 1 m to the right (-90 deg) and the left (+90 deg)
    assert at(-math.pi / 2) == pytest.approx(WIDTH / 2, abs=0.06)
    assert at(math.pi / 2) == pytest.approx(WIDTH / 2, abs=0.06)
    # straight ahead the first wall is past the 10 m range: no return
    assert math.isinf(at(0.0))


def test_lidar_offset_moves_the_scan_origin(car):
    car.reset((0.0, 0.5, math.pi / 2))  # facing the left wall, 0.5 m from it
    assert car.lidar(BEAMS, FOV, RANGE, 0.0, 0.0)[BEAMS // 2] == pytest.approx(0.5, abs=0.06)
    assert car.lidar(BEAMS, FOV, RANGE, 0.0, 0.2)[BEAMS // 2] == pytest.approx(0.3, abs=0.06)


def test_stops_at_a_wall(car):
    car.reset((0.0, 0.0, math.pi / 2))  # facing the left wall, 1 m away
    collided = False
    for _ in range(3000):
        car.step(1.0, 2.0, 0.0)
        collided |= car.state.collision
    s = car.state
    assert collided
    # still at the wall, still facing it, not through it
    assert s.x == pytest.approx(0.0, abs=1e-6)
    assert s.yaw == pytest.approx(math.pi / 2)
    assert 0.5 < s.y < WIDTH / 2
    assert abs(s.vx) < 0.1


def test_reset_puts_base_link_at_the_pose(car):
    drive(car, 1.0, 1.0)
    car.reset((1.0, -0.2, 0.1))
    s = car.state
    assert (s.x, s.y, s.yaw) == (1.0, -0.2, 0.1)
    assert s.time == pytest.approx(1.0)  # time doesn't go back
    s = drive(car, 0.1, 0.0)  # held still: base_link stays put
    assert s.x == pytest.approx(1.0) and s.y == pytest.approx(-0.2)
