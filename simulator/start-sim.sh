#!/bin/bash
# Container main process.
#   GUI unset  headless
#   GUI=3d     Gazebo 3D GUI via noVNC on :6080:
#              Gazebo GUI -> Xvfb (rendered by llvmpipe) -> x11vnc -> websockify/noVNC
#   BRIDGE=1   also start the ground-station bridge on :8765 (car_link on :9870/udp always runs)
# The world starts with no track; the ground station loads one through the bridge.
set -e

args=()
[ "${BRIDGE:-}" = 1 ] && args+=("bridge:=true")

case "${GUI:-}" in
  "") exec ros2 launch arc_sim sim.launch.py "${args[@]}" ;;
  3d) ;;
  *) echo "GUI must be 3d or unset, got '$GUI'" >&2; exit 2 ;;
esac

export DISPLAY=:1
Xvfb :1 -screen 0 1600x900x24 -nolisten tcp >/dev/null 2>&1 &
for _ in $(seq 50); do [ -e /tmp/.X11-unix/X1 ] && break; sleep 0.1; done

# Maximise every window (the Gazebo GUI) to fill the virtual screen.
mkdir -p ~/.fluxbox
printf '[app] (name=.*)\n  [Maximized] {yes}\n[end]\n' > ~/.fluxbox/apps
fluxbox >/dev/null 2>&1 &
x11vnc -display :1 -localhost -forever -shared -nopw -quiet -rfbport 5900 >/dev/null 2>&1 &
websockify --web /usr/share/novnc 6080 localhost:5900 >/dev/null 2>&1 &

# Once the GUI is up, make its camera follow the car (right-click the car > Follow does the same).
(
  for _ in $(seq 120); do
    gz service -l 2>/dev/null | grep -qx /gui/follow && break
    sleep 1
  done
  sleep 2
  gz service -s /gui/follow/offset --reqtype gz.msgs.Vector3d --reptype gz.msgs.Boolean \
    --timeout 2000 --req 'x: -1.2, y: -0.6, z: 0.7' >/dev/null 2>&1
  gz service -s /gui/follow --reqtype gz.msgs.StringMsg --reptype gz.msgs.Boolean \
    --timeout 2000 --req 'data: "racecar"' >/dev/null 2>&1
) &

exec ros2 launch arc_sim sim.launch.py gui:=true "${args[@]}"
