#pragma once
// MESSAGE ARC_ENVELOPE_SET PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_ENVELOPE_SET 52052


typedef struct __mavlink_arc_envelope_set_t {
 uint64_t time_ns; /*< [ns] When set, in sync-MCU time.*/
 float max_speed; /*< [m/s] Speed limit; 3 m/s by default for new experiments.*/
 float max_accel; /*< [m/s/s] Acceleration limit.*/
 float max_steer; /*< [rad] Steering-angle limit.*/
} mavlink_arc_envelope_set_t;

#define MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN 20
#define MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN 20
#define MAVLINK_MSG_ID_52052_LEN 20
#define MAVLINK_MSG_ID_52052_MIN_LEN 20

#define MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC 102
#define MAVLINK_MSG_ID_52052_CRC 102



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_ENVELOPE_SET { \
    52052, \
    "ARC_ENVELOPE_SET", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_envelope_set_t, time_ns) }, \
         { "max_speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_envelope_set_t, max_speed) }, \
         { "max_accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_envelope_set_t, max_accel) }, \
         { "max_steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_envelope_set_t, max_steer) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_ENVELOPE_SET { \
    "ARC_ENVELOPE_SET", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_envelope_set_t, time_ns) }, \
         { "max_speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_envelope_set_t, max_speed) }, \
         { "max_accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_envelope_set_t, max_accel) }, \
         { "max_steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_envelope_set_t, max_steer) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_envelope_set message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When set, in sync-MCU time.
 * @param max_speed [m/s] Speed limit; 3 m/s by default for new experiments.
 * @param max_accel [m/s/s] Acceleration limit.
 * @param max_steer [rad] Steering-angle limit.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_envelope_set_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float max_speed, float max_accel, float max_steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#else
    mavlink_arc_envelope_set_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_ENVELOPE_SET;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
}

/**
 * @brief Pack a arc_envelope_set message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When set, in sync-MCU time.
 * @param max_speed [m/s] Speed limit; 3 m/s by default for new experiments.
 * @param max_accel [m/s/s] Acceleration limit.
 * @param max_steer [rad] Steering-angle limit.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_envelope_set_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float max_speed, float max_accel, float max_steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#else
    mavlink_arc_envelope_set_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_ENVELOPE_SET;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#endif
}

/**
 * @brief Pack a arc_envelope_set message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] When set, in sync-MCU time.
 * @param max_speed [m/s] Speed limit; 3 m/s by default for new experiments.
 * @param max_accel [m/s/s] Acceleration limit.
 * @param max_steer [rad] Steering-angle limit.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_envelope_set_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float max_speed,float max_accel,float max_steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#else
    mavlink_arc_envelope_set_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_ENVELOPE_SET;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
}

/**
 * @brief Encode a arc_envelope_set struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_envelope_set C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_envelope_set_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_envelope_set_t* arc_envelope_set)
{
    return mavlink_msg_arc_envelope_set_pack(system_id, component_id, msg, arc_envelope_set->time_ns, arc_envelope_set->max_speed, arc_envelope_set->max_accel, arc_envelope_set->max_steer);
}

/**
 * @brief Encode a arc_envelope_set struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_envelope_set C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_envelope_set_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_envelope_set_t* arc_envelope_set)
{
    return mavlink_msg_arc_envelope_set_pack_chan(system_id, component_id, chan, msg, arc_envelope_set->time_ns, arc_envelope_set->max_speed, arc_envelope_set->max_accel, arc_envelope_set->max_steer);
}

/**
 * @brief Encode a arc_envelope_set struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_envelope_set C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_envelope_set_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_envelope_set_t* arc_envelope_set)
{
    return mavlink_msg_arc_envelope_set_pack_status(system_id, component_id, _status, msg,  arc_envelope_set->time_ns, arc_envelope_set->max_speed, arc_envelope_set->max_accel, arc_envelope_set->max_steer);
}

/**
 * @brief Send a arc_envelope_set message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] When set, in sync-MCU time.
 * @param max_speed [m/s] Speed limit; 3 m/s by default for new experiments.
 * @param max_accel [m/s/s] Acceleration limit.
 * @param max_steer [rad] Steering-angle limit.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_envelope_set_send(mavlink_channel_t chan, uint64_t time_ns, float max_speed, float max_accel, float max_steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET, buf, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#else
    mavlink_arc_envelope_set_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET, (const char *)&packet, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#endif
}

/**
 * @brief Send a arc_envelope_set message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_envelope_set_send_struct(mavlink_channel_t chan, const mavlink_arc_envelope_set_t* arc_envelope_set)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_envelope_set_send(chan, arc_envelope_set->time_ns, arc_envelope_set->max_speed, arc_envelope_set->max_accel, arc_envelope_set->max_steer);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET, (const char *)arc_envelope_set, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_envelope_set_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float max_speed, float max_accel, float max_steer)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET, buf, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#else
    mavlink_arc_envelope_set_t *packet = (mavlink_arc_envelope_set_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->max_speed = max_speed;
    packet->max_accel = max_accel;
    packet->max_steer = max_steer;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_ENVELOPE_SET, (const char *)packet, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_ENVELOPE_SET UNPACKING


/**
 * @brief Get field time_ns from arc_envelope_set message
 *
 * @return [ns] When set, in sync-MCU time.
 */
static inline uint64_t mavlink_msg_arc_envelope_set_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field max_speed from arc_envelope_set message
 *
 * @return [m/s] Speed limit; 3 m/s by default for new experiments.
 */
static inline float mavlink_msg_arc_envelope_set_get_max_speed(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field max_accel from arc_envelope_set message
 *
 * @return [m/s/s] Acceleration limit.
 */
static inline float mavlink_msg_arc_envelope_set_get_max_accel(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field max_steer from arc_envelope_set message
 *
 * @return [rad] Steering-angle limit.
 */
static inline float mavlink_msg_arc_envelope_set_get_max_steer(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Decode a arc_envelope_set message into a struct
 *
 * @param msg The message to decode
 * @param arc_envelope_set C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_envelope_set_decode(const mavlink_message_t* msg, mavlink_arc_envelope_set_t* arc_envelope_set)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_envelope_set->time_ns = mavlink_msg_arc_envelope_set_get_time_ns(msg);
    arc_envelope_set->max_speed = mavlink_msg_arc_envelope_set_get_max_speed(msg);
    arc_envelope_set->max_accel = mavlink_msg_arc_envelope_set_get_max_accel(msg);
    arc_envelope_set->max_steer = mavlink_msg_arc_envelope_set_get_max_steer(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN? msg->len : MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN;
        memset(arc_envelope_set, 0, MAVLINK_MSG_ID_ARC_ENVELOPE_SET_LEN);
    memcpy(arc_envelope_set, _MAV_PAYLOAD(msg), len);
#endif
}
