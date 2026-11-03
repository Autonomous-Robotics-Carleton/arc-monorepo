# The sync MCU's safety state, on every change and at 50 Hz. Mirrors
# ARC_SAFETY_STATE; the constants have the sync link's values.

# Heartbeat watchdog (ARC_WATCHDOG_STATE; SYS-04, ADR-0052)
uint8 WATCHDOG_OK=0           # heartbeats arriving and armed: the car may drive
uint8 WATCHDOG_NOT_ARMED=1    # heartbeats arriving, waiting for the operator's re-arm
uint8 WATCHDOG_NO_HEARTBEAT=2 # no heartbeats: braking, or held stopped

# The fault latch (ARC_LATCH_STATE; SYS-25, ADR-0039, ADR-0053)
uint8 LATCH_LATCHED=0          # stopped: waiting for the e-stop button on the car
uint8 LATCH_ESTOP_HELD=1       # e-stop held: waiting for the motor-bus cut at T
uint8 LATCH_CHECKING=2         # released after T: power-on checks running
uint8 LATCH_RESET=3            # the car may drive once the operator re-arms
uint8 LATCH_RESTART_REQUIRED=4 # only a power cycle and an inspection clear it

# Why it latched (ARC_LATCH_CAUSE); the detail is in /car/faults
uint8 CAUSE_NONE=0
uint8 CAUSE_POWER_ON=1
uint8 CAUSE_ESTOP=2
uint8 CAUSE_CORNER=3
uint8 CAUSE_CHECKS_FAILED=4
uint8 CAUSE_TOO_MANY_STOPS=5
uint8 CAUSE_OVERCURRENT=6
uint8 CAUSE_OVERVOLTAGE=7
uint8 CAUSE_ESTOP_WIRING=8
uint8 CAUSE_COMMAND_BUS=9
uint8 CAUSE_STEERING=10
uint8 CAUSE_STEERING_DISAGREEMENT=11

std_msgs/Header header
Envelope envelope # in force
uint8 watchdog    # WATCHDOG_*
bool estop        # the physical e-stop is active
uint8 latch       # LATCH_*
uint8 latch_cause # CAUSE_*; CAUSE_NONE once reset
