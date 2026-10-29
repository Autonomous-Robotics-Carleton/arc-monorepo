cmake_minimum_required(VERSION 3.8)
project(arc_logging)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# The recorder's command line and the run's name and record, shared by the
# launch file and the tests
ament_python_install_package(${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)
  ament_add_pytest_test(test_recorder test/test_recorder.py)
  # A short run with a crashing experiment, read back from the MCAP log
  add_launch_test(test/test_logging.py TIMEOUT 120)
endif()

ament_package()
