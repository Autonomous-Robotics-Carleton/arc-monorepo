# Our additions to the VESC firmware, built and tested on the host. arc.mk
# joins them to the firmware build (firmware/vesc/bldc).
cmake_minimum_required(VERSION 3.20)
project(arc_vesc C)

add_library(arc_vesc src/estop.c src/telemetry.c src/speed_limit.c)
target_include_directories(arc_vesc PUBLIC include)
target_compile_options(arc_vesc PRIVATE -Wall -Wextra -Werror)

enable_testing()

# The e-stop routine is a safety function: its own suite (ADR-0012).
add_executable(test_estop tests/test_estop.c)
target_link_libraries(test_estop arc_vesc m)
add_test(NAME estop COMMAND test_estop)

# Every input sequence against what the ADRs ask for, written apart from the
# worked cases above.
add_executable(test_estop_props tests/test_estop_props.c)
target_link_libraries(test_estop_props arc_vesc m)
add_test(NAME estop_props COMMAND test_estop_props)

# Its constants against config/cars/ and the ADRs they come from.
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_test(NAME estop_values
  COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/tests/estop_values.py
          ${CMAKE_CURRENT_SOURCE_DIR}/../../..)
# Its hooks in upstream's code, which an upstream update could drop.
add_test(NAME estop_hooks
  COMMAND Python3::Interpreter ${CMAKE_CURRENT_SOURCE_DIR}/tests/estop_hooks.py
          ${CMAKE_CURRENT_SOURCE_DIR}/../bldc)

add_executable(test_stubs tests/test_stubs.c)
target_link_libraries(test_stubs arc_vesc)
add_test(NAME stubs COMMAND test_stubs)
