cmake_minimum_required(VERSION 3.8)
project(arc_backend_replay)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(arc_msgs REQUIRED)
find_package(rosbag2_storage REQUIRED)
find_package(rosbag2_transport REQUIRED)

add_executable(backend src/backend.cpp)
target_compile_features(backend PUBLIC cxx_std_17)
ament_target_dependencies(backend rclcpp arc_msgs rosbag2_storage rosbag2_transport)

install(TARGETS backend DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)
  # The topics played and listened on, against ICD-ros2-msgs
  ament_add_pytest_test(test_topics_contract test/test_topics_contract.py)
  # The sample log (scripts/make_sample_log.py) as recorded, faster and slower
  foreach(rate 1.0 2.0 0.5)
    string(REPLACE "." "_" name ${rate})
    add_launch_test(test/test_replay.py
      TARGET test_replay_rate_${name}
      ARGS "bag:=${CMAKE_CURRENT_SOURCE_DIR}/test/data/sample.mcap" "rate:=${rate}"
      TIMEOUT 60)
  endforeach()
endif()

ament_package()
