/** @file
 *    @brief MAVLink comm protocol testsuite generated from arc_sync_link.xml
 *    @see https://mavlink.io/en/
 */
#pragma once
#ifndef ARC_SYNC_LINK_TESTSUITE_H
#define ARC_SYNC_LINK_TESTSUITE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAVLINK_TEST_ALL
#define MAVLINK_TEST_ALL

static void mavlink_test_arc_sync_link(uint8_t, uint8_t, mavlink_message_t *last_msg);

static void mavlink_test_all(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{

    mavlink_test_arc_sync_link(system_id, component_id, last_msg);
}
#endif




static void mavlink_test_arc_link_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_LINK_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_link_status_t packet_in = {
        93372036854775807ULL,963497880,963498088,18067
    };
    mavlink_arc_link_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.rx_gaps = packet_in.rx_gaps;
        packet1.rx_bad = packet_in.rx_bad;
        packet1.protocol_version = packet_in.protocol_version;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_LINK_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_link_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_link_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_link_status_pack(system_id, component_id, &msg , packet1.time_ns , packet1.protocol_version , packet1.rx_gaps , packet1.rx_bad );
    mavlink_msg_arc_link_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_link_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.protocol_version , packet1.rx_gaps , packet1.rx_bad );
    mavlink_msg_arc_link_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_link_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_link_status_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.protocol_version , packet1.rx_gaps , packet1.rx_bad );
    mavlink_msg_arc_link_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_LINK_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_LINK_STATUS) != NULL);
#endif
}

static void mavlink_test_arc_imu_sample(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_IMU_SAMPLE >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_imu_sample_t packet_in = {
        93372036854775807ULL,{ 73.0, 74.0, 75.0 },{ 157.0, 158.0, 159.0 },101
    };
    mavlink_arc_imu_sample_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.imu = packet_in.imu;
        
        mav_array_memcpy(packet1.accel, packet_in.accel, sizeof(float)*3);
        mav_array_memcpy(packet1.gyro, packet_in.gyro, sizeof(float)*3);
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_IMU_SAMPLE_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_imu_sample_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_imu_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_imu_sample_pack(system_id, component_id, &msg , packet1.time_ns , packet1.accel , packet1.gyro , packet1.imu );
    mavlink_msg_arc_imu_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_imu_sample_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.accel , packet1.gyro , packet1.imu );
    mavlink_msg_arc_imu_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_imu_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_imu_sample_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.accel , packet1.gyro , packet1.imu );
    mavlink_msg_arc_imu_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_IMU_SAMPLE") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_IMU_SAMPLE) != NULL);
#endif
}

static void mavlink_test_arc_angle_sample(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_ANGLE_SAMPLE >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_angle_sample_t packet_in = {
        93372036854775807ULL,73.0,41
    };
    mavlink_arc_angle_sample_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.angle = packet_in.angle;
        packet1.sensor = packet_in.sensor;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_ANGLE_SAMPLE_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_ANGLE_SAMPLE_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_angle_sample_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_angle_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_angle_sample_pack(system_id, component_id, &msg , packet1.time_ns , packet1.angle , packet1.sensor );
    mavlink_msg_arc_angle_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_angle_sample_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.angle , packet1.sensor );
    mavlink_msg_arc_angle_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_angle_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_angle_sample_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.angle , packet1.sensor );
    mavlink_msg_arc_angle_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_ANGLE_SAMPLE") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_ANGLE_SAMPLE) != NULL);
#endif
}

static void mavlink_test_arc_ride_height_sample(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_ride_height_sample_t packet_in = {
        93372036854775807ULL,73.0,41,108
    };
    mavlink_arc_ride_height_sample_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.range = packet_in.range;
        packet1.sensor = packet_in.sensor;
        packet1.status = packet_in.status;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_ride_height_sample_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_ride_height_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_ride_height_sample_pack(system_id, component_id, &msg , packet1.time_ns , packet1.range , packet1.sensor , packet1.status );
    mavlink_msg_arc_ride_height_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_ride_height_sample_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.range , packet1.sensor , packet1.status );
    mavlink_msg_arc_ride_height_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_ride_height_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_ride_height_sample_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.range , packet1.sensor , packet1.status );
    mavlink_msg_arc_ride_height_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_RIDE_HEIGHT_SAMPLE") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_RIDE_HEIGHT_SAMPLE) != NULL);
