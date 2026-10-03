# ADR-0014: Ground-speed sensor is a downward event camera with three lighting modes

- **Status:** Accepted
- **Date:** 2026-10-02
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-07, SYS-10, SYS-24, ADR-0007, RSK-01, RSK-07

## Context

SYS-10 needs ground velocity, forward and sideways, measured directly. The sideways part (sideslip) is the state the rest of the suite can't reconstruct: the car is 4WD, so no wheel rolls freely, and estimating sideslip from a tire model is circular when the tire model is what the research is studying. Constraints: everything onboard (no track infrastructure in v1), one hardware order, 0–12 m/s, a school floor (likely waxed VCT, terrazzo or sealed concrete).

## Options

| Option | Result |
| --- | --- |
| Optical flow chips (PMW3901, PAA5100JE, SparkFun OTOS) | Rejected: 0.58, 1.14 and 2.5 m/s maximum |
| Gaming mouse chip with custom optics | Rejected for v1: custom optics, low certainty |
| High-fps frame camera | Rejected: needs ~240 fps plus a strobe at 12 m/s |
| mm-wave radar Doppler | Rejected: smooth tile reflects the beam away |
| Automotive optical sensor (Kistler Correvit) | Rejected: sized for full-size cars |
| Lighthouse tracking / motion capture | Excluded: needs track infrastructure. Note for v2 if the team instruments its own track |
| Dead-wheel pod (spring-loaded omni wheels + AS5047) | High certainty but touches the floor (≤ 2 N, logged, retractable). **v2 / fallback** |
| Laser Doppler (self-mixing, two beams) | Measures one point directly, but ~20 MHz signal processing at 12 m/s. **v2 candidate** |
| **Downward event camera** | **Chosen** |

## Evidence

- **ETH Zurich (Boyle et al., arXiv 2505.11116):** downward DAVIS346 event camera on a 1:10 racing car, compared with motion capture: forward velocity RMSE ~0.05 m/s, sideways ~0.03 m/s. The same method on a full-size car reached 0.4% error at 32 m/s. Limitations they report: featureless surfaces, and ride-height changes needing a distance sensor for scale.
- **Floor texture:** Princeton's Micro-GPS and Accerion's Triton localize off floor microtexture on tile and concrete. School floors (VCT, terrazzo, concrete) are mottled by design and scuffed in use.
- **Laser speckle:** coherent light turns a surface's micro-roughness into a ~100% contrast pattern that moves with the floor. That's why laser mice track on glossy surfaces.

## Decision

| Element | Choice |
| --- | --- |
| Sensor | Second Prophesee GenX320 (320 × 320, 6.3 µm pixels, 25% default contrast threshold, adjustable) on carrier CSI port 4, pointing down |
| Mounting | Inside the chassis, looking through a floor window, as high as packaging allows; ~90° M12 lens. At 60–80 mm this gives ~0.4–0.5 mm of floor per pixel |
| Lighting mode 1 (default) | Cross-polarized LED ring (polarizer on the light, crossed polarizer on the lens), constant-current DC drive (no PWM; event cameras see flicker) |
| Lighting mode 2 | Grazing-angle LEDs to bring out scuffs and surface relief |
| Lighting mode 3 | **Off-the-shelf, certified IEC 60825-1 Class 1** IR VCSEL module, with a hardware current limit, to produce laser speckle on plain or pristine floors. Not Class 1M or higher. Lens aperture (~f/8) sized so speckle grains are ≥ 2 pixels |
| Enclosure | A light shroud between the floor window and the floor, so the car's own lighting overpowers the room's |
| Scale | The fifth ride-height ToF sensor (ADR-0007), right next to the camera |
| Control and timing | The sync board drives the three lighting modes (GPIO) and a trigger/sync line to the camera, so events land on the car's time base (SYS-07) |
| Processing | On the Orin: event histograms → optical flow → planar motion fit with outlier rejection, IMU for yaw rate, ToF for scale |

## Consequences

- CSI port 4 is used, so **no spare CSI port remains**. Adding another camera needs a CSI multiplexer or the v2 carrier.
- The second GenX320 shares the forward event camera's driver work (RSK-01).
- The sync board adds 3 lighting-control outputs and 1 trigger output, and powers the lights and the VCSEL from the 5 V rail through current-limited drivers.
- Floor risk (RSK-07) is mostly covered by the three lighting modes and the adjustable threshold. An optional phone test (`tests/ground-speed-phone-test.md`) checks the actual floor before the order.
- **The dead-wheel pod stays the documented fallback:**
  - two spring-loaded arms (~1 N preload, ≤ 15 g moving mass, long trailing arm, damper)
  - multi-roller omni wheels with AS5047s
  - arm-angle sensors to log the force on the car
  - servo retract
  - on the centerline near the CG

  Its underfloor space and sync-board channels are reserved.
