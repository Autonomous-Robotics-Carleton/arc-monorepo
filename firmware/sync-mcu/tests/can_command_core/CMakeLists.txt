cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sync_mcu_can_command_core_test)

# The modules under test, from the application
target_sources(app PRIVATE
  src/main.c
  ../../app/src/can_command_core.c
  ../../app/src/safety.c
  ../../app/generated/can_command/can_command.c
)
target_include_directories(app PRIVATE ../../app/src ../../app/generated/can_command)
