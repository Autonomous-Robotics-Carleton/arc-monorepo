#!/usr/bin/env bash
# Checks the dev container has working toolchains. CI runs it inside every
# newly built image; run it yourself after rebuilding the container.
set -euo pipefail

echo "== versions"
west --version
cmake --version | head -1
"$ZEPHYR_SDK_INSTALL_DIR/gnu/arm-zephyr-eabi/bin/arm-zephyr-eabi-gcc" --version | head -1
node --version
pnpm --version

# Build from outside the Zephyr workspace on purpose: west must find it
# through ZEPHYR_BASE, the way firmware in this repo is built.
out="$(mktemp -d)"
sample="$ZEPHYR_BASE/samples/hello_world"

echo "== native_sim: build and run"
west build -p always -b native_sim "$sample" -d "$out/native_sim" -- -DCONFIG_COMPILER_WARNINGS_AS_ERRORS=y
"$out/native_sim/zephyr/zephyr.exe" -stop_at=1 | tee "$out/native_sim.log"
grep -q "Hello World" "$out/native_sim.log"

echo "== nucleo_h723zg: build (Arm toolchain, STM32 HAL)"
west build -p always -b nucleo_h723zg "$sample" -d "$out/nucleo_h723zg"
test -f "$out/nucleo_h723zg/zephyr/zephyr.elf"

echo "== ROS 2 Jazzy"
# setup.bash reads unset variables, so relax -u while sourcing it
set +u; source /opt/ros/jazzy/setup.bash; set -u
ros2 pkg prefix rclcpp > /dev/null
colcon --help > /dev/null
python3 -c "import rclpy, launch_testing"

echo "== dev container OK"
