"""What the platform runs to log a run: where the log goes, the run record
written into it, and the `ros2 bag record` command line.

The recorder is rosbag2's own, with its MCAP storage: a log is a standard
rosbag2 directory (`metadata.yaml` and one `.mcap`) that plain rosbag2 reads,
so the replay backend (#86) and `ros2 bag` need nothing of ours.

Plain Python, so the same code runs in the launch file and in the tests.
"""

from datetime import datetime, timezone
import os
from pathlib import Path

STORAGE = 'mcap'


def log_root():
    """Where runs are logged: $ARC_LOG_DIR, else ~/arc_logs.

    The car's NVMe mount (E-04) is set through $ARC_LOG_DIR once the Orin's
    image (platform/) has one.
    """
    return Path(os.environ.get('ARC_LOG_DIR') or Path.home() / 'arc_logs')


def run_name(target, sim, now=None):
    """`<UTC start time>_<target>`, e.g. `20261029T140501Z_sim-gym`: sorts by time."""
    stamp = (now or datetime.now(timezone.utc)).strftime('%Y%m%dT%H%M%SZ')
    return f'{stamp}_{f"sim-{sim}" if target == "sim" else target}'


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
    rosbag2's MCAP storage also keeps in the .mcap file), so plain rosbag2 reads
    it back. So far it holds what the launch knows; still to come: the
    software versions (#160), the envelope, the experiment's profile and who
    drove.
    """
    record = {
        'target': target,
        'sensors': sensors,
        'car': car,
        'car_config': car_config_hash,  # config/cars/ hash (ADR-0051)
    }
    if target == 'sim':
        record['sim'] = sim
    return record


def record_command(output, record, node_name='arc_logging'):
    """`ros2 bag record` for every topic of the run, into `output`.

    --all-topics keeps discovering, so an experiment's topics are recorded
    from when they first have a publisher. The recorder matches each
    publisher's QoS: reliable where it's offered (ICD-ros2-msgs), and transient
    local for state, so a latched value published before it started is still
    logged. Its writes are double-buffered in memory (rosbag2's cache), never
    on a publisher's thread.
    """
    custom = [f'{key}={value}' for key, value in record.items()]
    return [
        'ros2', 'bag', 'record',
        '--all-topics',
        '--storage', STORAGE,
        '--output', str(output),
        '--node-name', node_name,
        '--disable-keyboard-controls',
        '--custom-data', *custom,
    ]
