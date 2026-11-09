"""bridge: load_track takes only bundled track names, never a path.

In the sim image: docker run --rm --entrypoint /entrypoint.sh arc-sim python3 -m pytest /sim_ws/src/arc_sim/test
"""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'scripts'))

import bridge  # noqa: E402


def test_bundled_tracks_are_names_only(tmp_path):
    tracks = tmp_path / 'tracks'
    tracks.mkdir()
    (tracks / 'oval.yaml').write_text('')
    (tracks / 'oval.png').write_bytes(b'')
    (tmp_path / 'secret.yaml').write_text('')
    names = bridge.bundled_tracks(str(tracks))
    assert names == {'oval'}
    for name in ('../secret', '/etc/passwd', 'oval/../oval', 'oval.yaml', ''):
        assert name not in names


def test_bundled_tracks_missing_folder():
    assert bridge.bundled_tracks('/nonexistent') == set()


def test_shipped_tracks():
    tracks = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'tracks')
    assert 'oval' in bridge.bundled_tracks(tracks)


def test_bridge_refuses_browsers():
    assert bridge.ORIGINS == [None]  # server.py sends no Origin; every browser does
