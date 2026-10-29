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

STORAGE = 'mcap'


class Plan(NamedTuple):
    """What the logger does this run, and what it tells the operator."""

    output: Optional[Path]  # the run's directory, or None when not recording
    warn: bool  # whether the operator is warned (on /diagnostics and the console)
    message: str


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


def plan(log, must_log, root, name, min_free_gb, free=free_gb):
    """Whether this run is recorded, where, and what the operator is told.

    The platform runs either way: an unlogged run is never stopped, only
    reported. Logs are never deleted to make room.
    """
    if not log:
        if must_log:
            return Plan(None, True, 'NOT LOGGING: log:=false was passed, this run is not recorded')
        return Plan(None, False, 'not logging (log:=false)')
    if root is None:
        return Plan(None, True,
                    'NOT LOGGING: $ARC_LOG_DIR is not set; set it to the log drive. '
                    'This run is not recorded')
    space = free(root)
    if space < min_free_gb:
        return Plan(None, True,
                    f'NOT LOGGING: {space:.1f} GB free on {root}, below min_free_gb '
                    f'({min_free_gb:g} GB). Free space on the log drive (logs are never '
                    'deleted automatically). This run is not recorded')
    output = run_dir(root, name)
    return Plan(output, False, f'recording to {output}')


def run_name(target, now=None):
    """`<UTC start time>_<target>`, e.g. `20261029T140501Z_car`: sorts by time.

    Without a target (arc_logging launched on its own) it's the time alone.
    """
    stamp = (now or datetime.now(timezone.utc)).strftime('%Y%m%dT%H%M%SZ')
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


def run_record(target, sim, sensors, car, car_config_hash):
    """The run record (#152 part 2): what a log needs to be read later.

    It's written as the log's custom data (metadata.yaml's `custom_data`, which
    rosbag2's MCAP storage also keeps in each .mcap file), so plain rosbag2
    reads it back. Every field is passed through as given; an empty one
    (`sim` off a simulator) is left out. So far it holds what the launch
    knows; still to come: the software versions (#160), the envelope, the
    experiment's profile and who drove.
    """
    return {
        'target': target,
        'sim': sim,
        'sensors': sensors,
        'car': car,
        'car_config': car_config_hash,  # config/cars/ hash (ADR-0051)
    }


def record_command(output, record, chunk_s, node_name='arc_logging_recorder'):
    """`ros2 bag record` for every topic of the run, into `output`.

    --all-topics keeps discovering, so an experiment's topics are recorded
    from when they first have a publisher. The recorder matches each
    publisher's QoS: reliable where it's offered (ICD-ros2-msgs), and transient
    local for state, so a latched value published before it started is still
    logged. The log is split into chunks of `chunk_s` seconds, each closed
    when the next starts, so a crash or power loss loses at most the last one.
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
        '--custom-data', *custom,
    ]


_LOST = re.compile(r'Total lost: (\d+)')


def lost_messages(line):
    """The count from a rosbag2 line reporting lost messages, else None.

    rosbag2 (Jazzy) reports them only in its log: messages lost on the
    transport per topic while recording, and its cache's losses when it closes.
    """
    match = _LOST.search(line)
    return int(match.group(1)) if match else None
