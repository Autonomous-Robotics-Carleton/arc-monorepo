cmake_minimum_required(VERSION 3.8)
project(arc_backend_sim_gym)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# Python, since the gym is: its module, the executable and the launch file.
# The gym itself is installed in the dev container (.devcontainer/requirements-gym.txt)
ament_python_install_package(${PROJECT_NAME})
install(PROGRAMS scripts/backend DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)
  # The vehicle model and wheel kinematics, without the gym or ROS
  ament_add_pytest_test(test_vehicle test/test_vehicle.py
    APPEND_ENV PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR})
  # The car in the gym, without ROS: driving, steering, the LiDAR, collisions
  ament_add_pytest_test(test_sim test/test_sim.py
    APPEND_ENV PYTHONPATH=${CMAKE_CURRENT_SOURCE_DIR}
    TIMEOUT 120)
  # Closed loop: an experiment drives a few metres through arc_control
  add_launch_test(test/test_drive.py TIMEOUT 120)
endif()

ament_package()
