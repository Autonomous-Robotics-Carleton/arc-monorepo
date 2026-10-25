#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "arc_sync_link/mavlink.h"
#include "controller_links.h"
#include "rate_monitor.h"
#include "safety_task.h"
#include "sync_link.h"
#include "time_base.h"

LOG_MODULE_REGISTER(safety, LOG_LEVEL_INF);

#define TASK_PERIOD_MS 1
#define LOG_COMMAND_MS 100
#define RATE_WINDOW_NS 1000000000LL

static struct safety state;
static struct safety_output last;
static bool have_last;
static struct safety_envelope last_envelope;
/* what was last logged: the command is logged when it changes, at most every LOG_COMMAND_MS */
static struct safety_output logged;
static int64_t logged_ms;
static K_MUTEX_DEFINE(lock);
/* arrival rate of ARC_DRIVE_COMMAND; measured, never acted on */
static struct rate_monitor command_rate;
static bool command_rate_ok = true;
static bool command_rate_seen;

static int64_t now_ms(void)
{
	return time_base_now_ns() / 1000000;
}

void safety_task_heartbeat(void)
{
	k_mutex_lock(&lock, K_FOREVER);
	safety_on_heartbeat(&state, now_ms());
	k_mutex_unlock(&lock);
}

void safety_task_command(struct drive_cmd cmd)
{
	k_mutex_lock(&lock, K_FOREVER);
	safety_on_command(&state, cmd, now_ms());
	rate_monitor_arrival(&command_rate, time_base_now_ns());
	k_mutex_unlock(&lock);
}

bool safety_task_envelope(const struct safety_envelope *env)
{
	k_mutex_lock(&lock, K_FOREVER);
	bool ok = safety_on_envelope(&state, env);

	k_mutex_unlock(&lock);
	if (!ok) {
		LOG_WRN("envelope refused: a limit is negative or not a number");
	}
	return ok;
}

struct safety_output safety_task_output(void)
{
	k_mutex_lock(&lock, K_FOREVER);
	/* until the first decision: stopped, nothing commanded */
	struct safety_output out = have_last ? last
					     : (struct safety_output){.reason = SAFETY_NOT_ARMED};

	k_mutex_unlock(&lock);
	return out;
}

static int watchdog_state(const struct safety_output *o)
{
	/* ARC_WATCHDOG_STATE has no "no heartbeat yet": report it as ROLLING (not arriving, not braking) */
	switch (o->reason) {
	case SAFETY_HEARTBEAT_LOST:
		return ARC_WATCHDOG_BRAKING;
	case SAFETY_NOT_ARMED:
		return ARC_WATCHDOG_ROLLING;
	default:
		return ARC_WATCHDOG_OK;
	}
}

static void log_output(const struct safety_output *o)
{
	LOG_INF("safety: %s%s%s speed=%d mm/s accel=%d mm/s2 steer=%d mrad",
		safety_reason_name(o->reason), o->brake ? " [brake]" : "",
		o->clamped ? " [clamped to the envelope]" : "", (int)(o->cmd.speed_mps * 1000),
		(int)(o->cmd.accel_mps2 * 1000), (int)(o->cmd.steer_rad * 1000));
}

static bool command_differs(const struct safety_output *a, const struct safety_output *b)
{
	return (int)(a->cmd.speed_mps * 1000) != (int)(b->cmd.speed_mps * 1000) ||
	       (int)(a->cmd.accel_mps2 * 1000) != (int)(b->cmd.accel_mps2 * 1000) ||
	       (int)(a->cmd.steer_rad * 1000) != (int)(b->cmd.steer_rad * 1000);
}

static void report_state(const struct safety_output *o, const struct safety_envelope *env)
{
	/* estop: the physical e-stop input isn't read yet */
	sync_link_send_safety_state(env->max_speed_mps, env->max_accel_mps2, env->max_steer_rad,
				    watchdog_state(o), 0);
}

static void check_command_rate(const struct rate_report *r)
{
	bool ok = rate_within(r->rate_hz, CONFIG_ARC_COMMAND_RATE_HZ,
			      CONFIG_ARC_COMMAND_RATE_TOLERANCE_PCT);

	if (!command_rate_seen && r->count == 0) {
		return; /* the Orin hasn't started commanding */
	}
	if (!ok && (command_rate_ok || !command_rate_seen)) {
		LOG_WRN("command rate: %u Hz, expected %d Hz +-%d%% (longest gap %d us)", r->rate_hz,
			CONFIG_ARC_COMMAND_RATE_HZ, CONFIG_ARC_COMMAND_RATE_TOLERANCE_PCT,
			(int)(r->max_gap_ns / 1000));
	} else if (!command_rate_seen || !command_rate_ok) {
		LOG_INF("command rate: %u Hz (longest gap %d us)", r->rate_hz,
			(int)(r->max_gap_ns / 1000));
	}
	command_rate_seen = true;
	command_rate_ok = ok;
}

static void task(void *a, void *b, void *c)
{
	ARG_UNUSED(a);
	ARG_UNUSED(b);
	ARG_UNUSED(c);

	while (true) {
		int corner;
		enum cl_reason latched = controller_links_stop(&corner);
		enum cl_reason current = controller_links_state(&corner);
		bool stopped = cl_is_stop(latched) || cl_is_stop(current);
		bool ready = current != CL_NOT_READY && current != CL_VERSION;

		k_mutex_lock(&lock, K_FOREVER);
		struct safety_output out = safety_decide(&state, now_ms(), stopped, ready);
		struct safety_envelope env = state.envelope;
		bool changed = !have_last || out.reason != last.reason || out.brake != last.brake ||
			       out.clamped != last.clamped ||
			       memcmp(&env, &last_envelope, sizeof(env)) != 0;
		int64_t now = now_ms();
		bool log = changed || (command_differs(&out, &logged) &&
				       now - logged_ms >= LOG_COMMAND_MS);

		last = out;
		last_envelope = env;
		have_last = true;
		struct rate_report rate;
		bool rate_ended = rate_monitor_poll(&command_rate, time_base_now_ns(), &rate);

		k_mutex_unlock(&lock);

		if (rate_ended) {
			check_command_rate(&rate);
		}
		if (log) {
			log_output(&out);
			logged = out;
			logged_ms = now;
		}
		if (changed) {
			report_state(&out, &env);
		}
		k_sleep(K_MSEC(TASK_PERIOD_MS));
	}
}

K_THREAD_STACK_DEFINE(task_stack, 2048);
static struct k_thread task_data;

int safety_task_init(void)
{
	safety_init(&state, CONFIG_ARC_HEARTBEAT_TIMEOUT_MS, CONFIG_ARC_COMMAND_TIMEOUT_MS,
		    now_ms());
	rate_monitor_init(&command_rate, RATE_WINDOW_NS, time_base_now_ns());
	k_thread_create(&task_data, task_stack, K_THREAD_STACK_SIZEOF(task_stack), task, NULL,
			NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
	k_thread_name_set(&task_data, "safety");
	LOG_INF("safety: heartbeat timeout %d ms, command timeout %d ms",
		CONFIG_ARC_HEARTBEAT_TIMEOUT_MS, CONFIG_ARC_COMMAND_TIMEOUT_MS);
	return 0;
}
