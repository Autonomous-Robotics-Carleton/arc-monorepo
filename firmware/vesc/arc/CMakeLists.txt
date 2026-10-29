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

add_executable(test_stubs tests/test_stubs.c)
target_link_libraries(test_stubs arc_vesc)
add_test(NAME stubs COMMAND test_stubs)
