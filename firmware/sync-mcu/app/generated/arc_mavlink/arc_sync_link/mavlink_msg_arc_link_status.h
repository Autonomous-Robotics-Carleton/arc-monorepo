#pragma once
// MESSAGE ARC_LINK_STATUS PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_LINK_STATUS 52000


typedef struct __mavlink_arc_link_status_t {
 uint64_t time_ns; /*< [ns] Sync-MCU time when sent.*/
 uint32_t rx_gaps; /*<  Received packets missing since start, from MAVLink sequence numbers.*/
 uint32_t rx_bad; /*<  Packets rejected (CRC or unknown message) since start.*/
 uint16_t protocol_version; /*<  The <version> of this file the sender was built from.*/
} mavlink_arc_link_status_t;

#define MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN 18
#define MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN 18
#define MAVLINK_MSG_ID_52000_LEN 18
#define MAVLINK_MSG_ID_52000_MIN_LEN 18

#define MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC 7
#define MAVLINK_MSG_ID_52000_CRC 7



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_LINK_STATUS { \
    52000, \
    "ARC_LINK_STATUS", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_link_status_t, time_ns) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_arc_link_status_t, protocol_version) }, \
         { "rx_gaps", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_link_status_t, rx_gaps) }, \
         { "rx_bad", NULL, MAVLINK_TYPE_UINT32_T, 0, 12, offsetof(mavlink_arc_link_status_t, rx_bad) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_LINK_STATUS { \
    "ARC_LINK_STATUS", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_link_status_t, time_ns) }, \
         { "protocol_version", NULL, MAVLINK_TYPE_UINT16_T, 0, 16, offsetof(mavlink_arc_link_status_t, protocol_version) }, \
         { "rx_gaps", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_link_status_t, rx_gaps) }, \
         { "rx_bad", NULL, MAVLINK_TYPE_UINT32_T, 0, 12, offsetof(mavlink_arc_link_status_t, rx_bad) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_link_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sync-MCU time when sent.
 * @param protocol_version  The <version> of this file the sender was built from.
 * @param rx_gaps  Received packets missing since start, from MAVLink sequence numbers.
 * @param rx_bad  Packets rejected (CRC or unknown message) since start.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_link_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, uint16_t protocol_version, uint32_t rx_gaps, uint32_t rx_bad)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, rx_gaps);
    _mav_put_uint32_t(buf, 12, rx_bad);
    _mav_put_uint16_t(buf, 16, protocol_version);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#else
    mavlink_arc_link_status_t packet;
    packet.time_ns = time_ns;
    packet.rx_gaps = rx_gaps;
    packet.rx_bad = rx_bad;
    packet.protocol_version = protocol_version;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_LINK_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
}

