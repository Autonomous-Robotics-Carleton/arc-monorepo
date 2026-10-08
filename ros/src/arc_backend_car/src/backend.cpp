// The car backend: the bridge to the sync MCU over the sync link
// (ICD-sync-link, MAVLink 2, ADR-0031; ADR-0028, ADR-0024).
// So far it only exchanges ARC_LINK_STATUS once a second and logs the sync
// MCU's protocol version; the car interface itself isn't implemented yet.

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <cstdint>

#include <rclcpp/rclcpp.hpp>

#include "arc_sync_link/mavlink.h"

namespace
{
constexpr uint8_t kSysId = 1;   // one car
constexpr uint8_t kCompId = 2;  // the Orin's bridge; the sync MCU is 1
}

class CarBackend : public rclcpp::Node
{
public:
  CarBackend()
  : Node("car_backend")
  {
    // UDP ports and the sync MCU's address: ICD-sync-link (TBC)
    const int port_rx = declare_parameter("port_rx", 52001);
    const int port_tx = declare_parameter("port_tx", 52000);
    const std::string peer = declare_parameter("sync_mcu_address", "127.0.0.1");

    RCLCPP_INFO(get_logger(), "car backend: not implemented yet");

    sock_ = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, 0);
    sockaddr_in local{};
    local.sin_family = AF_INET;
    local.sin_port = htons(port_rx);
    local.sin_addr.s_addr = htonl(INADDR_ANY);
    if (sock_ < 0 || bind(sock_, reinterpret_cast<sockaddr *>(&local), sizeof(local)) < 0) {
      RCLCPP_ERROR(get_logger(), "sync link: can't bind UDP %d", port_rx);
      return;
    }
    peer_.sin_family = AF_INET;
    peer_.sin_port = htons(port_tx);
    inet_pton(AF_INET, peer.c_str(), &peer_.sin_addr);
    RCLCPP_INFO(get_logger(), "sync link: listening on UDP %d, sending to %s:%d",
      port_rx, peer.c_str(), port_tx);

    using namespace std::chrono_literals;
    status_timer_ = create_wall_timer(1s, [this] {send_link_status();});
    poll_timer_ = create_wall_timer(10ms, [this] {poll();});
  }

  ~CarBackend() override
  {
    if (sock_ >= 0) {
      close(sock_);
    }
  }

private:
  void send_link_status()
  {
    mavlink_message_t msg{};
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    // time_ns is sync-MCU time; the Orin's clock isn't synced to it yet (ADR-0021)
    mavlink_msg_arc_link_status_pack(kSysId, kCompId, &msg, 0, MAVLINK_VERSION,
      status_.packet_rx_drop_count, status_.parse_error);
    const uint16_t len = mavlink_msg_to_send_buffer(buf, &msg);
    sendto(sock_, buf, len, 0, reinterpret_cast<sockaddr *>(&peer_), sizeof(peer_));
  }

  void poll()
  {
    uint8_t buf[MAVLINK_MAX_PACKET_LEN];
    ssize_t n;
    while ((n = recv(sock_, buf, sizeof(buf), 0)) > 0) {
      for (ssize_t i = 0; i < n; i++) {
        mavlink_message_t msg{};
        if (mavlink_parse_char(MAVLINK_COMM_0, buf[i], &msg, &status_) &&
          msg.msgid == MAVLINK_MSG_ID_ARC_LINK_STATUS && !peer_seen_)
        {
          RCLCPP_INFO(get_logger(), "sync link up: peer protocol v%u",
            mavlink_msg_arc_link_status_get_protocol_version(&msg));
          peer_seen_ = true;
        }
      }
    }
  }

  int sock_{-1};
  sockaddr_in peer_{};
  mavlink_status_t status_{};
  bool peer_seen_{false};
  rclcpp::TimerBase::SharedPtr status_timer_;
  rclcpp::TimerBase::SharedPtr poll_timer_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CarBackend>());
  rclcpp::shutdown();
  return 0;
}
