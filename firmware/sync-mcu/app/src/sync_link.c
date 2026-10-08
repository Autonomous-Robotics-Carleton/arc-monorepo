#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "sync_link.h"

LOG_MODULE_REGISTER(sync_link, LOG_LEVEL_INF);

#ifdef CONFIG_NET_SOCKETS

#include <zephyr/net/socket.h>

#include "arc_sync_link/mavlink.h"
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

		ssize_t n = zsock_recv(sock, buf, sizeof(buf), ZSOCK_MSG_DONTWAIT);

		for (ssize_t i = 0; i < n; i++) {
			if (!mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status)) {
				continue;
			}
			if (msg.msgid == MAVLINK_MSG_ID_ARC_LINK_STATUS && !peer_seen) {
				LOG_INF("sync link up: peer protocol v%u",
					mavlink_msg_arc_link_status_get_protocol_version(&msg));
				peer_seen = true;
			}
		}
		k_sleep(K_MSEC(10));
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

#endif
