/*
 * Stubs. Every function is fail-safe until it's implemented: see safety.h.
 * TODO: implement the envelope (SYS-22) and the watchdog (SYS-04), with tests.
 */

#include "safety.h"

void safety_envelope_default(struct safety_envelope *env)
{
	*env = (struct safety_envelope){0};
}

struct drive_cmd safety_clamp(const struct safety_envelope *env, struct drive_cmd cmd)
{
	(void)env;
	(void)cmd;
	return (struct drive_cmd){0};
}

void safety_watchdog_init(struct safety_watchdog *wd, int64_t timeout_ms, int64_t now_ms)
{
	wd->timeout_ms = timeout_ms;
	wd->last_heartbeat_ms = now_ms;
}

void safety_watchdog_feed(struct safety_watchdog *wd, int64_t now_ms)
{
	wd->last_heartbeat_ms = now_ms;
}

bool safety_watchdog_expired(const struct safety_watchdog *wd, int64_t now_ms)
{
	(void)wd;
	(void)now_ms;
	return true;
}
