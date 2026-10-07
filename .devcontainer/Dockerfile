# ARC dev container (ADR-0029): every toolchain the repo needs, at pinned versions.
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

# Python tools in one venv: west now, Zephyr's requirements below
ENV VIRTUAL_ENV=/opt/venv \
    PATH=/opt/venv/bin:$PATH
RUN python3 -m venv /opt/venv && pip install --no-cache-dir west

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
    && chown -R ubuntu:ubuntu /opt/zephyr-ws

# The image's own user (uid 1000); devcontainers remap it to the host user on Linux
RUN echo "ubuntu ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/ubuntu
USER ubuntu
