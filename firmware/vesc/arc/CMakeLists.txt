# Our additions to the VESC firmware, built and tested on the host. They
# join the firmware build (firmware/vesc/bldc) as they're implemented.
cmake_minimum_required(VERSION 3.20)
project(arc_vesc C)

add_library(arc_vesc src/estop.c src/telemetry.c src/speed_limit.c)
target_include_directories(arc_vesc PUBLIC include)
target_compile_options(arc_vesc PRIVATE -Wall -Wextra -Werror)

enable_testing()
add_executable(test_stubs tests/test_stubs.c)
target_link_libraries(test_stubs arc_vesc)
add_test(NAME stubs COMMAND test_stubs)
