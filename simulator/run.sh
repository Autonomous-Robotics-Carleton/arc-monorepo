#!/usr/bin/env bash
# Build (cached) and start the simulator.
#   ./run.sh           headless
#   ./run.sh --bridge  for the ground station: bridge on 127.0.0.1:8765, car_link on 127.0.0.1:9870/udp
#   ./run.sh --gui3d   Gazebo 3D GUI in your browser via noVNC (http://localhost:6080); CPU-heavy
#   --verbose          show Docker's full build output instead of one line per step
# There is no track option: the ground station picks the track and loads it through the bridge.
# Every phase says what it is doing; anything that waits says so every few seconds.
set -euo pipefail
cd "$(dirname "$0")"

usage="usage: $0 [--bridge | --gui3d] [--verbose]"
mode=""
verbose=""
while [ $# -gt 0 ]; do
  case "$1" in
    --bridge) mode=bridge ;;
    --gui3d) mode=3d ;;
    --verbose) verbose=1 ;;
    *) echo "$usage" >&2; exit 2 ;;
  esac
  shift
done

say() { echo "run.sh: $*"; }
fail() { echo "run.sh: $*" >&2; exit 1; }

DOCKER_WAIT=60  # s to wait for the Docker engine (Docker Desktop takes a while to start)
BRIDGE_WAIT=60  # s to wait for the bridge or viewer to answer

# `docker info` can hang while Docker Desktop is starting, so each try gets 5 s.
docker_up() {
  docker info >/dev/null 2>&1 &
  local pid=$! i
  for i in $(seq 50); do
    kill -0 "$pid" 2>/dev/null || { wait "$pid"; return; }
    sleep 0.1
  done
  kill "$pid" 2>/dev/null; wait "$pid" 2>/dev/null
  return 1
}

context=$(docker context show 2>/dev/null || echo unknown)
say "checking docker (context: $context)..."
start=$SECONDS
until docker_up; do
  waited=$((SECONDS - start))
  [ "$waited" -ge "$DOCKER_WAIT" ] && fail "docker isn't running after ${DOCKER_WAIT}s (context: $context). Start Docker Desktop and try again."
  say "waiting for docker... ${waited}s"
  sleep 5
done
say "docker is up"

if [ -n "$(docker ps -aq -f name=^arc-sim$)" ]; then
  fail "an arc-sim container already exists. Stop it first: docker stop arc-sim"
fi

# Name whatever already holds a port we publish, rather than leave Docker's "port is already allocated".
port_owner() {  # $1 port, $2 tcp|udp
  local c
  c=$(docker ps --filter "publish=$1/$2" --format '{{.Names}}' | head -1)
  if [ -n "$c" ]; then echo "container $c (stop it: docker stop $c)"; return; fi
  if [ "$2" = tcp ]; then
    lsof -nP -iTCP:"$1" -sTCP:LISTEN 2>/dev/null | awk 'NR == 2 {print "process " $1 " (pid " $2 ")"}'
  else
    lsof -nP -iUDP:"$1" 2>/dev/null | awk 'NR == 2 {print "process " $1 " (pid " $2 ")"}'
  fi
}
ports=()
[ "$mode" = bridge ] && ports=(8765/tcp 9870/udp)
[ "$mode" = 3d ] && ports=(6080/tcp)
for p in ${ports[@]+"${ports[@]}"}; do
  owner=$(port_owner "${p%/*}" "${p#*/}") || true  # lsof exits 1 when nothing holds the port
  [ -n "$owner" ] && fail "port $p is already in use by $owner"
done

say "building image arc-sim (the first build takes several minutes; later ones are cached)..."
start=$SECONDS
log=$(mktemp -t arc-sim-build)
set +e
if [ -n "$verbose" ]; then
  docker build --progress=plain -t arc-sim . 2>&1 | tee "$log"
  built=${PIPESTATUS[0]}
else
  # One line per build step: "#7 [4/9] RUN apt-get ..." plus "CACHED" when it was.
  docker build --progress=plain -t arc-sim . 2>&1 | tee "$log" \
    | grep --line-buffered -E '^#[0-9]+ (\[|CACHED)' | awk '{ print "  " $0; fflush() }'
  built=${PIPESTATUS[0]}
fi
set -e
if [ "$built" -ne 0 ]; then
  echo "run.sh: build failed; last lines of the build log ($log):" >&2
  tail -20 "$log" >&2
  exit 1
fi
rm -f "$log"
say "image ready ($((SECONDS - start))s)"

# Sensor settings (.env next to this script); the car model falls back to defaults without it.
env=()
if [ -f .env ]; then
  env=(--env-file .env)
  say "sensor settings: $(pwd)/.env"
else
  say "no .env: sensors use the car model's defaults"
fi

if [ -z "$mode" ]; then
  say "starting arc-sim headless, no ports published (Ctrl+C stops it)"
  exec docker run --rm --name arc-sim ${env[@]+"${env[@]}"} arc-sim
fi

# Ports are bound to this machine only; nothing on them has a password.
if [ "$mode" = bridge ]; then
  say "starting arc-sim: bridge on 127.0.0.1:8765, car_link on 127.0.0.1:9870/udp (Ctrl+C stops it)"
  docker run --rm --name arc-sim ${env[@]+"${env[@]}"} -e BRIDGE=1 \
    -p 127.0.0.1:8765:8765 -p 127.0.0.1:9870:9870/udp arc-sim &
  probe="http://localhost:8765/track.json"
else
  say "starting arc-sim: 3D viewer on 127.0.0.1:6080 (Ctrl+C stops it)"
  docker run --rm --name arc-sim ${env[@]+"${env[@]}"} -e GUI=3d -p 127.0.0.1:6080:6080 arc-sim &
  probe="http://localhost:6080/vnc.html"
fi
sim=$!
# Stop the container on Ctrl+C, or when the ground station terminates this script.
trap 'docker stop arc-sim >/dev/null 2>&1; wait "$sim" 2>/dev/null' INT TERM

up=""
start=$SECONDS
while [ $((SECONDS - start)) -lt "$BRIDGE_WAIT" ]; do
  curl -sf -o /dev/null "$probe" && { up=1; break; }
  kill -0 "$sim" 2>/dev/null || break
  sleep 1
  waited=$((SECONDS - start))
  [ $((waited % 5)) -eq 0 ] && say "waiting for $probe... ${waited}s"
done
if [ -z "$up" ]; then
  if kill -0 "$sim" 2>/dev/null; then
    say "not answering after ${BRIDGE_WAIT}s: $probe. Still running; its logs are above"
  else
    wait "$sim" && code=0 || code=$?
    fail "the container exited (code $code) before $probe answered; its logs are above"
  fi
elif [ "$mode" = bridge ]; then
  say "bridge ready: http://localhost:8765/track.json, ws://localhost:8765/ws; car_link: udp://127.0.0.1:9870"
else
  url="http://localhost:6080/vnc.html?autoconnect=1&resize=scale"
  say "viewer: $url"
  open "$url" 2>/dev/null || true
fi
wait "$sim"
