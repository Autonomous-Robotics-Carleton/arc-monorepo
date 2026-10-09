# systems-model

Reads `systems/` into a model of its IDs, definitions, cross-references and ADRs, and runs the checks behind `nx check systems`. TypeScript that Node runs directly (imports end in `.ts`); there's no build step.

## Who uses it

| User | For |
| --- | --- |
| `tools/check-systems.mts` | `nx check systems`: the checks below |
| `tools/systems-ids.mts` | Looking up IDs from the command line |
| The docs site (`apps/docs`) | Linking every ID in a page to its definition, `/ids.json`, ADR backlinks, the budget pages |

The docs site bundles this library, so code in its `source.config.ts` imports it by relative path, not by package name (`AGENTS.md`).

## What it exports

| Export | What |
| --- | --- |
| `loadSystems(root)` | Reads every Markdown file under `root` (`systems/`) into a `SystemsModel`: files, ID definitions, references and ADRs |
| `checkSystems(model)` | The checks, as a list of `Problem`s (file, line, message) |
| `idKinds`, `idPattern`, `canonicalId` | The ID kinds (`SYS`, `LIM`, `RSK`, `E`, `ADR`), where each is defined, and how they're matched and normalised (`ADR-12` → `ADR-0012`) |
| `idIndex(model)`, `referencedBy(model, id)` | One entry per defined ID; the files that mention an ID |
| `tableRows`, `splitRow`, `plain` | Markdown table helpers for the conventions `systems/` uses |
| `parseCsv` | CSV parsing for `systems/budgets/` |

## The checks

| Check | Fails when |
| --- | --- |
| Links | A relative Markdown link points at a file that doesn't exist |
| IDs | An ID is mentioned but not defined in its home file (`requirements/system.md`, `risks.md`, `bom/electrical.md`, `adr/`) |
| ADR headers | An ADR's header is incomplete, or a supersession isn't recorded on both sides |
| ADR index | `adr/README.md` misses an ADR, or lists it with a different status |
| Verification | A live requirement has no verification method, or no row in `verification/plan.md` |
| Ranges | A range like "ADRs 0001–0019" stops short of the latest ADR |

ADR amendments aren't checked yet.

## Test

```bash
npx nx test systems-model     # node --test on src/**/*.test.ts
npx nx check systems          # the checks, run on systems/
```
