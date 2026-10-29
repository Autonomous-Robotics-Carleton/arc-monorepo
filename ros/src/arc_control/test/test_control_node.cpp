// arc_control on a ROS graph: its topics, QoS and service, against a peer node
// standing in for the operator link, an experiment and the car backend. Each
// test runs under a namespace of its own, so it never hears another arc_control.

#include <gtest/gtest.h>

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <thread>

#include <rclcpp/rclcpp.hpp>

#include "arc_control/control_node.hpp"
#include "arc_msgs/srv/set_control_mode.hpp"

namespace
{
using namespace std::chrono_literals;
using arc_control::Command;
using arc_control::ControlMode;
using arc_control::ControlNode;
using arc_msgs::srv::SetControlMode;

constexpr char kNamespace[] = "/test_control_node";
const rclcpp::QoS kCommands = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().durability_volatile();
const rclcpp::QoS kState = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local();

Command cmd(float speed)
{
  Command c;
  c.speed = speed;
  c.accel = 2.0f;
  return c;
}

class ControlNodeTest : public ::testing::Test
{
protected:
  static void SetUpTestSuite() {rclcpp::init(0, nullptr);}
  static void TearDownTestSuite() {rclcpp::shutdown();}

  void SetUp() override
  {
    rclcpp::NodeOptions options;
    options.arguments({"--ros-args", "-r", std::string("__ns:=") + kNamespace});
    control_ = std::make_shared<ControlNode>(options);

    peer_ = std::make_shared<rclcpp::Node>("peer", kNamespace);
    teleop_pub_ = peer_->create_publisher<Command>("control/teleop", kCommands);
    experiment_pub_ = peer_->create_publisher<Command>("control/experiment", kCommands);
    drive_sub_ = peer_->create_subscription<Command>("car/cmd/drive", kCommands,
      [this](const Command & c) {drive_ = c;});
    mode_sub_ = peer_->create_subscription<ControlMode>("control/mode", kState,
      [this](const ControlMode & m) {mode_ = m.mode;});
    client_ = peer_->create_client<SetControlMode>("control/set_mode");

    executor_.add_node(control_);
    executor_.add_node(peer_);
  }

  // Spins until `done` holds or `limit` passes, publishing the teleop and the
  // experiment every 20 ms while they're set, as their sources would.
  bool spin_until(const std::function<bool()> & done, std::chrono::milliseconds limit)
  {
    const auto end = std::chrono::steady_clock::now() + limit;
    auto next_pub = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() < end) {
      if (std::chrono::steady_clock::now() >= next_pub) {
        if (teleop_) {teleop_pub_->publish(*teleop_);}
        if (experiment_) {experiment_pub_->publish(*experiment_);}
        next_pub += 20ms;
      }
      executor_.spin_some(5ms);
      if (done()) {
        return true;
      }
      std::this_thread::sleep_for(1ms);
    }
    return false;
  }

  void spin_for(std::chrono::milliseconds duration)
  {
    spin_until([] {return false;}, duration);
  }

  SetControlMode::Response::SharedPtr request(uint8_t mode)
  {
    EXPECT_TRUE(client_->wait_for_service(2s));
    auto req = std::make_shared<SetControlMode::Request>();
    req->mode = mode;
    auto future = client_->async_send_request(req).share();
    const bool replied = spin_until(
      [&] {return future.wait_for(0s) == std::future_status::ready;}, 2000ms);
    return replied ? future.get() : nullptr;
  }

  bool driving_at(float speed) const {return drive_ && drive_->speed == speed;}
  bool stopped() const {return drive_ && drive_->speed == 0.0f && drive_->accel == arc_control::kStopAccel;}

  // An experiment publishing at 2 m/s, and given control
  void hand_to_experiment()
  {
    experiment_ = cmd(2.0f);
    spin_for(500ms);  // discovery, and arc_control hearing from it
    auto res = request(ControlMode::MODE_EXPERIMENT);
    ASSERT_TRUE(res);
    ASSERT_TRUE(res->accepted) << res->reason;
    ASSERT_TRUE(spin_until([&] {return driving_at(2.0f) && mode_ == ControlMode::MODE_EXPERIMENT;}, 2000ms));
  }

  rclcpp::executors::SingleThreadedExecutor executor_;
  std::shared_ptr<ControlNode> control_;
  rclcpp::Node::SharedPtr peer_;
  rclcpp::Publisher<Command>::SharedPtr teleop_pub_;
  rclcpp::Publisher<Command>::SharedPtr experiment_pub_;
  rclcpp::Subscription<Command>::SharedPtr drive_sub_;
  rclcpp::Subscription<ControlMode>::SharedPtr mode_sub_;
  rclcpp::Client<SetControlMode>::SharedPtr client_;

  std::optional<Command> teleop_;      // what the peer publishes,
  std::optional<Command> experiment_;  // while set
  std::optional<Command> drive_;   // the last command to the car
  std::optional<uint8_t> mode_;    // the last mode reported
};

TEST_F(ControlNodeTest, StartsInOperatorModeSendingZero)
{
  EXPECT_TRUE(spin_until([&] {return stopped() && mode_ == ControlMode::MODE_OPERATOR;}, 3000ms));
}

TEST_F(ControlNodeTest, ForwardsTeleop)
{
  teleop_ = cmd(1.0f);
  EXPECT_TRUE(spin_until([&] {return driving_at(1.0f);}, 3000ms));
}

TEST_F(ControlNodeTest, RefusesExperimentModeWithoutAnExperiment)
{
  auto res = request(ControlMode::MODE_EXPERIMENT);
  ASSERT_TRUE(res);
  EXPECT_FALSE(res->accepted);
  EXPECT_FALSE(res->reason.empty());
}

TEST_F(ControlNodeTest, HandsControlToTheExperimentAndTakesItBack)
{
  teleop_ = cmd(0.0f);  // the operator's sticks, centred
  hand_to_experiment();
  spin_for(300ms);
  EXPECT_TRUE(driving_at(2.0f));  // centred sticks don't take over
  EXPECT_EQ(mode_, ControlMode::MODE_EXPERIMENT);

  teleop_ = cmd(0.5f);  // the operator pushes a stick
  EXPECT_TRUE(spin_until([&] {return driving_at(0.5f) && mode_ == ControlMode::MODE_OPERATOR;}, 2000ms));
}

TEST_F(ControlNodeTest, OperatorModeCanBeRequested)
{
  hand_to_experiment();
  auto res = request(ControlMode::MODE_OPERATOR);
  ASSERT_TRUE(res);
  EXPECT_TRUE(res->accepted);
  // no teleop: the car gets the zero command
  EXPECT_TRUE(spin_until([&] {return stopped() && mode_ == ControlMode::MODE_OPERATOR;}, 2000ms));
}

TEST_F(ControlNodeTest, SilentExperimentStopsTheCarThenLosesControl)
{
  hand_to_experiment();
  experiment_.reset();  // it crashes
  ASSERT_TRUE(spin_until([&] {return stopped();}, 800ms));
  EXPECT_EQ(mode_, ControlMode::MODE_EXPERIMENT);
  EXPECT_TRUE(spin_until([&] {return mode_ == ControlMode::MODE_OPERATOR;}, 2000ms));
}

}  // namespace
