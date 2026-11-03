"""Reads a car's configuration: its file merged over the one it extends,
checked against schema.json, with the values worked out from it.

Plain Python (PyYAML and jsonschema), so the same code runs in ROS nodes,
in launch files and in the checks, without a ROS install.
"""

import hashlib
import json
import math
import os
from pathlib import Path

import jsonschema
import yaml


class CarConfigError(Exception):
    pass


def cars_dir():
    """Where the car files are: $ARC_CARS_DIR, the installed share, or the repo."""
    if 'ARC_CARS_DIR' in os.environ:
        return Path(os.environ['ARC_CARS_DIR'])
    try:
        from ament_index_python.packages import get_package_share_directory
        return Path(get_package_share_directory('arc_car_config')) / 'cars'
    except (ImportError, LookupError):
        return Path(__file__).resolve().parents[4] / 'config' / 'cars'


def car_names(directory=None):
    directory = Path(directory or cars_dir())
    return sorted(p.stem for p in directory.glob('*.yaml'))


def _read(directory, name):
    path = directory / f'{name}.yaml'
    if not path.is_file():
        raise CarConfigError(f'no car file {path}')
    with path.open() as f:
        values = yaml.safe_load(f) or {}
    if not isinstance(values, dict):
        raise CarConfigError(f'{path}: expected a mapping at the top level')
    return values


def _merge(base, override):
    """Override wins key by key; mappings merge, everything else is replaced."""
    merged = dict(base)
    for key, value in override.items():
        if isinstance(value, dict) and isinstance(merged.get(key), dict):
            merged[key] = _merge(merged[key], value)
        else:
            merged[key] = value
    return merged


def resolve(name, directory=None):
    """The car's values after following `extends`, unchecked."""
    directory = Path(directory or cars_dir())
    chain = []
    values = {}
    while name is not None:
        if name in chain:
            raise CarConfigError(f'extends loops: {" -> ".join(chain + [name])}')
        chain.append(name)
        layer = _read(directory, name)
        name = layer.pop('extends', None)
        values = _merge(layer, values)
    return values


def validate(values, directory=None):
    directory = Path(directory or cars_dir())
    with (directory / 'schema.json').open() as f:
        schema = json.load(f)
    try:
        jsonschema.validate(values, schema)
    except jsonschema.ValidationError as e:
        where = '.'.join(str(p) for p in e.absolute_path) or '(top level)'
        raise CarConfigError(f'{values.get("name", "?")}: {where}: {e.message}') from None


def load(name, directory=None):
    """The checked configuration of car `name` (config/cars/<name>.yaml)."""
    values = resolve(name, directory)
    validate(values, directory)
    if values['name'] != name:
        raise CarConfigError(f'{name}.yaml: name is {values["name"]!r}, not {name!r}')
    return values


def config_hash(values):
    """SHA-256 of the merged values: the same configuration gives the same hash."""
    canonical = json.dumps(values, sort_keys=True, separators=(',', ':'))
    return hashlib.sha256(canonical.encode()).hexdigest()


def top_speed(values):
    """The fastest the car can go, m/s: the eRPM cap through the slower axle's gearing."""
    d = values['drivetrain']
    ratio = max(d['gear_ratio_front'], d['gear_ratio_rear'])
    wheel_rpm = d['erpm_max'] / d['motor_pole_pairs'] / ratio
    return wheel_rpm / 60.0 * math.pi * values['wheels']['rolling_diameter']
