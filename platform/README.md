# platform

Everything needed to turn a Jetson into the car's computer: image setup, kernel configuration, containers and system services.

ADR-0016 requires a reproducible image: the JetPack version (7.2.1), kernel configuration (PREEMPT_RT) and package set, pinned here in version control. Nothing is here yet: the image comes after the software setup plan (issue #89), and the experiment containers' limits (SYS-21) are still TBD (`experiments/README.md`, issue #88).

The image covers both builds (ADR-0046): v1.0 runs on the team's Orin Nano Super developer kit, flashed with the `jetson-orin-nano-devkit-super` configuration so its 25 W and MAXN_SUPER modes exist ([builds](../systems/builds.md)); v1.1 runs on the Orin NX on the Antmicro carrier.

`dev-kit/` has setup notes for that dev kit, written for bench use before it became v1.0's computer. They also render on the docs site under Handbook.
