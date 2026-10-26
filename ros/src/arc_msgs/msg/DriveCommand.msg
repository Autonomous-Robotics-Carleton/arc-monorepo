# The vehicle command (ADR-0048): what an experiment or the operator asks
# for. arc_control picks one and sends it on as ARC_DRIVE_COMMAND; the sync
# MCU clamps it to the envelope (SYS-22) and splits it into wheel speeds.
# header.stamp is when it was computed (SYS-23).

std_msgs/Header header
float32 speed # m/s, + forward; reverse is allowed (ADR-0052)
float32 accel # m/s^2, the acceleration limit for reaching speed
float32 steer # rad, + turns left
