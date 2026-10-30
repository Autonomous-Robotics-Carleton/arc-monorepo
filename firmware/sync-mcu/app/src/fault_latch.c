#include "fault_latch.h"

void fl_init(struct fault_latch *fl, const struct fl_config *cfg, int64_t now_ms)
{
	*fl = (struct fault_latch){
		.cfg = *cfg,
		.state = FL_LATCHED,
		.cause = FL_CAUSE_BOOT, /* ADR-0039: it boots latched */
		.corner_reason = CL_OK,
		.corner = -1,
		.since_ms = now_ms,
	};
}

static void enter(struct fault_latch *fl, enum fl_state s, int64_t now_ms)
{
	fl->state = s;
	fl->since_ms = now_ms;
}

/* A cause that only a restart and an inspection clear, or FL_CAUSE_NONE. */
static enum fl_cause restart_cause(const struct fl_inputs *in)
{
	if (in->corners == CL_FAULT && in->fault_code == FL_VESC_FAULT_ABS_OVER_CURRENT) {
		return FL_CAUSE_OVERCURRENT;
	}
	if (in->corners == CL_FAULT && in->fault_code == FL_VESC_FAULT_OVER_VOLTAGE) {
		return FL_CAUSE_OVERVOLTAGE;
	}
	return FL_CAUSE_NONE;
}

static enum fl_event stop(struct fault_latch *fl, int64_t now_ms, const struct fl_inputs *in)
{
	enum fl_cause restart = restart_cause(in);

	fl->corner_reason = in->corners;
	fl->corner = in->corner;
	/* a corner reporting the e-stop isn't a fault: the button was pressed,
	 * or its wiring is checked at the reset */
	if (in->corners != CL_ESTOP) {
		fl->stops++;
	}
	if (restart == FL_CAUSE_NONE && fl->stops >= fl->cfg.max_stops) {
		restart = FL_CAUSE_TOO_MANY;
	}
	if (restart != FL_CAUSE_NONE) {
		fl->cause = restart;
		enter(fl, FL_RESTART, now_ms);
		return FL_EV_RESTART;
	}
	fl->cause = FL_CAUSE_CORNER;
	enter(fl, FL_LATCHED, now_ms);
	return FL_EV_LATCHED;
}

static enum fl_event checks_failed(struct fault_latch *fl, int64_t now_ms,
				   const struct fl_inputs *in)
{
	enum fl_cause restart = restart_cause(in);

	fl->corner_reason = in->corners;
	fl->corner = in->corner;
	fl->wheels_moving = !in->wheels_stopped;
	if (in->corners == CL_ESTOP) {
		restart = FL_CAUSE_ESTOP_WIRING; /* ADR-0035: the button is released */
	}
	if (restart != FL_CAUSE_NONE) {
		fl->cause = restart;
		enter(fl, FL_RESTART, now_ms);
		return FL_EV_RESTART;
	}
	fl->cause = FL_CAUSE_CHECKS;
	enter(fl, FL_LATCHED, now_ms);
	return FL_EV_CHECKS_FAILED;
}

enum fl_event fl_update(struct fault_latch *fl, int64_t now_ms, const struct fl_inputs *in)
{
	switch (fl->state) {
	case FL_CLEAR:
		if (in->estop_pressed) {
			fl->cause = FL_CAUSE_ESTOP;
			fl->corner_reason = CL_OK;
			fl->corner = -1;
			enter(fl, FL_LATCHED, now_ms);
			return FL_EV_LATCHED;
		}
		if (cl_is_stop(in->corners)) {
			return stop(fl, now_ms, in);
		}
		return FL_EV_NONE;

	case FL_LATCHED:
		if (in->estop_pressed) {
			enter(fl, FL_PRESSED, now_ms);
			return FL_EV_PRESSED;
		}
		return FL_EV_NONE;

	case FL_PRESSED:
		if (in->estop_pressed) {
			return FL_EV_NONE;
		}
		if (now_ms - fl->since_ms >= fl->cfg.cut_ms) {
			enter(fl, FL_CHECKING, now_ms);
			return FL_EV_CHECKING;
		}
		enter(fl, FL_LATCHED, now_ms);
		return FL_EV_RELEASED_EARLY;

	case FL_CHECKING:
		if (in->estop_pressed) {
			enter(fl, FL_PRESSED, now_ms);
			return FL_EV_PRESSED;
		}
		if (in->corners == CL_OK && in->wheels_stopped) {
			fl->cause = FL_CAUSE_NONE;
			fl->corner_reason = CL_OK;
			fl->corner = -1;
			enter(fl, FL_CLEAR, now_ms);
			return FL_EV_CLEARED;
		}
		if (now_ms - fl->since_ms >= fl->cfg.check_window_ms) {
			return checks_failed(fl, now_ms, in);
		}
		return FL_EV_NONE;

	case FL_RESTART:
		return FL_EV_NONE;
	}
	return FL_EV_NONE;
}

bool fl_clear(const struct fault_latch *fl)
{
	return fl->state == FL_CLEAR;
}

bool fl_brake(const struct fault_latch *fl)
{
	/* Not while checking: the car stopped before the press, and the controllers
	 * coming back must see a live bus, not a brake that may not be configured */
	switch (fl->state) {
	case FL_LATCHED:
	case FL_PRESSED:
		return fl->cause != FL_CAUSE_BOOT;
	case FL_RESTART:
		return true;
	case FL_CHECKING:
	case FL_CLEAR:
		return false;
	}
	return true;
}

const char *fl_state_name(enum fl_state s)
{
	switch (s) {
	case FL_LATCHED:
		return "latched";
	case FL_PRESSED:
		return "e-stop held";
	case FL_CHECKING:
		return "power-on checks";
	case FL_CLEAR:
		return "reset";
	case FL_RESTART:
		return "restart required";
	}
	return "?";
}

const char *fl_cause_name(enum fl_cause c)
{
	switch (c) {
	case FL_CAUSE_NONE:
		return "none";
	case FL_CAUSE_BOOT:
		return "power-on";
	case FL_CAUSE_ESTOP:
		return "e-stop pressed";
	case FL_CAUSE_CORNER:
		return "corner stop";
	case FL_CAUSE_CHECKS:
		return "power-on checks failed";
	case FL_CAUSE_TOO_MANY:
		return "too many stops since power-on";
	case FL_CAUSE_OVERCURRENT:
		return "controller over-current";
	case FL_CAUSE_OVERVOLTAGE:
		return "controller over-voltage";
	case FL_CAUSE_ESTOP_WIRING:
		return "e-stop wiring fault";
	}
	return "?";
}

const char *fl_event_name(enum fl_event e)
{
	switch (e) {
	case FL_EV_NONE:
		return "none";
	case FL_EV_LATCHED:
		return "latched";
	case FL_EV_PRESSED:
		return "e-stop pressed";
	case FL_EV_RELEASED_EARLY:
		return "e-stop released before the motor-bus cut";
	case FL_EV_CHECKING:
		return "e-stop released: power-on checks";
	case FL_EV_CLEARED:
		return "power-on checks passed: re-arm to drive";
	case FL_EV_CHECKS_FAILED:
		return "power-on checks failed";
	case FL_EV_RESTART:
		return "restart required";
	}
	return "?";
}
