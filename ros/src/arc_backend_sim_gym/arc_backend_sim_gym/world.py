"""The world the gym simulates: an occupancy grid with its origin and
resolution, as a gym Track.

Two sources: the built-in oval, a made-up test track (not the venue, SYS-01),
or a map in the map_server format every F1TENTH track ships in (a YAML file
naming an image).
"""

import math
from pathlib import Path

import numpy as np
import yaml

FREE = 255.0  # the gym's occupancy values: 0 is a wall
OCCUPIED = 0.0


def oval_centreline(straight, radius, spacing=0.1):
    """Points along the oval's centreline, anticlockwise from the start of
    the bottom straight at (0, 0), heading +x."""
    length = 2 * straight + 2 * math.pi * radius
    s = np.arange(0.0, length, spacing)
    x = np.empty_like(s)
    y = np.empty_like(s)
    for i, si in enumerate(s):
        if si < straight:  # bottom straight, heading +x
            x[i], y[i] = si, 0.0
        elif si < straight + math.pi * radius:  # right-hand turn, centre (straight, radius)
            a = (si - straight) / radius - math.pi / 2
            x[i], y[i] = straight + radius * math.cos(a), radius + radius * math.sin(a)
        elif si < 2 * straight + math.pi * radius:  # top straight, heading -x
            x[i], y[i] = straight - (si - straight - math.pi * radius), 2 * radius
        else:  # left-hand turn, centre (0, radius)
            a = (si - 2 * straight - math.pi * radius) / radius + math.pi / 2
            x[i], y[i] = radius * math.cos(a), radius + radius * math.sin(a)
    return x, y


def oval_grid(straight, radius, width, resolution, margin=1.0):
    """The oval as an occupancy grid: free within width / 2 of the centreline.

    Returns (grid, origin): grid[row, col] covers the cell whose corner is at
    origin + (col, row) * resolution, the gym's layout (row 0 at the bottom).
    """
    half = width / 2
    x0, y0 = -radius - half - margin, -half - margin
    x1, y1 = straight + radius + half + margin, 2 * radius + half + margin
    cols = int(math.ceil((x1 - x0) / resolution))
    rows = int(math.ceil((y1 - y0) / resolution))
    xs = x0 + (np.arange(cols) + 0.5) * resolution
    ys = y0 + (np.arange(rows) + 0.5) * resolution
    gx, gy = np.meshgrid(xs, ys)

    # distance from each cell to the centreline: to the straights between the
    # two turn centres, or to the circles around them
    along = np.clip(gx, 0.0, straight)
    on_straights = np.minimum(np.hypot(gx - along, gy), np.hypot(gx - along, gy - 2 * radius))
    centre_x = np.where(gx < straight / 2, 0.0, straight)
    on_turns = np.abs(np.hypot(gx - centre_x, gy - radius) - radius)
    in_turns = (gx < 0.0) | (gx > straight)
    distance = np.where(in_turns, np.minimum(on_turns, on_straights), on_straights)

    grid = np.where(distance <= half, FREE, OCCUPIED).astype(np.float32)
    return grid, (x0, y0, 0.0)


def load_map_yaml(path):
    """A map_server map: (grid, origin, resolution), in the gym's layout."""
    from PIL import Image

    path = Path(path)
    with path.open() as f:
        spec = yaml.safe_load(f)
    image = Image.open(path.parent / spec['image']).convert('L')
    pixels = np.array(image.transpose(Image.Transpose.FLIP_TOP_BOTTOM), dtype=np.float32)
    if spec.get('negate', 0):
        pixels = 255.0 - pixels
    # the gym's own threshold (Track.from_track_name): above mid-grey is free
    grid = np.where(pixels > 128, FREE, OCCUPIED).astype(np.float32)
    origin = tuple(float(v) for v in spec['origin'])
    return grid, origin, float(spec['resolution'])


def make_track(grid, origin, resolution, name):
    """The gym's Track for a grid; no centreline or raceline, which only its
    environment's reset functions need."""
    from f1tenth_gym.envs.track import Track
    from f1tenth_gym.envs.track.track import TrackSpec

    spec = TrackSpec(
        name=name, image=None, resolution=resolution, origin=origin,
        negate=0, occupied_thresh=0.65, free_thresh=0.196)
    return Track(spec=spec, occupancy_map=grid)
