"""What the platform runs to log a run: whether it records and where, the
run's name and record, and the `ros2 bag record` command line.

The recorder is rosbag2's own, with its MCAP storage: a log is a standard
rosbag2 directory (`metadata.yaml` and one `.mcap` per chunk) that plain
rosbag2 reads, so the replay backend (#86) and `ros2 bag` need nothing of ours.

Nothing here knows the target: the platform launch (arc_bringup) decides
whether a run is logged by default and passes `log` and `must_log` down.

Plain Python, so the same code runs in the node and in the tests.
"""

from datetime import datetime, timezone
from pathlib import Path
import re
import shutil
from typing import NamedTuple, Optional

import rosbag2_py

STORAGE = 'mcap'

# Camera images, raw or encoded (image_transport's compressed, theora and ffmpeg
# transports): nearly all of a log's size (ADR-0056). New image types join here
# when they appear (v1.1's event cameras).
IMAGE_TYPES = (
    'sensor_msgs/msg/Image',
    'sensor_msgs/msg/CompressedImage',
    'theora_image_transport/msg/Packet',
    'ffmpeg_image_transport_msgs/msg/FFMPEGPacket',
)


class Profile(NamedTuple):
    """Which topics a run logs, and the free space it needs (ADR-0056)."""

    exclude_types: tuple  # message types left out of the log
    min_free_gb: float  # TBC: free space to start recording
    warn_free_gb: float  # TBC: free space below which a recording run warns
    about: str


PROFILES = {
    # ≤ 10 MB/s (TBC): 20 GB is a 10-minute run at that rate, x 3; 10 GB warns
    # ~16 minutes before the drive fills
    'minimal': Profile(IMAGE_TYPES, 20.0, 10.0, 'every topic except camera images'),
    # v1.0's ~296 MB/s: a 10-minute run, ~178 GB (total-electrical-cost.md), x 1.5;
    # 60 GB warns ~3 minutes before the drive fills
    'full': Profile((), 270.0, 60.0, 'every topic'),
}
# What a run logs when it doesn't have room for the profile it asked for
FALLBACK_PROFILE = 'minimal'


class Plan(NamedTuple):
    """What the logger does this run, and what it tells the operator."""

    output: Optional[Path]  # the run's directory, or None when not recording
    warn: bool  # whether the operator is warned (on /diagnostics and the console)
    message: str
    profile: str  # the profile recorded, or asked for when not recording


def choose_profile(name):
    """(profile, problem): `name` if it's a profile, else full and why.

    A name that isn't a profile records everything: more, never less, than
    was asked for.
    """
    if name in PROFILES:
        return name, None
    return 'full', (f"log_profile {name!r} isn't one of {', '.join(PROFILES)}: "
                    'every topic is recorded')


def log_root(log_dir, env_dir, must_log):
    """Where runs are logged: log_dir:=, else $ARC_LOG_DIR, else ~/arc_logs.

    A run that must be logged has no fallback: its directory is set
    explicitly (on the car the Orin's image points $ARC_LOG_DIR at the NVMe,
    #89), so None.
    """
    if log_dir or env_dir:
        return Path(log_dir or env_dir)
    return None if must_log else Path.home() / 'arc_logs'


def free_gb(path):
    """Free space where `path` is or would be created, in GB."""
    path = Path(path).absolute()
    while not path.exists():
        path = path.parent
    return shutil.disk_usage(path).free / 1e9


def plan(log, must_log, root, name, profile, min_free_gb, free=free_gb):
    """Whether this run is recorded, where, in which profile, and what the
    operator is told. `min_free_gb` is what `profile` needs to start.

    A run without room for its profile records the fallback profile if
    there's room for that, with a warning, rather than nothing. The platform
    runs either way: an unlogged run is never stopped, only reported. Logs
    are never deleted to make room.
    """
    if not log:
        if must_log:
            return Plan(None, True, 'NOT LOGGING: log:=false was passed, this run is not recorded',
                        profile)
        return Plan(None, False, 'not logging (log:=false)', profile)
    if root is None:
        return Plan(None, True,
                    'NOT LOGGING: $ARC_LOG_DIR is not set; set it to the log drive. '
                    'This run is not recorded', profile)
    space = free(root)
    if space < min_free_gb:
        low = (f'{space:.1f} GB free on {root}, below min_free_gb ({min_free_gb:g} GB) '
               f'for log_profile {profile}. Free space on the log drive (logs are never '
               'deleted automatically)')
        fallback = PROFILES[FALLBACK_PROFILE]
        if profile != FALLBACK_PROFILE and space >= fallback.min_free_gb:
            output = run_dir(root, name)
            return Plan(output, True,
                        f'NOT LOGGING {profile}: {low}. Logging {FALLBACK_PROFILE} instead '
                        f'({fallback.about}): recording to {output}', FALLBACK_PROFILE)
        return Plan(None, True, f'NOT LOGGING: {low}. This run is not recorded', profile)
    output = run_dir(root, name)
    return Plan(output, False, f'recording to {output}', profile)


def run_name(target, now=None):
    """`<UTC start time>_<target>`, e.g. `20261029T140501Z_car`: sorts by time.

    Without a target (arc_logging launched on its own) it's the time alone.
    """
    now = (now or datetime.now(timezone.utc)).astimezone(timezone.utc)
    stamp = now.strftime('%Y%m%dT%H%M%SZ')
    return '_'.join(filter(None, (stamp, target)))


