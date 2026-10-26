#!/usr/bin/env bash
# The sync MCU firmware (native_sim) against the fake motor controllers.
# Run in the dev container, from the repo root:
#   tools/sim/firmware_in_loop.sh
#
# The firmware gets a copy built with a 100 ms silence window and 500 ms
# heartbeat and command timeouts: a laptop running Python and native_sim in
# real time can't hold the real 5 ms and 150 ms (the unit tests in
# firmware/sync-mcu/tests keep those values).
set -euo pipefail
cd "$(git rev-parse --show-toplevel)"

build=firmware/sync-mcu/build/native_sim_sim
west build -p auto -b native_sim firmware/sync-mcu/app -d "$build" -- \
  -DCONFIG_ARC_CONTROLLER_SILENCE_MS=100 -DCONFIG_ARC_HEARTBEAT_TIMEOUT_MS=500 \
  -DCONFIG_ARC_COMMAND_TIMEOUT_MS=500 > /dev/null
fw="$build/zephyr/zephyr.exe"

logs="$(mktemp -d)"
trap 'kill $fw_pid 2>/dev/null || true; rm -rf -- "$logs"' EXIT
fails=0

# run NAME SCENARIO SECONDS EXPECT_REGEX FORBID_REGEX [ORIN_EXPECT_REGEX [BUS_CHECK]]
# The fake Orin joins when ORIN_EXPECT_REGEX is given: it is matched against
# the simulator's report of what the Orin received from the firmware.
# The fake command bus joins when BUS_CHECK is given: "COUNT:REGEX" wants at
# least COUNT lines of the simulator's bus report to match (one per corner).
run() {
  local name=$1 scenario=$2 secs=$3 expect=$4 forbid=$5 orin_expect=${6:-} bus_check=${7:-}
  local orin_args=()
  [ -z "$orin_expect" ] || orin_args=(--orin udp:52001:127.0.0.1:52000)
  [ -z "$bus_check" ] || orin_args+=(--can udp:52101:127.0.0.1:52100)
  "$fw" -rt > "$logs/fw.log" 2>&1 &
  fw_pid=$!
  for _ in $(seq 1 50); do
    [ "$(grep -c 'uart_ctrl_.* connected to pseudotty' "$logs/fw.log" 2>/dev/null || true)" -ge 4 ] && break
    sleep 0.1
  done
  pts() { sed -n "s|^uart_ctrl_$1 connected to pseudotty: ||p" "$logs/fw.log"; }
  python3 -W ignore tools/sim/sim.py \
    --link "FL=$(pts fl)" --link "FR=$(pts fr)" --link "RL=$(pts rl)" --link "RR=$(pts rr)" \
    "${orin_args[@]}" --scenario "tools/sim/scenarios/$scenario" --duration "$secs" > "$logs/sim.log" 2>&1 || {
      echo "FAIL $name: the simulator failed"; cat "$logs/sim.log"; fails=$((fails + 1)); }
  # what the firmware had logged the moment the controllers fell silent for good
  cp "$logs/fw.log" "$logs/fw.at-end.log"
  sleep 0.3
  kill "$fw_pid" 2>/dev/null || true
  wait "$fw_pid" 2>/dev/null || true
  if ! grep -Eq "$expect" "$logs/fw.log"; then
    echo "FAIL $name: expected /$expect/"; grep -E "controller|stop|command rate" "$logs/fw.log" | tail -8
    fails=$((fails + 1))
  elif [ -n "$forbid" ] && grep -Eq "$forbid" "$logs/fw.at-end.log"; then
    echo "FAIL $name: saw /$forbid/"; grep -E "controller|stop" "$logs/fw.at-end.log" | tail -8
    fails=$((fails + 1))
  elif [ -n "$orin_expect" ] && ! grep -Eq "$orin_expect" "$logs/sim.log"; then
    echo "FAIL $name: the Orin should have received /$orin_expect/"; tail -3 "$logs/sim.log"
    fails=$((fails + 1))
  elif [ -n "$bus_check" ] && [ "$(grep -Ec "${bus_check#*:}" "$logs/sim.log")" -lt "${bus_check%%:*}" ]; then
    echo "FAIL $name: wanted ${bus_check%%:*} bus lines matching /${bus_check#*:}/"; grep '^bus' "$logs/sim.log"
    fails=$((fails + 1))
  else
    local line
    line=$(sed 's/\x1b\[[0-9;]*m//g' "$logs/fw.at-end.log" | grep -E 'any-corner stop' | head -1 || true)
    [ -n "$line" ] || line=$(sed 's/\x1b\[[0-9;]*m//g' "$logs/fw.at-end.log" | grep -E 'safety: (driving|heartbeat|no fresh|controller stop)' | tail -1 || true)
    [ -n "$line" ] || line=$(sed 's/\x1b\[[0-9;]*m//g' "$logs/fw.at-end.log" | grep -E 'controller links: ok' | head -1 || true)
    echo "ok   $name: ${line#*> }"
    if [ -n "$bus_check" ]; then grep '^bus' "$logs/sim.log" | sed 's/^/       /' | head -4; fi
  fi
}

run "all four corners healthy"    healthy.json            4 'controller links: ok'              'any-corner stop'
run "FL silent for 300 ms"        silent-fl-300ms.json    4 'any-corner stop: FL silent'        'any-corner stop: (FR|RL|RR)'
run "FR reports a fault"          fault-fr.json           4 'any-corner stop: FR fault'         'any-corner stop: (FL|RL|RR)'
run "RL command timeout"          command-timeout-rl.json 4 'any-corner stop: RL command timeout' 'any-corner stop: (FL|FR|RR)'
run "RR e-stop"                   estop-rr.json           4 'any-corner stop: RR e-stop'        'any-corner stop: (FL|FR|RL)'

# with the fake Orin: heartbeat, commands and envelope over the sync link (watchdog 2 = BRAKING)
bus_line='^bus (FL|FR|RL|RR):'
# every corner's SET_RPM stream (4 frames per tick, 500 Hz) never goes quiet; no brake
run "Orin drives, inside the envelope"  orin-driving.json         3 'safety: driving.*speed=2000 mm/s accel=1000 mm/s2 steer=100 mrad' 'heartbeat lost|controller stop|clamped' 'ARC_SAFETY_STATE.*watchdog=0' "4:$bus_line.* rpm=[0-9]{3,} brake=0 .*max_gap_ms=([0-9]|[1-3][0-9]|4[0-9])\\."
run "Orin command rate is measured"  orin-driving.json         4 'command rate: (4[5-9][0-9]|5[0-4][0-9]) Hz' 'command rate: [0-9]+ Hz, expected' 'ARC_SAFETY_STATE'
run "Orin commands beyond the envelope" orin-clamp.json           3 'safety: driving \[clamped to the envelope\] speed=3000 mm/s accel=5000 mm/s2 steer=400 mrad' 'heartbeat lost|controller stop' 'max_speed=3 max_accel=5 max_steer=0.4'
# the heartbeat is lost at 1.5 s, the watchdog (500 ms here) trips, all four get brake frames
run "heartbeat stops at 1.5 s"          orin-heartbeat-loss.json  4 'safety: heartbeat lost: braking \[brake\]' 're-armed after' 'watchdog=2' "4:$bus_line.* brake=[1-9][0-9]* first_brake_ms=(19[5-9][0-9]|20[0-9][0-9])\\."
# ADR-0052: a re-arm while the heartbeat is lost is refused; once it's back, a re-arm drives again
run "re-arm after a heartbeat stop"     orin-heartbeat-rearm.json 4 're-armed after a heartbeat stop' '' 'watchdog=0'
# commands stop but the heartbeat goes on: a zero command, and the bus stays alive
run "commands stop, heartbeat goes on"  orin-command-stop.json    4 'safety: no fresh command' 'heartbeat lost|\[brake\]' 'watchdog=0' "4:$bus_line.* current=[0-9]{3,} .* brake=0 .*max_gap_ms=([0-9]|[1-3][0-9]|4[0-9])\\."
# a corner fault at 1.5 s: all four corners get brake frames within 60 ms (SYS-25 wants 20 ms: a laptop can't hold it)
run "FR fault while the Orin drives"    orin-driving-fault-fr.json 4 'safety: controller stop: braking \[brake\]' '' 'watchdog=0' "4:$bus_line.* brake=[1-9][0-9]* first_brake_ms=(15[0-5][0-9])\\."

[ "$fails" -eq 0 ] && echo "firmware in the loop: OK" || { echo "firmware in the loop: $fails failed"; exit 1; }
