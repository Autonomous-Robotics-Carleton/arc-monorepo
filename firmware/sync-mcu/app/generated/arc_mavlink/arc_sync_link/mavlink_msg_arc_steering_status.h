#pragma once
// MESSAGE ARC_STEERING_STATUS PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_STEERING_STATUS 52005


typedef struct __mavlink_arc_steering_status_t {
 uint64_t time_ns; /*< [ns] Sample time.*/
 float angle; /*< [rad] Steering output angle.*/
 float velocity; /*< [rad/s] Steering output velocity.*/
 float torque; /*< [N.m] Output torque estimate from q-axis current.*/
 float temp; /*< [degC] Controller temperature.*/
 uint8_t fault; /*<  moteus fault code; 0 is none.*/
} mavlink_arc_steering_status_t;

#define MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN 25
#define MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN 25
#define MAVLINK_MSG_ID_52005_LEN 25
#define MAVLINK_MSG_ID_52005_MIN_LEN 25

#define MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC 76
#define MAVLINK_MSG_ID_52005_CRC 76



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_STEERING_STATUS { \
    52005, \
    "ARC_STEERING_STATUS", \
    6, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_steering_status_t, time_ns) }, \
         { "angle", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_steering_status_t, angle) }, \
         { "velocity", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_steering_status_t, velocity) }, \
         { "torque", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_steering_status_t, torque) }, \
         { "temp", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_arc_steering_status_t, temp) }, \
         { "fault", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_arc_steering_status_t, fault) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_STEERING_STATUS { \
    "ARC_STEERING_STATUS", \
    6, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_steering_status_t, time_ns) }, \
         { "angle", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_steering_status_t, angle) }, \
         { "velocity", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_steering_status_t, velocity) }, \
         { "torque", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_steering_status_t, torque) }, \
         { "temp", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_arc_steering_status_t, temp) }, \
         { "fault", NULL, MAVLINK_TYPE_UINT8_T, 0, 24, offsetof(mavlink_arc_steering_status_t, fault) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_steering_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param angle [rad] Steering output angle.
 * @param velocity [rad/s] Steering output velocity.
 * @param torque [N.m] Output torque estimate from q-axis current.
 * @param temp [degC] Controller temperature.
 * @param fault  moteus fault code; 0 is none.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_steering_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float angle, float velocity, float torque, float temp, uint8_t fault)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, angle);
    _mav_put_float(buf, 12, velocity);
    _mav_put_float(buf, 16, torque);
    _mav_put_float(buf, 20, temp);
    _mav_put_uint8_t(buf, 24, fault);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#else
    mavlink_arc_steering_status_t packet;
    packet.time_ns = time_ns;
    packet.angle = angle;
    packet.velocity = velocity;
    packet.torque = torque;
    packet.temp = temp;
    packet.fault = fault;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_STEERING_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
}

