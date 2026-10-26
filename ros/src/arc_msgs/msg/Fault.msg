# A fault or event, when detected (SYS-19). Mirrors ARC_FAULT; the car
# backend also reports its own (protocol mismatch, receive gaps) here.
# header.stamp is the detection time.

# Where it was detected (ARC_FAULT_SOURCE)
uint8 SOURCE_SYNC_MCU=0
uint8 SOURCE_MOTOR_CONTROLLER=1
uint8 SOURCE_STEERING=2
uint8 SOURCE_POWER=3
uint8 SOURCE_SENSOR=4
uint8 SOURCE_LINK=5

std_msgs/Header header
uint8 source  # SOURCE_*
uint16 code   # source-specific
uint32 detail # source-specific
