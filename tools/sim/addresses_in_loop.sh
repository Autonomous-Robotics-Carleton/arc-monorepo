#!/usr/bin/env bash
# The sync link on the car's addresses (ICD-network, set 2026-10-25), with no
# shared loopback: the firmware (native_sim) and the fake Orin run in two
# containers on a Docker network of 192.168.70.0/24 with no gateway and no
# route out. If they talk, each used the other's address and port.
# Run on the host (it starts containers), from the repo root:
#   tools/sim/addresses_in_loop.sh
set -euo pipefail
cd "$(git rev-parse --show-toplevel)"

image=${ARC_DEV_IMAGE:-ghcr.io/autonomous-robotics-carleton/arc-dev:latest}
net=arcnet-$$
mcu=192.168.70.2
orin=192.168.70.1
orin_actual=${ARC_TEST_ORIN_IP:-$orin} # set to another address to see the checks fail
build=firmware/sync-mcu/build/native_sim_addr
run=(docker run --rm --platform linux/amd64 -v "$PWD":/work -w /work)
fails=0
logs="$(mktemp -d)"
cleanup() { docker rm -f "$net-mcu" >/dev/null 2>&1 || true; docker network rm "$net" >/dev/null 2>&1 || true; rm -rf -- "$logs"; }
trap cleanup EXIT

check() { # NAME CONDITION-EXIT-STATUS DETAIL
  if [ "$2" -eq 0 ]; then echo "ok   $1"; else echo "FAIL $1: $3"; fails=$((fails + 1)); fi
}

# the firmware's peer is the Orin's address; the ports are the defaults
"${run[@]}" "$image" bash -lc "west build -p auto -b native_sim firmware/sync-mcu/app -d $build -- \
  -DCONFIG_ARC_SYNC_LINK_PEER=\\\"$orin\\\" > /dev/null"

# Docker takes .1 for its bridge unless told otherwise, and .1 is the Orin (ICD-network)
docker network create --internal --subnet 192.168.70.0/24 --gateway 192.168.70.254 "$net" > /dev/null

# the sync MCU, 6 s
docker run -d --name "$net-mcu" --network "$net" --ip $mcu --platform linux/amd64 \
  -v "$PWD":/work -w /work "$image" bash -lc "timeout 6 $build/zephyr/zephyr.exe -rt" > /dev/null

# the Orin side: heartbeat, envelope, commands for 4 s
"${run[@]}" --network "$net" --ip $orin_actual "$image" bash -lc \
  "python3 -W ignore tools/sim/sim.py --orin udp:52001:$mcu:52000 \
   --scenario tools/sim/scenarios/orin-driving.json --duration 4" > "$logs/orin.log" 2>&1 || true
sleep 2.5
docker logs "$net-mcu" > "$logs/mcu.log" 2>&1 || true
sed -i 's/\x1b\[[0-9;]*m//g' "$logs/mcu.log"

grep -q "sending to $orin:52001" "$logs/mcu.log" && r=0 || r=1; check "the firmware sends to $orin:52001" $r "$(grep -E 'listening' "$logs/mcu.log")"
grep -q "sync link up" "$logs/mcu.log" && r=0 || r=1; check "the firmware hears the Orin ($orin) on $mcu:52000" $r "no 'sync link up'"
grep -q "ARC_LINK_STATUS" "$logs/orin.log" && r=0 || r=1; check "the Orin hears the firmware on $orin:52001" $r "$(tail -2 "$logs/orin.log")"
# with no controllers attached the fault latch can't reset (ADR-0039), so the re-arm is refused
grep -q "command rate: " "$logs/mcu.log" && r=0 || r=1; check "the Orin's commands arrive" $r "$(grep safety "$logs/mcu.log" | tail -2)"
grep -q "re-arm refused: latched" "$logs/mcu.log" && r=0 || r=1; check "the Orin's re-arm arrives (refused: no reset without controllers)" $r "$(grep -E 're-arm|safety' "$logs/mcu.log" | tail -2)"
grep -q "ARC_SAFETY_STATE" "$logs/orin.log" && r=0 || r=1; check "the Orin gets ARC_SAFETY_STATE" $r "$(tail -2 "$logs/orin.log")"

# no default gateway: the car's internal network has none (ADR-0015 rule 5)
out=$("${run[@]}" --network "$net" --ip 192.168.70.30 "$image" python3 -c '
import socket
dest = [l.split()[1] for l in open("/proc/net/route").read().splitlines()[1:]]
print("default" if "00000000" in dest else "none")') || true
[ "$out" = none ] && r=0 || r=1; check "no default route on the internal network" $r "$out"

# 52001 reserved: an explicit bind works, and the ephemeral ports never hand it out
out=$(docker run --rm --platform linux/amd64 --sysctl net.ipv4.ip_local_port_range="52000 52002" \
  --sysctl net.ipv4.ip_local_reserved_ports=52001 "$image" python3 -c '
import socket
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM); s.bind(("0.0.0.0", 52001)); s.close()
seen = set()
for _ in range(400):
    t = socket.socket(socket.AF_INET, socket.SOCK_DGRAM); t.bind(("0.0.0.0", 0))
    seen.add(t.getsockname()[1]); t.close()
print(sorted(seen))' 2>&1) || true
[ "$out" = "[52000, 52002]" ] || [ "$out" = "[52000]" ] || [ "$out" = "[52002]" ] && r=0 || r=1
check "ephemeral ports skip the reserved 52001 (bind to it still works)" $r "$out"

[ "$fails" -eq 0 ] && echo "addresses in the loop: OK" || { echo "addresses in the loop: $fails failed"; exit 1; }
