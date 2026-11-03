"""The car files in the repo load, and the loader merges, checks and hashes them."""

from pathlib import Path
import shutil

from arc_car_config.cli import check
from arc_car_config.loader import (
    CarConfigError,
    car_names,
    config_hash,
    load,
    top_speed,
)
import pytest
import yaml


def repo_cars():
    for parent in Path(__file__).resolve().parents:
        if (parent / 'config' / 'cars' / 'schema.json').is_file():
            return parent / 'config' / 'cars'
    raise RuntimeError('config/cars not found above this test')


CARS = repo_cars()


@pytest.fixture
def cars(tmp_path):
    """A copy of the repo's car files to change."""
    for f in CARS.iterdir():
        shutil.copy(f, tmp_path)
    return tmp_path


def write(directory, name, values):
    (directory / f'{name}.yaml').write_text(yaml.safe_dump(values))


def test_every_car_in_the_repo_loads():
    assert {'nominal', 'twin'} <= set(car_names(CARS))
    assert check(CARS) == 0


def test_the_twin_is_the_design():
    twin, nominal = load('twin', CARS), load('nominal', CARS)
    assert twin['name'] == 'twin'
    assert {k: v for k, v in twin.items() if k != 'name'} == \
        {k: v for k, v in nominal.items() if k != 'name'}


def test_a_car_overrides_only_what_it_measured(cars):
    write(cars, 'test-car', {
        'extends': 'nominal', 'name': 'test-car', 'build': 'v1.0',
        'wheels': {'rolling_diameter': 0.097},
        'drivetrain': {'gear_ratio_rear': 15.5},
    })
    car, nominal = load('test-car', cars), load('nominal', cars)
    assert car['wheels']['rolling_diameter'] == 0.097
    assert car['drivetrain'] == {**nominal['drivetrain'], 'gear_ratio_rear': 15.5}
    assert car['geometry'] == nominal['geometry']


def test_extends_chains_and_detects_loops(cars):
    write(cars, 'a', {'extends': 'twin', 'name': 'a'})
    assert load('a', cars)['geometry'] == load('twin', cars)['geometry']
    write(cars, 'b', {'extends': 'c', 'name': 'b'})
    write(cars, 'c', {'extends': 'b', 'name': 'c'})
    with pytest.raises(CarConfigError, match='loops'):
        load('b', cars)


def test_unknown_and_missing_values_are_rejected(cars):
    write(cars, 'typo', {'extends': 'nominal', 'name': 'typo', 'wheels': {'diameter': 0.1}})
    with pytest.raises(CarConfigError, match='wheels'):
        load('typo', cars)
    write(cars, 'bare', {'name': 'bare', 'build': 'v1.1'})
    with pytest.raises(CarConfigError, match='required'):
        load('bare', cars)


def test_the_name_matches_the_file(cars):
    write(cars, 'copy', {'extends': 'nominal'})
    with pytest.raises(CarConfigError, match="name is 'nominal'"):
        load('copy', cars)


def test_the_hash_follows_the_values(cars):
    assert config_hash(load('twin', cars)) == config_hash(load('twin', cars))
    write(cars, 'twin', {'extends': 'nominal', 'name': 'twin',
                         'steering': {'centre_offset': 0.01}})
    assert config_hash(load('twin', cars)) != config_hash(load('twin', CARS))


def test_top_speed_follows_the_slower_axle():
    nominal = load('nominal', CARS)
    # architecture.md: ~10.9 m/s at the eRPM cap with 18:1 and a 100 mm tyre
    assert top_speed(nominal) == pytest.approx(10.9, abs=0.05)
    slower = {**nominal, 'drivetrain': {**nominal['drivetrain'], 'gear_ratio_rear': 21.2}}
    assert top_speed(slower) < top_speed(nominal)
