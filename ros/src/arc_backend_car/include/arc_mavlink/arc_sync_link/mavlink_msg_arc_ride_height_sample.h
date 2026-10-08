#pragma once
// MESSAGE ARC_RIDE_HEIGHT_SAMPLE PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE 52003


typedef struct __mavlink_arc_ride_height_sample_t {
 uint64_t time_ns; /*< [ns] Sample time.*/
 float range; /*< [m] Distance to the floor.*/
 uint8_t sensor; /*<  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.*/
 uint8_t status; /*<  Sensor status code; 0 is valid.*/
} mavlink_arc_ride_height_sample_t;

#define MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN 14
#define MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN 14
#define MAVLINK_MSG_ID_52003_LEN 14
#define MAVLINK_MSG_ID_52003_MIN_LEN 14

#define MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC 243
#define MAVLINK_MSG_ID_52003_CRC 243



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_RIDE_HEIGHT_SAMPLE { \
    52003, \
    "ARC_RIDE_HEIGHT_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_ride_height_sample_t, time_ns) }, \
         { "range", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_ride_height_sample_t, range) }, \
         { "sensor", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_arc_ride_height_sample_t, sensor) }, \
         { "status", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_arc_ride_height_sample_t, status) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_RIDE_HEIGHT_SAMPLE { \
    "ARC_RIDE_HEIGHT_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_ride_height_sample_t, time_ns) }, \
         { "range", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_ride_height_sample_t, range) }, \
         { "sensor", NULL, MAVLINK_TYPE_UINT8_T, 0, 12, offsetof(mavlink_arc_ride_height_sample_t, sensor) }, \
         { "status", NULL, MAVLINK_TYPE_UINT8_T, 0, 13, offsetof(mavlink_arc_ride_height_sample_t, status) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_ride_height_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param range [m] Distance to the floor.
 * @param sensor  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.
 * @param status  Sensor status code; 0 is valid.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float range, uint8_t sensor, uint8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, range);
    _mav_put_uint8_t(buf, 12, sensor);
    _mav_put_uint8_t(buf, 13, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#else
    mavlink_arc_ride_height_sample_t packet;
    packet.time_ns = time_ns;
    packet.range = range;
    packet.sensor = sensor;
    packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
}

