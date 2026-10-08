#pragma once
// MESSAGE ARC_IMU_SAMPLE PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_IMU_SAMPLE 52001


typedef struct __mavlink_arc_imu_sample_t {
 uint64_t time_ns; /*< [ns] Sample time.*/
 float accel[3]; /*< [m/s/s] Acceleration, body frame (x forward, y left, z up).*/
 float gyro[3]; /*< [rad/s] Angular rate, body frame.*/
 uint8_t imu; /*<  Which IMU.*/
} mavlink_arc_imu_sample_t;

#define MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN 33
#define MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN 33
#define MAVLINK_MSG_ID_52001_LEN 33
#define MAVLINK_MSG_ID_52001_MIN_LEN 33

#define MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC 54
#define MAVLINK_MSG_ID_52001_CRC 54

#define MAVLINK_MSG_ARC_IMU_SAMPLE_FIELD_ACCEL_LEN 3
#define MAVLINK_MSG_ARC_IMU_SAMPLE_FIELD_GYRO_LEN 3

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_IMU_SAMPLE { \
    52001, \
    "ARC_IMU_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_imu_sample_t, time_ns) }, \
         { "accel", NULL, MAVLINK_TYPE_FLOAT, 3, 8, offsetof(mavlink_arc_imu_sample_t, accel) }, \
         { "gyro", NULL, MAVLINK_TYPE_FLOAT, 3, 20, offsetof(mavlink_arc_imu_sample_t, gyro) }, \
         { "imu", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_arc_imu_sample_t, imu) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_IMU_SAMPLE { \
    "ARC_IMU_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_imu_sample_t, time_ns) }, \
         { "accel", NULL, MAVLINK_TYPE_FLOAT, 3, 8, offsetof(mavlink_arc_imu_sample_t, accel) }, \
         { "gyro", NULL, MAVLINK_TYPE_FLOAT, 3, 20, offsetof(mavlink_arc_imu_sample_t, gyro) }, \
         { "imu", NULL, MAVLINK_TYPE_UINT8_T, 0, 32, offsetof(mavlink_arc_imu_sample_t, imu) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_imu_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param accel [m/s/s] Acceleration, body frame (x forward, y left, z up).
 * @param gyro [rad/s] Angular rate, body frame.
 * @param imu  Which IMU.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_imu_sample_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, const float *accel, const float *gyro, uint8_t imu)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint8_t(buf, 32, imu);
    _mav_put_float_array(buf, 8, accel, 3);
    _mav_put_float_array(buf, 20, gyro, 3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#else
    mavlink_arc_imu_sample_t packet;
    packet.time_ns = time_ns;
    packet.imu = imu;
    mav_array_memcpy(packet.accel, accel, sizeof(float)*3);
    mav_array_memcpy(packet.gyro, gyro, sizeof(float)*3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_IMU_SAMPLE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
}

/**
 * @brief Pack a arc_imu_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param accel [m/s/s] Acceleration, body frame (x forward, y left, z up).
 * @param gyro [rad/s] Angular rate, body frame.
 * @param imu  Which IMU.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_imu_sample_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, const float *accel, const float *gyro, uint8_t imu)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint8_t(buf, 32, imu);
    _mav_put_float_array(buf, 8, accel, 3);
    _mav_put_float_array(buf, 20, gyro, 3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#else
    mavlink_arc_imu_sample_t packet;
    packet.time_ns = time_ns;
    packet.imu = imu;
    mav_array_memcpy(packet.accel, accel, sizeof(float)*3);
    mav_array_memcpy(packet.gyro, gyro, sizeof(float)*3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_IMU_SAMPLE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#endif
}

/**
 * @brief Pack a arc_imu_sample message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sample time.
 * @param accel [m/s/s] Acceleration, body frame (x forward, y left, z up).
 * @param gyro [rad/s] Angular rate, body frame.
 * @param imu  Which IMU.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_imu_sample_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,const float *accel,const float *gyro,uint8_t imu)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint8_t(buf, 32, imu);
    _mav_put_float_array(buf, 8, accel, 3);
    _mav_put_float_array(buf, 20, gyro, 3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#else
    mavlink_arc_imu_sample_t packet;
    packet.time_ns = time_ns;
    packet.imu = imu;
    mav_array_memcpy(packet.accel, accel, sizeof(float)*3);
    mav_array_memcpy(packet.gyro, gyro, sizeof(float)*3);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_IMU_SAMPLE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
}

/**
 * @brief Encode a arc_imu_sample struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_imu_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_imu_sample_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_imu_sample_t* arc_imu_sample)
{
    return mavlink_msg_arc_imu_sample_pack(system_id, component_id, msg, arc_imu_sample->time_ns, arc_imu_sample->accel, arc_imu_sample->gyro, arc_imu_sample->imu);
}

/**
 * @brief Encode a arc_imu_sample struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_imu_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_imu_sample_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_imu_sample_t* arc_imu_sample)
{
    return mavlink_msg_arc_imu_sample_pack_chan(system_id, component_id, chan, msg, arc_imu_sample->time_ns, arc_imu_sample->accel, arc_imu_sample->gyro, arc_imu_sample->imu);
}

/**
 * @brief Encode a arc_imu_sample struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_imu_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_imu_sample_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_imu_sample_t* arc_imu_sample)
{
    return mavlink_msg_arc_imu_sample_pack_status(system_id, component_id, _status, msg,  arc_imu_sample->time_ns, arc_imu_sample->accel, arc_imu_sample->gyro, arc_imu_sample->imu);
}

/**
 * @brief Send a arc_imu_sample message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sample time.
 * @param accel [m/s/s] Acceleration, body frame (x forward, y left, z up).
 * @param gyro [rad/s] Angular rate, body frame.
 * @param imu  Which IMU.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_imu_sample_send(mavlink_channel_t chan, uint64_t time_ns, const float *accel, const float *gyro, uint8_t imu)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint8_t(buf, 32, imu);
    _mav_put_float_array(buf, 8, accel, 3);
    _mav_put_float_array(buf, 20, gyro, 3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE, buf, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#else
    mavlink_arc_imu_sample_t packet;
    packet.time_ns = time_ns;
    packet.imu = imu;
    mav_array_memcpy(packet.accel, accel, sizeof(float)*3);
    mav_array_memcpy(packet.gyro, gyro, sizeof(float)*3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE, (const char *)&packet, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#endif
}

/**
 * @brief Send a arc_imu_sample message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_imu_sample_send_struct(mavlink_channel_t chan, const mavlink_arc_imu_sample_t* arc_imu_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_imu_sample_send(chan, arc_imu_sample->time_ns, arc_imu_sample->accel, arc_imu_sample->gyro, arc_imu_sample->imu);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE, (const char *)arc_imu_sample, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_imu_sample_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, const float *accel, const float *gyro, uint8_t imu)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_uint8_t(buf, 32, imu);
    _mav_put_float_array(buf, 8, accel, 3);
    _mav_put_float_array(buf, 20, gyro, 3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE, buf, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#else
    mavlink_arc_imu_sample_t *packet = (mavlink_arc_imu_sample_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->imu = imu;
    mav_array_memcpy(packet->accel, accel, sizeof(float)*3);
    mav_array_memcpy(packet->gyro, gyro, sizeof(float)*3);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_IMU_SAMPLE, (const char *)packet, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_IMU_SAMPLE UNPACKING


/**
 * @brief Get field time_ns from arc_imu_sample message
 *
 * @return [ns] Sample time.
 */
static inline uint64_t mavlink_msg_arc_imu_sample_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field accel from arc_imu_sample message
 *
 * @return [m/s/s] Acceleration, body frame (x forward, y left, z up).
 */
static inline uint16_t mavlink_msg_arc_imu_sample_get_accel(const mavlink_message_t* msg, float *accel)
{
    return _MAV_RETURN_float_array(msg, accel, 3,  8);
}

/**
 * @brief Get field gyro from arc_imu_sample message
 *
 * @return [rad/s] Angular rate, body frame.
 */
static inline uint16_t mavlink_msg_arc_imu_sample_get_gyro(const mavlink_message_t* msg, float *gyro)
{
    return _MAV_RETURN_float_array(msg, gyro, 3,  20);
}

/**
 * @brief Get field imu from arc_imu_sample message
 *
 * @return  Which IMU.
 */
static inline uint8_t mavlink_msg_arc_imu_sample_get_imu(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  32);
}

/**
 * @brief Decode a arc_imu_sample message into a struct
 *
 * @param msg The message to decode
 * @param arc_imu_sample C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_imu_sample_decode(const mavlink_message_t* msg, mavlink_arc_imu_sample_t* arc_imu_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_imu_sample->time_ns = mavlink_msg_arc_imu_sample_get_time_ns(msg);
    mavlink_msg_arc_imu_sample_get_accel(msg, arc_imu_sample->accel);
    mavlink_msg_arc_imu_sample_get_gyro(msg, arc_imu_sample->gyro);
    arc_imu_sample->imu = mavlink_msg_arc_imu_sample_get_imu(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN? msg->len : MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN;
        memset(arc_imu_sample, 0, MAVLINK_MSG_ID_ARC_IMU_SAMPLE_LEN);
    memcpy(arc_imu_sample, _MAV_PAYLOAD(msg), len);
#endif
}
