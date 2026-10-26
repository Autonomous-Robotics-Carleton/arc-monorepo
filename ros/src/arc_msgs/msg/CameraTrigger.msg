# A camera trigger pulse, for stamping frames with their trigger time
# (ADR-0021). Mirrors ARC_CAMERA_TRIGGER. header.stamp is the trigger time.

std_msgs/Header header
uint32 frame  # frame sequence number for this camera
uint8 camera  # trigger output index
