# TEST: Ground-speed floor check with a phone (optional, before the order)

- **Reduces:** RSK-07
- **Supports:** ADR-0014
- **Cost:** nothing beyond polarizer film (~$10)
- **Owner:** TBD
- **Status:** Not started

## Objective

Check that the actual track floor shows enough texture for the downward event camera under its default lighting, before hardware is ordered. The laser-speckle mode is the backstop if it doesn't, so this test is a confidence check, not a gate.

## Steps

1. **Cross-polarized close-up.** Put polarizer film over the phone's flashlight and a second piece over the camera lens, rotated until reflections from the floor disappear. Photograph the floor from ~60–80 mm in several spots: clean areas, worn lanes, near grout lines.
2. **Grazing light.** Turn off the flashlight; hold a light source almost flat to the floor from the side. Photograph the same spots.
3. **Contrast check.** Scale each photo so one pixel is ~0.5 mm of floor (the event camera's footprint). Measure brightness differences between neighbouring pixels across the image (e.g. a short Python/OpenCV script). Record the fraction of pixels with neighbour differences above 20%.
4. **Flow check.** Record a 240 fps slow-motion clip while sliding the phone over the floor at ~70 mm height with the cross-polarized setup. Run OpenCV Farneback optical flow on the clip and check that the flow field is consistent across the frame.

## Pass

- Visible speckle, mottle or wear at the 1–2 mm scale across a ~150 mm patch, in both lighting modes.
- A meaningful fraction of pixels (target ≥ 10%, TBC) above 20% neighbour contrast.
- Optical flow tracks consistently along and across the direction of motion (not only across grout lines).

## Results

| Spot | Lighting | Pixels > 20% contrast | Flow consistent? | Notes |
| --- | --- | --- | --- | --- |
| | cross-polarized | | | |
| | grazing | | | |
