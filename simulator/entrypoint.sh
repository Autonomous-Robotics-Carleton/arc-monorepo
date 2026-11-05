#!/bin/bash
set -e
source /opt/ros/jazzy/setup.bash
source /sim_ws/install/setup.bash
exec "$@"
