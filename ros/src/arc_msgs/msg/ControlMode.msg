# Who drives: arc_control's mode (#151). The operator's teleop always wins
# while it's fresh.

uint8 MODE_OPERATOR=0
uint8 MODE_EXPERIMENT=1

std_msgs/Header header
uint8 mode # MODE_*
