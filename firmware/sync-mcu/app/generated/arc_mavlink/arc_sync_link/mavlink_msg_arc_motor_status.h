#pragma once
// MESSAGE ARC_MOTOR_STATUS PACKING

#include <stdint.h>

#define MAVLINK_MSG_ID_ARC_MOTOR_STATUS 52004


typedef struct __mavlink_arc_motor_status_t {
 uint64_t time_ns; /*< [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.*/
 int32_t erpm; /*< [rpm] Electrical speed.*/
 int32_t tachometer; /*<  Commutation steps since start.*/
 float current_q; /*< [A] Motor q-axis current (torque).*/
 float current_d; /*< [A] Motor d-axis current.*/
 float current_in; /*< [A] Input (battery-side) current.*/
 float duty; /*<  Duty cycle, -1 to 1.*/
 float voltage_in; /*< [V] Motor-bus voltage at the controller.*/
 float temp_fet; /*< [degC] Power-stage temperature.*/
 float temp_motor; /*< [degC] Motor temperature.*/
 uint8_t corner; /*<  Which corner.*/
 uint8_t fault; /*<  VESC fault code; 0 is none.*/
 uint8_t state; /*<  Why the controller is braking on its own; 0 is neither.*/
} mavlink_arc_motor_status_t;

#define MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN 47
#define MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN 47
#define MAVLINK_MSG_ID_52004_LEN 47
#define MAVLINK_MSG_ID_52004_MIN_LEN 47

#define MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC 76
#define MAVLINK_MSG_ID_52004_CRC 76



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_ARC_MOTOR_STATUS { \
    52004, \
    "ARC_MOTOR_STATUS", \
    13, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_motor_status_t, time_ns) }, \
         { "erpm", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_arc_motor_status_t, erpm) }, \
         { "tachometer", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_arc_motor_status_t, tachometer) }, \
         { "current_q", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_motor_status_t, current_q) }, \
         { "current_d", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_arc_motor_status_t, current_d) }, \
         { "current_in", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_arc_motor_status_t, current_in) }, \
         { "duty", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_arc_motor_status_t, duty) }, \
         { "voltage_in", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_arc_motor_status_t, voltage_in) }, \
         { "temp_fet", NULL, MAVLINK_TYPE_FLOAT, 0, 36, offsetof(mavlink_arc_motor_status_t, temp_fet) }, \
         { "temp_motor", NULL, MAVLINK_TYPE_FLOAT, 0, 40, offsetof(mavlink_arc_motor_status_t, temp_motor) }, \
         { "corner", NULL, MAVLINK_TYPE_UINT8_T, 0, 44, offsetof(mavlink_arc_motor_status_t, corner) }, \
         { "fault", NULL, MAVLINK_TYPE_UINT8_T, 0, 45, offsetof(mavlink_arc_motor_status_t, fault) }, \
         { "state", NULL, MAVLINK_TYPE_UINT8_T, 0, 46, offsetof(mavlink_arc_motor_status_t, state) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_ARC_MOTOR_STATUS { \
    "ARC_MOTOR_STATUS", \
    13, \
    {  { "time_ns", NULL, MAVLINK_TYPE_UINT64_T, 0, 0, offsetof(mavlink_arc_motor_status_t, time_ns) }, \
         { "erpm", NULL, MAVLINK_TYPE_INT32_T, 0, 8, offsetof(mavlink_arc_motor_status_t, erpm) }, \
         { "tachometer", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_arc_motor_status_t, tachometer) }, \
         { "current_q", NULL, MAVLINK_TYPE_FLOAT, 0, 16, offsetof(mavlink_arc_motor_status_t, current_q) }, \
         { "current_d", NULL, MAVLINK_TYPE_FLOAT, 0, 20, offsetof(mavlink_arc_motor_status_t, current_d) }, \
         { "current_in", NULL, MAVLINK_TYPE_FLOAT, 0, 24, offsetof(mavlink_arc_motor_status_t, current_in) }, \
         { "duty", NULL, MAVLINK_TYPE_FLOAT, 0, 28, offsetof(mavlink_arc_motor_status_t, duty) }, \
         { "voltage_in", NULL, MAVLINK_TYPE_FLOAT, 0, 32, offsetof(mavlink_arc_motor_status_t, voltage_in) }, \
         { "temp_fet", NULL, MAVLINK_TYPE_FLOAT, 0, 36, offsetof(mavlink_arc_motor_status_t, temp_fet) }, \
         { "temp_motor", NULL, MAVLINK_TYPE_FLOAT, 0, 40, offsetof(mavlink_arc_motor_status_t, temp_motor) }, \
         { "corner", NULL, MAVLINK_TYPE_UINT8_T, 0, 44, offsetof(mavlink_arc_motor_status_t, corner) }, \
         { "fault", NULL, MAVLINK_TYPE_UINT8_T, 0, 45, offsetof(mavlink_arc_motor_status_t, fault) }, \
         { "state", NULL, MAVLINK_TYPE_UINT8_T, 0, 46, offsetof(mavlink_arc_motor_status_t, state) }, \
         } \
}
#endif

