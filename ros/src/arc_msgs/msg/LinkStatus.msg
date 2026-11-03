# The sync link's health, both ways, at 1 Hz. The peer_* fields are what the
# sync MCU reports in ARC_LINK_STATUS; the rest are the car backend's own.

std_msgs/Header header
uint16 peer_protocol_version  # the sync MCU's sync-link.xml <version>
uint16 local_protocol_version # the car backend's; a mismatch stops the car
uint32 peer_rx_gaps # packets the sync MCU found missing since its start
uint32 peer_rx_bad  # packets the sync MCU rejected since its start
uint32 rx_gaps      # packets the car backend found missing since its start
uint32 rx_bad       # packets the car backend rejected since its start
