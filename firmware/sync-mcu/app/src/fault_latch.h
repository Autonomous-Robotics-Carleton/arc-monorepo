/*
 * The fault-stop latch and its reset (SYS-25, ADR-0039). Pure logic: times
 * and inputs are passed in, so the tests run it in virtual time. The Zephyr
 * side is safety_task.c.
 *
 * A stop latches and the car stays stopped. It boots latched, so a power
 * cycle can't skip the reset. The reset: press the e-stop button on the car,
 * hold it at least until the motor-bus cut at T, release it, and the
 * power-on checks pass within the check window: every corner reporting with
 * no state bits or faults, every wheel stopped. Then the operator re-arms
 * (safety.h). Some causes need a full restart and an inspection instead.
 *
 * Not checked yet: the steering (no steering driver; ADR-0038). Decided
 * 2026-10-25 to skip it everywhere until the driver exists.
 *
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_FAULT_LATCH_H
#define ARC_FAULT_LATCH_H

#include <stdbool.h>
#include <stdint.h>

#include "controller_link_core.h"

struct fl_config {
	int64_t cut_ms;          /* T: the motor-bus cut after a press (ADR-0034, TBC 2.4 s) */
	int64_t check_window_ms; /* how long the power-on checks may take after release (TBC 3 s) */
	int max_stops;           /* this many stops since power-on need a restart (TBC 3) */
};

enum fl_state {
	FL_LATCHED,  /* stopped: waiting for the e-stop button */
	FL_PRESSED,  /* the button is held: waiting for T */
	FL_CHECKING, /* released after T: the power-on checks */
	FL_CLEAR,    /* reset: the car may drive once the operator re-arms */
	FL_RESTART,  /* only a power cycle and an inspection clear this (ADR-0039 item 5) */
};

enum fl_cause {
	FL_CAUSE_NONE = 0,
	FL_CAUSE_BOOT,         /* power-on */
	FL_CAUSE_ESTOP,        /* the e-stop button was pressed */
	FL_CAUSE_CORNER,       /* a SYS-25 stop: see corner_reason, corner */
	FL_CAUSE_CHECKS,       /* the power-on checks failed: see corner_reason, corner, wheels */
	FL_CAUSE_TOO_MANY,     /* max_stops reached */
	FL_CAUSE_OVERCURRENT,  /* a controller reported over-current */
	FL_CAUSE_OVERVOLTAGE,  /* a controller reported over-voltage */
	FL_CAUSE_ESTOP_WIRING, /* a corner reports e-stop with the button released */
};

/* What the latch looks at, every tick. */
struct fl_inputs {
	bool estop_pressed;      /* the sync MCU's e-stop status input */
	enum cl_reason corners;  /* cl_check() now */
	int corner;              /* the corner it names, or -1 */
	uint8_t fault_code;      /* that corner's VESC fault code, when corners is CL_FAULT */
	bool wheels_stopped;     /* every corner reporting |eRPM| at or below the threshold */
};

struct fault_latch {
	struct fl_config cfg;
	enum fl_state state;
	enum fl_cause cause;
	enum cl_reason corner_reason; /* for FL_CAUSE_CORNER and FL_CAUSE_CHECKS */
	int corner;
	bool wheels_moving;           /* for FL_CAUSE_CHECKS */
	int stops;                    /* SYS-25 stops since power-on */
	int64_t since_ms;             /* when the current state began */
};

/* What changed in this update, for the log. */
enum fl_event {
	FL_EV_NONE = 0,
	FL_EV_LATCHED,         /* a stop: state LATCHED (or RESTART), cause set */
	FL_EV_PRESSED,         /* the button went down */
	FL_EV_RELEASED_EARLY,  /* released before T: still latched */
	FL_EV_CHECKING,        /* released after T: checks started */
	FL_EV_CLEARED,         /* the checks passed */
	FL_EV_CHECKS_FAILED,   /* the window ran out: latched again, cause CHECKS */
	FL_EV_RESTART,         /* a cause that needs a restart */
};

/* VESC fault codes that need a restart (mc_fault_code in the VESC firmware's datatypes.h) */
#define FL_VESC_FAULT_OVER_VOLTAGE 1
#define FL_VESC_FAULT_ABS_OVER_CURRENT 4

void fl_init(struct fault_latch *fl, const struct fl_config *cfg, int64_t now_ms);

enum fl_event fl_update(struct fault_latch *fl, int64_t now_ms, const struct fl_inputs *in);

/* Reset and not stopped since: drive is allowed once the operator re-arms. */
bool fl_clear(const struct fault_latch *fl);

/* The car must brake: stopped for anything but power-on. */
bool fl_brake(const struct fault_latch *fl);

const char *fl_state_name(enum fl_state s);
const char *fl_cause_name(enum fl_cause c);
const char *fl_event_name(enum fl_event e);

#endif /* ARC_FAULT_LATCH_H */
