#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "time_base.h"

LOG_MODULE_REGISTER(time_base, LOG_LEVEL_INF);

int time_base_init(void)
{
	LOG_INF("using kernel uptime until the hardware timer is set up");
	return 0;
}

int64_t time_base_now_ns(void)
{
	return (int64_t)k_ticks_to_ns_floor64(k_uptime_ticks());
}
