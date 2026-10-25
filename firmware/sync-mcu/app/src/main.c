/*
 * Sync MCU firmware (ADR-0017): the car's time base, sensors, CAN buses,
 * motor-controller telemetry links, safety envelope and heartbeat watchdog,
 * and the link to the Orin.
 * Architecture: systems/software/architecture.md.
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "can_buses.h"
#include "controller_links.h"
#include "safety_task.h"
#ifdef CONFIG_ARC_SIM_CAN_BRIDGE
#include "sim_can_bridge.h"
#endif
#include "sensors.h"
#include "sync_link.h"
#include "time_base.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("ARC sync MCU starting on %s", CONFIG_BOARD);

	time_base_init();
	sensors_init();
	safety_task_init(); /* before anything that reads its decision */
	can_buses_init();
	controller_links_init();
	sync_link_init();
#ifdef CONFIG_ARC_SIM_CAN_BRIDGE
	sim_can_bridge_init();
#endif

	while (true) {
		/* everything runs in threads: the controller links, the sync link, the safety task */
		k_sleep(K_SECONDS(1));
	}

	return 0;
}
