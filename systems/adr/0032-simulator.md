# ADR-0032: Gazebo Harmonic is the simulator for the platform software

- **Status:** Proposed (recommendation, not yet decided)
- **Date:** 2026-10-07
- **Deciders:** TBD (recommended by Shrikar Vempati; for review by the software team)
- **Traces to:** SYS-08, SYS-10, SYS-24, SYS-33, ADR-0014, ADR-0016, ADR-0028, ADR-0029

> [!IMPORTANT]
> **This is a recommendation, not a decision.** It answers SQ-4 in `software/setup-plan.md`, so the simulator backend can be scaffolded in phase 5. Push back on anything here; it becomes `Accepted` only after review.

## Context

ADR-0028 gives the platform software a simulator backend behind the car interface, so drivers, estimation, control and experiments run before the car exists, and again in CI. The simulator needs:

- **ROS 2 Jazzy** (ADR-0016);
- **the car's sensors:** 2D LiDAR, stereo and mono global-shutter cameras, two event cameras (one pointing at the floor for ground speed, ADR-0014), IMUs, wheel encoders;
- **4WD with one motor per wheel** (ADR-0026);
- **all three team operating systems,** through the dev container (ADR-0029), and headless in CI.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| **A. Gazebo Harmonic** | The recommended simulator for ROS 2 Jazzy; Ubuntu packages, so it goes straight into the dev container; LiDAR, camera, IMU sensors; a community event-camera plugin for Harmonic + Jazzy (ESIM-style); headless in CI | In a container on macOS and Windows there's no GPU acceleration, so camera-heavy scenes render slowly; the event-camera plugin is new and community-maintained |
| B. Webots | Runs natively on Linux, macOS (including Apple Silicon) and Windows with GPU acceleration, ROS in the container; maintained ROS 2 driver | No event-camera sensor (offline conversion with v2e only); a second process outside the container to set up on each laptop |
| C. NVIDIA Isaac Sim | Photo-real rendering; GPU physics; synthetic data for learned policies | Needs an NVIDIA RTX GPU: not on Macs or most laptops; heavy |
| D. F1TENTH gym | Very fast 2D kinematics; good for planning experiments | No cameras or event cameras; not a full-car simulator |

## Criteria

1. Every sensor the car carries, including the event cameras.
2. Runs for every team member (all three OSes) and in CI.
3. Maintained, and paired with ROS 2 Jazzy.

## Decision

**Recommended, not decided:**

- **Option A:** Gazebo Harmonic, installed in the dev container with ROS 2 Jazzy, as the simulator backend.
- **Event cameras:** the community Harmonic event-camera plugin; v2e conversion as the fallback.
- **Later, if wanted:** the F1TENTH gym as a second, lightweight backend for fast planning experiments.

## Consequences

- **The dev container grows** by ROS 2 Jazzy and Gazebo Harmonic (phase 5).
- **Laptops without GPU passthrough** (macOS, Windows) run camera-heavy scenes slowly. Headless and LiDAR-only scenes are fine.
- **A car model:** chassis, four driven wheels and the sensor set, built from the CAD block layout's dimensions.
- **To check while scaffolding:** the event-camera plugin's maturity; Gazebo performance in the container on each OS.
- **Reopen if:** container performance makes Gazebo unusable on macOS or Windows (Webots is the alternative), or the event-camera plugin proves unusable.
