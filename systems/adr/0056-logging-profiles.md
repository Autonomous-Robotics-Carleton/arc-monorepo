# ADR-0056: Each run logs a profile; camera images only when it asks for them

- **Status:** Proposed (2026-10-29)
- **Date:** 2026-10-29
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-03, SYS-09, SYS-19, SYS-21, SYS-23, SYS-24, E-04, ADR-0046, ICD-ros2-msgs

## Context

SYS-09 logged every sensor stream on every run, and SYS-24 logged each at the fastest rate its data is useful at. On v1.0 the camera images make up nearly all of that: the stereo pair and the ground camera write about 296 MB/s, packed to 10 bits, or 178 GB per 10-minute run ([logging](../bom/total-electrical-cost.md#camera-links-and-logging)). Everything else on the ROS graph is a few MB/s.

That rate costs on every run, whether or not anyone needs the images:

- **Drive space.** The 1 TB drive (E-04, ADR-0046) holds 4 runs between offloads.
- **Offload.** One run takes about 27 min over gigabit Ethernet.
- **A full drive meant no log at all.** A run without room for its images recorded nothing, losing the commands, state and faults too: the part needed to explain a crash (SYS-19, SYS-23).
- **The 10-bit packing doesn't exist.** ROS's image message has no packed 10-bit format; at 16 bits per pixel the rate is 1.6× higher (about 474 MB/s, 284 GB per run).
- **No cheap way to compress on v1.0.** The Orin Nano has no hardware video or JPEG encoder, so lossy compression would take several CPU cores at this rate. Lossless compression gains little on sensor noise.

The logs serve three uses. Fault attribution (mission 3) needs the commands, state, faults and the non-camera sensors, never full-rate images. Replay (#86) of classical pipelines needs the same. Training learned methods (mission 2) needs images, but on a club car that can be driven again, a dataset can be recorded when someone needs it. A crash can't be driven again.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Log everything on every run (SYS-09 as written) | Every run is a dataset | 178 GB a run; a full drive loses the whole log |
| **B. Each run logs a profile: `minimal` (no camera images) by default, `full` when asked** | Small logs by default; the part needed for crashes is always kept | A run's images can't be recovered afterwards unless it asked for `full` |
| C. Log nothing until it's needed | No cost | No record of the first drives, when the untested safety functions run for the first time |
| D. Compress the images | Every run keeps images | No encoder on v1.0; lossy artifacts hurt stereo, tracking and training |

## Criteria

1. A run never goes unrecorded for lack of room for images: what explains a crash is always logged.
2. Images at full rate stay available to whoever needs them.
3. Cost per ordinary run.

## Decision

Option B:

1. **Each run logs one profile,** `log_profile:=` on the platform launch:
   - **`minimal`** (the default on every target): every topic except camera images, left out by message type (`sensor_msgs/Image`, `sensor_msgs/CompressedImage` and image_transport's encoded packets). The camera info, camera triggers, LiDAR, every sync-link measurement, commands, operator inputs, the car's state and faults, and every experiment topic that isn't an image are all logged.
   - **`full`:** every topic.
2. **A `full` run without room for it logs `minimal` and warns,** rather than logging nothing.
3. **Free space to start a run, and to warn while running (TBC):**

   | Profile | Log rate | To start | Warn below |
   | --- | --- | --- | --- |
   | `minimal` | ≤ 10 MB/s (TBC, an upper estimate from ICD-ros2-msgs' rates; measure on the first car runs) | 20 GB | 10 GB |
   | `full` | v1.0's ~296 MB/s | 270 GB (a 10-minute run × 1.5) | 60 GB (about 3 minutes) |

4. **Profiles choose topics, not rates.** A profile with images at a lower rate needs the camera drivers to publish a decimated topic of their own: a copy of every frame to a separate throttling process would cost the very bandwidth it saves. Add one when the drivers exist.

## Consequences

- **SYS-09 and SYS-24 are reworded:** every run logs every command, operator input and non-image measurement; camera images on the runs that ask for them. SYS-24's rates and 50% margins still apply, with `full` as the case to size for.
- **E-04's sustained-write requirement is unchanged:** `full` still needs it, and v1.1 needs the 4 TB drive.
- **The 10-bit packing is decided with the camera drivers,** before the first `full` run. Until then, the 296 MB/s figure assumes a packing step that doesn't exist.
- **New image types join the `minimal` list when they appear** (v1.1's event cameras, ADR-0046). A heavy topic that isn't an image (say a dense point cloud from an experiment) is still logged in `minimal`; the running free-space warning catches it.
- **On `full` runs, image_transport's compressed topics are recorded too,** and a driver encodes each frame for them on the CPU. The camera drivers should publish raw images without those plugins.
- **Reopen if:** the measured `minimal` rate is far from the estimate, or the drive can't hold enough `full` runs between offloads for the team's dataset work.