#endif
}

static void mavlink_test_arc_motor_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_MOTOR_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_motor_status_t packet_in = {
        93372036854775807ULL,963497880,963498088,129.0,157.0,185.0,213.0,241.0,269.0,297.0,137,204
    };
    mavlink_arc_motor_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.erpm = packet_in.erpm;
        packet1.tachometer = packet_in.tachometer;
        packet1.current_q = packet_in.current_q;
        packet1.current_d = packet_in.current_d;
        packet1.current_in = packet_in.current_in;
        packet1.duty = packet_in.duty;
        packet1.voltage_in = packet_in.voltage_in;
        packet1.temp_fet = packet_in.temp_fet;
        packet1.temp_motor = packet_in.temp_motor;
        packet1.corner = packet_in.corner;
        packet1.fault = packet_in.fault;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_MOTOR_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_motor_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_motor_status_pack(system_id, component_id, &msg , packet1.time_ns , packet1.erpm , packet1.tachometer , packet1.current_q , packet1.current_d , packet1.current_in , packet1.duty , packet1.voltage_in , packet1.temp_fet , packet1.temp_motor , packet1.corner , packet1.fault );
    mavlink_msg_arc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_motor_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.erpm , packet1.tachometer , packet1.current_q , packet1.current_d , packet1.current_in , packet1.duty , packet1.voltage_in , packet1.temp_fet , packet1.temp_motor , packet1.corner , packet1.fault );
    mavlink_msg_arc_motor_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_motor_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_motor_status_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.erpm , packet1.tachometer , packet1.current_q , packet1.current_d , packet1.current_in , packet1.duty , packet1.voltage_in , packet1.temp_fet , packet1.temp_motor , packet1.corner , packet1.fault );
    mavlink_msg_arc_motor_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_MOTOR_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_MOTOR_STATUS) != NULL);
#endif
}

static void mavlink_test_arc_steering_status(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_STEERING_STATUS >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_steering_status_t packet_in = {
        93372036854775807ULL,73.0,101.0,129.0,157.0,77
    };
    mavlink_arc_steering_status_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.angle = packet_in.angle;
        packet1.velocity = packet_in.velocity;
        packet1.torque = packet_in.torque;
        packet1.temp = packet_in.temp;
        packet1.fault = packet_in.fault;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_STEERING_STATUS_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_steering_status_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_steering_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_steering_status_pack(system_id, component_id, &msg , packet1.time_ns , packet1.angle , packet1.velocity , packet1.torque , packet1.temp , packet1.fault );
    mavlink_msg_arc_steering_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_steering_status_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.angle , packet1.velocity , packet1.torque , packet1.temp , packet1.fault );
    mavlink_msg_arc_steering_status_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_steering_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_steering_status_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.angle , packet1.velocity , packet1.torque , packet1.temp , packet1.fault );
    mavlink_msg_arc_steering_status_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_STEERING_STATUS") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_STEERING_STATUS) != NULL);
#endif
}

static void mavlink_test_arc_camera_trigger(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_camera_trigger_t packet_in = {
        93372036854775807ULL,963497880,41
    };
    mavlink_arc_camera_trigger_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.frame = packet_in.frame;
        packet1.camera = packet_in.camera;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_camera_trigger_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_camera_trigger_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_camera_trigger_pack(system_id, component_id, &msg , packet1.time_ns , packet1.frame , packet1.camera );
    mavlink_msg_arc_camera_trigger_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_camera_trigger_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.frame , packet1.camera );
    mavlink_msg_arc_camera_trigger_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_camera_trigger_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_camera_trigger_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.frame , packet1.camera );
    mavlink_msg_arc_camera_trigger_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_CAMERA_TRIGGER") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_CAMERA_TRIGGER) != NULL);
#endif
}

