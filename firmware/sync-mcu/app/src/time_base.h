/*
 * The car's time base (ADR-0021): every sample is stamped in this clock,
 * and the Orin follows it in software. Today it reads Zephyr's uptime;
 * the real one runs from a hardware timer, with PPS to the Orin and the
 * GenX320 Trigger In pulses. Stub.
 */

#ifndef ARC_TIME_BASE_H
#define ARC_TIME_BASE_H

#include <stdint.h>

int time_base_init(void);

/* Time since boot, in nanoseconds. */
int64_t time_base_now_ns(void);

#endif /* ARC_TIME_BASE_H */
