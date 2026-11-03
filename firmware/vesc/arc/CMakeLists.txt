# Our additions to the VESC firmware, built and tested on the host. arc.mk
# joins them to the firmware build (firmware/vesc/bldc).
cmake_minimum_required(VERSION 3.20)
project(arc_vesc C)

add_library(arc_vesc src/estop.c src/telemetry.c src/speed_limit.c)
target_include_directories(arc_vesc PUBLIC include)
target_compile_options(arc_vesc PRIVATE -Wall -Wextra -Werror)

enable_testing()
add_executable(test_stubs tests/test_stubs.c)
target_link_libraries(test_stubs arc_vesc)
add_test(NAME stubs COMMAND test_stubs)

add_executable(test_speed_limit tests/test_speed_limit.c)
target_link_libraries(test_speed_limit arc_vesc m)
add_test(NAME speed_limit COMMAND test_speed_limit)

# Every kind of configured pair, written apart from the worked cases above.
add_executable(test_speed_limit_props tests/test_speed_limit_props.c)
target_link_libraries(test_speed_limit_props arc_vesc m)
add_test(NAME speed_limit_props COMMAND test_speed_limit_props)

# The limit against config/cars/, ADR-0034 and SYS-01.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME speed_limit_values
  COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/tests/speed_limit_values.py
          ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
