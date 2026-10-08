#pragma once
// MESSAGE ARC_SAFETY_STATE PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_SAFETY_STATE 52008


typedef struct __mavlink_arc_safety_state_t {
 uint64_t time_ns; /*< [ns] Time of the state.*/
 float max_speed; /*< [m/s] Envelope in force: speed.*/
 float max_accel; /*< [m/s/s] Envelope in force: acceleration.*/
 float max_steer; /*< [rad] Envelope in force: steering angle.*/
 uint8_t watchdog; /*<  Heartbeat watchdog state.*/
 uint8_t estop; /*<  1 while the physical e-stop is active.*/
} mavlink_arc_safety_state_t;

#define MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN 22
#define MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN 22
#define MAVLINK_MSG_ID_52008_LEN 22
#define MAVLINK_MSG_ID_52008_MIN_LEN 22

#define MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC 127
#define MAVLINK_MSG_ID_52008_CRC 127



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_SAFETY_STATE { \
    52008, \
    "ARC_SAFETY_STATE", \
    6, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_safety_state_t, time_ns) }, \
         { "max_speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_safety_state_t, max_speed) }, \
         { "max_accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_safety_state_t, max_accel) }, \
         { "max_steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_safety_state_t, max_steer) }, \
         { "watchdog", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_arc_safety_state_t, watchdog) }, \
         { "estop", NULL, MAVLINK_TYPE_UINT8_T, 0, 21, offsetof(mavlink_arc_safety_state_t, estop) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_SAFETY_STATE { \
    "ARC_SAFETY_STATE", \
    6, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_safety_state_t, time_ns) }, \
         { "max_speed", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_safety_state_t, max_speed) }, \
         { "max_accel", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_safety_state_t, max_accel) }, \
         { "max_steer", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_safety_state_t, max_steer) }, \
         { "watchdog", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_arc_safety_state_t, watchdog) }, \
         { "estop", NULL, MAVLINK_TYPE_UINT8_T, 0, 21, offsetof(mavlink_arc_safety_state_t, estop) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_safety_state message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Time of the state.
 * @param max_speed [m/s] Envelope in force: speed.
 * @param max_accel [m/s/s] Envelope in force: acceleration.
 * @param max_steer [rad] Envelope in force: steering angle.
 * @param watchdog  Heartbeat watchdog state.
 * @param estop  1 while the physical e-stop is active.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_safety_state_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float max_speed, float max_accel, float max_steer, uint8_t watchdog, uint8_t estop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);
    _mav_put_uint8_t(buf, 20, watchdog);
    _mav_put_uint8_t(buf, 21, estop);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#else
    mavlink_arc_safety_state_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;
    packet.watchdog = watchdog;
    packet.estop = estop;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_SAFETY_STATE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
}

