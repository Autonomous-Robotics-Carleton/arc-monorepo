// arc_control's node: the Arbiter on the ROS graph (ICD ros2-msgs).
// In: control/teleop, control/experiment, control/set_mode.
// Out: car/cmd/drive, control/mode.

#pragma once

#include <memory>

#include <rclcpp/rclcpp.hpp>

#include "arc_control/arbiter.hpp"
#include "arc_msgs/msg/control_mode.hpp"
#include "arc_msgs/msg/drive_command.hpp"
#include "arc_msgs/srv/set_control_mode.hpp"

namespace arc_control
{

class ControlNode : public rclcpp::Node
{
public:
  explicit ControlNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions());

private:
  Params read_params();
  void send(const std::optional<Command> & cmd);
  void publish_mode();
  void on_input(const Command & cmd, bool teleop);

  std::unique_ptr<Arbiter> arbiter_;
  rclcpp::Publisher<Command>::SharedPtr drive_pub_;
  rclcpp::Publisher<ControlMode>::SharedPtr mode_pub_;
  rclcpp::Subscription<Command>::SharedPtr teleop_sub_;
  rclcpp::Subscription<Command>::SharedPtr experiment_sub_;
  rclcpp::Service<arc_msgs::srv::SetControlMode>::SharedPtr set_mode_srv_;
  rclcpp::TimerBase::SharedPtr tick_timer_;
};

}  // namespace arc_control