/**
 * @brief Pack a arc_motor_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.
 * @param erpm [rpm] Electrical speed.
 * @param tachometer  Commutation steps since start.
 * @param current_q [A] Motor q-axis current (torque).
 * @param current_d [A] Motor d-axis current.
 * @param current_in [A] Input (battery-side) current.
 * @param duty  Duty cycle, -1 to 1.
 * @param voltage_in [V] Motor-bus voltage at the controller.
 * @param temp_fet [degC] Power-stage temperature.
 * @param temp_motor [degC] Motor temperature.
 * @param corner  Which corner.
 * @param fault  VESC fault code; 0 is none.
 * @param state  Why the controller is braking on its own; 0 is neither.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_motor_status_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint64_t time_ns, int32_t erpm, int32_t tachometer, float current_q, float current_d, float current_in, float duty, float voltage_in, float temp_fet, float temp_motor, uint8_t corner, uint8_t fault, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_int32_t(buf, 8, erpm);
    _mav_put_int32_t(buf, 12, tachometer);
    _mav_put_float(buf, 16, current_q);
    _mav_put_float(buf, 20, current_d);
    _mav_put_float(buf, 24, current_in);
    _mav_put_float(buf, 28, duty);
    _mav_put_float(buf, 32, voltage_in);
    _mav_put_float(buf, 36, temp_fet);
    _mav_put_float(buf, 40, temp_motor);
    _mav_put_uint8_t(buf, 44, corner);
    _mav_put_uint8_t(buf, 45, fault);
    _mav_put_uint8_t(buf, 46, state);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#else
    mavlink_arc_motor_status_t packet;
    packet.time_ns = time_ns;
    packet.erpm = erpm;
    packet.tachometer = tachometer;
    packet.current_q = current_q;
    packet.current_d = current_d;
    packet.current_in = current_in;
    packet.duty = duty;
    packet.voltage_in = voltage_in;
    packet.temp_fet = temp_fet;
    packet.temp_motor = temp_motor;
    packet.corner = corner;
    packet.fault = fault;
    packet.state = state;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_MOTOR_STATUS;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
}

/**
 * @brief Pack a arc_motor_status message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 *
 * @param time_ns [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.
 * @param erpm [rpm] Electrical speed.
 * @param tachometer  Commutation steps since start.
 * @param current_q [A] Motor q-axis current (torque).
 * @param current_d [A] Motor d-axis current.
 * @param current_in [A] Input (battery-side) current.
 * @param duty  Duty cycle, -1 to 1.
 * @param voltage_in [V] Motor-bus voltage at the controller.
 * @param temp_fet [degC] Power-stage temperature.
 * @param temp_motor [degC] Motor temperature.
 * @param corner  Which corner.
 * @param fault  VESC fault code; 0 is none.
 * @param state  Why the controller is braking on its own; 0 is neither.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_motor_status_pack_status(uint8_t system_id, uint8_t component_id, mavlink_status_t *_status, mavlink_message_t* msg,
                               uint64_t time_ns, int32_t erpm, int32_t tachometer, float current_q, float current_d, float current_in, float duty, float voltage_in, float temp_fet, float temp_motor, uint8_t corner, uint8_t fault, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_int32_t(buf, 8, erpm);
    _mav_put_int32_t(buf, 12, tachometer);
    _mav_put_float(buf, 16, current_q);
    _mav_put_float(buf, 20, current_d);
    _mav_put_float(buf, 24, current_in);
    _mav_put_float(buf, 28, duty);
    _mav_put_float(buf, 32, voltage_in);
    _mav_put_float(buf, 36, temp_fet);
    _mav_put_float(buf, 40, temp_motor);
    _mav_put_uint8_t(buf, 44, corner);
    _mav_put_uint8_t(buf, 45, fault);
    _mav_put_uint8_t(buf, 46, state);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#else
    mavlink_arc_motor_status_t packet;
    packet.time_ns = time_ns;
    packet.erpm = erpm;
    packet.tachometer = tachometer;
    packet.current_q = current_q;
    packet.current_d = current_d;
    packet.current_in = current_in;
    packet.duty = duty;
    packet.voltage_in = voltage_in;
    packet.temp_fet = temp_fet;
    packet.temp_motor = temp_motor;
    packet.corner = corner;
    packet.fault = fault;
    packet.state = state;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_MOTOR_STATUS;
#if MAVLINK_CRC_EXTRA
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#else
    return mavlink_finalize_message_buffer(msg, system_id, component_id, _status, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#endif
}

/**
 * @brief Pack a arc_motor_status message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param time_ns [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.
 * @param erpm [rpm] Electrical speed.
 * @param tachometer  Commutation steps since start.
 * @param current_q [A] Motor q-axis current (torque).
 * @param current_d [A] Motor d-axis current.
 * @param current_in [A] Input (battery-side) current.
 * @param duty  Duty cycle, -1 to 1.
 * @param voltage_in [V] Motor-bus voltage at the controller.
 * @param temp_fet [degC] Power-stage temperature.
 * @param temp_motor [degC] Motor temperature.
 * @param corner  Which corner.
 * @param fault  VESC fault code; 0 is none.
 * @param state  Why the controller is braking on its own; 0 is neither.
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_arc_motor_status_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint64_t time_ns,int32_t erpm,int32_t tachometer,float current_q,float current_d,float current_in,float duty,float voltage_in,float temp_fet,float temp_motor,uint8_t corner,uint8_t fault,uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_int32_t(buf, 8, erpm);
    _mav_put_int32_t(buf, 12, tachometer);
    _mav_put_float(buf, 16, current_q);
    _mav_put_float(buf, 20, current_d);
    _mav_put_float(buf, 24, current_in);
    _mav_put_float(buf, 28, duty);
    _mav_put_float(buf, 32, voltage_in);
    _mav_put_float(buf, 36, temp_fet);
    _mav_put_float(buf, 40, temp_motor);
    _mav_put_uint8_t(buf, 44, corner);
    _mav_put_uint8_t(buf, 45, fault);
    _mav_put_uint8_t(buf, 46, state);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#else
    mavlink_arc_motor_status_t packet;
    packet.time_ns = time_ns;
    packet.erpm = erpm;
    packet.tachometer = tachometer;
    packet.current_q = current_q;
    packet.current_d = current_d;
    packet.current_in = current_in;
    packet.duty = duty;
    packet.voltage_in = voltage_in;
    packet.temp_fet = temp_fet;
    packet.temp_motor = temp_motor;
    packet.corner = corner;
    packet.fault = fault;
    packet.state = state;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_ARC_MOTOR_STATUS;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
}

/**
 * @brief Encode a arc_motor_status struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param arc_motor_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_motor_status_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_arc_motor_status_t* arc_motor_status)
{
    return mavlink_msg_arc_motor_status_pack(system_id, component_id, msg, arc_motor_status->time_ns, arc_motor_status->erpm, arc_motor_status->tachometer, arc_motor_status->current_q, arc_motor_status->current_d, arc_motor_status->current_in, arc_motor_status->duty, arc_motor_status->voltage_in, arc_motor_status->temp_fet, arc_motor_status->temp_motor, arc_motor_status->corner, arc_motor_status->fault, arc_motor_status->state);
}

/**
 * @brief Encode a arc_motor_status struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param arc_motor_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_motor_status_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_arc_motor_status_t* arc_motor_status)
{
    return mavlink_msg_arc_motor_status_pack_chan(system_id, component_id, chan, msg, arc_motor_status->time_ns, arc_motor_status->erpm, arc_motor_status->tachometer, arc_motor_status->current_q, arc_motor_status->current_d, arc_motor_status->current_in, arc_motor_status->duty, arc_motor_status->voltage_in, arc_motor_status->temp_fet, arc_motor_status->temp_motor, arc_motor_status->corner, arc_motor_status->fault, arc_motor_status->state);
}

/**
 * @brief Encode a arc_motor_status struct with provided status structure
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param status MAVLink status structure
 * @param msg The MAVLink message to compress the data into
 * @param arc_motor_status C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_arc_motor_status_encode_status(uint8_t system_id, uint8_t component_id, mavlink_status_t* _status, mavlink_message_t* msg, const mavlink_arc_motor_status_t* arc_motor_status)
{
    return mavlink_msg_arc_motor_status_pack_status(system_id, component_id, _status, msg,  arc_motor_status->time_ns, arc_motor_status->erpm, arc_motor_status->tachometer, arc_motor_status->current_q, arc_motor_status->current_d, arc_motor_status->current_in, arc_motor_status->duty, arc_motor_status->voltage_in, arc_motor_status->temp_fet, arc_motor_status->temp_motor, arc_motor_status->corner, arc_motor_status->fault, arc_motor_status->state);
}

/**
 * @brief Send a arc_motor_status message
 * @param chan MAVLink channel to send the message
 *
 * @param time_ns [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.
 * @param erpm [rpm] Electrical speed.
 * @param tachometer  Commutation steps since start.
 * @param current_q [A] Motor q-axis current (torque).
 * @param current_d [A] Motor d-axis current.
 * @param current_in [A] Input (battery-side) current.
 * @param duty  Duty cycle, -1 to 1.
 * @param voltage_in [V] Motor-bus voltage at the controller.
 * @param temp_fet [degC] Power-stage temperature.
 * @param temp_motor [degC] Motor temperature.
 * @param corner  Which corner.
 * @param fault  VESC fault code; 0 is none.
 * @param state  Why the controller is braking on its own; 0 is neither.
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_arc_motor_status_send(mavlink_channel_t chan, uint64_t time_ns, int32_t erpm, int32_t tachometer, float current_q, float current_d, float current_in, float duty, float voltage_in, float temp_fet, float temp_motor, uint8_t corner, uint8_t fault, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN];
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_int32_t(buf, 8, erpm);
    _mav_put_int32_t(buf, 12, tachometer);
    _mav_put_float(buf, 16, current_q);
    _mav_put_float(buf, 20, current_d);
    _mav_put_float(buf, 24, current_in);
    _mav_put_float(buf, 28, duty);
    _mav_put_float(buf, 32, voltage_in);
    _mav_put_float(buf, 36, temp_fet);
    _mav_put_float(buf, 40, temp_motor);
    _mav_put_uint8_t(buf, 44, corner);
    _mav_put_uint8_t(buf, 45, fault);
    _mav_put_uint8_t(buf, 46, state);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS, buf, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#else
    mavlink_arc_motor_status_t packet;
    packet.time_ns = time_ns;
    packet.erpm = erpm;
    packet.tachometer = tachometer;
    packet.current_q = current_q;
    packet.current_d = current_d;
    packet.current_in = current_in;
    packet.duty = duty;
    packet.voltage_in = voltage_in;
    packet.temp_fet = temp_fet;
    packet.temp_motor = temp_motor;
    packet.corner = corner;
    packet.fault = fault;
    packet.state = state;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS, (const char *)&packet, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#endif
}

/**
 * @brief Send a arc_motor_status message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_arc_motor_status_send_struct(mavlink_channel_t chan, const mavlink_arc_motor_status_t* arc_motor_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_arc_motor_status_send(chan, arc_motor_status->time_ns, arc_motor_status->erpm, arc_motor_status->tachometer, arc_motor_status->current_q, arc_motor_status->current_d, arc_motor_status->current_in, arc_motor_status->duty, arc_motor_status->voltage_in, arc_motor_status->temp_fet, arc_motor_status->temp_motor, arc_motor_status->corner, arc_motor_status->fault, arc_motor_status->state);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS, (const char *)arc_motor_status, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#endif
}

#if MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by reusing
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_arc_motor_status_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint64_t time_ns, int32_t erpm, int32_t tachometer, float current_q, float current_d, float current_in, float duty, float voltage_in, float temp_fet, float temp_motor, uint8_t corner, uint8_t fault, uint8_t state)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_uint64_t(buf, 0, time_ns);
    _mav_put_int32_t(buf, 8, erpm);
    _mav_put_int32_t(buf, 12, tachometer);
    _mav_put_float(buf, 16, current_q);
    _mav_put_float(buf, 20, current_d);
    _mav_put_float(buf, 24, current_in);
    _mav_put_float(buf, 28, duty);
    _mav_put_float(buf, 32, voltage_in);
    _mav_put_float(buf, 36, temp_fet);
    _mav_put_float(buf, 40, temp_motor);
    _mav_put_uint8_t(buf, 44, corner);
    _mav_put_uint8_t(buf, 45, fault);
    _mav_put_uint8_t(buf, 46, state);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS, buf, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#else
    mavlink_arc_motor_status_t *packet = (mavlink_arc_motor_status_t *)msgbuf;
    packet->time_ns = time_ns;
    packet->erpm = erpm;
    packet->tachometer = tachometer;
    packet->current_q = current_q;
    packet->current_d = current_d;
    packet->current_in = current_in;
    packet->duty = duty;
    packet->voltage_in = voltage_in;
    packet->temp_fet = temp_fet;
    packet->temp_motor = temp_motor;
    packet->corner = corner;
    packet->fault = fault;
    packet->state = state;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_ARC_MOTOR_STATUS, (const char *)packet, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_CRC);
#endif
}
#endif

#endif

// MESSAGE ARC_MOTOR_STATUS UNPACKING


/**
 * @brief Get field time_ns from arc_motor_status message
 *
 * @return [ns] Sample time: the controller's clock on the UART link, sync-MCU time on the sync link.
 */
