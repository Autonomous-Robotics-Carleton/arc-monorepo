# ADR-0029: One dev container for every toolchain; boards are flashed from the host

- **Status:** Accepted
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-33, ADR-0016, ADR-0017, ADR-0028, ADR-0030

## Context

About six people write the car's software, on Linux, macOS and Windows. The toolchains:

- the Zephyr SDK and west (sync MCU, ADR-0017);
- the GNU Arm Embedded toolchain and make (VESC firmware, ADR-0030);
- ROS 2 Jazzy, which supports Ubuntu 24.04 (ADR-0016). Even the systems lead's laptop runs Ubuntu 25.10;
- Node and pnpm (docs, tooling).

Installing all of that natively on three operating systems, at the same versions, isn't realistic, and CI has to use the same versions.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Native installs, documented per OS | Fastest builds; direct USB access | Three sets of instructions to keep working; versions drift; ROS 2 Jazzy isn't supported on macOS or Windows |
| **B. A dev container (Dev Containers spec) with every toolchain, used by CI too** | One pinned environment on every OS; opens in VS Code, JetBrains or the `devcontainer` CLI; CI and laptops identical | Needs Docker Desktop, Podman or WSL 2; containers can't reach USB devices on macOS and Windows |
| C. A remote build machine | Nothing to install | Needs a server and network access; offline work impossible |

## Criteria

1. Works the same on Linux, macOS and Windows.
2. CI and laptops use identical toolchain versions.
3. A new member goes from clone to a passing build quickly.

## Decision

**Option B.**

- **Where it lives:** `.devcontainer/` in this repo. The image has pinned versions of the Zephyr SDK and west, the Arm GNU toolchain, ROS 2 Jazzy, Node and pnpm, and the repo's tools.
- **Who builds it:** CI builds the image, publishes it to GHCR, and runs its own build and test jobs in it.
- **Flashing:**
  - **Build in the container, flash from the host.** The container can't reach USB on macOS or Windows.
  - **Flashing tool:** `probe-rs`, cross-platform and supporting ST-Link, for the STM32 boards; VESC Tool for VESCs over USB or CAN.
  - **Linux users** may pass USB into the container instead.
- **Builds outside the container stay possible:** native Linux builds aren't forbidden. The container is the supported path and the one CI uses.

## Consequences

- **Every setup on every OS:** Docker Desktop (or Podman), Git and the editor's Dev Containers support. Linux, macOS and Windows (with WSL 2) all run the same Linux image.
- **The image is large** (several GB). It gets versioned tags, so an update is a PR like any other change.
- **To verify while scaffolding:**
  - Zephyr `native_sim` and ROS 2 on Apple Silicon Macs. The image may need to run as `linux/amd64` under emulation there.
  - Zephyr's loopback CAN inside the container on all three OSes.
- **Dev Setup** (handbook) gets a firmware section: install Docker, open the repo in the container, install `probe-rs` on the host.
- **Reopen if:** container builds are too slow or too heavy for team laptops, or USB flashing from the host proves unreliable.
