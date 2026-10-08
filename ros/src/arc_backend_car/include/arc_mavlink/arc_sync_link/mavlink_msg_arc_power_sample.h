#pragma once
// MESSAGE ARC_POWER_SAMPLE PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_POWER_SAMPLE 52007


typedef struct __mavlink_arc_power_sample_t {
 uint64_t time_ns; /*< [ns] Sample time.*/
 float voltage; /*< [V] Pack voltage.*/
 float current; /*< [A] Pack current, positive out of the pack.*/
 float cell_voltage[4]; /*< [V] Cell voltages, cell 1 at the pack negative.*/
} mavlink_arc_power_sample_t;

#define MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN 32
#define MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN 32
#define MAVLINK_MSG_ID_52007_LEN 32
#define MAVLINK_MSG_ID_52007_MIN_LEN 32

#define MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC 171
#define MAVLINK_MSG_ID_52007_CRC 171

#define MAVLINK_MSG_ARC_POWER_SAMPLE_FIELD_CELL_VOLTAGE_LEN 4

#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_POWER_SAMPLE { \
    52007, \
    "ARC_POWER_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_power_sample_t, time_ns) }, \
         { "voltage", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_power_sample_t, voltage) }, \
         { "current", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_power_sample_t, current) }, \
         { "cell_voltage", NULL, MAVLINK_TYPE_FLOAT, 4, 16, offsetof(mavlink_arc_power_sample_t, cell_voltage) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_POWER_SAMPLE { \
    "ARC_POWER_SAMPLE", \
    4, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_power_sample_t, time_ns) }, \
         { "voltage", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_arc_power_sample_t, voltage) }, \
         { "current", NULL, MAVLINK_TYPE_FLOAT, 0, 12, offsetof(mavlink_arc_power_sample_t, current) }, \
         { "cell_voltage", NULL, MAVLINK_TYPE_FLOAT, 4, 16, offsetof(mavlink_arc_power_sample_t, cell_voltage) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_power_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param voltage [V] Pack voltage.
 * @param current [A] Pack current, positive out of the pack.
 * @param cell_voltage [V] Cell voltages, cell 1 at the pack negative.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_power_sample_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, float voltage, float current, const float *cell_voltage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, voltage);
    _mav_put_float(buf, 12, current);
    _mav_put_float_array(buf, 16, cell_voltage, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#else
    mavlink_arc_power_sample_t packet;
    packet.time_ns = time_ns;
    packet.voltage = voltage;
    packet.current = current;
    mav_array_memcpy(packet.cell_voltage, cell_voltage, sizeof(float)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_POWER_SAMPLE;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
}

/**
 * @brief Pack a arc_power_sample message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time.
 * @param voltage [V] Pack voltage.
 * @param current [A] Pack current, positive out of the pack.
 * @param cell_voltage [V] Cell voltages, cell 1 at the pack negative.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_power_sample_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, float voltage, float current, const float *cell_voltage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, voltage);
    _mav_put_float(buf, 12, current);
    _mav_put_float_array(buf, 16, cell_voltage, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#else
    mavlink_arc_power_sample_t packet;
    packet.time_ns = time_ns;
    packet.voltage = voltage;
    packet.current = current;
    mav_array_memcpy(packet.cell_voltage, cell_voltage, sizeof(float)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_POWER_SAMPLE;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#endif
}

/**
 * @brief Pack a arc_power_sample message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sample time.
 * @param voltage [V] Pack voltage.
 * @param current [A] Pack current, positive out of the pack.
 * @param cell_voltage [V] Cell voltages, cell 1 at the pack negative.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_power_sample_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,float voltage,float current,const float *cell_voltage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, voltage);
    _mav_put_float(buf, 12, current);
    _mav_put_float_array(buf, 16, cell_voltage, 4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#else
    mavlink_arc_power_sample_t packet;
    packet.time_ns = time_ns;
    packet.voltage = voltage;
    packet.current = current;
    mav_array_memcpy(packet.cell_voltage, cell_voltage, sizeof(float)*4);
        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_POWER_SAMPLE;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
}

/**
 * @brief Encode a arc_power_sample struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_power_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_power_sample_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_power_sample_t* arc_power_sample)
{
    return mavlink_msg_arc_power_sample_pack(system_id, component_id, msg, arc_power_sample->time_ns, arc_power_sample->voltage, arc_power_sample->current, arc_power_sample->cell_voltage);
}

/**
 * @brief Encode a arc_power_sample struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_power_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_power_sample_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_power_sample_t* arc_power_sample)
{
    return mavlink_msg_arc_power_sample_pack_chan(system_id, component_id, chan, msg, arc_power_sample->time_ns, arc_power_sample->voltage, arc_power_sample->current, arc_power_sample->cell_voltage);
}

/**
 * @brief Encode a arc_power_sample struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_power_sample C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_power_sample_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_power_sample_t* arc_power_sample)
{
    return mavlink_msg_arc_power_sample_pack_status(system_id, component_id, _status, msg,  arc_power_sample->time_ns, arc_power_sample->voltage, arc_power_sample->current, arc_power_sample->cell_voltage);
}

/**
 * @brief Send a arc_power_sample message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sample time.
 * @param voltage [V] Pack voltage.
 * @param current [A] Pack current, positive out of the pack.
 * @param cell_voltage [V] Cell voltages, cell 1 at the pack negative.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_power_sample_send(mavlink_channel_t chan, uint64_t time_ns, float voltage, float current, const float *cell_voltage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, voltage);
    _mav_put_float(buf, 12, current);
    _mav_put_float_array(buf, 16, cell_voltage, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE, buf, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#else
    mavlink_arc_power_sample_t packet;
    packet.time_ns = time_ns;
    packet.voltage = voltage;
    packet.current = current;
    mav_array_memcpy(packet.cell_voltage, cell_voltage, sizeof(float)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE, (const char *)&packet, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#endif
}

/**
 * @brief Send a arc_power_sample message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_power_sample_send_struct(mavlink_channel_t chan, const mavlink_arc_power_sample_t* arc_power_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_power_sample_send(chan, arc_power_sample->time_ns, arc_power_sample->voltage, arc_power_sample->current, arc_power_sample->cell_voltage);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE, (const char *)arc_power_sample, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_power_sample_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, float voltage, float current, const float *cell_voltage)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_float(buf, 8, voltage);
    _mav_put_float(buf, 12, current);
    _mav_put_float_array(buf, 16, cell_voltage, 4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE, buf, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#else
    mavlink_arc_power_sample_t *packet = (mavlink_arc_power_sample_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->voltage = voltage;
    packet->current = current;
    mav_array_memcpy(packet->cell_voltage, cell_voltage, sizeof(float)*4);
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_POWER_SAMPLE, (const char *)packet, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_POWER_SAMPLE UNPACKING


/**
 * @brief Get field time_ns from arc_power_sample message
 *
 * @return [ns] Sample time.
 */
static inline uint64_t mavlink_msg_arc_power_sample_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field voltage from arc_power_sample message
 *
 * @return [V] Pack voltage.
 */
static inline float mavlink_msg_arc_power_sample_get_voltage(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field current from arc_power_sample message
 *
 * @return [A] Pack current, positive out of the pack.
 */
static inline float mavlink_msg_arc_power_sample_get_current(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  12);
}

/**
 * @brief Get field cell_voltage from arc_power_sample message
 *
 * @return [V] Cell voltages, cell 1 at the pack negative.
 */
static inline uint16_t mavlink_msg_arc_power_sample_get_cell_voltage(const mavlink_message_t* msg, float *cell_voltage)
{
    return _MAV_RETURN_float_array(msg, cell_voltage, 4,  16);
}

/**
 * @brief Decode a arc_power_sample message into a struct
 *
 * @param msg The message to decode
 * @param arc_power_sample C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_power_sample_decode(const mavlink_message_t* msg, mavlink_arc_power_sample_t* arc_power_sample)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_power_sample->time_ns = mavlink_msg_arc_power_sample_get_time_ns(msg);
    arc_power_sample->voltage = mavlink_msg_arc_power_sample_get_voltage(msg);
    arc_power_sample->current = mavlink_msg_arc_power_sample_get_current(msg);
    mavlink_msg_arc_power_sample_get_cell_voltage(msg, arc_power_sample->cell_voltage);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN? msg->len : MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN;
        memset(arc_power_sample, 0, MAVLINK_MSG_ID_ARC_POWER_SAMPLE_LEN);
    memcpy(arc_power_sample, _MAV_PAYLOAD(msg), len);
#endif
}
