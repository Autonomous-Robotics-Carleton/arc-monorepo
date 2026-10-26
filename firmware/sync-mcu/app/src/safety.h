/*
 * Safety envelope, heartbeat watchdog and the drive decision (SYS-04, SYS-22,
 * SYS-25, ADR-0039, ADR-0052). Pure logic: times are passed in (milliseconds), so the
 * tests run it in virtual time. The Zephyr task around it is safety_task.c.
 *
 * Fail-safe by construction: nothing moves the car unless heartbeats are
 * arriving, the operator has re-armed, the controllers are ready and not
 * stopped, and a fresh, valid command has arrived; and then only inside the
 * envelope.
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

/* Limits no command may exceed, set per session (SYS-22). Each bounds a
 * magnitude: reverse has the same speed limit as forward (ADR-0052). The same
 * struct holds the car's physical maximums, the cap for raising it. */
struct safety_envelope {
	float max_speed_mps;
	float max_accel_mps2;
	float max_steer_rad;
};

/* The envelope a new experiment starts with (ADR-0052). */
#define SAFETY_DEFAULT_SPEED_MPS 3.0f  /* SYS-22 */
#define SAFETY_DEFAULT_ACCEL_MPS2 5.0f /* TBC: the brake ramp's deceleration (ADR-0012) */

/* 3 m/s, 5 m/s² and the car's full steering range, each no higher than the
 * car's maximum. */
void safety_envelope_default(struct safety_envelope *env, const struct safety_envelope *car);

/* True if every limit is a finite number >= 0. A bad envelope is refused, never clamped. */
bool safety_envelope_valid(const struct safety_envelope *env);

/* True if no limit is above the car's (SYS-22: raisable to the physical maximum, no further). */
bool safety_envelope_within(const struct safety_envelope *env, const struct safety_envelope *car);

/* True if every field is finite and the acceleration limit isn't negative. */
bool safety_command_valid(const struct drive_cmd *cmd);

/*
 * The command limited to the envelope (SYS-22): speed and steering to
 * +-limit, acceleration to 0..limit. A command with a NaN or infinite field
 * becomes a zero command.
 */
struct drive_cmd safety_clamp(const struct safety_envelope *env, struct drive_cmd cmd);

/*
 * Operator heartbeat watchdog (SYS-04, ADR-0052).
 *
 * The car drives only while armed: the operator re-armed (ARC_REARM) while
 * heartbeats were arriving. It boots unarmed (ADR-0039). No heartbeat for
 * timeout_ms while armed trips it: the car brakes and holds, and a returning
 * heartbeat doesn't restart it; only a re-arm, once heartbeats are arriving
 * again, does. A clock that goes backwards counts as a timeout.
 *
 * A clock that stops can't be seen from here: a hung sync MCU is caught by
 * the controllers' own command timeout (ADR-0035).
 */
struct safety_watchdog {
	int64_t last_heartbeat_ms;
	int64_t timeout_ms;
	bool heard;   /* heartbeats are arriving: one since boot or the last timeout, within timeout_ms */
	bool armed;   /* the operator re-armed while heard */
	bool tripped; /* timed out while armed: brake until re-armed */
};

void safety_watchdog_init(struct safety_watchdog *wd, int64_t timeout_ms, int64_t now_ms);
void safety_watchdog_feed(struct safety_watchdog *wd, int64_t now_ms);

/* True unless armed with a heartbeat younger than timeout_ms (SYS-04: <= 150 ms,
 * so exactly timeout_ms has expired). Trips the watchdog on a timeout. */
bool safety_watchdog_expired(struct safety_watchdog *wd, int64_t now_ms);

/* The operator's re-arm. Accepted only while heartbeats are arriving. Faults
 * (SYS-25) have their own latch and reset (ADR-0039), in controller_links. */
bool safety_watchdog_rearm(struct safety_watchdog *wd, int64_t now_ms);

/* Why the output is what it is. */
enum safety_reason {
	SAFETY_DRIVING = 0,       /* the command, inside the envelope */
	SAFETY_NOT_ARMED,         /* not re-armed since boot: zero command */
	SAFETY_NO_COMMAND,        /* no fresh command: zero command */
	SAFETY_CONTROLLERS_NOT_READY, /* a corner hasn't reported, or a version mismatch: zero command */
	SAFETY_HEARTBEAT_LOST,    /* brake, and hold until re-armed (SYS-04, ADR-0052) */
	SAFETY_COMMAND_REJECTED,  /* brake: the command isn't a number, or its acceleration is negative */
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
	struct safety_envelope car; /* the car's maximums */
	struct safety_envelope envelope;
	struct safety_watchdog watchdog;
	struct drive_cmd command;
	int64_t command_ms; /* when it arrived */
	bool have_command;
	int64_t command_timeout_ms;
};

void safety_init(struct safety *s, const struct safety_envelope *car, int64_t heartbeat_timeout_ms,
		 int64_t command_timeout_ms, int64_t now_ms);

/* Messages from the Orin, with the arrival time. */
void safety_on_heartbeat(struct safety *s, int64_t now_ms);
void safety_on_command(struct safety *s, struct drive_cmd cmd, int64_t now_ms);
/* False, and the envelope unchanged, if the new one is invalid or above the car's maximum. */
bool safety_on_envelope(struct safety *s, const struct safety_envelope *env);
/* The operator's re-arm (ARC_REARM). False if refused: no heartbeat arriving. A command
 * that arrived before it is dropped, so only commands sent after the re-arm drive. */
bool safety_on_rearm(struct safety *s, int64_t now_ms);

/*
 * What the car should do now. controllers_stopped is the any-corner stop
 * (SYS-25, latched by controller_links); controllers_ready is false while a
 * corner hasn't reported or a protocol version differs.
 * Order: controller stop, heartbeat lost, a rejected command (all brake), then
 * the refusals.
 */
struct safety_output safety_decide(struct safety *s, int64_t now_ms, bool controllers_stopped,
				   bool controllers_ready);

#endif /* ARC_SAFETY_H */
