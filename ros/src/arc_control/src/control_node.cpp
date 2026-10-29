#include "arc_control/control_node.hpp"

#include <chrono>
#include <stdexcept>
#include <string>

namespace arc_control
{

namespace
{
using arc_msgs::srv::SetControlMode;

// How often a silent source's zero command goes out (TBC, ICD ros2-msgs)
constexpr auto kTickPeriod = std::chrono::milliseconds(10);

// QoS profiles (ICD ros2-msgs)
const rclcpp::QoS kCommands = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().durability_volatile();
const rclcpp::QoS kState = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local();

const char * mode_name(uint8_t mode)
{
  return mode == ControlMode::MODE_EXPERIMENT ? "experiment" : "operator";
}
}  // namespace

ControlNode::ControlNode(const rclcpp::NodeOptions & options)
: Node("arc_control", options)
{
  arbiter_ = std::make_unique<Arbiter>(read_params(),
    [this](uint8_t mode, const std::string & why) {
      RCLCPP_INFO(get_logger(), "control: %s (%s)", mode_name(mode), why.c_str());
      publish_mode();
    });

  drive_pub_ = create_publisher<Command>("car/cmd/drive", kCommands);
  mode_pub_ = create_publisher<ControlMode>("control/mode", kState);
  teleop_sub_ = create_subscription<Command>("control/teleop", kCommands,
    [this](const Command & cmd) {on_input(cmd, true);});
  experiment_sub_ = create_subscription<Command>("control/experiment", kCommands,
    [this](const Command & cmd) {on_input(cmd, false);});
  set_mode_srv_ = create_service<SetControlMode>("control/set_mode",
    [this](const SetControlMode::Request::SharedPtr req, SetControlMode::Response::SharedPtr res) {
      res->reason = arbiter_->request_mode(req->mode, now());
      res->accepted = res->reason.empty();
      if (!res->accepted) {
        RCLCPP_WARN(get_logger(), "control: %s refused (%s)",
          mode_name(req->mode), res->reason.c_str());
      }
    });
  // On the ROS clock, so it follows the simulator's in the twin (ADR-0049)
  tick_timer_ = create_timer(kTickPeriod, [this] {send(arbiter_->on_tick(now()));});

  publish_mode();
  // Logged last: the launch test stops the node as soon as it sees this line
  RCLCPP_INFO(get_logger(), "arc_control: operator mode, waiting for teleop");
}

Params ControlNode::read_params()
{
  const double teleop_timeout = declare_parameter("teleop_timeout", 0.2);  // s
  const double experiment_timeout = declare_parameter("experiment_timeout", 0.1);  // s
  const double experiment_lost = declare_parameter("experiment_lost_timeout", 1.0);  // s
  const double takeover_speed = declare_parameter("takeover_speed", 0.1);  // m/s
  const double takeover_steer = declare_parameter("takeover_steer", 0.05);  // rad

  if (teleop_timeout <= 0 || experiment_timeout <= 0 || experiment_lost < experiment_timeout ||
    takeover_speed < 0 || takeover_steer < 0)
  {
    throw std::invalid_argument(
            "arc_control: timeouts must be positive, experiment_lost_timeout at least "
            "experiment_timeout, and the takeover deadband not negative");
  }
  Params params;
  params.teleop_timeout = rclcpp::Duration::from_seconds(teleop_timeout);
  params.experiment_timeout = rclcpp::Duration::from_seconds(experiment_timeout);
  params.experiment_lost = rclcpp::Duration::from_seconds(experiment_lost);
  params.takeover_speed = takeover_speed;
  params.takeover_steer = takeover_steer;
  return params;
}

void ControlNode::on_input(const Command & cmd, bool teleop)
{
  if (!Arbiter::finite(cmd)) {
    RCLCPP_WARN_THROTTLE(get_logger(), *get_clock(), 1000,
      "control: dropping %s commands with a NaN or infinite field",
      teleop ? "teleop" : "experiment");
  }
  send(teleop ? arbiter_->on_teleop(cmd, now()) : arbiter_->on_experiment(cmd, now()));
}

void ControlNode::send(const std::optional<Command> & cmd)
{
  if (cmd) {
    drive_pub_->publish(*cmd);
  }
}

void ControlNode::publish_mode()
{
  if (!mode_pub_) {
    return;  // not created yet: the constructor publishes once it is
  }
  ControlMode msg;
  msg.header.stamp = now();
  msg.mode = arbiter_->mode();
  mode_pub_->publish(msg);
}

}  // namespace arc_control
