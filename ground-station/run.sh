#!/usr/bin/env bash
# Start the ground station: server.py (page + bridge relay, :47380) and helper.py (UDP to the car, :8081).
#   ./run.sh                    real mode; the car's address comes from config.json (car.host)
#   ./run.sh --car 192.168.1.50 override car.host
# Ctrl+C stops both, and the simulator if the server started it.
set -euo pipefail
cd "$(dirname "$0")"

usage="usage: $0 [--car <host>]"
server_args=()
while [ $# -gt 0 ]; do
  case "$1" in
    --car)
      [ $# -ge 2 ] || { echo "$usage" >&2; exit 2; }
      server_args+=("$1" "$2"); shift ;;
    *) echo "$usage" >&2; exit 2 ;;
  esac
  shift
done

if [ ! -x .venv/bin/python ]; then
  echo "Creating .venv..."
  python3 -m venv .venv
fi
# Install only when requirements.txt changed since the last install.
if ! cmp -s requirements.txt .venv/requirements.installed; then
  .venv/bin/pip install -q --disable-pip-version-check -r requirements.txt
  cp requirements.txt .venv/requirements.installed
fi

helper=""
server=""
stop() {
  trap - INT TERM EXIT
  [ -n "$server" ] && kill -TERM "$server" 2>/dev/null || true
  [ -n "$helper" ] && kill -TERM "$helper" 2>/dev/null || true
  # The server may take a few seconds: it stops the simulator if it started it.
  wait 2>/dev/null || true
}
trap 'stop; exit 130' INT
trap 'stop; exit 143' TERM
trap stop EXIT

.venv/bin/python helper.py &
helper=$!
.venv/bin/python server.py ${server_args[@]+"${server_args[@]}"} &
server=$!

url="http://localhost:47380/"
for _ in $(seq 50); do
  curl -s -o /dev/null "$url" && break
  kill -0 "$server" 2>/dev/null || { echo "server.py exited" >&2; exit 1; }
  sleep 0.1
done
echo "Ground station: $url"
if [ "$(uname)" = Darwin ]; then
  open "$url" 2>/dev/null || true
elif command -v xdg-open >/dev/null; then
  xdg-open "$url" >/dev/null 2>&1 || true
fi

# Exit when either process does; the EXIT trap stops the other.
while kill -0 "$server" 2>/dev/null && kill -0 "$helper" 2>/dev/null; do
  sleep 1 &
  wait $! || true
done
echo "A ground station process exited; stopping." >&2
