cmake_minimum_required(VERSION 3.8)
project(arc_bringup)

find_package(ament_cmake REQUIRED)

install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(launch_testing_ament_cmake REQUIRED)
  # one launch test per backend: the platform must come up on each
  foreach(sim gazebo webots gym)
    add_launch_test(test/test_target.py
      TARGET test_target_sim_${sim}
      ARGS "target:=sim" "sim:=${sim}"
      TIMEOUT 60)
  endforeach()
  foreach(target replay car)
    add_launch_test(test/test_target.py
      TARGET test_target_${target}
      ARGS "target:=${target}"
      TIMEOUT 60)
  endforeach()
endif()

ament_package()
