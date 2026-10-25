/*
 * What goes on the command bus (ICD can-command, ADR-0009, ADR-0035), with no
 * I/O: from the safety decision, the four frames for the four controllers.
 *
 *   brake            SET_CURRENT_BRAKE to all four, at the configured current;
 *                    with none configured, nothing is sent and the controllers
 *                    brake on their own command timeout (ADR-0035): slower than
 *                    SYS-25's 20 ms
 *   driving          SET_RPM from the speed, at the configured eRPM per m/s;
 *                    with none configured it is 0 rpm
 *   anything else    SET_CURRENT 0 A: the bus is never silent while drive is
 *                    enabled, so the controllers' timeout trips only on real loss
 *
 * The brake current and the eRPM per m/s have no value in the spec (TBD).
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_CAN_COMMAND_CORE_H
#define ARC_CAN_COMMAND_CORE_H

#include <stdbool.h>
#include <stdint.h>

#include "safety.h"

#define CC_CORNERS 4

struct cc_config {
	float erpm_per_mps;  /* 0: not configured */
	float brake_current_a; /* 0: not configured */
};

struct cc_frame {
	uint32_t id; /* 29-bit extended ID */
	uint8_t len;
	uint8_t data[8];
};

enum cc_kind {
	CC_NONE = 0,      /* braking, but no brake current configured: send nothing */
	CC_ZERO_CURRENT,  /* SET_CURRENT 0 A */
	CC_RPM,           /* SET_RPM */
	CC_BRAKE          /* SET_CURRENT_BRAKE */
};

struct cc_plan {
	enum cc_kind kind;
	struct cc_frame frame[CC_CORNERS]; /* FL, FR, RL, RR; valid unless kind is CC_NONE */
};

struct cc_plan cc_make_plan(const struct cc_config *cfg, const struct safety_output *out);

/* The STATUS_1 ID of a corner (extended), and whether an ID is one of them. */
uint32_t cc_status_1_id(int corner);
/* Corner 0..3 if id is a STATUS_1 frame from a controller, else -1. */
int cc_status_1_corner(uint32_t id);

const char *cc_kind_name(enum cc_kind k);

#endif /* ARC_CAN_COMMAND_CORE_H */
