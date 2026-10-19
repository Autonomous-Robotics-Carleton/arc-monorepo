# ADR-0044: Update the EE review inputs for custom corners and larger wheels

- **Status:** Proposed (alignment with upstream planning inputs; parts, ratings and layout remain unapproved)
- **Date:** 2026-10-09
- **Deciders:** Kunshpreet Baweja (proposer); component sizing review pending
- **Traces to:** SYS-01, SYS-02, SYS-03, SYS-05, SYS-14, SYS-16, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32, E-10, E-40, E-41, E-43, E-50, E-51, ADR-0040, ADR-0042, ADR-0043, RSK-04, RSK-08, RSK-18, RSK-21, RSK-22
- **Related:** ADR-0034 retains the bought A50S choice; its original small-wheel sizing is historical. This proposal records updated review inputs, not a new controller or motor selection.

## Context

Upstream main at `67392fac4bd2ad705ddf723508b0ed09570f4078` adds accepted custom-corner ADR-0040, vendor envelopes and revised drive/power scenarios. The EE draft previously used smaller wheels and lower mass/current/regen estimates. This brings the EE review in line with those files.

Upstream ADR-0040 is the custom-corner record. No upstream ID or accepted decision body is rewritten.

## Recorded upstream inputs

| Input | Current planning basis | Parent / remaining qualification |
| --- | --- | --- |
| Corner construction | Custom knuckles/uprights, arms and mounts; bought Arrma 6S generic drivetrain/wheel parts, Traxxas fallback | Accepted ADR-0040; geometry and strength review remain open |
| Wheels | TBC 100 mm rolling-diameter model; 42 mm width / 17 mm hex reference | ADR-0040, SYS-01; actual tyre and rolling diameter still need measurement |
| Gearing | 18:1 model: 16/64 stage 1 and 4.5:1 stage 2 | [Architecture](../architecture.md#drive), SYS-01; final fit and tooth/module design remain open |
| Mass | TBC 4–5 kg | SYS-16 / [power scenarios](../budgets/power-scenarios.md); replace with completed mass/CG budget |
| Pack peak at 9 m/s | TBC 36–43 A, including low-voltage loads | SYS-01, SYS-31; pack/input rating basis, not a released limit for every branch |
| Phase current per corner at 1 g | TBC 16–20 A | E-50/E-51, SYS-01; assumes the inherited 4400 kV motor model and equal wheel loading |
| Electrical stop with pack disconnected | TBC 160–200 J total / 185–230 W peak from 9 m/s | SYS-05, SYS-14; normal regen model |
| LiDAR supply | UST-10LX: 12 or 24 V nominal, 10–30 V operating range; up to 150 mA running and 450 mA startup | E-10, SYS-08, SYS-14; now recorded in upstream electrical BOM/power budget. Switch demand is additional |

## Proposed review direction

Use the current upstream scenario as the EE review input, retaining every TBC caveat. Keep the bought A50S controllers, 2 Mbit/s A50S UART telemetry under ADR-0034, combined power/sync motherboard and separate carrier. These updates do not approve new regulator, switch, shunt, resistor, connector or motor selections.

- **Distribution and cooling:** review pack input, anti-spark, switched bus, fuses, contacts, copper, shunts and controller cooling against the updated loads. Pack current and motor phase current are different quantities. The A50S heatsink option remains selected; the published current rating alone does not prove installed cooling.
- **Regen clamp:** the former two 3 Ω resistors in parallel give 1.5 Ω and dissipate `17.2² / 1.5 = 197.2 W` at the illustrative threshold. That is below the updated upper 230 W estimate at that voltage. Treat the pair as an incomplete sizing example; actual resistance, count, threshold, overshoot, pulse rating, heatsink and repeated-stop recovery are TBD. Do not select a replacement here. Splitting 160–200 J gives 80–100 J per resistor only with equal sharing; this is energy allocation, not a certified pulse rating.
- **Runtime:** upstream explicitly leaves the average hard-driving/capacity calculation at the older mass. Recompute it for measured mass, actual motors, gearing and electrical loads before accepting a pack for SYS-03. The starting 5 Ah pack is not a verified runtime solution.
- **Motor model:** the Castle 1010-4400kV stays (ADR-0026). Recheck its kV, pole count/eRPM, current, gearing and mounting against the revised scenario.
- **Packaging:** keep the motherboard estimates in proposed ADR-0043, conditional on current/thermal/connector placement. The new mechanical brief describes a carrier/module stack and additional cooler heights that may exceed the earlier compute-block height estimate; reconcile these with the pinned cooled Antmicro CAD before freezing an installed height.
- **Sensor boards:** the custom corners must provide the sensor/magnet mounts. ADR-0045 sets the inventory at 17 direct sensor runs / 111-conductor minimum TBC; passive 2-layer carriers do not add corner MCUs. Mechanical layout, extra returns and final sensor functions can increase the count.

## Evidence needed to accept sizing

Attach measured mass/CG and rolling diameter, the selected motor/gear/thermal model, complete per-rail load/rating and contact schedule, installed CAD/keep-outs, and clamp pulse/overshoot/repeated-stop measurements. Qualify runtime, sag/handover, sensor timing, UART rate and steering FD and e-stop cases separately through the [fab gate](../reviews/fab-gate.md) and ICD approvals. This document records the updated review baseline; it does not close those decisions or tests.
