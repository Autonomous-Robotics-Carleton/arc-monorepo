/*
 * Simulation only (native_sim): the e-stop button, pressed by a program
 * outside the firmware. A one-byte datagram on UDP ARC_SIM_ESTOP_PORT sets the
 * emulated e-stop input: 1 pressed, 0 released. Not part of the car: nothing
 * on the hardware boards includes it.
 */

#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_emul.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/socket.h>

#include "sim_estop.h"

LOG_MODULE_REGISTER(sim_estop, LOG_LEVEL_INF);

static const struct gpio_dt_spec pin = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), estop_gpios);
static int sock = -1;

static void from_simulator(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	uint8_t buf[8];

	while (true) {
		ssize_t n = zsock_recv(sock, buf, sizeof(buf), 0);

		if (n != 1 || buf[0] > 1) {
			continue;
		}
		/* the input's physical level: the pin is active high in the overlay */
		gpio_emul_input_set_dt(&pin, buf[0]);
	}
}

K_THREAD_STACK_DEFINE(sim_estop_stack, 1024);
static struct k_thread sim_estop_data;

int sim_estop_init(void)
{
	sock = zsock_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock < 0) {
		return -errno;
	}
	struct sockaddr_in local = {
		.sin_family = AF_INET,
		.sin_port = htons(CONFIG_ARC_SIM_ESTOP_PORT),
		.sin_addr.s_addr = htonl(INADDR_ANY),
	};

	if (zsock_bind(sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
		LOG_ERR("bind %d: %d", CONFIG_ARC_SIM_ESTOP_PORT, errno);
		return -errno;
	}
	k_thread_create(&sim_estop_data, sim_estop_stack, K_THREAD_STACK_SIZEOF(sim_estop_stack),
			from_simulator, NULL, NULL, NULL, K_PRIO_PREEMPT(6), 0, K_NO_WAIT);
	k_thread_name_set(&sim_estop_data, "sim_estop");
	LOG_INF("simulator e-stop: UDP %d in", CONFIG_ARC_SIM_ESTOP_PORT);
	return 0;
}
