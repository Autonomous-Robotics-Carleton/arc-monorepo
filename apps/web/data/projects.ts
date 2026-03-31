export type Project = {
  id: string;
  title: string;
  description: string;
  tags: string[];
  image?: string;
  github?: string;
};

export const projects: Project[] = [
  {
    id: "autonomy-stack",
    title: "Meow Meow",
    description:
      "Full self-driving software pipeline including perception, planning, and control modules for autonomous racing.",
    tags: ["ROS2", "Python", "C++"],
    github: "#",
  },
  {
    id: "perception",
    title: "Meow Meow 1",
    description:
      "LiDAR and camera fusion for real-time object detection and track boundary recognition at high speeds.",
    tags: ["Computer Vision", "LiDAR", "Deep Learning"],
    github: "#",
  },
  {
    id: "simulator",
    title: "Meow Meow 2",
    description:
      "Custom simulation environment for testing autonomous driving algorithms before deployment on hardware.",
    tags: ["Simulation", "Unreal Engine", "ROS2"],
    github: "#",
  },
  {
    id: "vehicle-platform",
    title: "Meow Meow 3",
    description:
      "Modified 1/10 scale RC car with custom sensor mounts, compute platform, and drive-by-wire system.",
    tags: ["Hardware", "Embedded", "CAD"],
    github: "#",
  },
];
