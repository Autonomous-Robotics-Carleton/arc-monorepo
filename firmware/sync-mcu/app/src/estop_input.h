/*
 * The e-stop status input (E-32 through the power section's stop circuit,
 * ICD-power-sync-stack ESTOP_STATUS): whether the button on the car is
 * pressed. The pin, level and polarity are TBD (#95); devicetree gives the
 * pin as the zephyr,user property estop-gpios, active when pressed.
 *
 * Fail-safe: a board without the property reads as pressed, so the car can't
 * be reset there (fault_latch.h). On native_sim it is an emulated pin that
 * the simulator drives (sim_estop.c).
 */

#ifndef ARC_ESTOP_INPUT_H
#define ARC_ESTOP_INPUT_H

#include <stdbool.h>

int estop_input_init(void);
bool estop_input_pressed(void);

#endif /* ARC_ESTOP_INPUT_H */
