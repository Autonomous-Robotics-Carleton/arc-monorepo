// F1TENTH gym simulator backend: 2D, fast, for planning and control experiments and CI (ADR-0032).
// Stub: it only says it's running.

#include <rclcpp/rclcpp.hpp>

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<rclcpp::Node>("sim_gym_backend");
  RCLCPP_INFO(node->get_logger(), "sim (gym) backend: not implemented yet");
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
