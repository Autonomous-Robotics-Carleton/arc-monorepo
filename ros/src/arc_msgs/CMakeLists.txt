cmake_minimum_required(VERSION 3.8)
project(arc_msgs)

find_package(ament_cmake REQUIRED)
find_package(rosidl_default_generators REQUIRED)
find_package(std_msgs REQUIRED)

# The car interface (ICD ros2-msgs). Standard types (sensor_msgs, nav_msgs,
# diagnostic_msgs) are used where they fit; these are the rest.
rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/AngleSample.msg"
  "msg/CameraTrigger.msg"
  "msg/ControlMode.msg"
  "msg/DriveCommand.msg"
  "msg/Envelope.msg"
  "msg/Fault.msg"
  "msg/Heartbeat.msg"
  "msg/LinkStatus.msg"
  "msg/MotorStatus.msg"
  "msg/Rearm.msg"
  "msg/SafetyState.msg"
  "msg/SteeringStatus.msg"
  "srv/SetControlMode.srv"
  DEPENDENCIES std_msgs
)

ament_export_dependencies(rosidl_default_runtime)
ament_package()
