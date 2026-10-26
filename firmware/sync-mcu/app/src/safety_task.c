#include <string.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include "arc_sync_link/mavlink.h"
#include "controller_links.h"
#include "estop_input.h"
#include "fault_latch.h"
#include "rate_monitor.h"
#include "safety_task.h"
#include "sync_link.h"
#include "time_base.h"

LOG_MODULE_REGISTER(safety, LOG_LEVEL_INF);

#define TASK_PERIOD_MS 1
#define LOG_COMMAND_MS 100
#define RATE_WINDOW_NS 1000000000LL

static struct safety state;
static struct fault_latch latch; /* the fault stop and its reset (ADR-0039), under lock too */
static bool last_estop;
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
		LOG_WRN("envelope refused: a limit is negative, not a number or above the car's");
	}
	return ok;
}

bool safety_task_rearm(void)
{
	k_mutex_lock(&lock, K_FOREVER);
	bool reset = fl_clear(&latch);
	enum fl_state latch_state = latch.state;
	bool after_stop = state.watchdog.tripped;
	bool ok = reset && safety_on_rearm(&state, now_ms());

	k_mutex_unlock(&lock);
	if (ok) {
		LOG_INF("re-armed%s", after_stop ? " after a heartbeat stop" : "");
	} else if (!reset) {
		LOG_WRN("re-arm refused: %s (ADR-0039: e-stop cycle and power-on checks first)",
			fl_state_name(latch_state));
	} else {
		LOG_WRN("re-arm refused: no heartbeat arriving");
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
	/* ARC_WATCHDOG_STATE has no "not armed yet": report it as ROLLING (not driving, not braking).
	 * A heartbeat stop stays BRAKING until re-armed (ADR-0052). */
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

static void report_state(const struct safety_output *o, const struct safety_envelope *env,
			 bool estop)
{
	sync_link_send_safety_state(env->max_speed_mps, env->max_accel_mps2, env->max_steer_rad,
				    watchdog_state(o), estop);
}

static const char *const corner_name[CL_CORNERS] = {"FL", "FR", "RL", "RR"};

static void log_latch(enum fl_event ev, const struct fault_latch *fl)
{
	const char *corner = fl->corner >= 0 && fl->corner < CL_CORNERS ? corner_name[fl->corner]
									: "-";

	switch (ev) {
	case FL_EV_NONE:
		break;
	case FL_EV_LATCHED:
	case FL_EV_RESTART:
	case FL_EV_CHECKS_FAILED:
		LOG_ERR("fault latch: %s: %s (%s %s%s; %d stops since power-on)",
			fl_event_name(ev), fl_cause_name(fl->cause), corner,
			cl_reason_name(fl->corner_reason), fl->wheels_moving ? ", wheels moving" : "",
			fl->stops);
		break;
	case FL_EV_CLEARED:
		LOG_INF("fault latch: %s", fl_event_name(ev));
		LOG_WRN("fault latch: the steering wasn't checked (no steering driver yet)");
		break;
	default:
		LOG_INF("fault latch: %s", fl_event_name(ev));
		break;
	}
}

/* The latch's view of the car, from the e-stop input and the controller links. */
static struct fl_inputs latch_inputs(void)
{
	struct fl_inputs in = {.estop_pressed = estop_input_pressed()};

	controller_links_reset_inputs(&in.corners, &in.corner, &in.fault_code,
				      &in.wheels_stopped);
	return in;
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
		struct fl_inputs in = latch_inputs();

		k_mutex_lock(&lock, K_FOREVER);
		bool was_clear = fl_clear(&latch);
		enum fl_event ev = fl_update(&latch, now_ms(), &in);
		struct fault_latch fl = latch;

		if (was_clear && !fl_clear(&latch)) {
			safety_disarm(&state); /* the operator re-arms after the reset */
		}
		bool stopped = fl_brake(&latch);
		bool ready = fl_clear(&latch) && in.corners != CL_NOT_READY &&
			     in.corners != CL_VERSION;
		struct safety_output out = safety_decide(&state, now_ms(), stopped, ready);
		struct safety_envelope env = state.envelope;
		bool changed = !have_last || out.reason != last.reason || out.brake != last.brake ||
			       out.clamped != last.clamped || in.estop_pressed != last_estop ||
			       memcmp(&env, &last_envelope, sizeof(env)) != 0;
		int64_t now = now_ms();
		bool log = changed || (command_differs(&out, &logged) &&
				       now - logged_ms >= LOG_COMMAND_MS);

		last = out;
		last_envelope = env;
		last_estop = in.estop_pressed;
		have_last = true;
		struct rate_report rate;
		bool rate_ended = rate_monitor_poll(&command_rate, time_base_now_ns(), &rate);

		k_mutex_unlock(&lock);

		log_latch(ev, &fl);
		if (ev == FL_EV_CLEARED) {
			controller_links_reset();
		}
		if (rate_ended) {
			check_command_rate(&rate);
		}
		if (log) {
			log_output(&out);
			logged = out;
			logged_ms = now;
		}
		if (changed) {
			report_state(&out, &env, in.estop_pressed);
		}
		k_sleep(K_MSEC(TASK_PERIOD_MS));
	}
}

K_THREAD_STACK_DEFINE(task_stack, 2048);
static struct k_thread task_data;

int safety_task_init(void)
{
	const struct safety_envelope car = {
		.max_speed_mps = CONFIG_ARC_CAR_MAX_SPEED_MMPS / 1000.0f,
		.max_accel_mps2 = CONFIG_ARC_CAR_MAX_ACCEL_MMPS2 / 1000.0f,
		.max_steer_rad = CONFIG_ARC_CAR_MAX_STEER_MRAD / 1000.0f,
	};

	const struct fl_config reset = {
		.cut_ms = CONFIG_ARC_ESTOP_CUT_MS,
		.check_window_ms = CONFIG_ARC_RESET_CHECK_WINDOW_MS,
		.max_stops = CONFIG_ARC_RESET_MAX_STOPS,
	};

	estop_input_init();
	safety_init(&state, &car, CONFIG_ARC_HEARTBEAT_TIMEOUT_MS, CONFIG_ARC_COMMAND_TIMEOUT_MS,
		    now_ms());
	fl_init(&latch, &reset, now_ms());
	LOG_INF("fault latch: latched at power-on; reset with the e-stop held %d ms, then re-arm",
		CONFIG_ARC_ESTOP_CUT_MS);
	rate_monitor_init(&command_rate, RATE_WINDOW_NS, time_base_now_ns());
	k_thread_create(&task_data, task_stack, K_THREAD_STACK_SIZEOF(task_stack), task, NULL,
			NULL, NULL, K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
	k_thread_name_set(&task_data, "safety");
	LOG_INF("safety: heartbeat timeout %d ms, command timeout %d ms",
		CONFIG_ARC_HEARTBEAT_TIMEOUT_MS, CONFIG_ARC_COMMAND_TIMEOUT_MS);
	return 0;
}
