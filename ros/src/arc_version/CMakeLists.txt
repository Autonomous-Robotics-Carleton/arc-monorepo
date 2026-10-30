cmake_minimum_required(VERSION 3.8)
project(arc_version NONE)

find_package(ament_cmake REQUIRED)

# The workspace's version, stamped at every build rather than at configure,
# so a rebuild after a new commit never carries the old hash (#160).
set(VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/version.txt)
add_custom_target(version ALL
  COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR} -DOUT=${VERSION_FILE}
          -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/stamp.cmake
  BYPRODUCTS ${VERSION_FILE}
  COMMENT "Stamping the workspace's git version")
install(FILES ${VERSION_FILE} DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_cmake_pytest REQUIRED)
  ament_add_pytest_test(test_version test/test_version.py
    ENV VERSION_FILE=${VERSION_FILE} SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR})
endif()

ament_package()
