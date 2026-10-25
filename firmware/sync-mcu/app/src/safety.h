/*
 * Safety envelope, heartbeat watchdog and the drive decision (SYS-04, SYS-22,
 * SYS-25, ADR-0039). Pure logic: times are passed in (milliseconds), so the
 * tests run it in virtual time. The Zephyr task around it is safety_task.c.
 *
 * Fail-safe by construction: nothing moves the car unless a heartbeat has
 * arrived, the watchdog hasn't expired, the controllers are ready and not
 * stopped, and a fresh command has arrived; and then only inside the envelope.
 *
 * A safety function: changes need their own tests and a second reviewer.
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

/*
 * The envelope a new experiment starts with. SYS-22 gives 3 m/s; it gives no
 * default for acceleration or steering (TBD), so those are 0: the car can't
 * accelerate or steer until the Orin sets an envelope (ARC_ENVELOPE_SET).
 */
void safety_envelope_default(struct safety_envelope *env);

/* True if every limit is a finite number >= 0. A bad envelope is refused, never clamped. */
bool safety_envelope_valid(const struct safety_envelope *env);

/*
 * The command limited to the envelope (SYS-22): speed and steering to
 * +-limit, acceleration to 0..limit. A command with a NaN or infinite field
 * becomes a zero command.
 */
struct drive_cmd safety_clamp(const struct safety_envelope *env, struct drive_cmd cmd);

/*
 * Operator heartbeat watchdog (SYS-04). Before the first heartbeat it is
 * expired (the car boots stopped, ADR-0039). Once expired after a heartbeat
 * it stays expired until safety_watchdog_rearm(): a heartbeat that comes
 * back mid-brake must not restart the car. The re-arm sequence of ADR-0039
 * isn't written, so today only a reboot clears it.
 */
struct safety_watchdog {
	int64_t last_heartbeat_ms;
	int64_t timeout_ms;
	bool seen;    /* a heartbeat has arrived */
	bool tripped; /* it expired after being seen */
};

void safety_watchdog_init(struct safety_watchdog *wd, int64_t timeout_ms, int64_t now_ms);
void safety_watchdog_feed(struct safety_watchdog *wd, int64_t now_ms);

/* No heartbeat for more than timeout_ms (exactly timeout_ms is still in time), or none yet. */
bool safety_watchdog_expired(struct safety_watchdog *wd, int64_t now_ms);

/* Clears a trip. Only the reset sequence may call it (ADR-0039, not written yet). */
void safety_watchdog_rearm(struct safety_watchdog *wd, int64_t now_ms);

/* Why the output is what it is. */
enum safety_reason {
	SAFETY_DRIVING = 0,       /* the command, inside the envelope */
	SAFETY_NOT_ARMED,         /* no heartbeat yet: zero command */
	SAFETY_NO_COMMAND,        /* no fresh command: zero command */
	SAFETY_CONTROLLERS_NOT_READY, /* a corner hasn't reported, or a version mismatch: zero command */
	SAFETY_HEARTBEAT_LOST,    /* brake (SYS-04) */
	SAFETY_CONTROLLER_STOP    /* brake: the any-corner stop (SYS-25) */
};

const char *safety_reason_name(enum safety_reason r);

struct safety_output {
	enum safety_reason reason;
	bool brake;           /* the e-stop ramp: the car must stop, not just stop accelerating */
	bool clamped;         /* the envelope changed the command */
	struct drive_cmd cmd; /* what to send; zero unless SAFETY_DRIVING */
};

struct safety {
	struct safety_envelope envelope;
	struct safety_watchdog watchdog;
	struct drive_cmd command;
	int64_t command_ms; /* when it arrived */
	bool have_command;
	int64_t command_timeout_ms;
};

void safety_init(struct safety *s, int64_t heartbeat_timeout_ms, int64_t command_timeout_ms,
		 int64_t now_ms);

/* Messages from the Orin, with the arrival time. */
void safety_on_heartbeat(struct safety *s, int64_t now_ms);
void safety_on_command(struct safety *s, struct drive_cmd cmd, int64_t now_ms);
/* False, and the envelope unchanged, if the new one is invalid. */
bool safety_on_envelope(struct safety *s, const struct safety_envelope *env);

/*
 * What the car should do now. controllers_stopped is the any-corner stop
 * (SYS-25, latched by controller_links); controllers_ready is false while a
 * corner hasn't reported or a protocol version differs.
 * Order: controller stop, heartbeat lost (both brake), then the refusals.
 */
struct safety_output safety_decide(struct safety *s, int64_t now_ms, bool controllers_stopped,
				   bool controllers_ready);

#endif /* ARC_SAFETY_H */
