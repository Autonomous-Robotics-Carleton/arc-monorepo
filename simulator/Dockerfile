# ARC simulator: ROS 2 Jazzy + Gazebo Harmonic, software rendering (Mesa llvmpipe).
# Build and run from simulator/:  ./run.sh [--bridge | --gui3d]
FROM ros:jazzy-ros-base

RUN apt-get update && apt-get install -y --no-install-recommends \
      ros-jazzy-ros-gz \
      ros-jazzy-xacro \
      ros-jazzy-robot-state-publisher \
      libgl1-mesa-dri \
      python3-numpy \
      python3-pil \
      python3-websockets \
      python3-yaml \
      xvfb fluxbox x11vnc novnc websockify \
    && rm -rf /var/lib/apt/lists/*

# No GPU in Docker on macOS: force Mesa's CPU renderer.
ENV LIBGL_ALWAYS_SOFTWARE=1 \
    GALLIUM_DRIVER=llvmpipe

# Own DDS domain, so the sim never discovers the real car's graph (default 0) even
# with host networking. Applies to every process, including docker exec.
ENV ROS_DOMAIN_ID=42

WORKDIR /sim_ws
COPY src ./src
RUN . /opt/ros/jazzy/setup.sh && colcon build

COPY entrypoint.sh start-sim.sh /
RUN chmod +x /entrypoint.sh /start-sim.sh
ENTRYPOINT ["/entrypoint.sh"]
# BRIDGE=1 serves the ground-station bridge on 8765; GUI=3d the noVNC Gazebo GUI on 6080. See start-sim.sh.
CMD ["/start-sim.sh"]
