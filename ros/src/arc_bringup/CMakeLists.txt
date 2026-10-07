cmake_minimum_required(VERSION 3.8)
project(arc_bringup)

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(launch_testing_ament_cmake REQUIRED)
  # one launch test per target: the platform must come up on each
  foreach(target sim replay car)
    add_launch_test(test/test_target.py
      TARGET test_target_${target}
      ARGS "target:=${target}"
      TIMEOUT 60)
  endforeach()
endif()

ament_package()
