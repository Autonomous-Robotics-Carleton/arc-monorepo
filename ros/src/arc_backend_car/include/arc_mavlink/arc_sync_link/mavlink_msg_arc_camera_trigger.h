#pragma once
// MESSAGE ARC_CAMERA_TRIGGER PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER 52006


typedef struct __mavlink_arc_camera_trigger_t {
 uint64_t time_ns; /*< [ns] Trigger time.*/
 uint32_t frame; /*<  Frame sequence number for this camera.*/
 uint8_t camera; /*<  Trigger output index.*/
} mavlink_arc_camera_trigger_t;

#define MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN 13
#define MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN 13
#define MAVLINK_MSG_ID_52006_LEN 13
#define MAVLINK_MSG_ID_52006_MIN_LEN 13

#define MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC 17
#define MAVLINK_MSG_ID_52006_CRC 17



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_CAMERA_TRIGGER { \
    52006, \
    "ARC_CAMERA_TRIGGER", \
    3, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_camera_trigger_t, time_ns) }, \
         { "frame", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_camera_trigger_t, frame) }, \
         { "camera", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_arc_camera_trigger_t, camera) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_CAMERA_TRIGGER { \
    "ARC_CAMERA_TRIGGER", \
    3, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_camera_trigger_t, time_ns) }, \
         { "frame", NULL, MAVLINK_TYPE_UINT32_T, 0, 8, offsetof(mavlink_arc_camera_trigger_t, frame) }, \
         { "camera", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_arc_camera_trigger_t, camera) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_camera_trigger message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Trigger time.
 * @param frame  Frame sequence number for this camera.
 * @param camera  Trigger output index.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t frame, uint8_t camera)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, frame);
    _mav_put_uint8_t(buf, 12, camera);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#else
    mavlink_arc_camera_trigger_t packet;
    packet.time_ns = time_ns;
    packet.frame = frame;
    packet.camera = camera;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
}

/**
 * @brief Pack a arc_camera_trigger message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Trigger time.
 * @param frame  Frame sequence number for this camera.
 * @param camera  Trigger output index.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, uint32_t frame, uint8_t camera)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, frame);
    _mav_put_uint8_t(buf, 12, camera);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#else
    mavlink_arc_camera_trigger_t packet;
    packet.time_ns = time_ns;
    packet.frame = frame;
    packet.camera = camera;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#endif
}

/**
 * @brief Pack a arc_camera_trigger message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Trigger time.
 * @param frame  Frame sequence number for this camera.
 * @param camera  Trigger output index.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,uint32_t frame,uint8_t camera)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, frame);
    _mav_put_uint8_t(buf, 12, camera);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#else
    mavlink_arc_camera_trigger_t packet;
    packet.time_ns = time_ns;
    packet.frame = frame;
    packet.camera = camera;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
}

/**
 * @brief Encode a arc_camera_trigger struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_camera_trigger C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_camera_trigger_t* arc_camera_trigger)
{
    return mavlink_msg_arc_camera_trigger_pack(system_id, component_id, msg, arc_camera_trigger->time_ns, arc_camera_trigger->frame, arc_camera_trigger->camera);
}

/**
 * @brief Encode a arc_camera_trigger struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_camera_trigger C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_camera_trigger_t* arc_camera_trigger)
{
    return mavlink_msg_arc_camera_trigger_pack_chan(system_id, component_id, chan, msg, arc_camera_trigger->time_ns, arc_camera_trigger->frame, arc_camera_trigger->camera);
}

/**
 * @brief Encode a arc_camera_trigger struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_camera_trigger C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_camera_trigger_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_camera_trigger_t* arc_camera_trigger)
{
    return mavlink_msg_arc_camera_trigger_pack_status(system_id, component_id, _status, msg,  arc_camera_trigger->time_ns, arc_camera_trigger->frame, arc_camera_trigger->camera);
}

/**
 * @brief Send a arc_camera_trigger message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Trigger time.
 * @param frame  Frame sequence number for this camera.
 * @param camera  Trigger output index.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_camera_trigger_send(mavlink_channel_t chan, uint64_t time_ns, uint32_t frame, uint8_t camera)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, frame);
    _mav_put_uint8_t(buf, 12, camera);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER, buf, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#else
    mavlink_arc_camera_trigger_t packet;
    packet.time_ns = time_ns;
    packet.frame = frame;
    packet.camera = camera;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER, (const char *)&packet, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#endif
}

/**
 * @brief Send a arc_camera_trigger message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_camera_trigger_send_struct(mavlink_channel_t chan, const mavlink_arc_camera_trigger_t* arc_camera_trigger)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_camera_trigger_send(chan, arc_camera_trigger->time_ns, arc_camera_trigger->frame, arc_camera_trigger->camera);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER, (const char *)arc_camera_trigger, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_camera_trigger_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, uint32_t frame, uint8_t camera)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint32_t(buf, 8, frame);
    _mav_put_uint8_t(buf, 12, camera);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER, buf, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#else
    mavlink_arc_camera_trigger_t *packet = (mavlink_arc_camera_trigger_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->frame = frame;
    packet->camera = camera;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER, (const char *)packet, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_CAMERA_TRIGGER UNPACKING


/**
 * @brief Get field time_ns from arc_camera_trigger message
 *
 * @return [ns] Trigger time.
 */
static inline uint64_t mavlink_msg_arc_camera_trigger_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field frame from arc_camera_trigger message
 *
 * @return  Frame sequence number for this camera.
 */
static inline uint32_t mavlink_msg_arc_camera_trigger_get_frame(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint32_t(msg,  8);
}

/**
 * @brief Get field camera from arc_camera_trigger message
 *
 * @return  Trigger output index.
 */
static inline uint8_t mavlink_msg_arc_camera_trigger_get_camera(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 * @brief Decode a arc_camera_trigger message into a struct
 *
 * @param msg The message to decode
 * @param arc_camera_trigger C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_camera_trigger_decode(const mavlink_message_t* msg, mavlink_arc_camera_trigger_t* arc_camera_trigger)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_camera_trigger->time_ns = mavlink_msg_arc_camera_trigger_get_time_ns(msg);
    arc_camera_trigger->frame = mavlink_msg_arc_camera_trigger_get_frame(msg);
    arc_camera_trigger->camera = mavlink_msg_arc_camera_trigger_get_camera(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN? msg->len : MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN;
        memset(arc_camera_trigger, 0, MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_LEN);
    memcpy(arc_camera_trigger, _MAV_PAYLOAD(msg), len);
#endif
}
