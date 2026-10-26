/*
 * The decision logic of the motor-controller links (ICD-controller-telemetry,
 * SYS-25, ADR-0035), with no I/O: it takes bytes and a time and answers.
 * That keeps it testable anywhere, in virtual time, with the exact sequences
 * that matter (a corner silent for 5 ms, a frame with a bad CRC).
 *
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_CONTROLLER_LINK_CORE_H
#define ARC_CONTROLLER_LINK_CORE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "arc_sync_link/mavlink.h"

#define CL_CORNERS 4 /* FL, FR, RL, RR: ARC_CORNER 0..3 */

/* Why the car should stop, or not yet drive. */
enum cl_reason {
	CL_OK = 0,
	CL_NOT_READY,      /* a corner hasn't reported yet: drive is refused, no stop */
	CL_VERSION,        /* a controller's protocol version differs: drive is refused */
	CL_SILENT,         /* no valid ARC_MOTOR_STATUS within the window (SYS-25) */
	CL_FAULT,          /* the controller reports a non-zero fault code */
	CL_ESTOP,          /* the controller is braking on its e-stop input */
	CL_COMMAND_TIMEOUT, /* the controller is braking for lack of commands */
	CL_UNKNOWN_STATE    /* a state bit this firmware doesn't know: ICD says any bit stops */
};

struct cl_corner {
	mavlink_message_t msg; /* parser state */
	mavlink_status_t status;
	bool seen;
	int64_t last_valid_ns; /* sync-MCU time the last valid status completed */
	uint8_t fault;
	uint8_t state; /* ARC_MOTOR_STATE bitmask */
	int32_t erpm;  /* electrical speed, from the last valid status */
	bool version_known;
	uint16_t peer_version;
	bool seq_known;
	uint8_t last_seq;
	uint32_t frames; /* valid frames of any message */
	uint32_t gaps;   /* frames missing, from the sequence numbers */
	uint32_t bad;    /* frames rejected: CRC, unknown message, wrong corner */
};

struct cl_links {
	struct cl_corner c[CL_CORNERS];
	int64_t silence_ns; /* longest a corner may go without a valid status */
	uint16_t version;   /* the protocol version this firmware was built from */
};

void cl_init(struct cl_links *l, int64_t silence_ns, uint16_t version);

/* Bytes read from a corner's UART at now_ns (sync-MCU time). */
void cl_feed(struct cl_links *l, int corner, const uint8_t *buf, size_t len, int64_t now_ns);

/*
 * The condition at now_ns. For a stop reason (CL_SILENT, CL_FAULT, CL_ESTOP,
 * CL_COMMAND_TIMEOUT) *corner is the first corner affected, lowest first.
 * A corner is silent when more than silence_ns has passed since its last
 * valid ARC_MOTOR_STATUS. A corner that has never reported isn't silent, it
 * isn't ready: that is CL_NOT_READY, which refuses drive without stopping.
 */
enum cl_reason cl_check(const struct cl_links *l, int64_t now_ns, int *corner);

/* Every corner has reported and its last |eRPM| is at most max_erpm: the
 * wheels are stopped (ADR-0039's power-on checks). Freshness is cl_check's. */
bool cl_all_stopped(const struct cl_links *l, int32_t max_erpm);

const char *cl_reason_name(enum cl_reason r);
bool cl_is_stop(enum cl_reason r);

#endif /* ARC_CONTROLLER_LINK_CORE_H */
