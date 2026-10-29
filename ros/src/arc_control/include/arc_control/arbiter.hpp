// Picks who drives the car: the operator's teleop or the experiment's
// command (#151, ICD ros2-msgs "Choosing who drives"). No ROS graph here, so
// the rules can be tested with made-up times; control_node.cpp wires it up.

#pragma once

#include <cstdint>
#include <functional>
#include <limits>
#include <optional>
#include <string>

#include <rclcpp/duration.hpp>
#include <rclcpp/time.hpp>

#include "arc_msgs/msg/control_mode.hpp"
#include "arc_msgs/msg/drive_command.hpp"

namespace arc_control
{

using Command = arc_msgs::msg::DriveCommand;
using arc_msgs::msg::ControlMode;

// All TBC (ICD ros2-msgs); the node reads them from its parameters
struct Params
{
  rclcpp::Duration teleop_timeout{0, 200'000'000};      // over the operator link
  rclcpp::Duration experiment_timeout{0, 100'000'000};  // on the Orin
  rclcpp::Duration experiment_lost{1, 0};               // then back to the operator
  double takeover_speed{0.1};  // m/s: teleop past this takes over
  double takeover_steer{0.05};  // rad
};

// The zero command's acceleration limit: as hard as the envelope allows. The
// sync MCU caps every command's accel at the envelope's max_accel (SYS-22).
constexpr float kStopAccel = std::numeric_limits<float>::max();

class Arbiter
{
public:
  // Called on every change of mode, with why it changed
  using ModeChanged = std::function<void (uint8_t mode, const std::string & why)>;

  explicit Arbiter(const Params & params, ModeChanged on_change = {});

  // A command arrived; returns what to send on /car/cmd/drive now, if anything.
  // Commands with a non-finite field are dropped, as if they never came.
  std::optional<Command> on_teleop(const Command & cmd, const rclcpp::Time & now);
  std::optional<Command> on_experiment(const Command & cmd, const rclcpp::Time & now);

  // Called periodically: the zero command while the source in control is
  // silent, and back to the operator once the experiment has been lost.
  std::optional<Command> on_tick(const rclcpp::Time & now);

  // /control/set_mode: empty when accepted, otherwise why not
  std::string request_mode(uint8_t mode, const rclcpp::Time & now);

  uint8_t mode() const {return mode_;}

  static bool finite(const Command & cmd);

private:
  bool fresh(
    const std::optional<rclcpp::Time> & last, const rclcpp::Duration & timeout,
    const rclcpp::Time & now) const;
  bool takes_over(const Command & cmd) const;
  void set_mode(uint8_t mode, const std::string & why);
  Command sent(const Command & cmd);
  Command zero(const rclcpp::Time & now);

  Params params_;
  ModeChanged on_change_;
  uint8_t mode_{ControlMode::MODE_OPERATOR};
  std::optional<rclcpp::Time> teleop_at_;      // when each last arrived,
  std::optional<rclcpp::Time> experiment_at_;  // on the car's clock
  float last_steer_{0.0f};  // held by the zero command
};

}  // namespace arc_control
