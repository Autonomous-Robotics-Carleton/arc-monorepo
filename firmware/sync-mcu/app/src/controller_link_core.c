#include <string.h>

#include "controller_link_core.h"

void cl_init(struct cl_links *l, int64_t silence_ns, uint16_t version)
{
	memset(l, 0, sizeof(*l));
	l->silence_ns = silence_ns;
	l->version = version;
}

static void count_sequence(struct cl_corner *c, uint8_t seq)
{
	if (c->seq_known) {
		c->gaps += (uint8_t)(seq - c->last_seq - 1);
	}
	c->seq_known = true;
	c->last_seq = seq;
	c->frames++;
}

static void handle(struct cl_links *l, int corner, const mavlink_message_t *m, int64_t now_ns)
{
	struct cl_corner *c = &l->c[corner];

	switch (m->msgid) {
	case MAVLINK_MSG_ID_ARC_MOTOR_STATUS: {
		mavlink_arc_motor_status_t s;

		mavlink_msg_arc_motor_status_decode(m, &s);
		if (s.corner != corner) {
			/* a miswired or crossed UART: not this corner's data */
			c->bad++;
			return;
		}
		count_sequence(c, m->seq);
		c->seen = true;
		c->last_valid_ns = now_ns;
		c->fault = s.fault;
		c->state = s.state;
		break;
	}
	case MAVLINK_MSG_ID_ARC_LINK_STATUS: {
		mavlink_arc_link_status_t s;

		mavlink_msg_arc_link_status_decode(m, &s);
		count_sequence(c, m->seq);
		c->version_known = true;
		c->peer_version = s.protocol_version;
		break;
	}
	default:
		c->bad++; /* not a message this link carries */
		break;
	}
}

void cl_feed(struct cl_links *l, int corner, const uint8_t *buf, size_t len, int64_t now_ns)
{
	if (corner < 0 || corner >= CL_CORNERS) {
		return;
	}

	struct cl_corner *c = &l->c[corner];

	for (size_t i = 0; i < len; i++) {
		mavlink_message_t out;
		mavlink_status_t out_status;
		uint8_t r = mavlink_frame_char_buffer(&c->msg, &c->status, buf[i], &out, &out_status);

		if (r == MAVLINK_FRAMING_OK) {
			handle(l, corner, &out, now_ns);
		} else if (r != MAVLINK_FRAMING_INCOMPLETE) {
			c->bad++; /* bad CRC or bad signature */
		}
	}
}

static enum cl_reason stop_reason(const struct cl_links *l, const struct cl_corner *c,
				  int64_t now_ns)
{
	if (!c->seen) {
		return CL_OK; /* not ready is judged separately */
	}
	if (c->state & ARC_MOTOR_STATE_ESTOP) {
		return CL_ESTOP;
	}
	if (c->state & ARC_MOTOR_STATE_COMMAND_TIMEOUT) {
		return CL_COMMAND_TIMEOUT;
	}
	if (c->state != 0) {
		/* ICD-controller-telemetry: any state bit stops the car. A bit this
		 * firmware doesn't know (a newer controller) must not be ignored. */
		return CL_UNKNOWN_STATE;
	}
	if (c->fault != 0) {
		return CL_FAULT;
	}
	if (now_ns - c->last_valid_ns > l->silence_ns) {
		return CL_SILENT;
	}
	return CL_OK;
}

enum cl_reason cl_check(const struct cl_links *l, int64_t now_ns, int *corner)
{
	for (int i = 0; i < CL_CORNERS; i++) {
		enum cl_reason r = stop_reason(l, &l->c[i], now_ns);

		if (r != CL_OK) {
			if (corner) {
				*corner = i;
			}
			return r;
		}
	}
	for (int i = 0; i < CL_CORNERS; i++) {
		if (l->c[i].version_known && l->c[i].peer_version != l->version) {
			if (corner) {
				*corner = i;
			}
			return CL_VERSION;
		}
	}
	for (int i = 0; i < CL_CORNERS; i++) {
		if (!l->c[i].seen) {
			if (corner) {
				*corner = i;
			}
			return CL_NOT_READY;
		}
	}
	return CL_OK;
}

bool cl_is_stop(enum cl_reason r)
{
	return r == CL_SILENT || r == CL_FAULT || r == CL_ESTOP || r == CL_COMMAND_TIMEOUT ||
	       r == CL_UNKNOWN_STATE;
}

const char *cl_reason_name(enum cl_reason r)
{
	switch (r) {
	case CL_OK:
		return "ok";
	case CL_NOT_READY:
		return "not ready";
	case CL_VERSION:
		return "protocol version mismatch";
	case CL_SILENT:
		return "silent";
	case CL_FAULT:
		return "fault";
	case CL_ESTOP:
		return "e-stop";
	case CL_COMMAND_TIMEOUT:
		return "command timeout";
	case CL_UNKNOWN_STATE:
		return "unknown state bits";
	}
	return "?";
}
