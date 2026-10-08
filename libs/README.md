# libs

Shared TypeScript packages used by the apps and by repo tooling. Each lib is a pnpm workspace package with its own `project.json`.

| Lib | What |
| --- | --- |
| `systems-model/` | Parses `systems/`: IDs and where they're defined, cross-references, ADR statuses, and the checks behind `nx check systems`. The docs site uses it to link IDs and build `/ids.json`. `nx test systems-model` |