/**
 * @brief Pack a arc_ride_height_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param range [m] Distance to the floor.
 * @param sensor  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.
 * @param status  Sensor status code; 0 is valid.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float range, uint8_t sensor, uint8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, range);
    _mav_put_uint8_t(buf, 12, sensor);
    _mav_put_uint8_t(buf, 13, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#else
    mavlink_arc_ride_height_sample_t packet;
    packet.time_ns = time_ns;
    packet.range = range;
    packet.sensor = sensor;
    packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#endif
}

/**
 * @brief Pack a arc_ride_height_sample message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sample time.
 * @param range [m] Distance to the floor.
 * @param sensor  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.
 * @param status  Sensor status code; 0 is valid.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float range,uint8_t sensor,uint8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, range);
    _mav_put_uint8_t(buf, 12, sensor);
    _mav_put_uint8_t(buf, 13, status);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#else
    mavlink_arc_ride_height_sample_t packet;
    packet.time_ns = time_ns;
    packet.range = range;
    packet.sensor = sensor;
    packet.status = status;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
}

/**
 * @brief Encode a arc_ride_height_sample struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_ride_height_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_ride_height_sample_t* arc_ride_height_sample)
{
    return mavlink_msg_arc_ride_height_sample_pack(system_id, component_id, msg, arc_ride_height_sample->time_ns, arc_ride_height_sample->range, arc_ride_height_sample->sensor, arc_ride_height_sample->status);
}

/**
 * @brief Encode a arc_ride_height_sample struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_ride_height_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_ride_height_sample_t* arc_ride_height_sample)
{
    return mavlink_msg_arc_ride_height_sample_pack_chan(system_id, component_id, chan, msg, arc_ride_height_sample->time_ns, arc_ride_height_sample->range, arc_ride_height_sample->sensor, arc_ride_height_sample->status);
}

/**
 * @brief Encode a arc_ride_height_sample struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_ride_height_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_ride_height_sample_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_ride_height_sample_t* arc_ride_height_sample)
{
    return mavlink_msg_arc_ride_height_sample_pack_status(system_id, component_id, _status, msg,  arc_ride_height_sample->time_ns, arc_ride_height_sample->range, arc_ride_height_sample->sensor, arc_ride_height_sample->status);
}

/**
 * @brief Send a arc_ride_height_sample message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sample time.
 * @param range [m] Distance to the floor.
 * @param sensor  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.
 * @param status  Sensor status code; 0 is valid.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_ride_height_sample_send(mavlink_channel_t chan, uint64_t time_ns, float range, uint8_t sensor, uint8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, range);
    _mav_put_uint8_t(buf, 12, sensor);
    _mav_put_uint8_t(buf, 13, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE, buf, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#else
    mavlink_arc_ride_height_sample_t packet;
    packet.time_ns = time_ns;
    packet.range = range;
    packet.sensor = sensor;
    packet.status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE, (const char *)&packet, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#endif
}

/**
 * @brief Send a arc_ride_height_sample message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_ride_height_sample_send_struct(mavlink_channel_t chan, const mavlink_arc_ride_height_sample_t* arc_ride_height_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_ride_height_sample_send(chan, arc_ride_height_sample->time_ns, arc_ride_height_sample->range, arc_ride_height_sample->sensor, arc_ride_height_sample->status);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE, (const char *)arc_ride_height_sample, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_ride_height_sample_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float range, uint8_t sensor, uint8_t status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, range);
    _mav_put_uint8_t(buf, 12, sensor);
    _mav_put_uint8_t(buf, 13, status);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE, buf, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#else
    mavlink_arc_ride_height_sample_t *packet = (mavlink_arc_ride_height_sample_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->range = range;
    packet->sensor = sensor;
    packet->status = status;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE, (const char *)packet, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_RIDE_HEIGHT_SAMPLE UNPACKING


/**
 * @brief Get field time_ns from arc_ride_height_sample message
 *
 * @return [ns] Sample time.
 */
static inline uint64_t mavlink_msg_arc_ride_height_sample_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field range from arc_ride_height_sample message
 *
 * @return [m] Distance to the floor.
 */
static inline float mavlink_msg_arc_ride_height_sample_get_range(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field sensor from arc_ride_height_sample message
 *
 * @return  Sensor index: 0–3 corners FL, FR, RL, RR; 4 the ground-speed camera's.
 */
static inline uint8_t mavlink_msg_arc_ride_height_sample_get_sensor(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  12);
}

/**
 * @brief Get field status from arc_ride_height_sample message
 *
 * @return  Sensor status code; 0 is valid.
 */
static inline uint8_t mavlink_msg_arc_ride_height_sample_get_status(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  13);
}

/**
 * @brief Decode a arc_ride_height_sample message into a struct
 *
 * @param msg The message to decode
 * @param arc_ride_height_sample C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_ride_height_sample_decode(const mavlink_message_t* msg, mavlink_arc_ride_height_sample_t* arc_ride_height_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_ride_height_sample->time_ns = mavlink_msg_arc_ride_height_sample_get_time_ns(msg);
    arc_ride_height_sample->range = mavlink_msg_arc_ride_height_sample_get_range(msg);
    arc_ride_height_sample->sensor = mavlink_msg_arc_ride_height_sample_get_sensor(msg);
    arc_ride_height_sample->status = mavlink_msg_arc_ride_height_sample_get_status(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN? msg->len : MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN;
        memset(arc_ride_height_sample, 0, MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_LEN);
    memcpy(arc_ride_height_sample, _MAV_PAYLOAD(msg), len);
#endif
}
