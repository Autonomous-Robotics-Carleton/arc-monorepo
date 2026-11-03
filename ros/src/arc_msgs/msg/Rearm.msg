# The operator's re-arm, forwarded by arc_safety like the heartbeat
# (ADR-0039, ADR-0052). Mirrors ARC_REARM; the sync MCU accepts it only when
# the watchdog is WATCHDOG_NOT_ARMED and the latch LATCH_RESET.

std_msgs/Header header
uint32 counter # the laptop's re-arm counter, passed through
