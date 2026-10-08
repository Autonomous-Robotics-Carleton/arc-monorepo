# ADR-0032: Simulators are pluggable backends: Gazebo Harmonic, Webots and the F1TENTH gym

- **Status:** Accepted
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-08, SYS-10, SYS-24, SYS-33, ADR-0014, ADR-0016, ADR-0026, ADR-0028, ADR-0029

## Context

ADR-0028 gives the platform software a simulator backend behind the car interface, so drivers, estimation, control and experiments run before the car exists, and again in CI. A simulator for this car would ideally offer:

- **ROS 2 Jazzy** (ADR-0016);
- **the car's sensors:** 2D LiDAR, stereo and mono global-shutter cameras, two event cameras (one pointing at the floor for ground speed, ADR-0014), IMUs, wheel encoders;
- **4WD with one motor per wheel** (ADR-0026);
- **all three team operating systems,** through the dev container (ADR-0029), and headless in CI.

No single simulator does all of that well. **Rendering cost is the main problem:** six frame cameras and two event cameras are expensive to render. Event cameras are the worst, because they're simulated from frames rendered at hundreds of frames per second. And containers on macOS get no GPU at all.

| Simulator | Strengths | Weaknesses |
| --- | --- | --- |
| Gazebo Harmonic | The recommended simulator for ROS 2 Jazzy; Ubuntu packages, so it goes in the dev container; LiDAR, camera, IMU; a community event-camera plugin for Harmonic + Jazzy; headless in CI | In a container on macOS (no GPU) and Windows (partial GPU), camera-heavy scenes are slow |
| Webots | Runs natively on Linux, macOS (including Apple Silicon) and Windows with full GPU, ROS in the container; maintained ROS 2 driver | No event-camera sensor; a second program to install on each laptop |
| F1TENTH gym | Very fast 2D kinematics; no GPU; good for planning and control experiments and for CI | No cameras or event cameras; not a full-car simulator |
| NVIDIA Isaac Sim | Photo-real; GPU physics; synthetic data for learned policies | Needs an NVIDIA RTX GPU: not cross-platform; heavy |

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. One simulator (Gazebo) | One integration to maintain | Mac users can't simulate cameras usefully; one simulator's limits become the platform's |
| **B. Pluggable simulator backends behind the car interface, several scaffolded** | Each experiment uses the simulator that suits it; no OS or GPU locks anyone out; adding one later is a new package, not a redesign | More than one integration to keep working; the car must be described once and shared |

## Criteria

1. No team member is locked out of simulation by their OS or GPU.
2. Every sensor the car carries can be simulated by at least one backend, including the event cameras.
3. Adding or dropping a simulator doesn't touch platform software.

## Decision

- **Option B.**
- **Each simulator is a backend package** implementing the car interface (ADR-0028). It's chosen at launch: `target:=sim sim:=gazebo | webots | gym`.
- **Scaffolded in phase 5,** as package skeletons and launch wiring, not working integrations:

| Backend | For | Default sensors |
| --- | --- | --- |
| `gazebo` (Gazebo Harmonic) | Full sensor suite including event cameras; Linux with a GPU | LiDAR only, others by profile |
| `webots` | Camera work on macOS and Windows, with the host's GPU | LiDAR only, others by profile |
| `gym` (F1TENTH gym) | Fast planning and control experiments; CI | LiDAR (2D) |

- **One car description** (URDF, from the CAD block layout's dimensions) is the source for every backend: converted to SDF for Gazebo and to a Webots model. The car is never described twice by hand.
- **Sensor profiles per launch** (`sensors:=lidar | stereo | all`): an experiment renders only the sensors it uses. Event cameras are off by default; offline conversion from recorded frames (v2e) is the fallback.
- **Simulated time:** a heavy scene runs slower than real time but stays correct.

## Consequences

- **The dev container gets** ROS 2 Jazzy, Gazebo Harmonic and the F1TENTH gym (phase 5). Webots runs on the host, which the Dev setup page must cover.
- **Each backend needs** someone to keep it working. A backend that nobody uses gets dropped rather than left to rot.
- **Webots stays** because the team has no shared Linux machine with a GPU: without it, macOS users can't simulate cameras or GPU LiDAR at a usable speed in the container.
- **To check once the backends run:** one scene (LiDAR + stereo) benchmarked in each backend, on each OS; the event-camera plugin's maturity.
- **Reopen if:** a backend can't be kept working, or one simulator turns out to cover everything after all.