/**
 * @brief Pack a arc_steering_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param angle [rad] Steering output angle.
 * @param velocity [rad/s] Steering output velocity.
 * @param torque [N.m] Output torque estimate from q-axis current.
 * @param temp [degC] Controller temperature.
 * @param fault  moteus fault code; 0 is none.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_steering_status_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float angle, float velocity, float torque, float temp, uint8_t fault)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, angle);
    _mav_put_float(buf, 12, velocity);
    _mav_put_float(buf, 16, torque);
    _mav_put_float(buf, 20, temp);
    _mav_put_uint8_t(buf, 24, fault);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#else
    mavlink_arc_steering_status_t packet;
    packet.time_ns = time_ns;
    packet.angle = angle;
    packet.velocity = velocity;
    packet.torque = torque;
    packet.temp = temp;
    packet.fault = fault;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_STEERING_STATUS;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#endif
}

/**
 * @brief Pack a arc_steering_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sample time.
 * @param angle [rad] Steering output angle.
 * @param velocity [rad/s] Steering output velocity.
 * @param torque [N.m] Output torque estimate from q-axis current.
 * @param temp [degC] Controller temperature.
 * @param fault  moteus fault code; 0 is none.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_steering_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float angle,float velocity,float torque,float temp,uint8_t fault)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, angle);
    _mav_put_float(buf, 12, velocity);
    _mav_put_float(buf, 16, torque);
    _mav_put_float(buf, 20, temp);
    _mav_put_uint8_t(buf, 24, fault);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#else
    mavlink_arc_steering_status_t packet;
    packet.time_ns = time_ns;
    packet.angle = angle;
    packet.velocity = velocity;
    packet.torque = torque;
    packet.temp = temp;
    packet.fault = fault;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_STEERING_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
}

/**
 * @brief Encode a arc_steering_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_steering_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_steering_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_steering_status_t* arc_steering_status)
{
    return mavlink_msg_arc_steering_status_pack(system_id, component_id, msg, arc_steering_status->time_ns, arc_steering_status->angle, arc_steering_status->velocity, arc_steering_status->torque, arc_steering_status->temp, arc_steering_status->fault);
}

/**
 * @brief Encode a arc_steering_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_steering_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_steering_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_steering_status_t* arc_steering_status)
{
    return mavlink_msg_arc_steering_status_pack_chan(system_id, component_id, chan, msg, arc_steering_status->time_ns, arc_steering_status->angle, arc_steering_status->velocity, arc_steering_status->torque, arc_steering_status->temp, arc_steering_status->fault);
}

/**
 * @brief Encode a arc_steering_status struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_steering_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_steering_status_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_steering_status_t* arc_steering_status)
{
    return mavlink_msg_arc_steering_status_pack_status(system_id, component_id, _status, msg,  arc_steering_status->time_ns, arc_steering_status->angle, arc_steering_status->velocity, arc_steering_status->torque, arc_steering_status->temp, arc_steering_status->fault);
}

/**
 * @brief Send a arc_steering_status message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sample time.
 * @param angle [rad] Steering output angle.
 * @param velocity [rad/s] Steering output velocity.
 * @param torque [N.m] Output torque estimate from q-axis current.
 * @param temp [degC] Controller temperature.
 * @param fault  moteus fault code; 0 is none.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_steering_status_send(mavlink_channel_t chan, uint64_t time_ns, float angle, float velocity, float torque, float temp, uint8_t fault)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, angle);
    _mav_put_float(buf, 12, velocity);
    _mav_put_float(buf, 16, torque);
    _mav_put_float(buf, 20, temp);
    _mav_put_uint8_t(buf, 24, fault);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS, buf, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#else
    mavlink_arc_steering_status_t packet;
    packet.time_ns = time_ns;
    packet.angle = angle;
    packet.velocity = velocity;
    packet.torque = torque;
    packet.temp = temp;
    packet.fault = fault;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS, (const char *)&packet, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#endif
}

/**
 * @brief Send a arc_steering_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_steering_status_send_struct(mavlink_channel_t chan, const mavlink_arc_steering_status_t* arc_steering_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_steering_status_send(chan, arc_steering_status->time_ns, arc_steering_status->angle, arc_steering_status->velocity, arc_steering_status->torque, arc_steering_status->temp, arc_steering_status->fault);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS, (const char *)arc_steering_status, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_steering_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float angle, float velocity, float torque, float temp, uint8_t fault)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, angle);
    _mav_put_float(buf, 12, velocity);
    _mav_put_float(buf, 16, torque);
    _mav_put_float(buf, 20, temp);
    _mav_put_uint8_t(buf, 24, fault);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS, buf, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#else
    mavlink_arc_steering_status_t *packet = (mavlink_arc_steering_status_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->angle = angle;
    packet->velocity = velocity;
    packet->torque = torque;
    packet->temp = temp;
    packet->fault = fault;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_STEERING_STATUS, (const char *)packet, MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN, MAVLINK_MSG_ID_ARC_STEERING_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_STEERING_STATUS UNPACKING


/**
 * @brief Get field time_ns from arc_steering_status message
 *
 * @return [ns] Sample time.
 */
static inline uint64_t mavlink_msg_arc_steering_status_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field angle from arc_steering_status message
 *
 * @return [rad] Steering output angle.
 */
static inline float mavlink_msg_arc_steering_status_get_angle(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field velocity from arc_steering_status message
 *
 * @return [rad/s] Steering output velocity.
 */
static inline float mavlink_msg_arc_steering_status_get_velocity(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field torque from arc_steering_status message
 *
 * @return [N.m] Output torque estimate from q-axis current.
 */
static inline float mavlink_msg_arc_steering_status_get_torque(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field temp from arc_steering_status message
 *
 * @return [degC] Controller temperature.
 */
static inline float mavlink_msg_arc_steering_status_get_temp(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field fault from arc_steering_status message
 *
 * @return  moteus fault code; 0 is none.
 */
static inline uint8_t mavlink_msg_arc_steering_status_get_fault(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  24);
}

/**
 * @brief Decode a arc_steering_status message into a struct
 *
 * @param msg The message to decode
 * @param arc_steering_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_steering_status_decode(const mavlink_message_t* msg, mavlink_arc_steering_status_t* arc_steering_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_steering_status->time_ns = mavlink_msg_arc_steering_status_get_time_ns(msg);
    arc_steering_status->angle = mavlink_msg_arc_steering_status_get_angle(msg);
    arc_steering_status->velocity = mavlink_msg_arc_steering_status_get_velocity(msg);
    arc_steering_status->torque = mavlink_msg_arc_steering_status_get_torque(msg);
    arc_steering_status->temp = mavlink_msg_arc_steering_status_get_temp(msg);
    arc_steering_status->fault = mavlink_msg_arc_steering_status_get_fault(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN? msg->len : MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN;
        memset(arc_steering_status, 0, MAVLINK_MSG_ID_ARC_STEERING_STATUS_LEN);
    memcpy(arc_steering_status, _MAV_PAYLOAD(msg), len);
#endif
}
