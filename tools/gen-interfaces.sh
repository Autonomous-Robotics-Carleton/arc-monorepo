#!/usr/bin/env bash
# Generates the sync-link MAVLink code from systems/icd/sync-link.xml
# (ADR-0031) into both sides of the link. Generated code is never edited
# by hand.
#
#   tools/gen-sync-link.sh           regenerate
#   tools/gen-sync-link.sh --check   fail if the committed code is stale (CI)
#
# Needs pymavlink (in the dev container).
set -euo pipefail

cd "$(git rev-parse --show-toplevel)"
schema=systems/icd/sync-link.xml
outputs=(
  firmware/sync-mcu/app/generated/arc_mavlink
  ros/src/arc_backend_car/include/arc_mavlink
)

work="$(mktemp -d)"
trap 'rm -rf -- "$work"' EXIT

# mavgen names the dialect after the file, which must be a C identifier
cp "$schema" "$work/arc_sync_link.xml"
python3 -m pymavlink.tools.mavgen --lang=C --wire-protocol=2.0 \
  --output="$work/out" "$work/arc_sync_link.xml" > /dev/null
# mavgen stamps today's date; pin it so the output depends only on the schema
sed -i 's/^#define MAVLINK_BUILD_DATE .*/#define MAVLINK_BUILD_DATE "see systems\/icd\/sync-link.xml"/' \
  "$work/out/arc_sync_link/version.h"

if [ "${1:-}" = "--check" ]; then
  stale=0
  for out in "${outputs[@]}"; do
    if ! diff -r -q "$work/out" "$out" > /dev/null 2>&1; then
      echo "stale: $out (run tools/gen-sync-link.sh)" >&2
      stale=1
    fi
  done
  exit $stale
fi

for out in "${outputs[@]}"; do
  rm -rf "$out"
  mkdir -p "$out"
  cp -r "$work/out/." "$out/"
  echo "generated $out"
done
