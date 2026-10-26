// ARC v1, for the Robots page. Facts come from systems/ (the source of truth):
// builds.md and ADR-0046 for the two builds, software/architecture.md for the computers,
// bom/electrical.md for the sensors. Update here when those change.

export type Computer = {
  name: string;
  hardware: string;
  job: string;
  link: string; // how it talks to the next computer in the chain
};

export const COMPUTERS: Computer[] = [
  {
    name: 'Operator laptop',
    hardware: 'Any laptop + gamepad',
    job: 'Drives the car, keeps it alive with a heartbeat, watches telemetry and pulls logs.',
    link: 'Wi-Fi',
  },
  {
    name: 'Jetson Orin',
    hardware: 'Orin Nano 8 GB (v1.0) · Orin NX 16 GB (v1.1)',
    job: 'ROS 2 platform software, plus each experiment in its own container.',
    link: 'Ethernet',
  },
  {
    name: 'Sync MCU',
    hardware: 'STM32H723 · Zephyr',
    job: "Owns the car's clock, reads every vehicle sensor, enforces the safety envelope and watchdog.",
    link: 'CAN',
  },
  {
    name: 'Motor + steering',
    hardware: '4 × A50S (VESC) · moteus-c1',
    job: 'One controller per wheel and one for steering. Each brakes itself on e-stop or a lost command.',
    link: '',
  },
];

export const SENSORS = [
  { name: '2D LiDAR', part: 'Hokuyo UST-20LX (v1.0) · UST-10LX (v1.1)', detail: '40 Hz, 0.25°, 270° field of view' },
  { name: 'Forward stereo', part: 'Arducam AR0234 pair', detail: 'Global shutter, hardware-triggered' },
  { name: 'Ground speed', part: 'Downward camera', detail: 'Frame camera on v1.0, Prophesee GenX320 event camera on v1.1' },
  { name: 'IMUs', part: '2 × ISM330DHCX', detail: 'One at the centre of gravity, one at the front axle' },
  { name: 'Angle sensors', part: '10 × AS5047P', detail: 'Wheel speed, suspension travel and steer angle at each front knuckle' },
  { name: 'Ride height', part: '5 × time-of-flight', detail: 'One per corner, plus one to scale the ground-speed camera' },
  { name: 'Side / rear cameras', part: 'Quad OV9281 (v1.1)', detail: 'Global-shutter mono, not fitted on v1.0' },
] as const;

// Rows where the builds differ (systems/builds.md); everything else is in both
export const BUILDS = [
  { item: 'Compute', v10: 'Orin Nano 8 GB, on loan from our first car', v11: 'Orin NX 16 GB' },
  { item: 'Carrier', v10: 'Developer kit', v11: 'Antmicro carrier' },
  { item: 'SSD', v10: '1 TB', v11: '4 TB' },
  { item: 'LiDAR', v10: 'Hokuyo UST-20LX, on loan', v11: 'Hokuyo UST-10LX' },
  { item: 'Cameras', v10: 'Stereo pair + ground camera', v11: '+ quad side/rear + forward event camera' },
  { item: 'Learned policies', v10: 'Classical autonomy only (8 GB)', v11: 'Alongside the platform stack' },
] as const;
