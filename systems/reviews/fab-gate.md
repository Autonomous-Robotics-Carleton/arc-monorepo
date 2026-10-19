# Fab Gate Checklist

Run before ordering any PCB, machined part, or purchase over the spending threshold. A reviewer from outside the subsystem signs off. Record the result as `reviews/<item>-<rev>.md` and tag the commit.

## All items

- [ ] Every requirement this item satisfies is listed by ID, with its verification method
- [ ] Every interface it touches has an ICD at `Proposed` or better, and the other side's owner has reviewed it
- [ ] Mass, power and cost entries in `budgets/` are updated, and the budgets still close
- [ ] Any risk this item depends on has had its spike test run, or the reason it's acceptable is written down
- [ ] Open `TBD`s affecting this item are listed, each with why it's safe to build anyway
- [ ] For drive telemetry: four independent 2 Mbit/s UART links, full ≥1 kHz coherent status, ≤50% measured load, qualified driver/DMA ownership, source timestamps/clock mapping, installed signal/ground margins and SYS-25 fault handling (SYS-07, SYS-24, SYS-29, ADR-0034). See [UART readiness](a50s-uart-readiness.md). The separate steering FD link retains its 1 / 5 Mbit/s baseline and ≤70% CAN load

## PCBs (add)

- [ ] ERC/DRC clean, or each waiver is explained
- [ ] Connector pinouts checked against the ICD pin by pin; A50S contacts checked using the [arrival procedure](../tests/a50s-arrival.md)
- [ ] Return map / transceiver reference conductors and exact voltage-domain / off-state limits reviewed; ADR-0041 remains Proposed until approved, with [grounding bench evidence](../tests/grounding-and-signal-integrity.md) planned or attached
- [ ] Input-voltage ratings checked against worst-case transients, not nominal
- [ ] Test points on every rail and bus; a bring-up plan exists
- [ ] Fab and assembly files generated from the tagged commit

## Mechanical parts (add)

- [ ] Fit checked in the full assembly, at the extremes of travel (steering lock, suspension bump/droop)
- [ ] Load case and material are written down for parts that carry drive torque or crash loads
