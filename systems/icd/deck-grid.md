# ICD-deck-grid

- **Revision:** draft outline (2026-10-03). Values come from the block layout (`mechanical/cad-workflow.md`, phase 2).
- **Status:** Draft. Needs an owner on each side and their sign-off.
- **Side A:** chassis: upper deck hole grid (owner TBD)
- **Side B:** anything mounted on it: sensor mounts, cameras, antennas, future experiments (owner TBD)
- **Traces to:** SYS-24, mission 4 (sensors already there; experiments need code, not a hardware build)

The upper deck carries a regular hole grid so mounts can be added or moved without changing the chassis. This document fixes the grid so mounts can be designed against it independently.

## Grid

| Item | Value |
| --- | --- |
| Pitch | TBD (CAD parameter `deck_grid_pitch`) |
| Hole: thread or clearance; heat-set insert size | TBD |
| Extent: which area of the deck | TBD |
| Origin and orientation relative to car coordinates | TBD |
| Allowed load per hole and per mount | TBD |

## Rules for mounts

- Mounts locate on at least two grid holes.
- Rigid mounts (stereo bar) and vibration-isolated mounts (IMUs) are both allowed; isolated mounts say so.
- Cables leave the mount through TBD routing paths in the deck.
- Nothing on the grid may block the LiDAR's 270° field of view or the Wi-Fi antennas.
