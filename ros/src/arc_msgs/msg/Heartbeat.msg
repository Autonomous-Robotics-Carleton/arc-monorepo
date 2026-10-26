# The operator's heartbeat, forwarded by arc_safety to the car backend
# (SYS-04). Mirrors ARC_OPERATOR_HEARTBEAT. header.stamp is when arc_safety
# forwarded it.

std_msgs/Header header
uint32 counter # the laptop's heartbeat counter, passed through