static inline uint64_t mavlink_msg_arc_motor_status_get_time_ns(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint64_t(msg,  0);
}

/**
 * @brief Get field erpm from arc_motor_status message
 *
 * @return [rpm] Electrical speed.
 */
static inline int32_t mavlink_msg_arc_motor_status_get_erpm(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  8);
}

/**
 * @brief Get field tachometer from arc_motor_status message
 *
 * @return  Commutation steps since start.
 */
static inline int32_t mavlink_msg_arc_motor_status_get_tachometer(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  12);
}

/**
 * @brief Get field current_q from arc_motor_status message
 *
 * @return [A] Motor q-axis current (torque).
 */
static inline float mavlink_msg_arc_motor_status_get_current_q(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  16);
}

/**
 * @brief Get field current_d from arc_motor_status message
 *
 * @return [A] Motor d-axis current.
 */
static inline float mavlink_msg_arc_motor_status_get_current_d(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  20);
}

/**
 * @brief Get field current_in from arc_motor_status message
 *
 * @return [A] Input (battery-side) current.
 */
static inline float mavlink_msg_arc_motor_status_get_current_in(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  24);
}

/**
 * @brief Get field duty from arc_motor_status message
 *
 * @return  Duty cycle, -1 to 1.
 */
static inline float mavlink_msg_arc_motor_status_get_duty(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  28);
}

