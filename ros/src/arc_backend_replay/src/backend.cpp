// Log-replay backend of the car interface (ADR-0028): plays a rosbag2 MCAP log
// back as if it were the car, with the log's own timing, or faster or slower.
//
// Only what the car itself publishes is played (ICD-ros2-msgs, "Measurements"
// and "The car's state"): the platform's own topics in the log, commands
// included, come from the live platform nodes instead. /clock comes from the
// player, on the log's time (ICD-ros2-msgs, "Time and transforms").
// A log can't be driven: commands sent to the car are received, logged and
// otherwise ignored (#86, open question).

#include <chrono>
#include <filesystem>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rosbag2_storage/storage_options.hpp>
#include <rosbag2_transport/play_options.hpp>
#include <rosbag2_transport/player.hpp>

#include "arc_msgs/msg/drive_command.hpp"
#include "arc_msgs/msg/envelope.hpp"
#include "arc_msgs/msg/heartbeat.hpp"
#include "arc_msgs/msg/rearm.hpp"

namespace
{
using namespace std::chrono_literals;

// The topics the car publishes (ICD-ros2-msgs, Side A): the sensors, the
// controllers, power, and the car's state. Not /clock, /tf, the commands, or
// the platform's /control and /estimate topics.
constexpr char kCarTopics[] =
  "^/(sensors/.+|car/(motor/.+|steering|power|safety_state|faults|link)|diagnostics)$";

// QoS profiles (ICD ros2-msgs)
const rclcpp::QoS kCommands = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().durability_volatile();
const rclcpp::QoS kState = rclcpp::QoS(rclcpp::KeepLast(1)).reliable().transient_local();
}  // namespace

// Listens on the command topics, as the car backend does, and drops what arrives
class CommandSink : public rclcpp::Node
{
public:
  CommandSink()
  : Node("replay_backend")
  {
    listen<arc_msgs::msg::DriveCommand>("car/cmd/drive", kCommands);
    listen<arc_msgs::msg::Heartbeat>("car/cmd/heartbeat", kCommands);
    listen<arc_msgs::msg::Rearm>("car/cmd/rearm", kCommands);
    listen<arc_msgs::msg::Envelope>("car/cmd/envelope", kState);
  }

  ~CommandSink() override
  {
    for (const auto & [topic, count] : ignored_) {
      RCLCPP_INFO(get_logger(), "replay backend: ignored %lu on %s", count, topic.c_str());
    }
  }

private:
  template<typename Msg>
  void listen(const std::string & topic, const rclcpp::QoS & qos)
  {
    subs_.push_back(create_subscription<Msg>(topic, qos,
      [this, topic](const Msg &) {
        if (ignored_[topic]++ == 0) {
          RCLCPP_INFO(get_logger(), "replay backend: ignoring %s: a log can't be commanded",
            topic.c_str());
        }
      }));
  }

  std::map<std::string, unsigned long> ignored_;  // commands received, by topic
  std::vector<rclcpp::SubscriptionBase::SharedPtr> subs_;
};

// The player for a log, paused or playing; throws if it can't be read
std::shared_ptr<rosbag2_transport::Player> open_log(
  const std::string & log, double rate, bool start_paused, double clock_frequency)
{
  if (!(rate > 0) || !(clock_frequency > 0)) {
    throw std::invalid_argument("rate and clock_frequency must be positive");
  }
  if (!std::filesystem::exists(log)) {
    throw std::invalid_argument("no log at " + log);
  }
  rosbag2_storage::StorageOptions storage;
  storage.uri = log;
  storage.storage_id = "mcap";
  rosbag2_transport::PlayOptions play;
  play.rate = static_cast<float>(rate);
  play.regex_to_filter = kCarTopics;
  play.clock_publish_frequency = clock_frequency;
  play.start_paused = start_paused;
  play.disable_keyboard_controls = true;  // run from a launch file, with no terminal
  auto player = std::make_shared<rosbag2_transport::Player>(storage, play, "replay_player");
  player->play();
  return player;
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  auto backend = std::make_shared<CommandSink>();
  const auto log = backend->declare_parameter<std::string>("log", "");  // .mcap file or bag directory
  const double rate = backend->declare_parameter("rate", 1.0);  // 2.0: twice as fast
  // Waits for /replay_player/resume, so subscribers can join before the first message
  const bool start_paused = backend->declare_parameter("start_paused", false);
  // Hz; rosbag2's own default for `ros2 bag play --clock`
  const double clock_frequency = backend->declare_parameter("clock_frequency", 40.0);
  auto logger = backend->get_logger();

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(backend);
  std::shared_ptr<rosbag2_transport::Player> player;
  rclcpp::TimerBase::SharedPtr end_timer;

  if (log.empty()) {
    RCLCPP_WARN(logger, "replay backend: no log given (log:=<.mcap file or bag directory>), "
      "publishing nothing");
  } else {
    try {
      player = open_log(log, rate, start_paused, clock_frequency);
    } catch (const std::exception & e) {
      RCLCPP_FATAL(logger, "replay backend: can't play %s: %s", log.c_str(), e.what());
      rclcpp::shutdown();
      return 1;
    }
    executor.add_node(player);
    // At the end, /clock stops too, as when `ros2 bag play --clock` exits:
    // the player's clock would otherwise run on past the log
    end_timer = backend->create_wall_timer(100ms, [&executor, &player, &end_timer, logger] {
      if (player->wait_for_playback_to_finish(0s)) {
        executor.remove_node(player);
        RCLCPP_INFO(logger, "replay backend: end of log");
        end_timer->cancel();
      }
    });
    RCLCPP_INFO(logger, "replay backend: playing %s at %.2gx%s", log.c_str(), rate,
      start_paused ? ", paused" : "");
  }

  executor.spin();
  if (player) {
    player->stop();
  }
  rclcpp::shutdown();
  return 0;
}
