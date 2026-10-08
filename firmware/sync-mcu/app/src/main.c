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
#include "safety.h"
#include "sensors.h"
#include "sync_link.h"
#include "time_base.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

int main(void)
{
	LOG_INF("ARC sync MCU starting on %s", CONFIG_BOARD);

	time_base_init();
	sensors_init();
	can_buses_init();
	controller_links_init();
	sync_link_init();

	struct safety_envelope envelope;
	struct safety_watchdog watchdog;

	safety_envelope_default(&envelope);
	safety_watchdog_init(&watchdog, CONFIG_ARC_HEARTBEAT_TIMEOUT_MS, k_uptime_get());

	while (true) {
		/* No heartbeats reach the watchdog yet: this only shows the loop. */
		k_sleep(K_MSEC(100));
	}

	return 0;
}
