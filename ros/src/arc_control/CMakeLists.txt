cmake_minimum_required(VERSION 3.8)
project(arc_control)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(arc_msgs REQUIRED)

# The rules (arbiter) and their ROS wiring (control_node), shared by the
# executable and the tests
add_library(arc_control_node STATIC src/arbiter.cpp src/control_node.cpp)
target_include_directories(arc_control_node PUBLIC include)
target_compile_features(arc_control_node PUBLIC cxx_std_17)
ament_target_dependencies(arc_control_node PUBLIC rclcpp arc_msgs)

add_executable(control src/main.cpp)
target_link_libraries(control arc_control_node)

install(TARGETS control DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  # The rules on made-up times, compiled with the sanitizers: on the command
  # path, so memory errors and undefined behaviour fail the test
  ament_add_gtest(test_arbiter test/test_arbiter.cpp src/arbiter.cpp)
  target_include_directories(test_arbiter PRIVATE include)
  target_compile_features(test_arbiter PUBLIC cxx_std_17)
  target_compile_options(test_arbiter PRIVATE -fsanitize=address,undefined -fno-sanitize-recover=all)
  target_link_options(test_arbiter PRIVATE -fsanitize=address,undefined)
  ament_target_dependencies(test_arbiter rclcpp arc_msgs)
  # The node on a ROS graph
  ament_add_gtest(test_control_node test/test_control_node.cpp TIMEOUT 120)
  target_link_libraries(test_control_node arc_control_node)
endif()

ament_package()