static void mavlink_test_arc_power_sample(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_POWER_SAMPLE >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_power_sample_t packet_in = {
        93372036854775807ULL,73.0,101.0,{ 129.0, 130.0, 131.0, 132.0 }
    };
    mavlink_arc_power_sample_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.voltage = packet_in.voltage;
        packet1.current = packet_in.current;
        
        mav_array_memcpy(packet1.cell_voltage, packet_in.cell_voltage, sizeof(float)*4);
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_POWER_SAMPLE_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_power_sample_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_power_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_power_sample_pack(system_id, component_id, &msg , packet1.time_ns , packet1.voltage , packet1.current , packet1.cell_voltage );
    mavlink_msg_arc_power_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_power_sample_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.voltage , packet1.current , packet1.cell_voltage );
    mavlink_msg_arc_power_sample_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_power_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_power_sample_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.voltage , packet1.current , packet1.cell_voltage );
    mavlink_msg_arc_power_sample_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_POWER_SAMPLE") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_POWER_SAMPLE) != NULL);
#endif
}

static void mavlink_test_arc_safety_state(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_SAFETY_STATE >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_safety_state_t packet_in = {
        93372036854775807ULL,73.0,101.0,129.0,65,132
    };
    mavlink_arc_safety_state_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.max_speed = packet_in.max_speed;
        packet1.max_accel = packet_in.max_accel;
        packet1.max_steer = packet_in.max_steer;
        packet1.watchdog = packet_in.watchdog;
        packet1.estop = packet_in.estop;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_SAFETY_STATE_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_safety_state_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_safety_state_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_safety_state_pack(system_id, component_id, &msg , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer , packet1.watchdog , packet1.estop );
    mavlink_msg_arc_safety_state_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_safety_state_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer , packet1.watchdog , packet1.estop );
    mavlink_msg_arc_safety_state_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_safety_state_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_safety_state_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer , packet1.watchdog , packet1.estop );
    mavlink_msg_arc_safety_state_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_SAFETY_STATE") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_SAFETY_STATE) != NULL);
#endif
}

static void mavlink_test_arc_fault(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_FAULT >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_fault_t packet_in = {
        93372036854775807ULL,963497880,17859,175
    };
    mavlink_arc_fault_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.detail = packet_in.detail;
        packet1.code = packet_in.code;
        packet1.source = packet_in.source;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_FAULT_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_FAULT_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_fault_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_fault_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_fault_pack(system_id, component_id, &msg , packet1.time_ns , packet1.detail , packet1.code , packet1.source );
    mavlink_msg_arc_fault_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_fault_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.detail , packet1.code , packet1.source );
    mavlink_msg_arc_fault_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_fault_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_fault_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.detail , packet1.code , packet1.source );
    mavlink_msg_arc_fault_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_FAULT") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_FAULT) != NULL);
#endif
}

static void mavlink_test_arc_drive_command(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_DRIVE_COMMAND >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_drive_command_t packet_in = {
        93372036854775807ULL,73.0,101.0,129.0
    };
    mavlink_arc_drive_command_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.speed = packet_in.speed;
        packet1.accel = packet_in.accel;
        packet1.steer = packet_in.steer;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_DRIVE_COMMAND_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_drive_command_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_drive_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_drive_command_pack(system_id, component_id, &msg , packet1.time_ns , packet1.speed , packet1.accel , packet1.steer );
    mavlink_msg_arc_drive_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_drive_command_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.speed , packet1.accel , packet1.steer );
    mavlink_msg_arc_drive_command_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_drive_command_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_drive_command_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.speed , packet1.accel , packet1.steer );
    mavlink_msg_arc_drive_command_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_DRIVE_COMMAND") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_DRIVE_COMMAND) != NULL);
#endif
}

static void mavlink_test_arc_operator_heartbeat(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_operator_heartbeat_t packet_in = {
        93372036854775807ULL,963497880
    };
    mavlink_arc_operator_heartbeat_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.counter = packet_in.counter;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_operator_heartbeat_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_operator_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_operator_heartbeat_pack(system_id, component_id, &msg , packet1.time_ns , packet1.counter );
    mavlink_msg_arc_operator_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_operator_heartbeat_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.counter );
    mavlink_msg_arc_operator_heartbeat_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_operator_heartbeat_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_operator_heartbeat_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.counter );
    mavlink_msg_arc_operator_heartbeat_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_OPERATOR_HEARTBEAT") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT) != NULL);