/**
 * @brief Get field voltage_in from arc_motor_status message
 *
 * @return [V] Motor-bus voltage at the controller.
 */
static inline float mavlink_msg_arc_motor_status_get_voltage_in(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  32);
}

/**
 * @brief Get field temp_fet from arc_motor_status message
 *
 * @return [degC] Power-stage temperature.
 */
static inline float mavlink_msg_arc_motor_status_get_temp_fet(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  36);
}

/**
 * @brief Get field temp_motor from arc_motor_status message
 *
 * @return [degC] Motor temperature.
 */
static inline float mavlink_msg_arc_motor_status_get_temp_motor(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  40);
}

/**
 * @brief Get field corner from arc_motor_status message
 *
 * @return  Which corner.
 */
static inline uint8_t mavlink_msg_arc_motor_status_get_corner(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  44);
}

/**
 * @brief Get field fault from arc_motor_status message
 *
 * @return  VESC fault code; 0 is none.
 */
static inline uint8_t mavlink_msg_arc_motor_status_get_fault(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  45);
}

/**
 * @brief Get field state from arc_motor_status message
 *
 * @return  Why the controller is braking on its own; 0 is neither.
 */
static inline uint8_t mavlink_msg_arc_motor_status_get_state(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  46);
}

/**
 * @brief Decode a arc_motor_status message into a struct
 *
 * @param msg The message to decode
 * @param arc_motor_status C-struct to decode the message contents into
 */
