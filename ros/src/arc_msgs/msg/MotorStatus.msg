# One motor controller's status (ADR-0034). Mirrors ARC_MOTOR_STATUS; the
# topic says which corner (ICD-ros2-msgs). header.stamp is the sample time,
# mapped from the controller's clock to the sync MCU's.

# Why the controller is braking on its own (ARC_MOTOR_STATE, ADR-0035). Any
# bit set stops all four corners (SYS-25)
uint8 STATE_ESTOP=1
uint8 STATE_COMMAND_TIMEOUT=2

std_msgs/Header header
int32 erpm         # electrical rpm, the controller's own unit; wheel speed in
                   # m/s comes as its own field with the car configuration (#143)
int32 tachometer   # commutation steps since start
float32 current_q  # A, motor q-axis (torque)
float32 current_d  # A, motor d-axis
float32 current_in # A, battery side
float32 duty       # -1 to 1
float32 voltage_in # V, motor bus at the controller
float32 temp_fet   # degC
float32 temp_motor # degC
uint8 fault        # VESC fault code; 0 is none
uint8 state        # STATE_* bits; 0 is neither
