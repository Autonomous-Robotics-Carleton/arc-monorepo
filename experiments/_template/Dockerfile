# Experiment container (SYS-21). The base image, runtime and resource
# limits are TBD: see experiments/README.md. Until the platform image
# exists, this builds on the upstream ROS 2 Jazzy image.
FROM ros:jazzy-ros-base

COPY src/ /experiment/
CMD ["python3", "/experiment/main.py"]
