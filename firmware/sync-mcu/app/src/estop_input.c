#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

#include "estop_input.h"

LOG_MODULE_REGISTER(estop_input, LOG_LEVEL_INF);

#if DT_NODE_HAS_PROP(DT_PATH(zephyr_user), estop_gpios)

static const struct gpio_dt_spec pin = GPIO_DT_SPEC_GET(DT_PATH(zephyr_user), estop_gpios);
static bool ready;

int estop_input_init(void)
{
	if (!gpio_is_ready_dt(&pin) || gpio_pin_configure_dt(&pin, GPIO_INPUT) < 0) {
		LOG_ERR("e-stop input not ready: reads as pressed");
		return -ENODEV;
	}
	ready = true;
	return 0;
}

bool estop_input_pressed(void)
{
	/* anything but a clean "released" is pressed */
	return !ready || gpio_pin_get_dt(&pin) != 0;
}

#else

int estop_input_init(void)
{
	LOG_WRN("no e-stop input on this board (#95): reads as pressed, so the car can't be reset");
	return 0;
}

bool estop_input_pressed(void)
{
	return true;
}

#endif
