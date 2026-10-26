#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/spinlock.h>
#include <zephyr/sys/ring_buffer.h>

#include "controller_links.h"
#include "time_base.h"

LOG_MODULE_REGISTER(controller_links, LOG_LEVEL_INF);

static struct k_spinlock lock;
static enum cl_reason latched = CL_OK;
static int latched_corner = -1;
static enum cl_reason current = CL_NOT_READY;
static int current_corner = -1;
static uint8_t current_fault;
static bool wheels_stopped;

/* K_SPINLOCK must be left by falling out of its block: a return inside skips the unlock */
enum cl_reason controller_links_stop(int *corner)
{
	enum cl_reason r = CL_OK;
	int c = -1;

	K_SPINLOCK(&lock) {
		r = latched;
		c = latched_corner;
	}
	if (corner) {
		*corner = c;
	}
	return r;
}

enum cl_reason controller_links_state(int *corner)
{
	enum cl_reason r = CL_NOT_READY;
	int c = -1;

	K_SPINLOCK(&lock) {
		r = current;
		c = current_corner;
	}
	if (corner) {
		*corner = c;
	}
	return r;
}

void controller_links_reset_inputs(enum cl_reason *reason, int *corner, uint8_t *fault_code,
				   bool *stopped)
{
	K_SPINLOCK(&lock) {
		*reason = current;
		*corner = current_corner;
		*fault_code = current_fault;
		*stopped = wheels_stopped;
	}
}

void controller_links_reset(void)
{
	K_SPINLOCK(&lock) {
		latched = CL_OK;
		latched_corner = -1;
	}
	LOG_INF("any-corner stop reset");
}

#if DT_NODE_EXISTS(DT_ALIAS(arc_ctrl_fl)) && DT_NODE_EXISTS(DT_ALIAS(arc_ctrl_fr)) && \
	DT_NODE_EXISTS(DT_ALIAS(arc_ctrl_rl)) && DT_NODE_EXISTS(DT_ALIAS(arc_ctrl_rr))

static const char *const corner_name[CL_CORNERS] = {"FL", "FR", "RL", "RR"};

#define CTRL_UART(n) DEVICE_DT_GET(DT_ALIAS(arc_ctrl_##n))

static const struct device *const uart[CL_CORNERS] = {
	CTRL_UART(fl), CTRL_UART(fr), CTRL_UART(rl), CTRL_UART(rr),
};

#define RX_BYTES 1024 /* about 18 ms of one controller's traffic */

static uint8_t rx_mem[CL_CORNERS][RX_BYTES];
static struct ring_buf rx[CL_CORNERS];
static uint32_t overruns[CL_CORNERS];
static uint32_t overruns_reported[CL_CORNERS];
/* When each corner's newest bytes arrived (set in the interrupt): a frame is
 * as old as its arrival, not as old as the thread's turn to read it */
static int64_t arrived_ns[CL_CORNERS];
static K_SEM_DEFINE(rx_sem, 0, 1);
static struct cl_links links;

/* The receive interrupt only moves bytes; the thread below decides. */
static void uart_isr(const struct device *dev, void *user_data)
{
	int i = (int)(intptr_t)user_data;
	uint8_t buf[64];

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (!uart_irq_rx_ready(dev)) {
			break;
		}
		int n = uart_fifo_read(dev, buf, sizeof(buf));

		if (n <= 0) {
			break;
		}
		if (ring_buf_put(&rx[i], buf, n) < (uint32_t)n) {
			overruns[i]++;
		}
		arrived_ns[i] = time_base_now_ns();
	}
	k_sem_give(&rx_sem);
}

static void update(int64_t now)
{
	int corner = -1;
	enum cl_reason r = cl_check(&links, now, &corner);
	bool first_stop = false;
	bool changed = false;

	bool stopped = cl_all_stopped(&links, CONFIG_ARC_WHEEL_STOPPED_ERPM);
	uint8_t fault = corner >= 0 ? links.c[corner].fault : 0;

	K_SPINLOCK(&lock) {
		changed = (r != current || corner != current_corner);
		current = r;
		current_corner = corner;
		current_fault = fault;
		wheels_stopped = stopped;
		if (cl_is_stop(r) && latched == CL_OK) {
			latched = r;
			latched_corner = corner;
			first_stop = true;
		}
	}
	if (first_stop) {
		LOG_ERR("any-corner stop: %s %s", corner_name[corner], cl_reason_name(r));
	} else if (changed && !cl_is_stop(r)) {
		LOG_INF("controller links: %s%s%s", cl_reason_name(r), corner >= 0 ? " at " : "",
			corner >= 0 ? corner_name[corner] : "");
	}
}

static void links_thread(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	uint8_t buf[128];

	while (true) {
		k_sem_take(&rx_sem, K_MSEC(1)); /* wake at least every ms to judge silence */

		for (int i = 0; i < CL_CORNERS; i++) {
			/* the arrival time first, then the bytes: anything that landed in
			 * between is stamped slightly early, never late */
			unsigned int key = irq_lock();
			int64_t arrived = arrived_ns[i];

			irq_unlock(key);

			uint32_t n;

			while ((n = ring_buf_get(&rx[i], buf, sizeof(buf))) > 0) {
				cl_feed(&links, i, buf, n, arrived);
			}
			if (overruns[i] != overruns_reported[i]) {
				LOG_WRN("controller %s: receive buffer overran (%u so far)",
					corner_name[i], overruns[i]);
				overruns_reported[i] = overruns[i];
			}
		}
		update(time_base_now_ns());
	}
}

/* The detection path of SYS-25: ahead of everything but the interrupts */
K_THREAD_STACK_DEFINE(links_stack, 4096);
static struct k_thread links_thread_data;

int controller_links_init(void)
{
	cl_init(&links, (int64_t)CONFIG_ARC_CONTROLLER_SILENCE_MS * 1000000, MAVLINK_VERSION);

	for (int i = 0; i < CL_CORNERS; i++) {
		if (!device_is_ready(uart[i])) {
			LOG_ERR("controller UART %s not ready", corner_name[i]);
			return -ENODEV;
		}
		ring_buf_init(&rx[i], sizeof(rx_mem[i]), rx_mem[i]);
		uart_irq_callback_user_data_set(uart[i], uart_isr, (void *)(intptr_t)i);
		uart_irq_rx_enable(uart[i]);
	}
	k_thread_create(&links_thread_data, links_stack, K_THREAD_STACK_SIZEOF(links_stack),
			links_thread, NULL, NULL, NULL, K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
	k_thread_name_set(&links_thread_data, "controller_links");
	LOG_INF("controller links up, stop after %d ms of silence", CONFIG_ARC_CONTROLLER_SILENCE_MS);
	return 0;
}

#else

int controller_links_init(void)
{
	LOG_INF("no controller UARTs on this board yet");
	return 0;
}

#endif
