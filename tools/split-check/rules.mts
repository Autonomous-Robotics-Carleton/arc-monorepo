// What the split check looks for, and where target names are allowed
// (ADR-0047: "Code above the split never asks which target it's on. Target
// names appear only in board files, device models and target
// configurations, and CI checks it"). See tools/README.md.
//
// Paths are globs relative to the repo root: `*` matches within one path
// segment, `**` any number of segments. Every path not allowed below is
// checked, so a new ROS package, firmware module or top-level folder is
// covered without touching this file.

// Names that mean "which target am I on". Matched as whole string literals
// ('sim', "car") and as bare words in shell, CMake and Kconfig.
// Targets, worlds and modes: systems/software/simulation.md
// ("Choosing what to run"); boards: ADR-0047.
export const TARGET_NAMES = [
  'sim', 'devhw', 'car', // target:= (ADR-0047)
  'gazebo', 'webots', 'gym', // the simulated world (ADR-0032)
  'replay', 'interface', // the shortcuts (ADR-0047)
  'native_sim', 'nucleo_h723zg', 'arc_sync', // sync MCU boards
];

// Words that also count inside identifiers (is_sim, inSim, SIM_MODE,
// gazebo_world). `car` and `interface` aren't here: they're everyday words
// in this repo (car_config, the car interface).
export const IDENTIFIER_WORDS = [
  'sim', 'simulation', 'simulated', 'simulator', 'devhw',
  'gazebo', 'webots', 'gym', 'replay', 'nucleo',
];

// Identifiers that contain one of those words but don't ask for a target.
export const IDENTIFIER_EXEMPT = [
  'use_sim_time', // ROS's standard clock parameter: platform code reads time through the ROS clock on every target (ADR-0049)
];

// Paths where naming a target is allowed, or that aren't code above the split.
export const ALLOWED: { path: string; why: string }[] = [
  // The places ADR-0047 allows
  { path: 'firmware/sync-mcu/boards/**', why: 'Board definitions (arc_sync)' },
  { path: 'firmware/sync-mcu/app/boards/**', why: 'Per-board configuration of the sync MCU application' },
  { path: 'config/**', why: 'Car configurations (ADR-0051) and target configurations' },
  { path: 'tools/sim/**', why: 'The simulator and the device models' },
  // Launch-time choice of target (ADR-0028, ADR-0047)
  { path: 'ros/src/arc_backend_*/**', why: 'Backends: each one is a target (ADR-0028); covers new backends' },
  { path: 'ros/src/arc_bringup/**', why: 'Bringup: chooses the backend from target:=' },
  // Not code above the split
  { path: 'tools/**', why: "Repo scripts, the firmware-in-the-loop test and this check: they run on developers' machines and in CI, not on the car" },
  { path: 'firmware/vesc/bldc/**', why: 'Upstream VESC firmware (ADR-0033): its hardware targets are upstream code' },
  { path: 'firmware/sync-mcu/app/generated/**', why: 'Generated from systems/icd/ (tools/gen-interfaces.sh)' },
  { path: 'apps/**', why: 'The websites' },
  { path: 'libs/**', why: 'The systems/ parser' },
  { path: 'systems/**', why: 'Requirements and decisions, not code' },
  { path: 'hardware/**', why: 'Board designs' },
  { path: '.github/**', why: 'CI workflows' },
  { path: '.devcontainer/**', why: 'The dev container' },
];

// Known violations in code that can't be fixed in the same change. Each one
// names the line (a piece of its text) and the follow-up that removes it.
// The check warns when an entry no longer matches anything, so it can go.
export const EXCEPTIONS: { path: string; contains: string; followUp: string }[] = [];
