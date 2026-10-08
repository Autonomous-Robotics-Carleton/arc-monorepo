#pragma once
// MESSAGE ARC_OPERATOR_HEARTBEAT PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT 52051


typedef struct __mavlink_arc_operator_heartbeat_t {
 uint64_t time_ns; /*< [ns] When the Orin forwarded it, in sync-MCU time.*/
 uint32_t counter; /*<  The laptop's heartbeat counter, passed through.*/
} mavlink_arc_operator_heartbeat_t;

#define MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN 12
#define MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN 12
#define MAVLINK_MSG_ID_52051_LEN 12
#define MAVLINK_MSG_ID_52051_MIN_LEN 12

#define MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC 49
#define MAVLINK_MSG_ID_52051_CRC 49



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_OPERATOR_HEARTBEAT { \
    52051, \
    "ARC_OPERATOR_HEARTBEAT", \
    2, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_operator_heartbeat_t, time_ns) }, \
         { "counter", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_operator_heartbeat_t, counter) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_OPERATOR_HEARTBEAT { \
    "ARC_OPERATOR_HEARTBEAT", \
    2, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_operator_heartbeat_t, time_ns) }, \
         { "counter", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_operator_heartbeat_t, counter) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_operator_heartbeat message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's heartbeat counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#else
    mavlink_arc_operator_heartbeat_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
}

/**
 * @brief Pack a arc_operator_heartbeat message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's heartbeat counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#else
    mavlink_arc_operator_heartbeat_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#endif
}

/**
 * @brief Pack a arc_operator_heartbeat message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's heartbeat counter, passed through.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#else
    mavlink_arc_operator_heartbeat_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
}

/**
 * @brief Encode a arc_operator_heartbeat struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_operator_heartbeat C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_operator_heartbeat_t* arc_operator_heartbeat)
{
    return mavlink_msg_arc_operator_heartbeat_pack(system_id, component_id, msg, arc_operator_heartbeat->time_ns, arc_operator_heartbeat->counter);
}

/**
 * @brief Encode a arc_operator_heartbeat struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_operator_heartbeat C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_operator_heartbeat_t* arc_operator_heartbeat)
{
    return mavlink_msg_arc_operator_heartbeat_pack_chan(system_id, component_id, chan, msg, arc_operator_heartbeat->time_ns, arc_operator_heartbeat->counter);
}

/**
 * @brief Encode a arc_operator_heartbeat struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_operator_heartbeat C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_operator_heartbeat_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_operator_heartbeat_t* arc_operator_heartbeat)
{
    return mavlink_msg_arc_operator_heartbeat_pack_status(system_id, component_id, _status, msg,  arc_operator_heartbeat->time_ns, arc_operator_heartbeat->counter);
}

/**
 * @brief Send a arc_operator_heartbeat message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] When the Orin forwarded it, in sync-MCU time.
 * @param counter  The laptop's heartbeat counter, passed through.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_operator_heartbeat_send(mavlink_channel_t chan, uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT, buf, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#else
    mavlink_arc_operator_heartbeat_t packet;
    packet.time_ns = time_ns;
    packet.counter = counter;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT, (const char *)&packet, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#endif
}

/**
 * @brief Send a arc_operator_heartbeat message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_operator_heartbeat_send_struct(mavlink_channel_t chan, const mavlink_arc_operator_heartbeat_t* arc_operator_heartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_operator_heartbeat_send(chan, arc_operator_heartbeat->time_ns, arc_operator_heartbeat->counter);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT, (const char *)arc_operator_heartbeat, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_operator_heartbeat_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, uint32_t counter)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, counter);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT, buf, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#else
    mavlink_arc_operator_heartbeat_t *packet = (mavlink_arc_operator_heartbeat_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->counter = counter;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT, (const char *)packet, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_OPERATOR_HEARTBEAT UNPACKING


/**
 * @brief Get field time_ns from arc_operator_heartbeat message
 *
 * @return [ns] When the Orin forwarded it, in sync-MCU time.
 */
static inline uint64_t mavlink_msg_arc_operator_heartbeat_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field counter from arc_operator_heartbeat message
 *
 * @return  The laptop's heartbeat counter, passed through.
 */
static inline uint32_t mavlink_msg_arc_operator_heartbeat_get_counter(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Decode a arc_operator_heartbeat message into a struct
 *
 * @param msg The message to decode
 * @param arc_operator_heartbeat C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_operator_heartbeat_decode(const mavlink_message_t* msg, mavlink_arc_operator_heartbeat_t* arc_operator_heartbeat)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_operator_heartbeat->time_ns = mavlink_msg_arc_operator_heartbeat_get_time_ns(msg);
    arc_operator_heartbeat->counter = mavlink_msg_arc_operator_heartbeat_get_counter(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN? msg->len : MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN;
        memset(arc_operator_heartbeat, 0, MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_LEN);
    memcpy(arc_operator_heartbeat, _MAV_PAYLOAD(msg), len);
#endif
}
