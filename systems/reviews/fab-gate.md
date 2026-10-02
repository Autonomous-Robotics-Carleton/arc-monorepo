# Fab Gate Checklist

Run before ordering any PCB, machined part, or purchase over the spending threshold. A reviewer from outside the subsystem signs off. Record the result as `reviews/<item>-<rev>.md` and tag the commit.

## All items

- [ ] Every requirement this item satisfies is listed by ID, with its verification method
- [ ] Every interface it touches has an ICD at `Proposed` or better, and the other side's owner has reviewed it
- [ ] Mass, power and cost entries in `budgets/` are updated, and the budgets still close
- [ ] Any risk this item depends on has had its spike test run, or the reason it's acceptable is written down
- [ ] Open `TBD`s affecting this item are listed, each with why it's safe to build anyway

## PCBs (add)

- [ ] ERC/DRC clean, or each waiver is explained
- [ ] Connector pinouts checked against the ICD pin by pin
- [ ] Input-voltage ratings checked against worst-case transients, not nominal
- [ ] Test points on every rail and bus; a bring-up plan exists
- [ ] Fab and assembly files generated from the tagged commit

## Mechanical parts (add)

- [ ] Fit checked in the full assembly, at the extremes of travel (steering lock, suspension bump/droop)
- [ ] Load case and material are written down for parts that carry drive torque or crash loads
