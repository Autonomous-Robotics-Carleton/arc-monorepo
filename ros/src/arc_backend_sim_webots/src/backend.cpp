// Webots simulator backend: Webots runs on the host with its GPU (macOS, Windows, Linux) and connects to ROS in the dev container (ADR-0032).
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("sim_webots_backend");
  RCLCPP_INFO(node->get_logger(), "sim (webots) backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
