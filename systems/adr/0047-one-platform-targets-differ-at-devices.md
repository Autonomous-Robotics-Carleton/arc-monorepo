# ADR-0047: One platform for every target: targets differ only at the devices

- **Status:** Accepted (2026-10-25), amended by ADR-0049 (the twin runs in lockstep on the simulator's clock, not in real time)
- **Date:** 2026-10-25
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-04, SYS-21, SYS-22, SYS-25, SYS-29, SYS-33, ADR-0021, ADR-0028, ADR-0032
- **Amends:** ADR-0028 (where the targets split) and ADR-0032 (what a simulator package is)

## Context

SYS-33 asks for every software component to build and run from the same source on three kinds of target: simulation, development hardware and the car, with target-specific code limited to board configuration and drivers. ADR-0028 decided how: each target gets a **backend** that implements the car interface (`arc_msgs`), chosen at launch with `target:=sim | replay | car`.

That puts the split between targets high up. Everything below the car interface differs per target: on the car it's the car backend, the sync link, the sync MCU firmware, the motor-controller firmware and the sensor drivers; in simulation it's a simulator backend that publishes the same topics directly. So:

- The code that only runs on the car (the car backend, the sync link's message handling, the firmware's safety logic, the camera-frame pairing in ADR-0021, the arming sequence in ADR-0039) is first tested when the car exists.
- Each simulator backend has to re-implement the car's behaviour to be useful. The working sim on `yufeng/ground-station` shows this: its `car_link.py` re-implements the heartbeat watchdog and brake ramp in Python, a second copy of safety logic.
- The team's goal is a car that works with minimal effort when the hardware arrives (ADR-0046: hardware comes with each funded order, and v1.0 has no hardware in hand yet). With the split at the car interface, the effort lands exactly when the hardware does.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Keep the split at the car interface (ADR-0028 as is) | Simplest simulators; runs faster than real time | The car's own code path (backend, sync link, firmware, drivers) is untested until the car exists; each simulator re-implements car behaviour, including safety |
| **B. Split at the devices: one stack on every target; simulation supplies models of the devices and the world underneath the real firmware and drivers** | The code that ships is the code that's tested; moving to the car means swapping board files and device models, measuring the car's configuration and calibrating the models | More to build first: device models, a board configuration for each target, an interface for the models to attach to; the full stack runs in real time only (the firmware runs on the wall clock) |
| C. Split at the sync link: simulation emulates the sync MCU over its link, without the firmware | Less work than B; the Orin side is exercised fully | The firmware and the motor-controller code are untested; a second implementation of the sync MCU's behaviour, safety included |

## Criteria

1. Moving to the car needs only target files (board configuration, device models, measured car configuration), no code changes above them.
2. Tests run the code that goes on the car, not a copy (ADR-0028, criterion 2).
3. Safety behaviour (SYS-04, SYS-22, SYS-25) is tested through the real code before any hardware exists.
4. A developer can still iterate quickly on control or perception.

## Decision

**Option B.**

**Written once, the same on every target:**

| Part | Lives in |
| --- | --- |
| Ground station | its own project (#71) |
| Platform nodes on the Orin (listed in [`ros/README.md`](../../ros/README.md)) | `ros/` |
| Car backend (the Orin's end of the sync link) and the sensor drivers on the Orin | `ros/` |
| Sync MCU application | `firmware/sync-mcu/app/` |
| Our motor-controller modules | `firmware/vesc/arc/` |
| Experiments | `experiments/` |

**Different per target, and nothing else:**

| Target | Sync MCU | Motor controllers and steering | Sensors | World |
| --- | --- | --- | --- | --- |
| Simulation | `native_sim` board files | Models running our `arc/` modules, on virtual CAN and serial ports | Device models attached where the real device would be (emulated SPI and I2C sensors, the LiDAR's network protocol, numbered camera frames at the firmware's trigger times) | A simulator (Gazebo, the gym) |
| Development hardware | `nucleo_h723zg` board files | A stock A50S, then ours | Breakout boards | A bench |
| The car | `arc_sync` board files | The A50S with our firmware; the moteus-c1 | The car's sensors | The floor |

Plus one **target configuration** per target (addresses, which devices exist) and one **car configuration** per car (IDs, calibration).

> [!NOTE]
> ADR-0054 (2026-10-27) narrows the car configuration: the CAN IDs are the same on every car (`can-command.dbc`) and stay out of it.

**Shortcuts.** Two modes may skip the stack below the car interface, for speed, and are always labelled as shortcuts:

- **Replay** plays logged topics back at the car interface (ADR-0028's replay backend)
- **Interface mode** lets a simulator drive the car interface directly, with the firmware's own `safety.c` compiled in so the envelope and watchdog still apply. It runs faster than real time, for control and learning work

A shortcut is never the only evidence for a requirement.

**Rule.** Code above the split never asks which target it's on. Target names appear only in board files, device models and target configurations, and CI checks it.

## Consequences

- ADR-0028's three kinds of target stand; what changes is where they split. `target:=` chooses where the devices are (`sim`, `devhw`, `car`); the simulated world and the shortcuts are separate choices. [`software/simulation.md`](../software/simulation.md) describes the profiles built from these choices.
- ADR-0032's simulator packages become **world and device-model providers** instead of car-interface backends. The single car description it requires still holds.
- New work, all listed in [`software/platform-plan.md`](../software/platform-plan.md):
  - device models (sensors, motor controllers, steering, power, LiDAR, cameras)
  - a board configuration for the NUCLEO that enables Ethernet and the buses now
  - an interface for the models to reach `native_sim` (the sim link)
  - a separate control interface for the simulator, which doesn't exist on the car
  - the car configuration
  - a CI check for the rule
- The full stack runs in real time, because the firmware runs on the wall clock. A simulator that falls behind real time makes the firmware's timing meaningless. Who keeps time in simulation is an open decision ([`software/simulation.md`](../software/simulation.md), Time).
- The car backend, the sensor drivers and the firmware's message handling move to the front of the work, since every target now needs them.
- **Reopen if:** running the full stack in real time is too slow on the team's laptops to be usable, or a device can't reasonably be modelled where its driver expects it.
