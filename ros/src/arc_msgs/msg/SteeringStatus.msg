# The steering actuator's status (moteus-c1, ADR-0019). Mirrors
# ARC_STEERING_STATUS. header.stamp is the sample time on the sync MCU's
# time base.

std_msgs/Header header
float32 angle    # rad, steering output; + turns left
float32 velocity # rad/s
float32 torque   # N.m, estimate from q-axis current
float32 temp     # degC, controller
uint8 fault      # moteus fault code; 0 is none
uint8 mode       # moteus mode register, as in ARC_STEERING_STATUS
