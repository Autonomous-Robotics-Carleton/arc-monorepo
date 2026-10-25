cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sync_mcu_controller_link_test)

# The decision logic from the application: no Zephyr, no I/O
target_sources(app PRIVATE src/main.c ../../app/src/controller_link_core.c)
target_include_directories(app PRIVATE ../../app/src ../../app/generated/arc_mavlink)
