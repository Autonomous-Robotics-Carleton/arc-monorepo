# The safety envelope's limits (SYS-22). Mirrors ARC_ENVELOPE_SET when sent
# to the car; inside SafetyState, the limits in force.

float32 max_speed # m/s; 3 m/s by default for a new experiment (SYS-22)
float32 max_accel # m/s^2
float32 max_steer # rad
