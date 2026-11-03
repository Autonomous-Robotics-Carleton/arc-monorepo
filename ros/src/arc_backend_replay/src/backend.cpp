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
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>
#include <rosbag2_cpp/reader.hpp>
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

// Every one of them by name, to say which a log doesn't have
// (test_topics_contract.py keeps this list and the ICD the same)
const std::vector<std::string> kCarTopicNames = {
  "/sensors/imu/cg", "/sensors/imu/front",
  "/sensors/angle/wheel_fl", "/sensors/angle/wheel_fr",
  "/sensors/angle/wheel_rl", "/sensors/angle/wheel_rr",
  "/sensors/angle/suspension_fl", "/sensors/angle/suspension_fr",
  "/sensors/angle/suspension_rl", "/sensors/angle/suspension_rr",
  "/sensors/angle/knuckle_fl", "/sensors/angle/knuckle_fr",
  "/sensors/ride_height/fl", "/sensors/ride_height/fr",
  "/sensors/ride_height/rl", "/sensors/ride_height/rr",
  "/sensors/ride_height/ground_camera",
  "/sensors/camera/triggers",
  "/car/motor/fl", "/car/motor/fr", "/car/motor/rl", "/car/motor/rr",
  "/car/steering", "/car/power",
  "/sensors/lidar/scan",
  "/sensors/camera/stereo/left/image_raw", "/sensors/camera/stereo/right/image_raw",
  "/sensors/camera/stereo/left/camera_info", "/sensors/camera/stereo/right/camera_info",
  "/sensors/camera/ground/image_raw", "/sensors/camera/ground/camera_info",
  "/sensors/ground_velocity",
  "/car/safety_state", "/car/faults", "/car/link", "/diagnostics",
};

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

// The car's topics with no messages in the log, in kCarTopicNames' order:
// nothing is published on them. A log can leave topics out on purpose (camera
// images in a log recorded without them, #152; log_exclude:=), or a sensor was down.
std::vector<std::string> missing_topics(const rosbag2_storage::StorageOptions & storage)
{
  rosbag2_cpp::Reader reader;
  reader.open(storage);
  std::set<std::string> logged;
  for (const auto & topic : reader.get_metadata().topics_with_message_count) {
    if (topic.message_count > 0) {
      logged.insert(topic.topic_metadata.name);
    }
  }
  std::vector<std::string> missing;
  for (const auto & name : kCarTopicNames) {
    if (logged.count(name) == 0) {
      missing.push_back(name);
    }
  }
  return missing;
}

// The player for a log, paused or playing, and the car's topics the log
// doesn't have; throws if it can't be read
std::shared_ptr<rosbag2_transport::Player> open_log(
  const std::string & bag, double rate, bool start_paused, double clock_frequency,
  std::vector<std::string> & missing)
{
  if (!(rate > 0) || !(clock_frequency > 0)) {
    throw std::invalid_argument("rate and clock_frequency must be positive");
  }
  if (!std::filesystem::exists(bag)) {
    throw std::invalid_argument("no log at " + bag);
  }
  rosbag2_storage::StorageOptions storage;
  storage.uri = bag;
  storage.storage_id = "mcap";
  missing = missing_topics(storage);
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
  // The log to play, a .mcap file or a bag directory; not `log`, which is
  // arc_logging's switch for recording the run (#152)
  const auto bag = backend->declare_parameter<std::string>("bag", "");
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

  if (bag.empty()) {
    RCLCPP_WARN(logger, "replay backend: no log given (bag:=<.mcap file or bag directory>), "
      "publishing nothing");
  } else {
    std::vector<std::string> missing;
    try {
      player = open_log(bag, rate, start_paused, clock_frequency, missing);
    } catch (const std::exception & e) {
      RCLCPP_FATAL(logger, "replay backend: can't play %s: %s", bag.c_str(), e.what());
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
    if (!missing.empty()) {
      std::string names;
      for (const auto & name : missing) {
        names += (names.empty() ? "" : ", ") + name;
      }
      RCLCPP_WARN(logger, "replay backend: %zu of the car's %zu topics aren't in this log, "
        "so nothing is published on them: %s", missing.size(), kCarTopicNames.size(),
        names.c_str());
    }
    RCLCPP_INFO(logger, "replay backend: playing %s at %.2gx%s", bag.c_str(), rate,
      start_paused ? ", paused" : "");
  }

  executor.spin();
  if (player) {
    player->stop();
  }
  rclcpp::shutdown();
  return 0;
}
