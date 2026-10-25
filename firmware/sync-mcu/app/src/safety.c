#include <math.h>
#include <string.h>

#include "safety.h"

#define DEFAULT_MAX_SPEED_MPS 3.0f /* SYS-22 */

void safety_envelope_default(struct safety_envelope *env)
{
	/* acceleration and steering defaults are TBD (SYS-22): no motion until set */
	*env = (struct safety_envelope){.max_speed_mps = DEFAULT_MAX_SPEED_MPS};
}

static bool limit_ok(float v)
{
	return isfinite(v) && v >= 0.0f;
}

bool safety_envelope_valid(const struct safety_envelope *env)
{
	return limit_ok(env->max_speed_mps) && limit_ok(env->max_accel_mps2) &&
	       limit_ok(env->max_steer_rad);
}

static float limit_symmetric(float v, float limit)
{
	if (v > limit) {
		return limit;
	}
	if (v < -limit) {
		return -limit;
	}
	return v;
}

struct drive_cmd safety_clamp(const struct safety_envelope *env, struct drive_cmd cmd)
{
	if (!safety_envelope_valid(env) || !isfinite(cmd.speed_mps) || !isfinite(cmd.accel_mps2) ||
	    !isfinite(cmd.steer_rad)) {
		return (struct drive_cmd){0};
	}

	struct drive_cmd out = {
		.speed_mps = limit_symmetric(cmd.speed_mps, env->max_speed_mps),
		.accel_mps2 = cmd.accel_mps2 < 0.0f ? 0.0f
			      : cmd.accel_mps2 > env->max_accel_mps2 ? env->max_accel_mps2
								     : cmd.accel_mps2,
		.steer_rad = limit_symmetric(cmd.steer_rad, env->max_steer_rad),
	};

	return out;
}

void safety_watchdog_init(struct safety_watchdog *wd, int64_t timeout_ms, int64_t now_ms)
{
	*wd = (struct safety_watchdog){.timeout_ms = timeout_ms, .last_heartbeat_ms = now_ms};
}

void safety_watchdog_feed(struct safety_watchdog *wd, int64_t now_ms)
{
	if (wd->tripped) {
		return; /* a heartbeat after a trip doesn't restart the car */
	}
	wd->seen = true;
	wd->last_heartbeat_ms = now_ms;
}

bool safety_watchdog_expired(struct safety_watchdog *wd, int64_t now_ms)
{
	if (!wd->seen || wd->tripped) {
		return true;
	}
	if (now_ms - wd->last_heartbeat_ms > wd->timeout_ms) {
		wd->tripped = true;
		return true;
	}
	return false;
}

void safety_watchdog_rearm(struct safety_watchdog *wd, int64_t now_ms)
{
	safety_watchdog_init(wd, wd->timeout_ms, now_ms);
}

const char *safety_reason_name(enum safety_reason r)
{
	switch (r) {
	case SAFETY_DRIVING:
		return "driving";
	case SAFETY_NOT_ARMED:
		return "not armed (no heartbeat yet)";
	case SAFETY_NO_COMMAND:
		return "no fresh command";
	case SAFETY_CONTROLLERS_NOT_READY:
		return "controllers not ready";
	case SAFETY_HEARTBEAT_LOST:
		return "heartbeat lost: braking";
	case SAFETY_CONTROLLER_STOP:
		return "controller stop: braking";
	}
	return "?";
}

void safety_init(struct safety *s, int64_t heartbeat_timeout_ms, int64_t command_timeout_ms,
		 int64_t now_ms)
{
	memset(s, 0, sizeof(*s));
	safety_envelope_default(&s->envelope);
	safety_watchdog_init(&s->watchdog, heartbeat_timeout_ms, now_ms);
	s->command_timeout_ms = command_timeout_ms;
}

void safety_on_heartbeat(struct safety *s, int64_t now_ms)
{
	safety_watchdog_feed(&s->watchdog, now_ms);
}

void safety_on_command(struct safety *s, struct drive_cmd cmd, int64_t now_ms)
{
	s->command = cmd;
	s->command_ms = now_ms;
	s->have_command = true;
}

bool safety_on_envelope(struct safety *s, const struct safety_envelope *env)
{
	if (!safety_envelope_valid(env)) {
		return false;
	}
	s->envelope = *env;
	return true;
}

struct safety_output safety_decide(struct safety *s, int64_t now_ms, bool controllers_stopped,
				   bool controllers_ready)
{
	struct safety_output out = {0};
	bool expired = safety_watchdog_expired(&s->watchdog, now_ms);

	if (controllers_stopped) {
		out.reason = SAFETY_CONTROLLER_STOP;
		out.brake = true;
	} else if (expired && s->watchdog.seen) {
		out.reason = SAFETY_HEARTBEAT_LOST;
		out.brake = true;
	} else if (expired) {
		out.reason = SAFETY_NOT_ARMED;
	} else if (!controllers_ready) {
		out.reason = SAFETY_CONTROLLERS_NOT_READY;
	} else if (!s->have_command || now_ms - s->command_ms > s->command_timeout_ms) {
		out.reason = SAFETY_NO_COMMAND;
	} else {
		out.reason = SAFETY_DRIVING;
		out.cmd = safety_clamp(&s->envelope, s->command);
		out.clamped = memcmp(&out.cmd, &s->command, sizeof(out.cmd)) != 0;
	}
	return out;
}
