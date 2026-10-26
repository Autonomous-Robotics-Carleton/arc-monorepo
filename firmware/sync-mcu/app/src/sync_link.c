#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sync_link.h"

LOG_MODULE_REGISTER(sync_link, LOG_LEVEL_INF);

#ifdef CONFIG_NET_SOCKETS

#include <zephyr/net/socket.h>

#include "arc_sync_link/mavlink.h"
#include "safety_task.h"
#include "time_base.h"

#define SYS_ID 1  /* MAVLink system and component IDs: one car, one sync MCU */
#define COMP_ID 1

static int sock = -1;
static struct sockaddr_in peer;

static void send_link_status(uint32_t gaps, uint32_t bad)
{
	mavlink_message_t msg;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];

	mavlink_msg_arc_link_status_pack(SYS_ID, COMP_ID, &msg, time_base_now_ns(),
					 MAVLINK_VERSION, gaps, bad);
	uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

	zsock_sendto(sock, buf, len, 0, (struct sockaddr *)&peer, sizeof(peer));
}

void sync_link_send_safety_state(float max_speed, float max_accel, float max_steer,
				 int watchdog, int estop)
{
	mavlink_message_t msg;
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];

	if (sock < 0) {
		return;
	}
	mavlink_msg_arc_safety_state_pack(SYS_ID, COMP_ID, &msg, time_base_now_ns(), max_speed,
					  max_accel, max_steer, watchdog, estop);
	uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);

	zsock_sendto(sock, buf, len, 0, (struct sockaddr *)&peer, sizeof(peer));
}

static void handle(const mavlink_message_t *msg, bool *peer_seen)
{
	switch (msg->msgid) {
	case MAVLINK_MSG_ID_ARC_LINK_STATUS:
		if (!*peer_seen) {
			LOG_INF("sync link up: peer protocol v%u",
				mavlink_msg_arc_link_status_get_protocol_version(msg));
			*peer_seen = true;
		}
		break;
	case MAVLINK_MSG_ID_ARC_OPERATOR_HEARTBEAT:
		safety_task_heartbeat();
		break;
	case MAVLINK_MSG_ID_ARC_REARM:
		safety_task_rearm();
		break;
	case MAVLINK_MSG_ID_ARC_DRIVE_COMMAND: {
		mavlink_arc_drive_command_t c;

		mavlink_msg_arc_drive_command_decode(msg, &c);
		safety_task_command((struct drive_cmd){c.speed, c.accel, c.steer});
		break;
	}
	case MAVLINK_MSG_ID_ARC_ENVELOPE_SET: {
		mavlink_arc_envelope_set_t e;

		mavlink_msg_arc_envelope_set_decode(msg, &e);
		safety_task_envelope(&(struct safety_envelope){e.max_speed, e.max_accel,
							       e.max_steer});
		break;
	}
	default:
		break;
	}
}

static void link_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	mavlink_status_t status = {0};
	mavlink_message_t msg = {0};
	uint8_t buf[MAVLINK_MAX_PACKET_LEN];
	int64_t next_status = 0;
	bool peer_seen = false;

	while (true) {
		if (k_uptime_get() >= next_status) {
			send_link_status(status.packet_rx_drop_count, status.parse_error);
			next_status = k_uptime_get() + 1000;
		}

		/* one datagram per call: drain them all, the Orin sends 500 commands a second */
		ssize_t n;

		while ((n = zsock_recv(sock, buf, sizeof(buf), ZSOCK_MSG_DONTWAIT)) > 0) {
			for (ssize_t i = 0; i < n; i++) {
				if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
					handle(&msg, &peer_seen);
				}
			}
		}
		k_sleep(K_MSEC(1));
	}
}

K_THREAD_STACK_DEFINE(link_stack, 4096);
static struct k_thread link_thread_data;

int sync_link_init(void)
{
	sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		LOG_ERR("socket: %d", errno);
		return -errno;
	}

	struct sockaddr_in local = {
		.sin_family = AF_INET,
		.sin_port = htons(CONFIG_ARC_SYNC_LINK_PORT_RX),
		.sin_addr.s_addr = htonl(INADDR_ANY),
	};

	if (zsock_bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
		LOG_ERR("bind %d: %d", CONFIG_ARC_SYNC_LINK_PORT_RX, errno);
		return -errno;
	}

	peer.sin_family = AF_INET;
	peer.sin_port = htons(CONFIG_ARC_SYNC_LINK_PORT_TX);
	zsock_inet_pton(AF_INET, CONFIG_ARC_SYNC_LINK_PEER, &peer.sin_addr);

	k_thread_create(&link_thread_data, link_stack, K_THREAD_STACK_SIZEOF(link_stack),
			link_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(7), 0, K_NO_WAIT);
	LOG_INF("listening on UDP %d, sending to %s:%d", CONFIG_ARC_SYNC_LINK_PORT_RX,
		CONFIG_ARC_SYNC_LINK_PEER, CONFIG_ARC_SYNC_LINK_PORT_TX);
	return 0;
}

#else

int sync_link_init(void)
{
	LOG_INF("not implemented yet");
	return 0;
}

void sync_link_send_safety_state(float max_speed, float max_accel, float max_steer,
				 int watchdog, int estop)
{
	ARG_UNUSED(max_speed);
	ARG_UNUSED(max_accel);
	ARG_UNUSED(max_steer);
	ARG_UNUSED(watchdog);
	ARG_UNUSED(estop);
}

#endif
