cmake_minimum_required(VERSION 3.8)
project(arc_backend_car)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(arc_msgs REQUIRED)

add_executable(backend src/backend.cpp)
target_compile_features(backend PUBLIC cxx_std_17)
ament_target_dependencies(backend rclcpp arc_msgs)

install(TARGETS backend DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

ament_package()
