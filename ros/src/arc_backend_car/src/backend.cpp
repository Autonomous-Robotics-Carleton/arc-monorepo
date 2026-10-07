// The car backend: the bridge to the sync MCU over the sync link (ADR-0028, ICD-sync-link, ADR-0024).
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("car_backend");
  RCLCPP_INFO(node->get_logger(), "car backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
