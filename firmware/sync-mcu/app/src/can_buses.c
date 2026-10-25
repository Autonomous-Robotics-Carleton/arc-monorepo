#include <zephyr/drivers/can.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "can_buses.h"

LOG_MODULE_REGISTER(can_buses, LOG_LEVEL_INF);

#if DT_NODE_EXISTS(DT_ALIAS(arc_can_command))

#include "can_command_core.h"
#include "safety_task.h"
#include "time_base.h"

#define COMMAND_BITRATE 1000000 /* ADR-0009: 1 Mbit/s */
#define TX_PERIOD_MS 2          /* 500 Hz (bus-load.csv, TBC) */

static const struct device *const can = DEVICE_DT_GET(DT_ALIAS(arc_can_command));
static const struct cc_config config = {
	.erpm_per_mps = CONFIG_ARC_CAN_ERPM_PER_MPS,
	.brake_current_a = CONFIG_ARC_CAN_BRAKE_CURRENT_MA / 1000.0f,
};

static atomic_t status_count[CC_CORNERS];
static int64_t status_last_ns[CC_CORNERS];

const struct device *can_buses_command_device(void)
{
	return can;
}

uint32_t can_buses_status_count(int corner)
{
	return corner >= 0 && corner < CC_CORNERS ? (uint32_t)atomic_get(&status_count[corner]) : 0;
}

int64_t can_buses_status_last_ns(int corner)
{
	return corner >= 0 && corner < CC_CORNERS ? status_last_ns[corner] : 0;
}

static void on_status(const struct device *dev, struct can_frame *frame, void *user_data)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(user_data);

	int corner = cc_status_1_corner(frame->id);

	if (corner >= 0) {
		status_last_ns[corner] = time_base_now_ns();
		atomic_inc(&status_count[corner]);
	}
}

static void tx_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	enum cc_kind last_kind = (enum cc_kind)-1;
	uint32_t errors = 0;

	while (true) {
		struct safety_output decision = safety_task_output();
		struct cc_plan plan = cc_make_plan(&config, &decision);

		if (plan.kind != last_kind) {
			LOG_INF("command bus: sending %s", cc_kind_name(plan.kind));
			if (plan.kind == CC_NONE) {
				LOG_ERR("braking, but ARC_CAN_BRAKE_CURRENT_MA is not set: the "
					"controllers brake on their own timeout (150 ms), not "
					"within SYS-25's 20 ms");
			}
			last_kind = plan.kind;
		}
		if (plan.kind != CC_NONE) {
			for (int i = 0; i < CC_CORNERS; i++) {
				struct can_frame f = {.id = plan.frame[i].id, .flags = CAN_FRAME_IDE,
						      .dlc = plan.frame[i].len};

				memcpy(f.data, plan.frame[i].data, plan.frame[i].len);
				int err = can_send(can, &f, K_NO_WAIT, NULL, NULL);

				if (err && (errors++ % 1000) == 0) {
					LOG_WRN("command bus: send failed (%d), %u so far", err,
						errors);
				}
			}
		}
		k_sleep(K_MSEC(TX_PERIOD_MS));
	}
}

K_THREAD_STACK_DEFINE(can_tx_stack, 2048);
static struct k_thread tx_data;

int can_buses_init(void)
{
	if (!device_is_ready(can)) {
		LOG_ERR("command bus device not ready");
		return -ENODEV;
	}
	int err = can_set_bitrate(can, COMMAND_BITRATE);

	if (err && err != -ENOTSUP) {
		LOG_ERR("command bus bitrate: %d", err);
		return err;
	}
#if DT_NODE_HAS_COMPAT(DT_ALIAS(arc_can_command), zephyr_can_loopback)
	/* the loopback device hands a sent frame to the receive filters only in this mode:
	 * that is how sim_can_bridge.c sees what the firmware sends */
	err = can_set_mode(can, CAN_MODE_LOOPBACK);
	if (err) {
		LOG_ERR("command bus mode: %d", err);
		return err;
	}
#endif
	err = can_start(can);
	if (err) {
		LOG_ERR("command bus start: %d", err);
		return err;
	}
	for (int i = 0; i < CC_CORNERS; i++) {
		struct can_filter filter = {.id = cc_status_1_id(i), .mask = CAN_EXT_ID_MASK,
					    .flags = CAN_FILTER_IDE};

		if (can_add_rx_filter(can, on_status, NULL, &filter) < 0) {
			LOG_ERR("command bus: no filter for STATUS_1 of corner %d", i);
			return -ENOSPC;
		}
	}
	k_thread_create(&tx_data, can_tx_stack, K_THREAD_STACK_SIZEOF(can_tx_stack), tx_thread, NULL, NULL,
			NULL, K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
	k_thread_name_set(&tx_data, "can_tx");
	LOG_INF("command bus up: %d bit/s, every %d ms to the four controllers", COMMAND_BITRATE,
		TX_PERIOD_MS);
	return 0;
}

#else

const struct device *can_buses_command_device(void)
{
	return NULL;
}

uint32_t can_buses_status_count(int corner)
{
	ARG_UNUSED(corner);
	return 0;
}

int64_t can_buses_status_last_ns(int corner)
{
	ARG_UNUSED(corner);
	return 0;
}

int can_buses_init(void)
{
	LOG_INF("not implemented yet");
	return 0;
}

#endif
