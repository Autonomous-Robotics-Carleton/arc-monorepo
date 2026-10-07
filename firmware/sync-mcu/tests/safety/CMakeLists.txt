cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(sync_mcu_safety_test)

# The module under test, from the application
target_sources(app PRIVATE src/main.c ../../app/src/safety.c)
target_include_directories(app PRIVATE ../../app/src)
