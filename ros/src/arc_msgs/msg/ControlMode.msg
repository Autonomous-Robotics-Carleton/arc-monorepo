# Who drives: arc_control's mode (#151). Only a /control/set_mode request
# gives the experiment control; teleop past the deadband takes it back
# (ICD ros2-msgs, "Choosing who drives").

uint8 MODE_OPERATOR=0
uint8 MODE_EXPERIMENT=1

std_msgs/Header header
uint8 mode # MODE_*
