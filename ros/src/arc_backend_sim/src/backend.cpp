// Simulator backend of the car interface. One package per simulator comes with ADR-0032; this stub stands in for them.
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("sim_backend");
  RCLCPP_INFO(node->get_logger(), "sim backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
