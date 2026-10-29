// The rules for who drives (Arbiter), on made-up times: switching, takeover,
// timeouts and the zero command. Built with AddressSanitizer and
// UndefinedBehaviorSanitizer.

#include <gtest/gtest.h>

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include "arc_control/arbiter.hpp"

namespace
{
using arc_control::Arbiter;
using arc_control::Command;
using arc_control::ControlMode;
using arc_control::kStopAccel;
using arc_control::Params;

constexpr uint8_t kOperator = ControlMode::MODE_OPERATOR;
constexpr uint8_t kExperiment = ControlMode::MODE_EXPERIMENT;

rclcpp::Time at_ms(int64_t ms)
{
  return rclcpp::Time(ms * 1'000'000, RCL_ROS_TIME);
}

Command cmd(float speed, float steer = 0.0f, float accel = 2.0f)
{
  Command c;
  c.speed = speed;
  c.accel = accel;
  c.steer = steer;
  return c;
}

void expect_zero(const std::optional<Command> & out, float steer)
{
  ASSERT_TRUE(out);
  EXPECT_EQ(out->speed, 0.0f);
  EXPECT_EQ(out->accel, kStopAccel);
  EXPECT_EQ(out->steer, steer);
}

class ArbiterTest : public ::testing::Test
{
protected:
  ArbiterTest()
  : arbiter(Params{}, [this](uint8_t mode, const std::string & why) {
        changes.push_back(mode);
        reasons.push_back(why);
      })
  {
  }

  // Into experiment mode at t, with a fresh experiment command
  void start_experiment(int64_t t)
  {
    arbiter.on_experiment(cmd(2.0f), at_ms(t));
    ASSERT_EQ(arbiter.request_mode(kExperiment, at_ms(t)), "");
  }

