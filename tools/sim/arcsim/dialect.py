"""The ARC MAVLink dialect, generated from the ICD (systems/icd/sync-link.xml)
the way tools/gen-interfaces.sh generates the C: one schema for firmware, ROS
bridge and simulator, so they can't drift apart."""

import functools
import importlib.util
import re
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO = Path(__file__).resolve().parents[3]
SCHEMA = REPO / "systems" / "icd" / "sync-link.xml"


@functools.lru_cache(maxsize=None)
def load_dialect(schema=SCHEMA):
    """Returns (module, schema <version>). mavgen wants a C-identifier file name."""
    work = Path(tempfile.mkdtemp(prefix="arc_dialect_"))
    xml = work / "arc_sync_link.xml"
    shutil.copy(schema, xml)
    out = work / "arc_sync_link.py"
    subprocess.run(
        [sys.executable, "-m", "pymavlink.tools.mavgen", "--lang=Python3",
         "--wire-protocol=2.0", f"--output={out}", str(xml)],
        check=True, capture_output=True)
    spec = importlib.util.spec_from_file_location("arc_sync_link", out)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    version = int(re.search(r"<version>(\d+)</version>", Path(schema).read_text()).group(1))
    return module, version
