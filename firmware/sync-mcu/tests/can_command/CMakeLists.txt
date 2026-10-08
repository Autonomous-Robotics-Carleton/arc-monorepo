cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sync_mcu_can_command_test)

# The generated command-bus code, from the application
target_sources(app PRIVATE src/main.c ../../app/generated/can_command/can_command.c)
target_include_directories(app PRIVATE ../../app/generated/can_command)
