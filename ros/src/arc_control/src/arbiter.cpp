// The rules for who drives (ICD ros2-msgs, "Choosing who drives"):
// - Operator mode: the teleop drives; the experiment's commands are dropped.
// - Experiment mode: the experiment drives, until teleop past the deadband
//   takes over, or the experiment is lost.
// - A silent source in control is replaced by the zero command, never by its
//   last value. Only a request (/control/set_mode) gives the experiment control.

#include "arc_control/arbiter.hpp"

#include <cmath>
#include <cstdio>
#include <utility>

namespace arc_control
{

Arbiter::Arbiter(const Params & params, ModeChanged on_change)
: params_(params), on_change_(std::move(on_change))
{
}

std::optional<Command> Arbiter::on_teleop(const Command & cmd, const rclcpp::Time & now)
{
  if (!finite(cmd)) {
    return std::nullopt;
  }
  teleop_at_ = now;
  if (mode_ == ControlMode::MODE_EXPERIMENT) {
    if (!takes_over(cmd)) {
      return std::nullopt;  // centred sticks don't end an experiment
    }
    set_mode(ControlMode::MODE_OPERATOR, "the operator took over");
  }
  return sent(cmd);
}

std::optional<Command> Arbiter::on_experiment(const Command & cmd, const rclcpp::Time & now)
{
  if (!finite(cmd)) {
    return std::nullopt;
  }
  experiment_at_ = now;
  if (mode_ != ControlMode::MODE_EXPERIMENT) {
    return std::nullopt;
  }
  return sent(cmd);
}

std::optional<Command> Arbiter::on_tick(const rclcpp::Time & now)
{
  if (mode_ == ControlMode::MODE_EXPERIMENT &&
    !fresh(experiment_at_, params_.experiment_lost, now))
  {
    char why[64];
    std::snprintf(why, sizeof(why), "no experiment command for %.1f s",
      params_.experiment_lost.seconds());
    set_mode(ControlMode::MODE_OPERATOR, why);
  }
  const bool in_control_fresh = mode_ == ControlMode::MODE_EXPERIMENT ?
    fresh(experiment_at_, params_.experiment_timeout, now) :
    fresh(teleop_at_, params_.teleop_timeout, now);
  if (in_control_fresh) {
    return std::nullopt;  // its commands are forwarded as they arrive
  }
  return zero(now);
}

std::string Arbiter::request_mode(uint8_t mode, const rclcpp::Time & now)
{
  switch (mode) {
    case ControlMode::MODE_OPERATOR:
      if (mode_ != ControlMode::MODE_OPERATOR) {
        set_mode(ControlMode::MODE_OPERATOR, "requested");
      }
      return {};
    case ControlMode::MODE_EXPERIMENT:
      if (mode_ == ControlMode::MODE_EXPERIMENT) {
        return {};
      }
      if (!fresh(experiment_at_, params_.experiment_timeout, now)) {
        char why[64];
        std::snprintf(why, sizeof(why), "no experiment command in the last %.1f s",
          params_.experiment_timeout.seconds());
        return why;
      }
      set_mode(ControlMode::MODE_EXPERIMENT, "requested");
      return {};
    default:
      return "unknown mode " + std::to_string(mode);
  }
}

bool Arbiter::finite(const Command & cmd)
{
  return std::isfinite(cmd.speed) && std::isfinite(cmd.accel) && std::isfinite(cmd.steer);
}

bool Arbiter::fresh(
  const std::optional<rclcpp::Time> & last, const rclcpp::Duration & timeout,
  const rclcpp::Time & now) const
{
  // A clock that went back (a simulator reset) makes every source stale
  return last && now >= *last && now - *last <= timeout;
}

bool Arbiter::takes_over(const Command & cmd) const
{
  // in float, like the command: 0.1f is a little over 0.1
  return std::fabs(cmd.speed) > static_cast<float>(params_.takeover_speed) ||
         std::fabs(cmd.steer) > static_cast<float>(params_.takeover_steer);
}

void Arbiter::set_mode(uint8_t mode, const std::string & why)
{
  mode_ = mode;
  if (on_change_) {
    on_change_(mode, why);
  }
}

Command Arbiter::sent(const Command & cmd)
{
  last_steer_ = cmd.steer;
  return cmd;
}

Command Arbiter::zero(const rclcpp::Time & now)
{
  // Stop as hard as the envelope allows, on the steering it had: straightening
  // the wheels while stopping would change the car's path.
  Command cmd;
  cmd.header.stamp = now;
  cmd.speed = 0.0f;
  cmd.accel = kStopAccel;
  cmd.steer = last_steer_;
  return cmd;
}

}  // namespace arc_control
