#!/usr/bin/env bash
# Firmware in the loop (phase 6): the sync MCU firmware on native_sim and
# the ROS car backend exchange ARC_LINK_STATUS over UDP on this machine
# (ICD-sync-link). Passes when both log "sync link up". Needs the
# native_sim firmware and the ROS workspace built (nx build sync-mcu ros).
set -euo pipefail

cd "$(git rev-parse --show-toplevel)"
fw=firmware/sync-mcu/build/native_sim/zephyr/zephyr.exe
[ -x "$fw" ] || { echo "no $fw: run nx build sync-mcu" >&2; exit 1; }
[ -f ros/install/setup.bash ] || { echo "no ros/install: run nx build ros" >&2; exit 1; }

logs="$(mktemp -d)"
cleanup() { kill "${pids[@]}" 2>/dev/null || true; rm -rf -- "$logs"; }
pids=()
trap cleanup EXIT

"$fw" -rt > "$logs/firmware.log" 2>&1 &
pids+=($!)
# setup.bash reads unset variables
(set +u; source ros/install/setup.bash; exec ros2 run arc_backend_car backend) > "$logs/ros.log" 2>&1 &
pids+=($!)

for _ in $(seq 1 30); do
  if grep -q "sync link up" "$logs/firmware.log" && grep -q "sync link up" "$logs/ros.log"; then
    grep -h "sync link up" "$logs/firmware.log" "$logs/ros.log"
    echo "firmware in the loop: OK"
    exit 0
  fi
  sleep 0.5
done

echo "firmware in the loop: no link within 15 s" >&2
echo "--- firmware" >&2; tail -20 "$logs/firmware.log" >&2
echo "--- ros" >&2; tail -20 "$logs/ros.log" >&2
exit 1