  Arbiter arbiter;
  std::vector<uint8_t> changes;
  std::vector<std::string> reasons;
};

TEST_F(ArbiterTest, StartsInOperatorModeSendingZero)
{
  EXPECT_EQ(arbiter.mode(), kOperator);
  expect_zero(arbiter.on_tick(at_ms(0)), 0.0f);
}

TEST_F(ArbiterTest, OperatorModeForwardsTeleopAndDropsExperiment)
{
  const auto out = arbiter.on_teleop(cmd(1.0f, 0.2f, 3.0f), at_ms(0));
  ASSERT_TRUE(out);
  EXPECT_EQ(out->speed, 1.0f);
  EXPECT_EQ(out->accel, 3.0f);
  EXPECT_EQ(out->steer, 0.2f);
  EXPECT_FALSE(arbiter.on_tick(at_ms(10)));  // fresh: nothing extra to send
  EXPECT_FALSE(arbiter.on_experiment(cmd(5.0f), at_ms(20)));
}

TEST_F(ArbiterTest, ForwardsTheSendersStamp)
{
  Command in = cmd(1.0f);
  in.header.stamp = at_ms(97);  // computed 3 ms before it arrived (SYS-23)
  const auto out = arbiter.on_teleop(in, at_ms(100));
  ASSERT_TRUE(out);
  EXPECT_EQ(rclcpp::Time(out->header.stamp, RCL_ROS_TIME), at_ms(97));
}

TEST_F(ArbiterTest, SilentTeleopIsReplacedByZeroHoldingSteering)
{
  arbiter.on_teleop(cmd(1.0f, 0.3f), at_ms(0));
  EXPECT_FALSE(arbiter.on_tick(at_ms(200)));  // at the timeout: still fresh
  const auto out = arbiter.on_tick(at_ms(201));
  expect_zero(out, 0.3f);
  EXPECT_EQ(rclcpp::Time(out->header.stamp, RCL_ROS_TIME), at_ms(201));
  // and it keeps coming while the source is silent
  expect_zero(arbiter.on_tick(at_ms(211)), 0.3f);
}

TEST_F(ArbiterTest, ExperimentNeedsAFreshCommandToTakeControl)
{
  EXPECT_NE(arbiter.request_mode(kExperiment, at_ms(0)), "");  // never heard from
  arbiter.on_experiment(cmd(2.0f), at_ms(0));
  EXPECT_NE(arbiter.request_mode(kExperiment, at_ms(101)), "");  // too old
  EXPECT_EQ(arbiter.mode(), kOperator);
  EXPECT_TRUE(changes.empty());

  arbiter.on_experiment(cmd(2.0f), at_ms(150));
  EXPECT_EQ(arbiter.request_mode(kExperiment, at_ms(160)), "");
  EXPECT_EQ(arbiter.mode(), kExperiment);
  EXPECT_EQ(changes, std::vector<uint8_t>{kExperiment});
}

TEST_F(ArbiterTest, ExperimentModeForwardsExperimentAndIgnoresCentredSticks)
{
  start_experiment(0);
  const auto out = arbiter.on_experiment(cmd(2.5f, -0.1f), at_ms(10));
  ASSERT_TRUE(out);
  EXPECT_EQ(out->speed, 2.5f);
  // teleop within the deadband: the operator isn't touching the sticks
  EXPECT_FALSE(arbiter.on_teleop(cmd(0.0f), at_ms(20)));
  EXPECT_FALSE(arbiter.on_teleop(cmd(0.1f, 0.05f), at_ms(30)));  // on the edge
  EXPECT_FALSE(arbiter.on_teleop(cmd(-0.1f, -0.05f), at_ms(40)));
  EXPECT_EQ(arbiter.mode(), kExperiment);
}

TEST_F(ArbiterTest, TeleopPastTheDeadbandTakesOver)
{
  for (const Command & stick : {cmd(0.2f), cmd(-0.2f), cmd(0.0f, 0.1f), cmd(0.0f, -0.1f)}) {
    Arbiter a(Params{});
    a.on_experiment(cmd(2.0f), at_ms(0));
    ASSERT_EQ(a.request_mode(kExperiment, at_ms(0)), "");
    const auto out = a.on_teleop(stick, at_ms(10));
    ASSERT_TRUE(out);
    EXPECT_EQ(out->speed, stick.speed);
    EXPECT_EQ(out->steer, stick.steer);
    EXPECT_EQ(a.mode(), kOperator);
    // and the experiment no longer drives
    EXPECT_FALSE(a.on_experiment(cmd(2.0f), at_ms(20)));
  }
}

TEST_F(ArbiterTest, TakeoverIsReported)
{
  start_experiment(0);
  arbiter.on_teleop(cmd(1.0f), at_ms(10));
  EXPECT_EQ(changes, (std::vector<uint8_t>{kExperiment, kOperator}));
  EXPECT_EQ(reasons.back(), "the operator took over");
}

TEST_F(ArbiterTest, SilentExperimentStopsTheCarButKeepsControl)
{
  start_experiment(0);
  arbiter.on_experiment(cmd(2.0f, 0.2f), at_ms(10));
  EXPECT_FALSE(arbiter.on_tick(at_ms(110)));
  expect_zero(arbiter.on_tick(at_ms(111)), 0.2f);
  EXPECT_EQ(arbiter.mode(), kExperiment);
  // a hiccup: it comes back before it's lost and drives again
  const auto out = arbiter.on_experiment(cmd(1.5f), at_ms(500));
  ASSERT_TRUE(out);
  EXPECT_EQ(out->speed, 1.5f);
  EXPECT_EQ(arbiter.mode(), kExperiment);
}

TEST_F(ArbiterTest, LostExperimentHandsBackToTheOperator)
{
  start_experiment(0);
  expect_zero(arbiter.on_tick(at_ms(1000)), 0.0f);
  EXPECT_EQ(arbiter.mode(), kExperiment);
  expect_zero(arbiter.on_tick(at_ms(1001)), 0.0f);  // no fresh teleop: still zero
  EXPECT_EQ(arbiter.mode(), kOperator);
  EXPECT_EQ(reasons.back(), "no experiment command for 1.0 s");
  // coming back doesn't give it control again: that takes a request
  EXPECT_FALSE(arbiter.on_experiment(cmd(2.0f), at_ms(1100)));
  EXPECT_EQ(arbiter.mode(), kOperator);
}

TEST_F(ArbiterTest, OperatorCanAlwaysBeRequested)
{
  EXPECT_EQ(arbiter.request_mode(kOperator, at_ms(0)), "");  // already: no change
  EXPECT_TRUE(changes.empty());
  start_experiment(10);
  EXPECT_EQ(arbiter.request_mode(kOperator, at_ms(20)), "");  // no fresh teleop needed
  EXPECT_EQ(arbiter.mode(), kOperator);
  EXPECT_EQ(changes, (std::vector<uint8_t>{kExperiment, kOperator}));
}

TEST_F(ArbiterTest, UnknownModeIsRefused)
{
  EXPECT_EQ(arbiter.request_mode(7, at_ms(0)), "unknown mode 7");
  EXPECT_EQ(arbiter.mode(), kOperator);
}

TEST_F(ArbiterTest, NonFiniteCommandsAreDropped)
{
  const float nan = std::numeric_limits<float>::quiet_NaN();
  const float inf = std::numeric_limits<float>::infinity();
  arbiter.on_teleop(cmd(1.0f), at_ms(0));
  for (const Command & bad : {cmd(nan), cmd(1.0f, nan), cmd(1.0f, 0.0f, inf)}) {
    EXPECT_FALSE(arbiter.on_teleop(bad, at_ms(150)));
  }
  // they didn't count as arrivals: the teleop is stale at 201 ms
  expect_zero(arbiter.on_tick(at_ms(201)), 0.0f);

  arbiter.on_experiment(cmd(nan), at_ms(210));
  EXPECT_NE(arbiter.request_mode(kExperiment, at_ms(210)), "");
}

TEST_F(ArbiterTest, ClockGoingBackMakesSourcesStale)
{
  arbiter.on_teleop(cmd(1.0f), at_ms(5000));
  expect_zero(arbiter.on_tick(at_ms(0)), 0.0f);  // a simulator reset
}

}  // namespace
