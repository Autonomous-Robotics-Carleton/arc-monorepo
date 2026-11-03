// The car interface's constants (arc_msgs, ICD ros2-msgs) have the sync
// link's values (sync-link.xml), so the car backend copies them across
// unchanged. Checked at compile time: a value that drifts, or an enum that
// gains an entry here or there, fails the build.

#include <gtest/gtest.h>

#include "arc_msgs/msg/angle_sample.hpp"
#include "arc_msgs/msg/fault.hpp"
#include "arc_msgs/msg/motor_status.hpp"
#include "arc_msgs/msg/safety_state.hpp"
#include "arc_sync_link/mavlink.h"

namespace
{
using arc_msgs::msg::AngleSample;
using arc_msgs::msg::Fault;
using arc_msgs::msg::MotorStatus;
using arc_msgs::msg::SafetyState;

static_assert(SafetyState::WATCHDOG_OK == ARC_WATCHDOG_OK);
static_assert(SafetyState::WATCHDOG_NOT_ARMED == ARC_WATCHDOG_NOT_ARMED);
static_assert(SafetyState::WATCHDOG_NO_HEARTBEAT == ARC_WATCHDOG_NO_HEARTBEAT);
static_assert(SafetyState::WATCHDOG_NO_HEARTBEAT + 1 == ARC_WATCHDOG_STATE_ENUM_END);

static_assert(SafetyState::LATCH_LATCHED == ARC_LATCH_LATCHED);
static_assert(SafetyState::LATCH_ESTOP_HELD == ARC_LATCH_ESTOP_HELD);
static_assert(SafetyState::LATCH_CHECKING == ARC_LATCH_CHECKING);
static_assert(SafetyState::LATCH_RESET == ARC_LATCH_RESET);
static_assert(SafetyState::LATCH_RESTART_REQUIRED == ARC_LATCH_RESTART_REQUIRED);
static_assert(SafetyState::LATCH_RESTART_REQUIRED + 1 == ARC_LATCH_STATE_ENUM_END);

static_assert(SafetyState::CAUSE_NONE == ARC_LATCH_CAUSE_NONE);
static_assert(SafetyState::CAUSE_POWER_ON == ARC_LATCH_CAUSE_POWER_ON);
static_assert(SafetyState::CAUSE_ESTOP == ARC_LATCH_CAUSE_ESTOP);
static_assert(SafetyState::CAUSE_CORNER == ARC_LATCH_CAUSE_CORNER);
static_assert(SafetyState::CAUSE_CHECKS_FAILED == ARC_LATCH_CAUSE_CHECKS_FAILED);
static_assert(SafetyState::CAUSE_TOO_MANY_STOPS == ARC_LATCH_CAUSE_TOO_MANY_STOPS);
static_assert(SafetyState::CAUSE_OVERCURRENT == ARC_LATCH_CAUSE_OVERCURRENT);
static_assert(SafetyState::CAUSE_OVERVOLTAGE == ARC_LATCH_CAUSE_OVERVOLTAGE);
static_assert(SafetyState::CAUSE_ESTOP_WIRING == ARC_LATCH_CAUSE_ESTOP_WIRING);
static_assert(SafetyState::CAUSE_COMMAND_BUS == ARC_LATCH_CAUSE_COMMAND_BUS);
static_assert(SafetyState::CAUSE_STEERING == ARC_LATCH_CAUSE_STEERING);
static_assert(SafetyState::CAUSE_STEERING_DISAGREEMENT == ARC_LATCH_CAUSE_STEERING_DISAGREEMENT);
static_assert(SafetyState::CAUSE_STEERING_DISAGREEMENT + 1 == ARC_LATCH_CAUSE_ENUM_END);

static_assert(AngleSample::STATUS_MAGNET_LOW == ARC_ANGLE_STATUS_MAGNET_LOW);
static_assert(AngleSample::STATUS_MAGNET_HIGH == ARC_ANGLE_STATUS_MAGNET_HIGH);
static_assert(AngleSample::STATUS_CORDIC_OVERFLOW == ARC_ANGLE_STATUS_CORDIC_OVERFLOW);
static_assert(AngleSample::STATUS_READ_ERROR == ARC_ANGLE_STATUS_READ_ERROR);
static_assert(AngleSample::STATUS_NOT_READY == ARC_ANGLE_STATUS_NOT_READY);
static_assert(AngleSample::STATUS_NOT_READY + 1 == ARC_ANGLE_STATUS_ENUM_END);

static_assert(MotorStatus::STATE_ESTOP == ARC_MOTOR_STATE_ESTOP);
static_assert(MotorStatus::STATE_COMMAND_TIMEOUT == ARC_MOTOR_STATE_COMMAND_TIMEOUT);
static_assert(MotorStatus::STATE_COMMAND_TIMEOUT + 1 == ARC_MOTOR_STATE_ENUM_END);

static_assert(Fault::SOURCE_SYNC_MCU == ARC_FAULT_SYNC_MCU);
static_assert(Fault::SOURCE_MOTOR_CONTROLLER == ARC_FAULT_MOTOR_CONTROLLER);
static_assert(Fault::SOURCE_STEERING == ARC_FAULT_STEERING);
static_assert(Fault::SOURCE_POWER == ARC_FAULT_POWER);
static_assert(Fault::SOURCE_SENSOR == ARC_FAULT_SENSOR);
static_assert(Fault::SOURCE_LINK == ARC_FAULT_LINK);
static_assert(Fault::SOURCE_LINK + 1 == ARC_FAULT_SOURCE_ENUM_END);
// The car backend's own source stays clear of the sync link's
static_assert(ARC_FAULT_SOURCE_ENUM_END <= Fault::SOURCE_CAR_BACKEND);
}  // namespace

// The checks above run when this file compiles; the test makes them show up
// in the results.
TEST(MsgConstants, MatchTheSyncLink)
{
  SUCCEED();
}
