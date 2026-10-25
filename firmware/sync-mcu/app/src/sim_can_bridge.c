/*
 * Simulation only (native_sim): the command-bus loopback device, carried over
 * UDP so a program outside the firmware can be the controllers on the bus.
 * Every frame the firmware sends (but not the STATUS_1 frames it receives) goes
 * to UDP ARC_SIM_CAN_PORT_TX; a datagram on ARC_SIM_CAN_PORT_RX is put on the
 * bus. 13 bytes: the 29-bit ID (big-endian), the length, 8 data bytes.
 * Not part of the car: nothing on the hardware boards includes it.
 */

#include <string.h>

#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#include "can_buses.h"
#include "can_command_core.h"
#include "sim_can_bridge.h"

LOG_MODULE_REGISTER(sim_can_bridge, LOG_LEVEL_INF);

#define WIRE_LEN 13

static int sock = -1;
static struct sockaddr_in peer;

static void to_simulator(const struct device *dev, struct can_frame *frame, void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	if (cc_status_1_corner(frame->id) >= 0) {
		return; /* we put those on the bus ourselves */
	}
	uint8_t buf[WIRE_LEN] = {frame->id >> 24, frame->id >> 16, frame->id >> 8, frame->id,
				 can_dlc_to_bytes(frame->dlc)};

	memcpy(&buf[5], frame->data, 8);
	zsock_sendto(sock, buf, sizeof(buf), 0, (struct sockaddr *)&peer, sizeof(peer));
}

static void from_simulator(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	const struct device *can = can_buses_command_device();
	uint8_t buf[64];

	while (true) {
		ssize_t n = zsock_recv(sock, buf, sizeof(buf), 0);

		if (n != WIRE_LEN || buf[4] > 8) {
			continue;
		}
		struct can_frame f = {
			.id = ((uint32_t)buf[0] << 24 | buf[1] << 16 | buf[2] << 8 | buf[3]) &
			      CAN_EXT_ID_MASK,
			.flags = CAN_FRAME_IDE,
			.dlc = buf[4],
		};

		memcpy(f.data, &buf[5], 8);
		can_send(can, &f, K_MSEC(5), NULL, NULL);
	}
}

K_THREAD_STACK_DEFINE(sim_can_stack, 2048);
static struct k_thread bridge_data;

int sim_can_bridge_init(void)
{
	const struct device *can = can_buses_command_device();

	if (!can) {
		return -ENODEV;
	}
	sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		return -errno;
	}
	struct sockaddr_in local = {
		.sin_family = AF_INET,
		.sin_port = htons(CONFIG_ARC_SIM_CAN_PORT_RX),
		.sin_addr.s_addr = htonl(INADDR_ANY),
	};

	if (zsock_bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
		LOG_ERR("bind %d: %d", CONFIG_ARC_SIM_CAN_PORT_RX, errno);
		return -errno;
	}
	peer.sin_family = AF_INET;
	peer.sin_port = htons(CONFIG_ARC_SIM_CAN_PORT_TX);
	zsock_inet_pton(AF_INET, "127.0.0.1", &peer.sin_addr);

	struct can_filter any = {.id = 0, .mask = 0, .flags = CAN_FILTER_IDE};

	if (can_add_rx_filter(can, to_simulator, NULL, &any) < 0) {
		return -ENOSPC;
	}
	k_thread_create(&bridge_data, sim_can_stack, K_THREAD_STACK_SIZEOF(sim_can_stack),
			from_simulator, NULL, NULL, NULL, K_PRIO_PREEMPT(6), 0, K_NO_WAIT);
	k_thread_name_set(&bridge_data, "sim_can");
	LOG_INF("simulator CAN bridge: UDP %d in, %d out (127.0.0.1)", CONFIG_ARC_SIM_CAN_PORT_RX,
		CONFIG_ARC_SIM_CAN_PORT_TX);
	return 0;
}
