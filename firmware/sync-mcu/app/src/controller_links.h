/*
 * Telemetry from the four motor controllers: one UART each, point to
 * point, full status at 1 kHz stamped at sampling (ADR-0024, ADR-0034).
 * Message set: ICD-controller-telemetry (draft): MAVLink 2, ARC_MOTOR_STATUS
 * from systems/icd/sync-link.xml. The decisions are in controller_link_core.
 *
 * Today only native_sim has the UARTs (pseudo-terminals, boards/native_sim.overlay);
 * the other boards build a stub until their UARTs are assigned.
 * A safety function: changes need their own tests and a second reviewer.
 */

#ifndef ARC_CONTROLLER_LINKS_H
#define ARC_CONTROLLER_LINKS_H

#include "controller_link_core.h"

int controller_links_init(void);

/*
 * The any-corner stop (SYS-25): the first stop reason seen, held until
 * controller_links_reset(). CL_OK if the car hasn't been stopped. *corner is
 * the corner that caused it. The brake commands that follow belong to the
 * command bus (not implemented yet).
 */
enum cl_reason controller_links_stop(int *corner);

/*
 * The condition now. Drive must check it: CL_NOT_READY (a corner has never
 * reported, so it can't be judged silent) and CL_VERSION refuse drive but
 * don't stop the car, so a dead controller is only caught here.
 */
enum cl_reason controller_links_state(int *corner);

/* Clears the latch. Only the reset sequence may call it (ADR-0039, not written yet). */
void controller_links_reset(void);

#endif /* ARC_CONTROLLER_LINKS_H */
