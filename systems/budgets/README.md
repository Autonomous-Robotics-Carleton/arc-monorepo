# Budgets

One CSV per budget. Each row has: item, value, unit, margin/contingency, source (datasheet link, measured, or estimate), owner.
A budget "closes" when the total plus contingency is within its `SYS` limit.

| File | Limit from | Status |
| --- | --- | --- |
| [`mass.csv`](mass.csv) (mass and CG position) | SYS-16 | Draft: item list seeded; masses and positions come from the block layout |
| [`power.csv`](power.csv) + [`power-scenarios.md`](power-scenarios.md) (loads per rail; operating scenarios) | SYS-03, SYS-05, SYS-14, SYS-30, SYS-31 | Draft: system-level estimates for the EE; rail sizing is the EE's |
| [`bus-load.csv`](bus-load.csv) (command bus, steering bus, controller UART links, Ethernet) | SYS-24 | Draft: all buses within ceiling |
| [`latency.csv`](latency.csv) (sample → timestamped on the Orin, per sensor) | SYS-06, SYS-11, SYS-29 | Draft: estimates; LiDAR, frame cameras and ToF are the hardware-limited items |
| [`cost.csv`](cost.csv) | SYS-17 | Draft: rough ranges, mostly estimates; prices marked by source |
