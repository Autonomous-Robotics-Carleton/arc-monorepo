"""Template experiment: does nothing yet.

An experiment publishes one vehicle command: speed, acceleration limit and
steering angle, through the safety envelope (SYS-22, ADR-0048), as
arc_msgs/DriveCommand on /control/experiment (ICD ros2-msgs). It drives only
while arc_control gives it control: it keeps publishing, at least every
100 ms (TBC), and resets its controller when /control/mode becomes
MODE_EXPERIMENT ("Choosing who drives").
"""


def main():
    print('template experiment: not implemented yet')


if __name__ == '__main__':
    main()
