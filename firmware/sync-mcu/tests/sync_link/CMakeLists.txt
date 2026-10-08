cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sync_mcu_sync_link_test)

# The generated sync-link code, from the application
target_sources(app PRIVATE src/main.c)
target_include_directories(app PRIVATE ../../app/generated/arc_mavlink)
