# AGENTS.md

Guidance for AI coding agents (and people) working in this repo. Human-facing setup and conventions are in [`.github/CONTRIBUTING.md`](.github/CONTRIBUTING.md); this file is the short version plus what an agent needs to avoid mistakes.

## What this repo is

The monorepo for ARC (Autonomous Robotics Carleton): a 1/10-scale 4WD autonomy research car. It holds the car's systems engineering, the code that will run on it, its hardware designs, the docs site and the club website. It's public and MIT licensed.

| Path | What | Format |
| --- | --- | --- |
| `systems/` | **Source of truth for the car**: requirements, decisions (ADRs), interfaces (ICDs), budgets, risks, BOM, verification. Read [`systems/README.md`](systems/README.md) first | Markdown + CSV |
| `ros/` | ROS 2 Jazzy workspace for the Orin NX (empty so far) | colcon |
| `firmware/` | `sync-mcu/` (Zephyr) and `vesc/` firmware (empty so far) | west / VESC |
| `hardware/` | Board designs and mechanical exports (empty so far) | |
| `platform/` | Jetson image and services; `dev-kit/` has Orin Nano bench notes | |
| `tools/` | Repo scripts, e.g. the `systems/` checker and ID lookup | Node (TypeScript) |
| `libs/systems-model/` | Parser for `systems/`: IDs, definitions, references, ADRs, checks | TypeScript |
| `apps/docs/` | Docs site (Fumadocs on Next.js), docs.arcarleton.ca | MDX |
| `apps/web/` | Club website (Next.js); its docs are in `apps/web/docs/` | |

## Commands

Node 22 (`.nvmrc`); pnpm via `corepack enable`. It's an Nx workspace: run things through `npx nx`.

```bash
pnpm install
npx nx show projects                     # docs, web, systems, systems-model, ros, sync-mcu
npx nx affected -t lint test check build # what CI runs, for your changes
npx nx check systems                     # validate systems/ (links, IDs, ADRs, verification)
npx nx test systems-model
npx nx dev docs                          # http://localhost:3000
node tools/systems-ids.mts SYS-04 ADR-0012   # look up IDs (or a kind: RSK)
```

## Rules for `systems/`

- **It wins.** If `architecture.md`, a README, code or anything else disagrees with a requirement, ADR or ICD in `systems/`, the `systems/` file is right. Flag the conflict; don't silently pick one.
- **IDs**: `SYS-nn` and `LIM-nn` (requirements/system.md), `RSK-nn` (risks.md), `E-nn` (bom/electrical.md), `ADR-nnnn` (one file per decision in adr/). IDs are never reused; a dropped item keeps its row marked *Deleted*. Every mention of an ID must refer to one that's defined; `nx check systems` enforces it.
- **Every number has a parent requirement.** Unknown values are written `TBD` (no value yet) or `TBC` (proposed, not confirmed), never "about".
- **Never edit an accepted ADR's decision.** Write a new ADR that supersedes it, and record the supersession on both sides (`Status: Superseded by ADR-x` / `Supersedes: ADR-y`), plus the status in `adr/README.md`.
- **Don't make engineering decisions.** Aligning a doc with decisions already recorded is fine; choosing a part, value or design is the owner's call. Record new questions as open items instead.
- Keep `systems/` free of site config: plain Markdown that reads well on GitHub. Use GitHub alerts (`> [!NOTE]`) rather than JSX.

## Docs site

- Pages come from several places and are mounted in `apps/docs/lib/source.ts`: `apps/docs/content/docs/` (handbook, MDX), `systems/` (→ `/docs/car`), `apps/web/docs/`, `platform/dev-kit/`, `.github/CONTRIBUTING.md`.
- **Format:** MDX for site-written pages; plain Markdown for `systems/` and `CONTRIBUTING.md`. Don't convert between them.
- **Links are written as relative file paths** (`../systems/risks.md`) so they work on GitHub too; the site turns them into page URLs, or GitHub links for files that aren't pages.
- **IDs are linked automatically** (remark plugin in `apps/docs/lib/remark-system-ids.ts`); don't hand-link them.
- For LLMs: `/llms.txt`, `/llms-full.txt`, `/ids.json`, and every page as Markdown at its URL + `.md`.

Gotchas:
- After changing a remark plugin, delete `apps/docs/.next`: compiled pages are cached by content.
- Don't run `nx build docs` (or `nx affected … build`) while `nx dev docs` is running: both write `apps/docs/.next`, and the dev server starts returning 500s. Stop it, or restart it after with a clean `.next`.
- Never point a docs collection at the repo root: fumadocs watches each collection's directory in dev, and the root includes `node_modules`.
- `libs/systems-model` is TypeScript that Node runs directly (imports use `.ts`). The site bundles it; `source.config.ts` code must import it by relative path, not by package name.
- The production server doesn't run fumadocs-mdx; anything that reads files must happen at build time (static routes, prerendered pages).

## Git and PRs

- Branch from `main`: `yourname/<type>/<issue>-<short-name>` (issue number optional).
- [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/): `type(scope): summary`, imperative, ≤ 72 chars. Scope is usually the project: `systems`, `docs`, `web`, `ros`, `sync-mcu`, `vesc`, `ci`. One logical change per commit.
- Don't add AI co-author trailers or "generated with" lines to commits or PRs.
- Rebase on `origin/main`; never merge `main` into a branch, and don't use GitHub's "Update branch" (it creates a merge commit).
- PRs use the template in `.github/pull_request_template.md`. They're merged **once**, with "Rebase and merge" (or "Squash and merge" for messy branches). If a PR's commits are already on `main`, close it instead.
- Check `git status` before committing: files already staged by `git mv` or `git rm` get swept into the next commit.
