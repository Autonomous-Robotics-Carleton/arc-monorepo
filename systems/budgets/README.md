# Budgets

One CSV per budget. Each row has: item, value, unit, margin/contingency, source (datasheet link, measured, or estimate), owner.
A budget "closes" when the total plus contingency is within its `SYS` limit.

| File | Limit from | Status |
| --- | --- | --- |
| `mass.csv` (mass and CG position) | SYS-16 | TBD |
| `power.csv` (per rail: peak and continuous) | SYS-03, ICD power-rails | TBD |
| [`bus-load.csv`](bus-load.csv) (command bus, two FD telemetry buses, Ethernet) | SYS-24 | Draft: all buses within ceiling |
| [`latency.csv`](latency.csv) (sample → timestamped on the Orin, per sensor) | SYS-06, SYS-11, SYS-29 | Draft: estimates; LiDAR, frame cameras and ToF are the hardware-limited items |
| [`cost.csv`](cost.csv) | SYS-17 | Draft: rough ranges, mostly estimates; prices marked by source |
