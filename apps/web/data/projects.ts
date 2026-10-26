// The car's subsystems, listed on the Robots page with a link to where each lives in the repo
export type Project = {
  id: string;
  title: string;
  description: string;
  tags: string[];
  github?: string;
};

const repo = "https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/tree/main";

export const projects: Project[] = [
  {
    id: "systems",
    title: "Systems engineering",
    description:
      "Requirements, decision records, interface documents, budgets and risks: the source of truth every other part of the car is built against.",
    tags: ["Requirements", "ADRs", "ICDs"],
    github: `${repo}/systems`,
  },
  {
    id: "autonomy-stack",
    title: "Autonomy stack",
    description:
      "A ROS 2 Jazzy workspace for the Orin. One car interface sits over a backend per target (Gazebo, Webots, a gym environment, log replay and the car itself), so the same stack runs in simulation and on hardware.",
    tags: ["ROS 2", "C++", "Python", "Simulation"],
    github: `${repo}/ros`,
  },
  {
    id: "sync-mcu",
    title: "Sync MCU firmware",
    description:
      "Zephyr firmware for the STM32H723. It owns the time base, runs the heartbeat watchdog and safety envelope, drives the CAN buses and streams every sample to the Orin within a quarter millisecond.",
    tags: ["Zephyr", "Embedded", "MAVLink"],
    github: `${repo}/firmware/sync-mcu`,
  },
  {
    id: "motor-control",
    title: "Motor control",
    description:
      "VESC firmware for the four A50S controllers with our own modules: an e-stop brake ramp, a command timeout, a speed limit and 1 kHz telemetry back to the sync MCU.",
    tags: ["VESC", "FOC", "CAN"],
    github: `${repo}/firmware/vesc`,
  },
  {
    id: "boards",
    title: "Boards",
    description:
      "The motherboard that carries the sync and power circuitry, and the small sensor boards for the IMUs, angle sensors and ToF, designed in Altium and assembled in house.",
    tags: ["PCB", "Altium", "Power"],
    github: `${repo}/hardware`,
  },
];
