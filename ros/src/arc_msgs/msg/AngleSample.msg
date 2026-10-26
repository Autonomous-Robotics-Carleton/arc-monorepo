# One angle-sensor sample: wheel, suspension or steering knuckle (AS5047P).
# Mirrors ARC_ANGLE_SAMPLE; the topic and frame say which sensor (ICD-ros2-msgs).
# header.stamp is the sample time on the sync MCU's time base (SYS-07).

# Error flags (ARC_ANGLE_STATUS): 0 is valid; any bit means the angle can't
# be trusted (ADR-0038)
uint8 STATUS_MAGNET_LOW=1
uint8 STATUS_MAGNET_HIGH=2
uint8 STATUS_CORDIC_OVERFLOW=4
uint8 STATUS_READ_ERROR=8
uint8 STATUS_NOT_READY=16

std_msgs/Header header
float32 angle  # rad, absolute
uint8 status