#endif
}

static void mavlink_test_arc_envelope_set(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
    mavlink_status_t *status = mavlink_get_channel_status(MAVLINK_COMM_0);
        if ((status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) && MAVLINK_MSG_ID_ARC_ENVELOPE_SET >= 256) {
            return;
        }
#endif
    mavlink_message_t msg;
        uint8_t buffer[MAVLINK_MAX_PACKET_LEN];
        uint16_t i;
    mavlink_arc_envelope_set_t packet_in = {
        93372036854775807ULL,73.0,101.0,129.0
    };
    mavlink_arc_envelope_set_t packet1, packet2;
        memset(&packet1, 0, sizeof(packet1));
        packet1.time_ns = packet_in.time_ns;
        packet1.max_speed = packet_in.max_speed;
        packet1.max_accel = packet_in.max_accel;
        packet1.max_steer = packet_in.max_steer;
        
        
#ifdef MAVLINK_STATUS_FLAG_OUT_MAVLINK1
        if (status->flags & MAVLINK_STATUS_FLAG_OUT_MAVLINK1) {
           // cope with extensions
           memset(MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN + (char *)&packet1, 0, sizeof(packet1)-MAVLINK_MSG_ID_ARC_ENVELOPE_SET_MIN_LEN);
        }
#endif
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_envelope_set_encode(system_id, component_id, &msg, &packet1);
    mavlink_msg_arc_envelope_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_envelope_set_pack(system_id, component_id, &msg , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer );
    mavlink_msg_arc_envelope_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_envelope_set_pack_chan(system_id, component_id, MAVLINK_COMM_0, &msg , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer );
    mavlink_msg_arc_envelope_set_decode(&msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

        memset(&packet2, 0, sizeof(packet2));
        mavlink_msg_to_send_buffer(buffer, &msg);
        for (i=0; i<mavlink_msg_get_send_buffer_length(&msg); i++) {
            comm_send_ch(MAVLINK_COMM_0, buffer[i]);
        }
    mavlink_msg_arc_envelope_set_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);
        
        memset(&packet2, 0, sizeof(packet2));
    mavlink_msg_arc_envelope_set_send(MAVLINK_COMM_1 , packet1.time_ns , packet1.max_speed , packet1.max_accel , packet1.max_steer );
    mavlink_msg_arc_envelope_set_decode(last_msg, &packet2);
        MAVLINK_ASSERT(memcmp(&packet1, &packet2, sizeof(packet1)) == 0);

#ifdef MAVLINK_HAVE_GET_MESSAGE_INFO
    MAVLINK_ASSERT(mavlink_get_message_info_by_name("ARC_ENVELOPE_SET") != NULL);
    MAVLINK_ASSERT(mavlink_get_message_info_by_id(MAVLINK_MSG_ID_ARC_ENVELOPE_SET) != NULL);
#endif
}

static void mavlink_test_arc_sync_link(uint8_t system_id, uint8_t component_id, mavlink_message_t *last_msg)
{
    mavlink_test_arc_link_status(system_id, component_id, last_msg);
    mavlink_test_arc_imu_sample(system_id, component_id, last_msg);
    mavlink_test_arc_angle_sample(system_id, component_id, last_msg);
    mavlink_test_arc_ride_height_sample(system_id, component_id, last_msg);
    mavlink_test_arc_motor_status(system_id, component_id, last_msg);
    mavlink_test_arc_steering_status(system_id, component_id, last_msg);
    mavlink_test_arc_camera_trigger(system_id, component_id, last_msg);
    mavlink_test_arc_power_sample(system_id, component_id, last_msg);
    mavlink_test_arc_safety_state(system_id, component_id, last_msg);
    mavlink_test_arc_fault(system_id, component_id, last_msg);
    mavlink_test_arc_drive_command(system_id, component_id, last_msg);
    mavlink_test_arc_operator_heartbeat(system_id, component_id, last_msg);
    mavlink_test_arc_envelope_set(system_id, component_id, last_msg);
}

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ARC_SYNC_LINK_TESTSUITE_H
