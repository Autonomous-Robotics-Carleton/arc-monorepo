# ARC dev container (ADR-0029): every toolchain the repo needs, at pinned versions:
# Zephyr (firmware), ROS 2 Jazzy (platform software), Node (docs, tooling).
# Used by VS Code / JetBrains / the devcontainer CLI on Linux, macOS and
# Windows, and by CI. Built for linux/amd64 only: Zephyr's native_sim needs
# 32-bit host libraries (gcc-multilib) that don't exist on arm64. Apple
# Silicon Macs run it under emulation.
#
# Build context is the repo root (see devcontainer.json).

FROM ubuntu:24.04

ARG ZEPHYR_SDK_VERSION=1.0.1
ARG NODE_VERSION=22.23.3

ENV DEBIAN_FRONTEND=noninteractive \
    LANG=C.UTF-8

# Zephyr host dependencies (Zephyr 4.4 getting-started guide), minus the GUI ones
RUN apt-get update && apt-get install -y --no-install-recommends \
      ca-certificates curl wget git sudo less \
      cmake ninja-build gperf ccache dfu-util device-tree-compiler \
      python3-dev python3-venv python3-pip \
      xz-utils file make gcc g++ gcc-multilib g++-multilib libmagic1 \
    && rm -rf /var/lib/apt/lists/*

# Node (docs site, tooling) with pnpm through corepack, pinned by package.json
RUN curl -fsSL "https://nodejs.org/dist/v${NODE_VERSION}/node-v${NODE_VERSION}-linux-x64.tar.xz" \
      | tar -xJ -C /usr/local --strip-components=1 \
    && corepack enable

# Python tools in one venv: west now, Zephyr's requirements below (base,
# build-test, and run-test, which twister needs). It sees the system site
# packages, so ROS's Python libraries (rclpy, launch, the rosidl
# generators) still import from it.
ENV VIRTUAL_ENV=/opt/venv \
    PATH=/opt/venv/bin:$PATH
RUN python3 -m venv --system-site-packages /opt/venv && pip install --no-cache-dir west

# Zephyr SDK: the minimal package plus only the Arm toolchain
ENV ZEPHYR_SDK_INSTALL_DIR=/opt/zephyr-sdk-${ZEPHYR_SDK_VERSION}
RUN cd /opt \
    && wget -q "https://github.com/zephyrproject-rtos/sdk-ng/releases/download/v${ZEPHYR_SDK_VERSION}/zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    && tar -xJf "zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    && rm "zephyr-sdk-${ZEPHYR_SDK_VERSION}_linux-x86_64_minimal.tar.xz" \
    && cd "${ZEPHYR_SDK_INSTALL_DIR}" && ./setup.sh -t arm-zephyr-eabi

# Zephyr and its modules at the version pinned in firmware/sync-mcu/west.yml.
# West finds this workspace from anywhere through ZEPHYR_BASE.
ENV ZEPHYR_BASE=/opt/zephyr-ws/zephyr
COPY firmware/sync-mcu/west.yml /opt/zephyr-ws/manifest/west.yml
RUN cd /opt/zephyr-ws \
    && git -C manifest init -q \
    && west init -l manifest \
    && west update --narrow --fetch-opt=--depth=1 \
    && pip install --no-cache-dir \
         -r zephyr/scripts/requirements-base.txt \
         -r zephyr/scripts/requirements-build-test.txt \
         -r zephyr/scripts/requirements-run-test.txt \
    && chown -R ubuntu:ubuntu /opt/zephyr-ws

# ROS 2 Jazzy (ADR-0016): the base install, the build tools (colcon, rosdep),
# MCAP bags for log replay, and launch_testing. The simulators come later
# (ADR-0032, phase 5). Near the end so changing it doesn't rebuild Zephyr.
ARG ROS_APT_SOURCE_VERSION=1.3.0
RUN curl -fsSL -o /tmp/ros2-apt-source.deb \
      "https://github.com/ros-infrastructure/ros-apt-source/releases/download/${ROS_APT_SOURCE_VERSION}/ros2-apt-source_${ROS_APT_SOURCE_VERSION}.noble_all.deb" \
    && dpkg -i /tmp/ros2-apt-source.deb && rm /tmp/ros2-apt-source.deb \
    && apt-get update && apt-get install -y --no-install-recommends \
      ros-jazzy-ros-base ros-dev-tools \
      ros-jazzy-rosbag2-storage-mcap ros-jazzy-launch-testing-ament-cmake \
    && rm -rf /var/lib/apt/lists/*

# Code generators for the machine-readable ICDs (tools/gen-interfaces.sh):
# mavgen for the sync link (ADR-0031), cantools for the command bus
ARG PYMAVLINK_VERSION=2.4.50
ARG CANTOOLS_VERSION=44.2.1
RUN pip install --no-cache-dir "pymavlink==${PYMAVLINK_VERSION}" "cantools==${CANTOOLS_VERSION}"

# The motor controllers' VESC firmware (firmware/vesc/, ADR-0033, ADR-0034) builds
# with Arm's GCC 7 2018-q2, the compiler upstream release_7_00 expects
ARG VESC_GCC_SHA256=bb17109f0ee697254a5d4ae6e5e01440e3ea8f0277f2e8169bf95d07c7d5fe69
RUN curl -fsSL -o /tmp/gcc-arm.tar.bz2 \
      "https://developer.arm.com/-/media/Files/downloads/gnu-rm/7-2018q2/gcc-arm-none-eabi-7-2018-q2-update-linux.tar.bz2" \
    && echo "${VESC_GCC_SHA256}  /tmp/gcc-arm.tar.bz2" | sha256sum -c - \
    && tar -xjf /tmp/gcc-arm.tar.bz2 -C /opt && rm /tmp/gcc-arm.tar.bz2
ENV PATH=$PATH:/opt/gcc-arm-none-eabi-7-2018-q2-update/bin

# The image's own user (uid 1000); devcontainers remap it to the host user on Linux.
# CI and rootless Docker run as root instead: without safe.directory, git
# refuses the ubuntu-owned Zephyr repos and west loses its build commands.
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu \
    && git config --system --add safe.directory '*' \
    && echo 'source /opt/ros/jazzy/setup.bash' >> /home/ubuntu/.bashrc
USER ubuntu
