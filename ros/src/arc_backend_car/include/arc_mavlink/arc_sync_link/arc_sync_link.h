/** @file
 *  @brief MAVLink comm protocol generated from arc_sync_link.xml
 *  @see http://mavlink.org
 */
#pragma once
#ifndef MAVLINK_ARC_SYNC_LINK_H
#define MAVLINK_ARC_SYNC_LINK_H

#ifndef MAVLINK_H
    #error Wrong include order: MAVLINK_ARC_SYNC_LINK.H MUST NOT BE DIRECTLY USED. Include mavlink.h from the same directory instead or set ALL AND EVERY defines from MAVLINK.H manually accordingly, including the #define MAVLINK_H call.
#endif

#define MAVLINK_ARC_SYNC_LINK_XML_HASH 8688937756362150935

#ifdef __cplusplus
extern "C" {
#endif

// MESSAGE LENGTHS AND CRCS

#ifndef MAVLINK_MESSAGE_LENGTHS
#define MAVLINK_MESSAGE_LENGTHS {}
#endif

#ifndef MAVLINK_MESSAGE_CRCS
#define MAVLINK_MESSAGE_CRCS {{52000, 7, 18, 18, 0, 0, 0}, {52001, 54, 33, 33, 0, 0, 0}, {52002, 100, 13, 13, 0, 0, 0}, {52003, 243, 14, 14, 0, 0, 0}, {52004, 231, 46, 46, 0, 0, 0}, {52005, 76, 25, 25, 0, 0, 0}, {52006, 17, 13, 13, 0, 0, 0}, {52007, 171, 32, 32, 0, 0, 0}, {52008, 127, 22, 22, 0, 0, 0}, {52009, 43, 15, 15, 0, 0, 0}, {52050, 17, 20, 20, 0, 0, 0}, {52051, 49, 12, 12, 0, 0, 0}, {52052, 102, 20, 20, 0, 0, 0}}
#endif

#include "../protocol.h"

#define MAVLINK_ENABLED_ARC_SYNC_LINK

// ENUM DEFINITIONS


/** @brief Which IMU. */
#ifndef HAVE_ENUM_ARC_IMU
#define HAVE_ENUM_ARC_IMU
typedef enum ARC_IMU
{
   ARC_IMU_CG=0, /* Navigation-grade IMU at the centre of gravity. | */
   ARC_IMU_FRONT=1, /* Consumer-grade IMU at the front axle. | */
   ARC_IMU_ENUM_END=2, /*  | */
} ARC_IMU;
#endif

/** @brief Which angle sensor (AS5047 family). */
#ifndef HAVE_ENUM_ARC_ANGLE_SENSOR
#define HAVE_ENUM_ARC_ANGLE_SENSOR
typedef enum ARC_ANGLE_SENSOR
{
   ARC_ANGLE_WHEEL_FL=0, /*  | */
   ARC_ANGLE_WHEEL_FR=1, /*  | */
   ARC_ANGLE_WHEEL_RL=2, /*  | */
   ARC_ANGLE_WHEEL_RR=3, /*  | */
   ARC_ANGLE_SUSPENSION_FL=4, /*  | */
   ARC_ANGLE_SUSPENSION_FR=5, /*  | */
   ARC_ANGLE_SUSPENSION_RL=6, /*  | */
   ARC_ANGLE_SUSPENSION_RR=7, /*  | */
   ARC_ANGLE_KNUCKLE_FL=8, /*  | */
   ARC_ANGLE_KNUCKLE_FR=9, /*  | */
   ARC_ANGLE_SENSOR_ENUM_END=10, /*  | */
} ARC_ANGLE_SENSOR;
#endif

/** @brief Which corner. */
#ifndef HAVE_ENUM_ARC_CORNER
#define HAVE_ENUM_ARC_CORNER
typedef enum ARC_CORNER
{
   ARC_CORNER_FL=0, /*  | */
   ARC_CORNER_FR=1, /*  | */
   ARC_CORNER_RL=2, /*  | */
   ARC_CORNER_RR=3, /*  | */
   ARC_CORNER_ENUM_END=4, /*  | */
} ARC_CORNER;
#endif

/** @brief Heartbeat watchdog (SYS-04). */
#ifndef HAVE_ENUM_ARC_WATCHDOG_STATE
#define HAVE_ENUM_ARC_WATCHDOG_STATE
typedef enum ARC_WATCHDOG_STATE
{
   ARC_WATCHDOG_OK=0, /* Heartbeat arriving. | */
   ARC_WATCHDOG_ROLLING=1, /* Heartbeat lost; within the roll-out before braking. | */
   ARC_WATCHDOG_BRAKING=2, /* Braking on the e-stop ramp. | */
   ARC_WATCHDOG_STATE_ENUM_END=3, /*  | */
} ARC_WATCHDOG_STATE;
#endif

/** @brief Where a fault was detected (SYS-19). */
#ifndef HAVE_ENUM_ARC_FAULT_SOURCE
#define HAVE_ENUM_ARC_FAULT_SOURCE
typedef enum ARC_FAULT_SOURCE
{
   ARC_FAULT_SYNC_MCU=0, /*  | */
   ARC_FAULT_MOTOR_CONTROLLER=1, /*  | */
   ARC_FAULT_STEERING=2, /*  | */
   ARC_FAULT_POWER=3, /*  | */
   ARC_FAULT_SENSOR=4, /*  | */
   ARC_FAULT_LINK=5, /*  | */
   ARC_FAULT_SOURCE_ENUM_END=6, /*  | */
} ARC_FAULT_SOURCE;
#endif

// MAVLINK VERSION

#ifndef MAVLINK_VERSION
#define MAVLINK_VERSION 1
#endif

#if (MAVLINK_VERSION == 0)
#undef MAVLINK_VERSION
#define MAVLINK_VERSION 1
#endif

// MESSAGE DEFINITIONS
#include "./mavlink_msg_arc_link_status.h"
#include "./mavlink_msg_arc_imu_sample.h"
#include "./mavlink_msg_arc_angle_sample.h"
#include "./mavlink_msg_arc_ride_height_sample.h"
#include "./mavlink_msg_arc_motor_status.h"
#include "./mavlink_msg_arc_steering_status.h"
#include "./mavlink_msg_arc_camera_trigger.h"
#include "./mavlink_msg_arc_power_sample.h"
#include "./mavlink_msg_arc_safety_state.h"
#include "./mavlink_msg_arc_fault.h"
#include "./mavlink_msg_arc_drive_command.h"
#include "./mavlink_msg_arc_operator_heartbeat.h"
#include "./mavlink_msg_arc_envelope_set.h"

// base include



#if MAVLINK_ARC_SYNC_LINK_XML_HASH == MAVLINK_PRIMARY_XML_HASH
# define MAVLINK_MESSAGE_INFO {MAVLINK_MESSAGE_INFO_ARC_LINK_STATUS, MAVLINK_MESSAGE_INFO_ARC_IMU_SAMPLE, MAVLINK_MESSAGE_INFO_ARC_ANGLE_SAMPLE, MAVLINK_MESSAGE_INFO_ARC_RIDE_HEIGHT_SAMPLE, MAVLINK_MESSAGE_INFO_ARC_MOTOR_STATUS, MAVLINK_MESSAGE_INFO_ARC_STEERING_STATUS, MAVLINK_MESSAGE_INFO_ARC_CAMERA_TRIGGER, MAVLINK_MESSAGE_INFO_ARC_POWER_SAMPLE, MAVLINK_MESSAGE_INFO_ARC_SAFETY_STATE, MAVLINK_MESSAGE_INFO_ARC_FAULT, MAVLINK_MESSAGE_INFO_ARC_DRIVE_COMMAND, MAVLINK_MESSAGE_INFO_ARC_OPERATOR_HEARTBEAT, MAVLINK_MESSAGE_INFO_ARC_ENVELOPE_SET}
# define MAVLINK_MESSAGE_NAMES {{ "ARC_ANGLE_SAMPLE", 52002 }, { "ARC_CAMERA_TRIGGER", 52006 }, { "ARC_DRIVE_COMMAND", 52050 }, { "ARC_ENVELOPE_SET", 52052 }, { "ARC_FAULT", 52009 }, { "ARC_IMU_SAMPLE", 52001 }, { "ARC_LINK_STATUS", 52000 }, { "ARC_MOTOR_STATUS", 52004 }, { "ARC_OPERATOR_HEARTBEAT", 52051 }, { "ARC_POWER_SAMPLE", 52007 }, { "ARC_RIDE_HEIGHT_SAMPLE", 52003 }, { "ARC_SAFETY_STATE", 52008 }, { "ARC_STEERING_STATUS", 52005 }}
# if MAVLINK_COMMAND_24BIT
#  include "../mavlink_get_info.h"
# endif
#endif

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // MAVLINK_ARC_SYNC_LINK_H