/**
 * @brief Pack a arc_safety_state message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Time of the state.
 * @param max_speed [m/s] Envelope in force: speed.
 * @param max_accel [m/s/s] Envelope in force: acceleration.
 * @param max_steer [rad] Envelope in force: steering angle.
 * @param watchdog  Heartbeat watchdog state.
 * @param estop  1 while the physical e-stop is active.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_safety_state_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float max_speed, float max_accel, float max_steer, uint8_t watchdog, uint8_t estop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);
    _mav_put_uint8_t(buf, 20, watchdog);
    _mav_put_uint8_t(buf, 21, estop);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#else
    mavlink_arc_safety_state_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;
    packet.watchdog = watchdog;
    packet.estop = estop;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_SAFETY_STATE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#endif
}

/**
 * @brief Pack a arc_safety_state message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Time of the state.
 * @param max_speed [m/s] Envelope in force: speed.
 * @param max_accel [m/s/s] Envelope in force: acceleration.
 * @param max_steer [rad] Envelope in force: steering angle.
 * @param watchdog  Heartbeat watchdog state.
 * @param estop  1 while the physical e-stop is active.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_safety_state_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float max_speed,float max_accel,float max_steer,uint8_t watchdog,uint8_t estop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);
    _mav_put_uint8_t(buf, 20, watchdog);
    _mav_put_uint8_t(buf, 21, estop);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#else
    mavlink_arc_safety_state_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;
    packet.watchdog = watchdog;
    packet.estop = estop;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_SAFETY_STATE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
}

/**
 * @brief Encode a arc_safety_state struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_safety_state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_safety_state_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_safety_state_t* arc_safety_state)
{
    return mavlink_msg_arc_safety_state_pack(system_id, component_id, msg, arc_safety_state->time_ns, arc_safety_state->max_speed, arc_safety_state->max_accel, arc_safety_state->max_steer, arc_safety_state->watchdog, arc_safety_state->estop);
}

/**
 * @brief Encode a arc_safety_state struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_safety_state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_safety_state_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_safety_state_t* arc_safety_state)
{
    return mavlink_msg_arc_safety_state_pack_chan(system_id, component_id, chan, msg, arc_safety_state->time_ns, arc_safety_state->max_speed, arc_safety_state->max_accel, arc_safety_state->max_steer, arc_safety_state->watchdog, arc_safety_state->estop);
}

/**
 * @brief Encode a arc_safety_state struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_safety_state C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_safety_state_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_safety_state_t* arc_safety_state)
{
    return mavlink_msg_arc_safety_state_pack_status(system_id, component_id, _status, msg,  arc_safety_state->time_ns, arc_safety_state->max_speed, arc_safety_state->max_accel, arc_safety_state->max_steer, arc_safety_state->watchdog, arc_safety_state->estop);
}

/**
 * @brief Send a arc_safety_state message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Time of the state.
 * @param max_speed [m/s] Envelope in force: speed.
 * @param max_accel [m/s/s] Envelope in force: acceleration.
 * @param max_steer [rad] Envelope in force: steering angle.
 * @param watchdog  Heartbeat watchdog state.
 * @param estop  1 while the physical e-stop is active.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_safety_state_send(mavlink_channel_t chan, uint64_t time_ns, float max_speed, float max_accel, float max_steer, uint8_t watchdog, uint8_t estop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);
    _mav_put_uint8_t(buf, 20, watchdog);
    _mav_put_uint8_t(buf, 21, estop);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE, buf, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#else
    mavlink_arc_safety_state_t packet;
    packet.time_ns = time_ns;
    packet.max_speed = max_speed;
    packet.max_accel = max_accel;
    packet.max_steer = max_steer;
    packet.watchdog = watchdog;
    packet.estop = estop;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE, (const char *)&packet, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#endif
}

/**
 * @brief Send a arc_safety_state message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_safety_state_send_struct(mavlink_channel_t chan, const mavlink_arc_safety_state_t* arc_safety_state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_safety_state_send(chan, arc_safety_state->time_ns, arc_safety_state->max_speed, arc_safety_state->max_accel, arc_safety_state->max_steer, arc_safety_state->watchdog, arc_safety_state->estop);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE, (const char *)arc_safety_state, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_safety_state_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float max_speed, float max_accel, float max_steer, uint8_t watchdog, uint8_t estop)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, max_speed);
    _mav_put_float(buf, 12, max_accel);
    _mav_put_float(buf, 16, max_steer);
    _mav_put_uint8_t(buf, 20, watchdog);
    _mav_put_uint8_t(buf, 21, estop);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE, buf, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#else
    mavlink_arc_safety_state_t *packet = (mavlink_arc_safety_state_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->max_speed = max_speed;
    packet->max_accel = max_accel;
    packet->max_steer = max_steer;
    packet->watchdog = watchdog;
    packet->estop = estop;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_SAFETY_STATE, (const char *)packet, MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN, MAVLINK_MSG_ID_ARC_SAFETY_STATE_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_SAFETY_STATE UNPACKING


/**
 * @brief Get field time_ns from arc_safety_state message
 *
 * @return [ns] Time of the state.
 */
static inline uint64_t mavlink_msg_arc_safety_state_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field max_speed from arc_safety_state message
 *
 * @return [m/s] Envelope in force: speed.
 */
static inline float mavlink_msg_arc_safety_state_get_max_speed(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field max_accel from arc_safety_state message
 *
 * @return [m/s/s] Envelope in force: acceleration.
 */
static inline float mavlink_msg_arc_safety_state_get_max_accel(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field max_steer from arc_safety_state message
 *
 * @return [rad] Envelope in force: steering angle.
 */
static inline float mavlink_msg_arc_safety_state_get_max_steer(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field watchdog from arc_safety_state message
 *
 * @return  Heartbeat watchdog state.
 */
static inline uint8_t mavlink_msg_arc_safety_state_get_watchdog(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  20);
}

/**
 * @brief Get field estop from arc_safety_state message
 *
 * @return  1 while the physical e-stop is active.
 */
static inline uint8_t mavlink_msg_arc_safety_state_get_estop(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  21);
}

/**
 * @brief Decode a arc_safety_state message into a struct
 *
 * @param msg The message to decode
 * @param arc_safety_state C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_safety_state_decode(const mavlink_message_t* msg, mavlink_arc_safety_state_t* arc_safety_state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_safety_state->time_ns = mavlink_msg_arc_safety_state_get_time_ns(msg);
    arc_safety_state->max_speed = mavlink_msg_arc_safety_state_get_max_speed(msg);
    arc_safety_state->max_accel = mavlink_msg_arc_safety_state_get_max_accel(msg);
    arc_safety_state->max_steer = mavlink_msg_arc_safety_state_get_max_steer(msg);
    arc_safety_state->watchdog = mavlink_msg_arc_safety_state_get_watchdog(msg);
    arc_safety_state->estop = mavlink_msg_arc_safety_state_get_estop(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN? msg->len : MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN;
        memset(arc_safety_state, 0, MAVLINK_MSG_ID_ARC_SAFETY_STATE_LEN);
    memcpy(arc_safety_state, _MAV_PAYLOAD(msg), len);
#endif
}
