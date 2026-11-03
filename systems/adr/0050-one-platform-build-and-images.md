# ADR-0050: One platform build for the Orin and laptops; images for amd64 and arm64; experiments in Docker

- **Status:** Accepted (2026-10-25)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-21, SYS-33, ADR-0016, ADR-0029, ADR-0047, ADR-0048
- **Amends:** ADR-0016 (how the platform image is built; the container runtime)

## Context

ADR-0047 says the code tested in the twin is the code that drives the car. The last gap is packaging:
- If the Orin and laptops each get their own image (different bases, versions or setup), the twin tests something slightly different from what drives.
- The dev container (ADR-0029) is amd64 only, so team members on Apple Silicon Macs can't run the twin.
- ADR-0016 puts experiments in containers but doesn't name the runtime.

One difference must remain: the Orin's real-time tuning (PREEMPT_RT priorities and reserved cores, ADR-0016, ADR-0048) means nothing on a laptop.

A literal single image isn't possible either. Jetson's base images (L4T, with the GPU libraries) exist only for Jetson, and laptops need an ordinary Ubuntu base.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Separate images for the Orin and laptops | Simple to start | The twin can drift from the car; two setups to maintain |
| **B. One platform build: the same Dockerfile, packages and versions, on a Jetson base for the Orin and an Ubuntu base for laptops; real-time settings in configuration** | The twin runs what the car runs; one place to change | Two base images to keep in step; a build matrix in CI |

## Decision

**Option B.**

1. **One platform build.**
   - One Dockerfile, one set of ROS packages and one set of pinned versions.
   - It's built on two bases, chosen by a build argument:
     - **The Orin:** JetPack 7.2.1's L4T base for Jazzy (ADR-0016), arm64.
     - **Laptops and CI:** `ros:jazzy` on Ubuntu 24.04, for amd64 and arm64.
   - Anything else that differs between the two goes in configuration, not in separate images.
2. **Real-time settings are configuration.** Core reservations and priorities are read at start-up and have no effect off the car.
3. **The simulator image** is built for amd64 and arm64, so the twin runs on Apple Silicon. Like the dev container, it's tagged by the hash of its inputs.
4. **Experiments run in Docker.**
   - Docker ships with JetPack, and NVIDIA's container runtime gives containers the GPU.
   - CPU, memory, GPU and disk limits and real-time cores are set per container.
   - The numbers are TBD and get measured on the dev kit (#88).

## Consequences

- **ADR-0016:** the "reproducible image" is this build. Experiments run in Docker containers.
- **CI** builds the platform for three combinations: the Jetson base on arm64, and Ubuntu on amd64 and on arm64.
- **The dev container stays amd64 only** for firmware builds, because `native_sim` needs 32-bit host libraries. Mac users run the twin from the simulator image and the platform image.
- **#89** (the image and deploy) builds and deploys this; experiments build on it (`experiments/README.md`).
- **Reopen if** the Jetson and Ubuntu bases can't be kept on the same package versions.
