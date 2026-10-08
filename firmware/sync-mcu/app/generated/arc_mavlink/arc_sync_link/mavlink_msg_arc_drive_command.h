#pragma once
// MESSAGE ARC_DRIVE_COMMAND PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_DRIVE_COMMAND 52050


typedef struct __mavlink_arc_drive_command_t {
 uint64_t time_ns; /*< [ns] When the command was computed, in sync-MCU time.*/
 float speed; /*< [m/s] Target speed.*/
 float accel; /*< [m/s/s] Acceleration limit for reaching it.*/
 float steer; /*< [rad] Target steering angle.*/
} mavlink_arc_drive_command_t;

#define MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN 20
#define MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN 20
#define MAVLINK_MSG_ID_52050_LEN 20
#define MAVLINK_MSG_ID_52050_MIN_LEN 20

#define MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC 17
#define MAVLINK_MSG_ID_52050_CRC 17



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_DRIVE_COMMAND { \
    52050, \
    "ARC_DRIVE_COMMAND", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_drive_command_t, time_ns) }, \
         { "speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_drive_command_t, speed) }, \
         { "accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_drive_command_t, accel) }, \
         { "steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_drive_command_t, steer) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_DRIVE_COMMAND { \
    "ARC_DRIVE_COMMAND", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_drive_command_t, time_ns) }, \
         { "speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_drive_command_t, speed) }, \
         { "accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_drive_command_t, accel) }, \
         { "steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_drive_command_t, steer) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_drive_command message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the command was computed, in sync-MCU time.
 * @param speed [m/s] Target speed.
 * @param accel [m/s/s] Acceleration limit for reaching it.
 * @param steer [rad] Target steering angle.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_drive_command_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float speed, float accel, float steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, speed);
    _mav_put_float(buf, 12, accel);
    _mav_put_float(buf, 16, steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#else
    mavlink_arc_drive_command_t packet;
    packet.time_ns = time_ns;
    packet.speed = speed;
    packet.accel = accel;
    packet.steer = steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_DRIVE_COMMAND;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
}

/**
 * @brief Pack a arc_drive_command message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the command was computed, in sync-MCU time.
 * @param speed [m/s] Target speed.
 * @param accel [m/s/s] Acceleration limit for reaching it.
 * @param steer [rad] Target steering angle.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_drive_command_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float speed, float accel, float steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, speed);
    _mav_put_float(buf, 12, accel);
    _mav_put_float(buf, 16, steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#else
    mavlink_arc_drive_command_t packet;
    packet.time_ns = time_ns;
    packet.speed = speed;
    packet.accel = accel;
    packet.steer = steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_DRIVE_COMMAND;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#endif
}

/**
 * @brief Pack a arc_drive_command message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] When the command was computed, in sync-MCU time.
 * @param speed [m/s] Target speed.
 * @param accel [m/s/s] Acceleration limit for reaching it.
 * @param steer [rad] Target steering angle.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_drive_command_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float speed,float accel,float steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, speed);
    _mav_put_float(buf, 12, accel);
    _mav_put_float(buf, 16, steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#else
    mavlink_arc_drive_command_t packet;
    packet.time_ns = time_ns;
    packet.speed = speed;
    packet.accel = accel;
    packet.steer = steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_DRIVE_COMMAND;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
}

/**
 * @brief Encode a arc_drive_command struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_drive_command C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_drive_command_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_drive_command_t* arc_drive_command)
{
    return mavlink_msg_arc_drive_command_pack(system_id, component_id, msg, arc_drive_command->time_ns, arc_drive_command->speed, arc_drive_command->accel, arc_drive_command->steer);
}

/**
 * @brief Encode a arc_drive_command struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_drive_command C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_drive_command_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_drive_command_t* arc_drive_command)
{
    return mavlink_msg_arc_drive_command_pack_chan(system_id, component_id, chan, msg, arc_drive_command->time_ns, arc_drive_command->speed, arc_drive_command->accel, arc_drive_command->steer);
}

/**
 * @brief Encode a arc_drive_command struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_drive_command C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_drive_command_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_drive_command_t* arc_drive_command)
{
    return mavlink_msg_arc_drive_command_pack_status(system_id, component_id, _status, msg,  arc_drive_command->time_ns, arc_drive_command->speed, arc_drive_command->accel, arc_drive_command->steer);
}

/**
 * @brief Send a arc_drive_command message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] When the command was computed, in sync-MCU time.
 * @param speed [m/s] Target speed.
 * @param accel [m/s/s] Acceleration limit for reaching it.
 * @param steer [rad] Target steering angle.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_drive_command_send(mavlink_channel_t chan, uint64_t time_ns, float speed, float accel, float steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, speed);
    _mav_put_float(buf, 12, accel);
    _mav_put_float(buf, 16, steer);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND, buf, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#else
    mavlink_arc_drive_command_t packet;
    packet.time_ns = time_ns;
    packet.speed = speed;
    packet.accel = accel;
    packet.steer = steer;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND, (const char *)&packet, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#endif
}

/**
 * @brief Send a arc_drive_command message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_drive_command_send_struct(mavlink_channel_t chan, const mavlink_arc_drive_command_t* arc_drive_command)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_drive_command_send(chan, arc_drive_command->time_ns, arc_drive_command->speed, arc_drive_command->accel, arc_drive_command->steer);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND, (const char *)arc_drive_command, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_drive_command_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float speed, float accel, float steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, speed);
    _mav_put_float(buf, 12, accel);
    _mav_put_float(buf, 16, steer);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND, buf, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#else
    mavlink_arc_drive_command_t *packet = (mavlink_arc_drive_command_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->speed = speed;
    packet->accel = accel;
    packet->steer = steer;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND, (const char *)packet, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_DRIVE_COMMAND UNPACKING


/**
 * @brief Get field time_ns from arc_drive_command message
 *
 * @return [ns] When the command was computed, in sync-MCU time.
 */
static inline uint64_t mavlink_msg_arc_drive_command_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field speed from arc_drive_command message
 *
 * @return [m/s] Target speed.
 */
static inline float mavlink_msg_arc_drive_command_get_speed(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field accel from arc_drive_command message
 *
 * @return [m/s/s] Acceleration limit for reaching it.
 */
static inline float mavlink_msg_arc_drive_command_get_accel(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field steer from arc_drive_command message
 *
 * @return [rad] Target steering angle.
 */
static inline float mavlink_msg_arc_drive_command_get_steer(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Decode a arc_drive_command message into a struct
 *
 * @param msg The message to decode
 * @param arc_drive_command C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_drive_command_decode(const mavlink_message_t* msg, mavlink_arc_drive_command_t* arc_drive_command)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_drive_command->time_ns = mavlink_msg_arc_drive_command_get_time_ns(msg);
    arc_drive_command->speed = mavlink_msg_arc_drive_command_get_speed(msg);
    arc_drive_command->accel = mavlink_msg_arc_drive_command_get_accel(msg);
    arc_drive_command->steer = mavlink_msg_arc_drive_command_get_steer(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN? msg->len : MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN;
        memset(arc_drive_command, 0, MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_LEN);
    memcpy(arc_drive_command, _MAV_PAYLOAD(msg), len);
#endif
}
