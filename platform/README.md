# platform

Everything needed to turn a Jetson into the car's computer: image setup, kernel configuration, containers and system services.

ADR-0016 requires a reproducible image: the JetPack version (7.2.1), kernel configuration (PREEMPT_RT) and package set, pinned here in version control. Nothing is here yet: the image comes after the software setup plan (issue #89), and the experiment containers' limits (SYS-21) are still TBD (`experiments/README.md`, issue #88).

`dev-kit/` has setup notes for the team's Orin Nano dev kit (bench work, not the car). They also render on the docs site under Handbook.
