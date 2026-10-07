/*
 * Safety envelope and heartbeat watchdog (SYS-04, SYS-22). Stub.
 *
 * The interface is scaffolding; the behaviour is for the module's owner
 * to implement. Until then every function is fail-safe: nothing it guards
 * can move the car. A safety function: changes need their own tests and a
 * second reviewer.
 */

#ifndef ARC_SAFETY_H
#define ARC_SAFETY_H

#include <stdbool.h>
#include <stdint.h>

/* A drive and steering command from the Orin. */
struct drive_cmd {
	float speed_mps;
	float accel_mps2;
	float steer_rad;
};

/* Limits no command may exceed, set per session (SYS-22). */
struct safety_envelope {
	float max_speed_mps;
	float max_accel_mps2;
	float max_steer_rad;
};

/* The envelope a new experiment starts with (SYS-22: 3 m/s).
 * Stub: an envelope that allows no motion. */
void safety_envelope_default(struct safety_envelope *env);

/* The command limited to the envelope (SYS-22).
 * Stub: always a zero command. */
struct drive_cmd safety_clamp(const struct safety_envelope *env, struct drive_cmd cmd);

/* Operator heartbeat watchdog (SYS-04). */
struct safety_watchdog {
	int64_t last_heartbeat_ms;
	int64_t timeout_ms;
};

void safety_watchdog_init(struct safety_watchdog *wd, int64_t timeout_ms, int64_t now_ms);
void safety_watchdog_feed(struct safety_watchdog *wd, int64_t now_ms);

/* Stub: always expired, as if no heartbeat had arrived. */
bool safety_watchdog_expired(const struct safety_watchdog *wd, int64_t now_ms);

#endif /* ARC_SAFETY_H */
