"""Check the car files, or print one car's merged configuration.

    car_config check [DIR]    every car in DIR (default: the installed cars) loads and validates
    car_config show CAR       the merged values, the hash and the top speed

Without ROS: PYTHONPATH=ros/src/arc_car_config python3 -m arc_car_config.cli check config/cars
"""

import argparse
import sys

import yaml

from arc_car_config.loader import CarConfigError, car_names, cars_dir, config_hash, load, top_speed


def check(directory):
    names = car_names(directory)
    if not names:
        print(f'no car files in {directory}', file=sys.stderr)
        return 1
    failed = 0
    for name in names:
        try:
            values = load(name, directory)
        except CarConfigError as e:
            print(f'FAIL {e}', file=sys.stderr)
            failed += 1
            continue
        print(f'ok   {name} ({values["build"]}): config {config_hash(values)[:12]}, '
              f'top speed {top_speed(values):.1f} m/s')
    return 1 if failed else 0


def show(name, directory):
    try:
        values = load(name, directory)
    except CarConfigError as e:
        print(e, file=sys.stderr)
        return 1
    print(yaml.safe_dump(values, sort_keys=False), end='')
    print(f'# hash: {config_hash(values)}')
    print(f'# top speed: {top_speed(values):.2f} m/s')
    return 0


def main(argv=None):
    parser = argparse.ArgumentParser(prog='car_config', description=__doc__.splitlines()[0])
    sub = parser.add_subparsers(dest='command', required=True)
    p = sub.add_parser('check', help='load and validate every car file')
    p.add_argument('dir', nargs='?', help='directory of car files')
    p = sub.add_parser('show', help="print a car's merged configuration")
    p.add_argument('car')
    p.add_argument('--dir', help='directory of car files')
    args = parser.parse_args(argv)
    directory = args.dir or cars_dir()
    return check(directory) if args.command == 'check' else show(args.car, directory)


if __name__ == '__main__':
    sys.exit(main())
