#pragma once
// MESSAGE ARC_REARM PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_REARM 52053


typedef struct __mavlink_arc_rearm_t {
 uint64_t time_ns; /*< [ns] When the Orin forwarded it, in sync-MCU time.*/
 uint32_t counter; /*<  The laptop's re-arm counter, passed through.*/
} mavlink_arc_rearm_t;

#define MAVLINK_MSG_ID_ARC_REARM_LEN 12
#define MAVLINK_MSG_ID_ARC_REARM_MIN_LEN 12
#define MAVLINK_MSG_ID_52053_LEN 12
#define MAVLINK_MSG_ID_52053_MIN_LEN 12

#define MAVLINK_MSG_ID_ARC_REARM_CRC 21
#define MAVLINK_MSG_ID_52053_CRC 21



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_REARM { \
    52053, \
    "ARC_REARM", \
    2, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_rearm_t, time_ns) }, \
         { "counter", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_rearm_t, counter) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_REARM { \
    "ARC_REARM", \
    2, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_rearm_t, time_ns) }, \
         { "counter", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_rearm_t, counter) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_rearm message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's re-arm counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_rearm_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_REARM_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_REARM_LEN);
#else
    mavlink_arc_rearm_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_REARM_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_REARM;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
}

/**
 * @brief Pack a arc_rearm message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's re-arm counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_rearm_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_REARM_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_REARM_LEN);
#else
    mavlink_arc_rearm_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_REARM_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_REARM;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN);
#endif
}

/**
 * @brief Pack a arc_rearm message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's re-arm counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_rearm_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_REARM_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_REARM_LEN);
#else
    mavlink_arc_rearm_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_REARM_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_REARM;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
}

/**
 * @brief Encode a arc_rearm struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_rearm C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_rearm_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_rearm_t* arc_rearm)
{
    return mavlink_msg_arc_rearm_pack(system_id, component_id, msg, arc_rearm->time_ns, arc_rearm->counter);
}

/**
 * @brief Encode a arc_rearm struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_rearm C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_rearm_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_rearm_t* arc_rearm)
{
    return mavlink_msg_arc_rearm_pack_chan(system_id, component_id, chan, msg, arc_rearm->time_ns, arc_rearm->counter);
}

/**
 * @brief Encode a arc_rearm struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_rearm C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_rearm_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_rearm_t* arc_rearm)
{
    return mavlink_msg_arc_rearm_pack_status(system_id, component_id, _status, msg,  arc_rearm->time_ns, arc_rearm->counter);
}

/**
 * @brief Send a arc_rearm message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's re-arm counter, passed through.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_rearm_send(mavlink_channel_t chan, uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_REARM_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_REARM, buf, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#else
    mavlink_arc_rearm_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_REARM, (const char *)&packet, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#endif
}

/**
 * @brief Send a arc_rearm message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_rearm_send_struct(mavlink_channel_t chan, const mavlink_arc_rearm_t* arc_rearm)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_rearm_send(chan, arc_rearm->time_ns, arc_rearm->counter);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_REARM, (const char *)arc_rearm, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_REARM_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_rearm_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_REARM, buf, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#else
    mavlink_arc_rearm_t *packet = (mavlink_arc_rearm_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->counter = counter;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_REARM, (const char *)packet, MAVLINK_MSG_ID_ARC_REARM_MIN_LEN, MAVLINK_MSG_ID_ARC_REARM_LEN, MAVLINK_MSG_ID_ARC_REARM_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_REARM UNPACKING


/**
 * @brief Get field time_ns from arc_rearm message
 *
 * @return [ns] When the Orin forwarded it, in sync-MCU time.
 */
static inline uint64_t mavlink_msg_arc_rearm_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field counter from arc_rearm message
 *
 * @return  The laptop's re-arm counter, passed through.
 */
static inline uint32_t mavlink_msg_arc_rearm_get_counter(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Decode a arc_rearm message into a struct
 *
 * @param msg The message to decode
 * @param arc_rearm C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_rearm_decode(const mavlink_message_t* msg, mavlink_arc_rearm_t* arc_rearm)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_rearm->time_ns = mavlink_msg_arc_rearm_get_time_ns(msg);
    arc_rearm->counter = mavlink_msg_arc_rearm_get_counter(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_REARM_LEN? msg->len : MAVLINK_MSG_ID_ARC_REARM_LEN;
        memset(arc_rearm, 0, MAVLINK_MSG_ID_ARC_REARM_LEN);
    memcpy(arc_rearm, _MAV_PAYLOAD(msg), len);
#endif
}