static inline void mavlink_msg_arc_motor_status_decode(const mavlink_message_t* msg, mavlink_arc_motor_status_t* arc_motor_status)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    arc_motor_status->time_ns = mavlink_msg_arc_motor_status_get_time_ns(msg);
    arc_motor_status->erpm = mavlink_msg_arc_motor_status_get_erpm(msg);
    arc_motor_status->tachometer = mavlink_msg_arc_motor_status_get_tachometer(msg);
    arc_motor_status->current_q = mavlink_msg_arc_motor_status_get_current_q(msg);
    arc_motor_status->current_d = mavlink_msg_arc_motor_status_get_current_d(msg);
    arc_motor_status->current_in = mavlink_msg_arc_motor_status_get_current_in(msg);
    arc_motor_status->duty = mavlink_msg_arc_motor_status_get_duty(msg);
    arc_motor_status->voltage_in = mavlink_msg_arc_motor_status_get_voltage_in(msg);
    arc_motor_status->temp_fet = mavlink_msg_arc_motor_status_get_temp_fet(msg);
    arc_motor_status->temp_motor = mavlink_msg_arc_motor_status_get_temp_motor(msg);
    arc_motor_status->corner = mavlink_msg_arc_motor_status_get_corner(msg);
    arc_motor_status->fault = mavlink_msg_arc_motor_status_get_fault(msg);
    arc_motor_status->state = mavlink_msg_arc_motor_status_get_state(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN? msg->len : MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN;
        memset(arc_motor_status, 0, MAVLINK_MSG_ID_ARC_MOTOR_STATUS_LEN);
    memcpy(arc_motor_status, _MAV_PAYLOAD(msg), len);
#endif
}
