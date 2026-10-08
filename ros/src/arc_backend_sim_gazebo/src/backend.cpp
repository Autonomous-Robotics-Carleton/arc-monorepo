// Gazebo Harmonic simulator backend: the full sensor suite, event cameras included; Linux with a GPU (ADR-0032).
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("sim_gazebo_backend");
  RCLCPP_INFO(node->get_logger(), "sim (gazebo) backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
