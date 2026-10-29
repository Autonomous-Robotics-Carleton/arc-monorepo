cmake_minimum_required(VERSION 3.8)
project(arc_logging)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

# The node, and what it runs (the run's plan, name and record, the
# recorder's command line), shared with the tests
ament_python_install_package(${PROJECT_NAME})
install(PROGRAMS scripts/logger DESTINATION lib/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  find_package(launch_testing_ament_cmake REQUIRED)
  ament_add_pytest_test(test_recorder test/test_recorder.py)
  # A short run in chunks with a crashing experiment, read back from the log
  add_launch_test(test/test_logging.py TIMEOUT 120)
  # The operator is warned on /diagnostics, and the platform isn't stopped
  foreach(case log_off no_log_dir low_space recorder_dies bad_exclude)
    add_launch_test(test/test_warnings.py
      TARGET test_warnings_${case}
      ARGS "case:=${case}"
      TIMEOUT 60)
  endforeach()
endif()

ament_package()