/**
 * @brief Pack a arc_link_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sync-MCU time when sent.
 * @param protocol_version  The <version> of this file the sender was built from.
 * @param rx_gaps  Received packets missing since start, from MAVLink sequence numbers.
 * @param rx_bad  Packets rejected (CRC or unknown message) since start.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_link_status_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, uint16_t protocol_version, uint32_t rx_gaps, uint32_t rx_bad)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, rx_gaps);
    _mav_put_uint32_t(buf, 12, rx_bad);
    _mav_put_uint16_t(buf, 16, protocol_version);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#else
    mavlink_arc_link_status_t packet;
    packet.time_ns = time_ns;
    packet.rx_gaps = rx_gaps;
    packet.rx_bad = rx_bad;
    packet.protocol_version = protocol_version;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_LINK_STATUS;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#endif
}

/**
 * @brief Pack a arc_link_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sync-MCU time when sent.
 * @param protocol_version  The <version> of this file the sender was built from.
 * @param rx_gaps  Received packets missing since start, from MAVLink sequence numbers.
 * @param rx_bad  Packets rejected (CRC or unknown message) since start.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_link_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,uint16_t protocol_version,uint32_t rx_gaps,uint32_t rx_bad)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, rx_gaps);
    _mav_put_uint32_t(buf, 12, rx_bad);
    _mav_put_uint16_t(buf, 16, protocol_version);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#else
    mavlink_arc_link_status_t packet;
    packet.time_ns = time_ns;
    packet.rx_gaps = rx_gaps;
    packet.rx_bad = rx_bad;
    packet.protocol_version = protocol_version;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_LINK_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
}

/**
 * @brief Encode a arc_link_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_link_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_link_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_link_status_t* arc_link_status)
{
    return mavlink_msg_arc_link_status_pack(system_id, component_id, msg, arc_link_status->time_ns, arc_link_status->protocol_version, arc_link_status->rx_gaps, arc_link_status->rx_bad);
}

/**
 * @brief Encode a arc_link_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_link_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_link_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_link_status_t* arc_link_status)
{
    return mavlink_msg_arc_link_status_pack_chan(system_id, component_id, chan, msg, arc_link_status->time_ns, arc_link_status->protocol_version, arc_link_status->rx_gaps, arc_link_status->rx_bad);
}

/**
 * @brief Encode a arc_link_status struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_link_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_link_status_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_link_status_t* arc_link_status)
{
    return mavlink_msg_arc_link_status_pack_status(system_id, component_id, _status, msg,  arc_link_status->time_ns, arc_link_status->protocol_version, arc_link_status->rx_gaps, arc_link_status->rx_bad);
}

/**
 * @brief Send a arc_link_status message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sync-MCU time when sent.
 * @param protocol_version  The <version> of this file the sender was built from.
 * @param rx_gaps  Received packets missing since start, from MAVLink sequence numbers.
 * @param rx_bad  Packets rejected (CRC or unknown message) since start.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_link_status_send(mavlink_channel_t chan, uint64_t time_ns, uint16_t protocol_version, uint32_t rx_gaps, uint32_t rx_bad)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, rx_gaps);
    _mav_put_uint32_t(buf, 12, rx_bad);
    _mav_put_uint16_t(buf, 16, protocol_version);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_LINK_STATUS, buf, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#else
    mavlink_arc_link_status_t packet;
    packet.time_ns = time_ns;
    packet.rx_gaps = rx_gaps;
    packet.rx_bad = rx_bad;
    packet.protocol_version = protocol_version;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_LINK_STATUS, (const char *)&packet, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#endif
}

/**
 * @brief Send a arc_link_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_link_status_send_struct(mavlink_channel_t chan, const mavlink_arc_link_status_t* arc_link_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_link_status_send(chan, arc_link_status->time_ns, arc_link_status->protocol_version, arc_link_status->rx_gaps, arc_link_status->rx_bad);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_LINK_STATUS, (const char *)arc_link_status, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_link_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, uint16_t protocol_version, uint32_t rx_gaps, uint32_t rx_bad)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, rx_gaps);
    _mav_put_uint32_t(buf, 12, rx_bad);
    _mav_put_uint16_t(buf, 16, protocol_version);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_LINK_STATUS, buf, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#else
    mavlink_arc_link_status_t *packet = (mavlink_arc_link_status_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->rx_gaps = rx_gaps;
    packet->rx_bad = rx_bad;
    packet->protocol_version = protocol_version;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_LINK_STATUS, (const char *)packet, MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN, MAVLINK_MSG_ID_ARC_LINK_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_LINK_STATUS UNPACKING


/**
 * @brief Get field time_ns from arc_link_status message
 *
 * @return [ns] Sync-MCU time when sent.
 */
static inline uint64_t mavlink_msg_arc_link_status_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field protocol_version from arc_link_status message
 *
 * @return  The <version> of this file the sender was built from.
 */
static inline uint16_t mavlink_msg_arc_link_status_get_protocol_version(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint16_t(msg,  16);
}

/**
 * @brief Get field rx_gaps from arc_link_status message
 *
 * @return  Received packets missing since start, from MAVLink sequence numbers.
 */
static inline uint32_t mavlink_msg_arc_link_status_get_rx_gaps(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Get field rx_bad from arc_link_status message
 *
 * @return  Packets rejected (CRC or unknown message) since start.
 */
static inline uint32_t mavlink_msg_arc_link_status_get_rx_bad(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  12);
}

/**
 * @brief Decode a arc_link_status message into a struct
 *
 * @param msg The message to decode
 * @param arc_link_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_link_status_decode(const mavlink_message_t* msg, mavlink_arc_link_status_t* arc_link_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_link_status->time_ns = mavlink_msg_arc_link_status_get_time_ns(msg);
    arc_link_status->rx_gaps = mavlink_msg_arc_link_status_get_rx_gaps(msg);
    arc_link_status->rx_bad = mavlink_msg_arc_link_status_get_rx_bad(msg);
    arc_link_status->protocol_version = mavlink_msg_arc_link_status_get_protocol_version(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN? msg->len : MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN;
        memset(arc_link_status, 0, MAVLINK_MSG_ID_ARC_LINK_STATUS_LEN);
    memcpy(arc_link_status, _MAV_PAYLOAD(msg), len);
#endif
}
