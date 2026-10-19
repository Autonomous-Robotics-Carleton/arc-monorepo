# ADR-0042: Power and sync MCU share one motherboard PCB

- **Status:** Accepted (circuit design and layout remain draft)
- **Date:** 2026-10-09
- **Deciders:** Shrikar Vempati, Kunshpreet Baweja
- **Traces to:** SYS-05, SYS-07, SYS-19, SYS-24, SYS-30, SYS-31, SYS-32, E-30, E-41, E-46, ADR-0011, ADR-0022, ADR-0025

## Context

The BOM, handoff and mechanical brief previously described separate power and sync boards, with a stacking header. The EE proposed developing one combined PCB; a later two-PCB implementation remains possible. This changes packaging, while the recorded power, timing, communications and safety functions still apply.

## Decision

1. Develop **one motherboard PCB** containing E-41 power circuitry, E-46 cell monitoring and E-30 sync MCU circuitry. Keep E-30 and E-41 as distinct functional BOM IDs; their quantities do not represent separate PCBs.
2. Keep the **Jetson carrier E-02 separate**, based on Antmicro under ADR-0025. Its imported CAD stays in `hardware/orin-carrier/`; the combined board's work lives in `hardware/motherboard/`.
3. Power/sync signals become internal PCB connections. Retain the logical contract in [power-sync-stack](../icd/power-sync-stack.md); no stacking-header connector is required for the current plan. A future split needs a recorded packaging change and a reviewed physical interface.
4. Keep the bought A50S controllers separate. The bus clamp still requires placement/interface qualification; this decision does not assign them to a PCB or approve a circuit.

## Consequences and open items

- Sync and power component lists describe two sections of the same motherboard. Mounting, cost and mass estimates must avoid counting two bare boards.
- Monitor transport, grounding/isolation, rail ratings, part selection, pin mapping, board envelope, thermal behavior and return-current control remain TBD/TBC as already recorded. ADR-0022 and ADR-0041 remain proposals.
- The e-stop and its hardware torque cut (SYS-05, ADR-0012) must still work with the sync MCU reset or unpowered.
- No schematic, layout, fabrication release or technical interface sign-off is implied by this packaging instruction. Combined-board verification and the fab gate remain required.
