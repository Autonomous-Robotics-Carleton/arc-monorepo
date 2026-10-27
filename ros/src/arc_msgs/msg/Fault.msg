# A fault or event, when detected (SYS-19). Mirrors ARC_FAULT; the car
# backend also reports its own here (SOURCE_CAR_BACKEND).
# header.stamp is the detection time.

# Where it was detected (ARC_FAULT_SOURCE)
uint8 SOURCE_SYNC_MCU=0
uint8 SOURCE_MOTOR_CONTROLLER=1
uint8 SOURCE_STEERING=2
uint8 SOURCE_POWER=3
uint8 SOURCE_SENSOR=4
uint8 SOURCE_LINK=5
# The car backend's own, above the sync link's range
uint8 SOURCE_CAR_BACKEND=128

# Codes for SOURCE_CAR_BACKEND
uint16 CODE_PROTOCOL_MISMATCH=1  # detail: the sync MCU's protocol version
uint16 CODE_RECEIVE_GAP=2        # detail: how many messages were missed
uint16 CODE_COMMAND_PUBLISHERS=3 # a command topic has the wrong publishers; detail: COMMAND_*

# The command topics, for CODE_COMMAND_PUBLISHERS
uint32 COMMAND_DRIVE=0
uint32 COMMAND_HEARTBEAT=1
uint32 COMMAND_REARM=2
uint32 COMMAND_ENVELOPE=3

std_msgs/Header header
uint8 source  # SOURCE_*
uint16 code   # source-specific
uint32 detail # source-specific
