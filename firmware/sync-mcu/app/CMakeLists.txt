# Sync MCU firmware (ADR-0017). Build for any target from the repo root:
#   west build -b native_sim     firmware/sync-mcu/app -d firmware/sync-mcu/build/native_sim
#   west build -b nucleo_h723zg  firmware/sync-mcu/app -d firmware/sync-mcu/build/nucleo_h723zg
#   west build -b arc_sync       firmware/sync-mcu/app -d firmware/sync-mcu/build/arc_sync
# or `npx nx build sync-mcu` for all three (ADR-0028).

cmake_minimum_required(VERSION 3.20.0)

# Our own board definitions (boards/arc/arc_sync)
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(arc_sync_mcu)

# Generated from the ICDs (tools/gen-interfaces.sh): sync-link messages
# (systems/icd/sync-link.xml, ADR-0031) and command-bus frames
# (systems/icd/can-command.dbc)
target_include_directories(app PRIVATE generated/arc_mavlink generated/can_command)

target_sources(app PRIVATE
  src/main.c
  src/time_base.c
  src/sensors.c
  src/can_buses.c
  generated/can_command/can_command.c
  src/controller_links.c
  src/safety.c
  src/sync_link.c
)
