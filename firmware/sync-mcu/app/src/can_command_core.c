#include <math.h>
#include <string.h>

#include "can_command.h"
#include "can_command_core.h"

#define ERPM_LIMIT 200000.0f /* can-command.dbc range */
#define BRAKE_LIMIT_A 200.0f

/* One function per frame kind and corner: the generated code names them per corner. */
#define KIND_FUNCS(KIND, kind, field)                                                    \
	static void kind##_frame(int c, double v, struct cc_frame *f)                         \
	{                                                                                     \
		switch (c) {                                                                  \
		case 0: {                                                                     \
			struct can_command_##kind##_fl_t m = {.field = can_command_##kind##_fl_##field##_encode(v)}; \
			f->len = can_command_##kind##_fl_pack(f->data, &m, 8);               \
			f->id = CAN_COMMAND_##KIND##_FL_FRAME_ID;                             \
			break; }                                                              \
		case 1: {                                                                     \
			struct can_command_##kind##_fr_t m = {.field = can_command_##kind##_fr_##field##_encode(v)}; \
			f->len = can_command_##kind##_fr_pack(f->data, &m, 8);               \
			f->id = CAN_COMMAND_##KIND##_FR_FRAME_ID;                             \
			break; }                                                              \
		case 2: {                                                                     \
			struct can_command_##kind##_rl_t m = {.field = can_command_##kind##_rl_##field##_encode(v)}; \
			f->len = can_command_##kind##_rl_pack(f->data, &m, 8);               \
			f->id = CAN_COMMAND_##KIND##_RL_FRAME_ID;                             \
			break; }                                                              \
		default: {                                                                    \
			struct can_command_##kind##_rr_t m = {.field = can_command_##kind##_rr_##field##_encode(v)}; \
			f->len = can_command_##kind##_rr_pack(f->data, &m, 8);               \
			f->id = CAN_COMMAND_##KIND##_RR_FRAME_ID;                             \
			break; }                                                              \
		}                                                                             \
	}

KIND_FUNCS(SET_CURRENT, set_current, current)
KIND_FUNCS(SET_CURRENT_BRAKE, set_current_brake, current_brake)
KIND_FUNCS(SET_RPM, set_rpm, erpm)

static float limit(float v, float lo, float hi)
{
	return v < lo ? lo : v > hi ? hi : v;
}

struct cc_plan cc_make_plan(const struct cc_config *cfg, const struct safety_output *out)
{
	struct cc_plan plan = {0};

	if (out->brake) {
		if (!(cfg->brake_current_a > 0.0f)) {
			plan.kind = CC_NONE;
			return plan;
		}
		plan.kind = CC_BRAKE;
		for (int c = 0; c < CC_CORNERS; c++) {
			set_current_brake_frame(c, limit(cfg->brake_current_a, 0.0f, BRAKE_LIMIT_A),
						&plan.frame[c]);
		}
		return plan;
	}

	if (out->reason == SAFETY_DRIVING && cfg->erpm_per_mps > 0.0f) {
		plan.kind = CC_RPM;
		float erpm = limit(out->cmd.speed_mps * cfg->erpm_per_mps, -ERPM_LIMIT, ERPM_LIMIT);

		for (int c = 0; c < CC_CORNERS; c++) {
			set_rpm_frame(c, erpm, &plan.frame[c]);
		}
		return plan;
	}

	plan.kind = CC_ZERO_CURRENT;
	for (int c = 0; c < CC_CORNERS; c++) {
		set_current_frame(c, 0.0, &plan.frame[c]);
	}
	return plan;
}

uint32_t cc_status_1_id(int corner)
{
	static const uint32_t id[CC_CORNERS] = {
		CAN_COMMAND_STATUS_1_FL_FRAME_ID, CAN_COMMAND_STATUS_1_FR_FRAME_ID,
		CAN_COMMAND_STATUS_1_RL_FRAME_ID, CAN_COMMAND_STATUS_1_RR_FRAME_ID,
	};

	return corner >= 0 && corner < CC_CORNERS ? id[corner] : 0;
}

int cc_status_1_corner(uint32_t id)
{
	for (int c = 0; c < CC_CORNERS; c++) {
		if (id == cc_status_1_id(c)) {
			return c;
		}
	}
	return -1;
}

const char *cc_kind_name(enum cc_kind k)
{
	switch (k) {
	case CC_NONE:
		return "nothing (no brake current configured)";
	case CC_ZERO_CURRENT:
		return "zero current";
	case CC_RPM:
		return "rpm";
	case CC_BRAKE:
		return "brake";
	}
	return "?";
}
