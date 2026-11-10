"""Tracks: a ROS occupancy map (.yaml + image) turned into Gazebo walls.

The map format is ROS map_server's, the same one f1tenth_gym and slam_toolbox use:

    image: track.png          # relative to the yaml
    resolution: 0.05          # metres per pixel
    origin: [x, y, yaw]       # world pose of the image's bottom-left corner
    negate: 0
    occupied_thresh: 0.65
    free_thresh: 0.196
    start: [x, y, yaw]        # optional, ARC-only: where the car spawns (default 0, 0, 0)

Occupied pixels become walls; free and unknown pixels stay open floor. Neighbouring
occupied pixels are merged into as few boxes as possible, so a wall is a handful of
boxes rather than one per pixel.

The bridge spawns the walls into the running sim when the ground station picks a track.

    python3 -m arc_sim.track map.yaml > walls.sdf    # inspect the generated walls model
"""

import os
import sys
from dataclasses import dataclass

import numpy as np
import yaml
from PIL import Image


@dataclass
class Track:
    name: str
    yaml_path: str
    image_path: str
    resolution: float
    origin: tuple  # x, y, yaw
    start: tuple  # x, y, yaw
    occupied: np.ndarray  # bool, rows top to bottom, as in the image
    unknown: np.ndarray  # bool


def load(yaml_path):
    """Read a map yaml and classify its pixels the way map_server does (trinary mode)."""
    with open(yaml_path) as f:
        meta = yaml.safe_load(f)
    image_path = os.path.join(os.path.dirname(os.path.abspath(yaml_path)), meta['image'])

    img = Image.open(image_path)
    # Alpha is ignored, as map_server ignores it; colour is averaged to grey.
    grey = np.asarray(img.convert('RGB'), dtype=np.float32).mean(axis=2)
    p = grey / 255.0 if meta.get('negate', 0) else (255.0 - grey) / 255.0
    occupied = p > float(meta['occupied_thresh'])
    free = p < float(meta['free_thresh'])

    origin = [float(v) for v in meta['origin']] + [0.0] * 3
    start = [float(v) for v in meta.get('start', [0, 0, 0])] + [0.0] * 3
    return Track(
        name=os.path.splitext(os.path.basename(yaml_path))[0],
        yaml_path=yaml_path,
        image_path=image_path,
        resolution=float(meta['resolution']),
        origin=tuple(origin[:3]),
        start=tuple(start[:3]),
        occupied=occupied,
        unknown=~occupied & ~free,
    )


def rectangles(mask):
    """Cover the True cells of a 2D mask with non-overlapping rectangles.

    Runs of True in each row are merged downwards while the run below spans exactly the
    same columns. Returns (row0, col0, row1, col1) with exclusive ends.
    """
    open_runs = {}  # (col0, col1) -> row0 of a rectangle still growing
    done = []
    for r, row in enumerate(np.vstack([mask, np.zeros((1, mask.shape[1]), bool)])):
        edges = np.flatnonzero(np.diff(np.concatenate(([0], row.astype(np.int8), [0]))))
        runs = set(zip(edges[::2], edges[1::2]))
        for run in list(open_runs):
            if run not in runs:
                done.append((open_runs.pop(run), run[0], r, run[1]))
        for run in runs:
            open_runs.setdefault(run, r)
    return done


# Walls are smooth: with the default friction (mu 1) a car pushing into a wall climbs it
# with its tyres and flips over.
SURFACE = '<surface><friction><ode><mu>0.1</mu><mu2>0.1</mu2></ode></friction></surface>'


def walls_model(track, height, name='track_walls'):
    """SDF <model> of static boxes, one per merged rectangle of occupied pixels."""
    res = track.resolution
    rows = track.occupied.shape[0]
    ox, oy, oyaw = track.origin
    parts = []
    for i, (r0, c0, r1, c1) in enumerate(rectangles(track.occupied)):
        # Pixel row 0 is the top of the image; the model frame is its bottom-left corner.
        x = (c0 + c1) / 2 * res
        y = (rows - (r0 + r1) / 2) * res
        geometry = '<geometry><box><size>%.4f %.4f %.4f</size></box></geometry>' % (
            (c1 - c0) * res, (r1 - r0) * res, height)
        pose = '<pose>%.4f %.4f %.4f 0 0 0</pose>' % (x, y, height / 2)
        parts.append(
            '        <collision name="c%d">%s%s%s</collision>\n'
            '        <visual name="v%d">%s%s<material>'
            '<ambient>0.15 0.15 0.17 1</ambient><diffuse>0.15 0.15 0.17 1</diffuse>'
            '</material></visual>\n' % (i, pose, geometry, SURFACE, i, pose, geometry))
    return (
        '    <model name="%s">\n'
        '      <static>true</static>\n'
        '      <pose>%.4f %.4f 0 0 0 %.6f</pose>\n'
        '      <link name="link">\n%s'
        '      </link>\n'
        '    </model>\n' % (name, ox, oy, oyaw, ''.join(parts)))


if __name__ == '__main__':
    if len(sys.argv) != 2:
        sys.exit('usage: python3 -m arc_sim.track <map.yaml>')
    sys.stdout.write(walls_model(load(sys.argv[1]), 0.3))