def run_dir(root, name):
    """A directory for this run that doesn't exist yet: rosbag2 won't write into one that does."""
    root = Path(root)
    path = root / name
    n = 1
    while path.exists():
        path = root / f'{name}-{n}'
        n += 1
    return path


def run_record(target, sim, sensors, car, car_config_hash, log_exclude='', log_profile='',
               source=None):
    """The run record (#152 part 2): what a log needs to be read later.

    It's written as the log's custom data (metadata.yaml's `custom_data`, which
    rosbag2's MCAP storage also keeps in each .mcap file), so plain rosbag2
    reads it back. Every field is passed through as given; an empty one
    (`sim` off a simulator, `log_exclude` when nothing is left out) is left
    out. `log_profile` and `log_exclude` say what the log doesn't hold: the
    profile recorded (ADR-0056) and the pattern of topics left out. So far
    the record holds what the launch knows; still to come: the software
    versions (#160), the envelope, the experiment's profile and who drove.
    `source` is the log this run replays, if any (`source_record`).
    """
    return {
        'target': target,
        'sim': sim,
        'sensors': sensors,
        'car': car,
        'car_config': car_config_hash,  # config/cars/ hash (ADR-0051)
        'log_profile': log_profile,
        'log_exclude': log_exclude,
        **(source or {}),
    }


# What a replay's record keeps from the replayed log's own record
SOURCE_FIELDS = ('target', 'car', 'car_config', 'log_profile')


def read_custom_data(path):
    """(custom data, problem): a log's run record, or {} and why it couldn't be read."""
    try:
        return dict(rosbag2_py.Info().read_metadata(str(path), STORAGE).custom_data), None
    except RuntimeError as e:
        return {}, f"source_log {path}: its run record couldn't be read ({e})"


def source_record(path, read=read_custom_data):
    """(fields, problem): what a run's record says about the log it replays (#205).

    The log's absolute path, and its own target, car, car configuration and
    profile when its record has them, each as `source_<field>`: so a replay's
    log points back to the run it came from and isn't taken for a run of its
    own. A log from before run records has only its path. Nothing when no log
    is replayed.
    """
    if not path:
        return {}, None
    if '=' in path:
        # rosbag2 splits each custom-data pair on every '=' (check_exclude)
        return {}, (f"source_log {path!r} not recorded: it contains '=', which the run "
                    "record can't hold")
    data, problem = read(path)
    fields = {'source_log': str(Path(path).absolute())}
    fields.update({f'source_{key}': data[key] for key in SOURCE_FIELDS
                   if data.get(key) and '=' not in data[key]})
    return fields, problem


def check_exclude(pattern):
    """Why `pattern` can't be used as log_exclude, or None if it can (or is empty).

    rosbag2 splits each custom-data pair on every '=', so a pattern with one
    would be stored cut short and the log would misstate what it left out.
    """
    if '=' in pattern:
        return "it contains '=', which the run record can't hold"
    try:
        re.compile(pattern)
    except re.error as e:
        return f"it isn't a valid regex ({e})"
    return None


def record_command(output, record, chunk_s, exclude='', exclude_types=(),
                   node_name='arc_logging_recorder'):
    """`ros2 bag record` for every topic of the run, into `output`.

    --all-topics keeps discovering, so an experiment's topics are recorded
    from when they first have a publisher. The recorder matches each
    publisher's QoS: reliable where it's offered (ICD-ros2-msgs), and transient
    local for state, so a latched value published before it started is still
    logged. The log is split into chunks of `chunk_s` seconds, each closed
    when the next starts, so a crash or power loss loses at most the last one.
    `exclude` leaves out every topic whose name contains a match (rosbag2's
    --exclude-regex); `exclude_types` leaves out every topic of those message
    types (the profile's, ADR-0056); both empty record everything.
    """
    custom = [f'{key}={value}' for key, value in record.items() if value != '']
    return [
        'ros2', 'bag', 'record',
        '--all-topics',
        '--storage', STORAGE,
        '--output', str(output),
        '--max-bag-duration', str(int(chunk_s)),
        '--node-name', node_name,
        '--disable-keyboard-controls',
        *(['--exclude-regex', exclude] if exclude else []),
        *(['--exclude-topic-types', *exclude_types] if exclude_types else []),
        '--custom-data', *custom,
    ]


def started(line, output):
    """Whether this recorder output line shows rosbag2 recording into `output`.

    rosbag2 (Jazzy) logs "Recording..." once it has opened its writer, which
    creates the output directory; until then nothing is being recorded.
    """
    return 'Recording...' in line and Path(output).is_dir()


def start_status(code, waited_s, start_timeout_s, must_log):
    """(warn, message) while the recorder hasn't started recording.

    `code` is its exit code if it has exited. Never "recording": that's said
    only once rosbag2 shows it is (`started`).
    """
    if code is not None:
        return True, (f'NOT LOGGING: the recorder failed to start (exit code {code}): '
                      'this run is not recorded')
    if waited_s > start_timeout_s:
        return True, (f'NOT LOGGING: the recorder has not started after {waited_s:.0f} s '
                      f'(start_timeout_s {start_timeout_s:g} s): this run is not being recorded')
    return must_log, 'starting the recorder: not recording yet'


_LOST = re.compile(r'Total lost: (\d+)')


def lost_messages(line):
    """The count from a rosbag2 line reporting lost messages, else None.

    rosbag2 (Jazzy) reports them only in its log: messages lost on the
    transport per topic while recording, and its cache's losses when it closes.
    """
    match = _LOST.search(line)
    return int(match.group(1)) if match else None
