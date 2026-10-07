// Log-replay backend of the car interface: plays MCAP logs back as if they were the car (ADR-0028).
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("replay_backend");
  RCLCPP_INFO(node->get_logger(), "replay backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
