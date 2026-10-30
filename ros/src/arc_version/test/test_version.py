"""The stamped version is the commit the workspace was built from (#160)."""

import os
from pathlib import Path
import re
import subprocess

VERSION = Path(os.environ['VERSION_FILE']).read_text()
SOURCE = os.environ['SOURCE_DIR']


def git(*args):
    return subprocess.run(['git', '-c', 'safe.directory=*', '-C', SOURCE, *args],
                          capture_output=True, text=True)


def test_one_line_hash_with_a_dirty_flag_or_unknown():
    assert re.fullmatch(r'([0-9a-f]{12}(-dirty)?|unknown)\n', VERSION), VERSION


def test_names_the_checked_out_commit_and_its_changes():
    """Tests run on the tree just built: a stale stamp fails here."""
    head = git('rev-parse', '--short=12', 'HEAD')
    if head.returncode:
        assert VERSION == 'unknown\n'
        return
    dirty = git('diff', '--quiet', 'HEAD', '--', ':(top)ros', ':(top)config').returncode != 0
    assert VERSION == f"{head.stdout.strip()}{'-dirty' if dirty else ''}\n"
