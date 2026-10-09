# AGENTS.md

Guidance for AI coding agents (and people) working in this repo. Human-facing setup and conventions are in [`.github/CONTRIBUTING.md`](.github/CONTRIBUTING.md); this file is the short version plus what an agent needs to avoid mistakes.

## What this repo is

The monorepo for ARC (Autonomous Robotics Carleton): a 1/10-scale 4WD autonomy research car. It holds the car's systems engineering, the code that will run on it, its hardware designs, the docs site and the club website. It's public and MIT licensed, except `firmware/vesc/` (GPL-3.0, the vendored VESC firmware).

| Path | What | Format |
| --- | --- | --- |
| `systems/` | **Source of truth for the car**: requirements, decisions (ADRs), interfaces (ICDs), budgets, risks, BOM, verification. Read [`systems/README.md`](systems/README.md) first | Markdown + CSV |
| `ros/` | ROS 2 Jazzy workspace for the Orin NX: the car interface, a backend per target (Gazebo, Webots, gym, replay, car) and `arc_bringup` (`target:=`, `sim:=`, `sensors:=`) | colcon |
| `firmware/` | `sync-mcu/` (Zephyr; builds for native_sim, NUCLEO-H723ZG and the car's `arc_sync`) and `vesc/` (upstream VESC firmware for the A50S motor controllers, plus our modules in `vesc/arc/`) | west, make, CMake |
| `experiments/` | Team experiments and their template (scaffold) | |
| `.devcontainer/` | The dev container every firmware and ROS build runs in; CI uses the same image | Docker |
| `hardware/` | Board designs and mechanical exports (no boards started yet) | |
| `platform/` | Jetson image and services; `dev-kit/` has Orin Nano bench notes | |
| `tools/` | Repo scripts: the `systems/` checker and ID lookup, interface code generation, VESC vendoring, the firmware-in-the-loop test | Node, Bash |
| `libs/systems-model/` | Parser for `systems/`: IDs, definitions, references, ADRs, checks | TypeScript |
| `apps/docs/` | Docs site (Fumadocs on Next.js), docs.arcarleton.ca | MDX |
| `apps/web/` | Club website (Next.js); its docs are in `apps/web/docs/` | |

## Commands

Node 22 (`.nvmrc`); pnpm via `corepack enable`. It's an Nx workspace: run things through `npx nx`.

```bash
pnpm install
npx nx show projects                     # docs, web, systems, systems-model, sync-mcu, vesc, ros, firmware-in-loop
npx nx affected -t lint test check build # what CI runs, for your changes
npx nx check systems                     # validate systems/ (links, IDs, ADRs, verification)
npx nx test systems-model
npx nx dev docs                          # http://localhost:3000
node tools/systems-ids.mts SYS-04 ADR-0012   # look up IDs (or a kind: RSK)
```

Firmware and ROS are built in the dev container (`.devcontainer/`, ADR-0029: the supported path, and what CI uses):

```bash
npx nx build sync-mcu && npx nx test sync-mcu   # three boards in one build; twister on native_sim
npx nx build vesc && npx nx test vesc           # A50S firmware; host tests for vesc/arc/
npx nx build ros && npx nx test ros             # colcon; launch tests per backend, sync-link round trip
npx nx test firmware-in-loop                    # native_sim firmware and ROS link up over UDP
tools/gen-interfaces.sh                         # regenerate code from systems/icd/ (--check in CI)
```

## Rules for `systems/`

- **It wins.** If `architecture.md`, a README, code or anything else disagrees with a requirement, ADR or ICD in `systems/`, the `systems/` file is right. Flag the conflict; don't silently pick one.
- **IDs**: `SYS-nn` and `LIM-nn` (requirements/system.md), `RSK-nn` (risks.md), `E-nn` (bom/electrical.md), `ADR-nnnn` (one file per decision in adr/). IDs are never reused; a dropped item keeps its row marked *Deleted*. Every mention of an ID must refer to one that's defined; `nx check systems` enforces it.
- **Every number has a parent requirement.** Unknown values are written `TBD` (no value yet) or `TBC` (proposed, not confirmed), never "about".
- **Never edit an accepted ADR's decision.** Write a new ADR that supersedes it, and record the supersession on both sides (`Status: Superseded by ADR-x` / `Supersedes: ADR-y`), plus the status in `adr/README.md`.
- **Don't make engineering decisions.** Aligning a doc with decisions already recorded is fine; choosing a part, value or design is the owner's call. Record new questions as open items instead.
- Keep `systems/` free of site config: plain Markdown that reads well on GitHub. Use GitHub alerts (`> [!NOTE]`) rather than JSX.

## Firmware, ROS and generated code

- **Generated code is never edited by hand.** `tools/gen-interfaces.sh` generates C from `systems/icd/sync-link.xml` (MAVLink 2) into `firmware/sync-mcu/app/generated/arc_mavlink/` and `ros/src/arc_backend_car/include/arc_mavlink/`, and from `systems/icd/can-command.dbc` into `firmware/sync-mcu/app/generated/can_command/`. Change the schema, regenerate, commit both; `nx check sync-mcu` and `nx check ros` fail on stale code.
- **`firmware/vesc/bldc/` is upstream VESC firmware, GPL-3.0** (ADR-0033). Our changes are ordinary commits there; upstream updates go through `tools/vesc-upstream.sh update <ref>` in a PR of their own. Never copy code from `firmware/vesc/` into MIT parts of the repo. Don't touch its USB, CAN or firmware-upload code: a broken build can then only be recovered over SWD (RSK-20).
- **Safety functions** (e-stop routine, watchdog, safety envelope) stay fail-safe until implemented, and every change gets its own tests and a second reviewer.
- **The dev container image is tagged by the hash of its inputs** (`.devcontainer/**`, `west.yml`); CI builds and smoke-tests a new one when they change, and runs the affected firmware and ROS projects in it. A change to `.devcontainer/` alone affects no Nx project, so run `npx nx run-many -t build test -p sync-mcu vesc ros firmware-in-loop` in the new image yourself. Fork PRs run in `:latest`.

Gotchas:
- With rootless Docker, the container's root is your own user: run containers with `--user root` to write to the repo, and never `chown` files to uid 1000 inside one (that maps to a different user on the host, and git can no longer delete them).
- Upstream VESC's own `.gitignore` ignores `tools` and `.vscode`, which it also tracks; `tools/vesc-upstream.sh` force-adds on import for this reason.

## Docs site

- Pages come from several places, declared in `apps/docs/source.config.ts` and combined in `apps/docs/lib/source.ts`: `apps/docs/content/docs/` (index, handbook, car topology; MDX), `systems/` (→ `/docs/car`; its sidebar is `carSidebar` in `lib/source.ts`), `apps/web/docs/`, `platform/dev-kit/`, `.github/CONTRIBUTING.md`.
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

- Open work is in GitHub issues, indexed in #110 and labelled by area (`firmware`, `ros`, `electrical`, `mechanical`, `ground-station`, `safety`) plus `needs-decision` / `blocked`. No ground-station software exists yet (#71).
- **Every new issue goes in #110.** A workflow (`.github/workflows/issue-index.yml`) adds it under "New, not sorted yet" with its labels. Whoever opened it then moves the line into its area section, with what it waits on (`- #123 (after #75)`), and adds the area label if it's missing.
- Branch from `main`: `yourname/<type>/<issue>-<short-name>` (issue number optional).
- [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/): `type(scope): summary`, imperative, ≤ 72 chars. Scope is usually the project: `systems`, `systems-model`, `docs`, `web`, `ros`, `sync-mcu`, `vesc`, `firmware-in-loop`, `tools`, `devcontainer`, `experiments`, `hardware`, `platform`, `ci`. One logical change per commit.
- Rebase on `origin/main`; never merge `main` into a branch, and don't use GitHub's "Update branch" (it creates a merge commit).
- PRs use the template in `.github/pull_request_template.md`. They're merged **once**, with "Rebase and merge" (or "Squash and merge" for messy branches). If a PR's commits are already on `main`, close it instead.
- Check `git status` before committing: files already staged by `git mv` or `git rm` get swept into the next commit.
