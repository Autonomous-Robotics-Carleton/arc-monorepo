#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "arc_control/control_node.hpp"

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<arc_control::ControlNode>());
  rclcpp::shutdown();
  return 0;
}
