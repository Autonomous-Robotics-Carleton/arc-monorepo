cmake_minimum_required(VERSION 3.8)
project(arc_backend_car)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(arc_msgs REQUIRED)

add_executable(backend src/backend.cpp)
# Sync-link messages, generated from systems/icd/sync-link.xml (ADR-0031)
target_include_directories(backend SYSTEM PRIVATE include/arc_mavlink)
target_compile_features(backend PUBLIC cxx_std_17)
ament_target_dependencies(backend rclcpp arc_msgs)

install(TARGETS backend DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_gtest REQUIRED)
  ament_add_gtest(test_sync_link test/test_sync_link.cpp)
  target_include_directories(test_sync_link SYSTEM PRIVATE include/arc_mavlink)
endif()

ament_package()
