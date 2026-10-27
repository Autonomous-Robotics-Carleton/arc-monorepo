cmake_minimum_required(VERSION 3.8)
project(arc_sim)

find_package(ament_cmake REQUIRED)
find_package(ament_cmake_python REQUIRED)

ament_python_install_package(${PROJECT_NAME})

install(DIRECTORY config launch meshes tracks urdf worlds
  DESTINATION share/${PROJECT_NAME})

install(PROGRAMS scripts/bridge.py scripts/car_link.py scripts/lidar_noise.py scripts/imu_noise.py
  DESTINATION lib/${PROJECT_NAME})

ament_package()
