# Contributing to ARC

Thanks for helping build the car. This page covers how to set up the repo, how to get a change merged, and the rules that keep the car's documentation trustworthy.

Everyone taking part follows the [Code of Conduct](../CODE_OF_CONDUCT.md). Report unacceptable behaviour to `devlead@arcarleton.ca` or `financeadmin@arcarleton.ca`.

## Where things live

| Folder | What |
| --- | --- |
| `systems/` | Source of truth for the car: requirements, decisions (ADRs), interfaces (ICDs), budgets, risks. Read [`systems/README.md`](../systems/README.md) before changing anything here. |
| `ros/` | ROS 2 workspace for the Orin |
| `firmware/` | Sync MCU (Zephyr) and VESC firmware |
| `hardware/` | Board designs and mechanical exports |
| `platform/` | Jetson image and system services |
| `tools/` | Developer scripts |
| `libs/` | Shared TypeScript packages |
| `experiments/` | Team experiments and their template |
| `.devcontainer/` | The dev container firmware and ROS build in |
| `apps/docs`, `apps/web` | Docs site and public website |

## Setup

You need Git and Node.js 22 (`nvm use` picks it up from `.nvmrc`).

```bash
git clone https://github.com/Autonomous-Robotics-Carleton/arc-monorepo.git
cd arc-monorepo
corepack enable   # provides the pinned pnpm version
pnpm install
```

The repo is an [Nx](https://nx.dev) workspace. Every project is driven through Nx:

```bash
npx nx show projects              # list projects
npx nx dev docs                   # docs site on http://localhost:3000
npx nx dev web                    # website on http://localhost:3001
npx nx check systems              # check links and IDs in systems/
npx nx affected -t lint test check build   # what CI runs, for whatever you changed
```

Firmware and ROS (`sync-mcu`, `vesc`, `ros`, `firmware-in-loop`) are built in the dev container (ADR-0029: the supported path, and what CI uses): see **Dev setup** in the handbook.

## Making a change

1. **Start from an issue.** Open work is indexed in [#110](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/110); issues are labelled by area (`firmware`, `ros`, `electrical`, `mechanical`, `ground-station`, `safety`), and `needs-decision` or `blocked` when they can't start yet. For a new issue, use the issue forms: *Failure report* for any hardware or firmware failure during a run, *Bug* for software, *Proposal* for features and design changes. A new issue is added to #110 automatically, under "New, not sorted yet"; move it into its area there.
2. **Branch from `main`** as `yourname/<type>/<issue>-<short-name>`, e.g. `jdoe/feat/42-imu-driver`. Leave out the issue number if there isn't one.
3. **Commit** using [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/): `type(scope): summary` in the imperative, under 72 characters. The scope is usually the project: `systems`, `systems-model`, `docs`, `web`, `ros`, `sync-mcu`, `vesc`, `firmware-in-loop`, `tools`, `devcontainer`, `experiments`, `hardware`, `platform`, `ci`. One logical change per commit.

   ```
   docs(systems): ADR-0034 off-the-shelf motor controllers
   feat(sync-mcu): timestamp wheel encoder edges
   fix(web): sponsor logos overflow on mobile
   ```

4. **Stay current by rebasing**, not merging: `git fetch origin && git rebase origin/main`, then `git push --force-with-lease`.
5. **Open a PR** and fill in the template. Run `npx nx affected -t lint test check build` first. A change to `systems/` also affects the firmware and ROS projects, so run it in the dev container, or on the host with `--exclude='tag:env:dev-container'`; CI runs both.

## How PRs are accepted

- CI passes.
- A code owner approves (see [`.github/CODEOWNERS`](CODEOWNERS)).
- A change to a requirement, ICD or budget links the ADR or issue that justifies it.
- A change to an ICD is approved by the owners on both sides of the interface.
- A change to a safety function (e-stop, watchdog, safety envelope) has its own tests and a second reviewer.

The maintainer merges with **Rebase and merge** when every commit is a clean, meaningful step, and **Squash and merge** otherwise. There are no merge commits on `main`.

## Rules for `systems/`

The full rules are in [`systems/README.md`](../systems/README.md). The short version:

- Every number has a parent requirement. Unknown values are `TBD` (no value yet) or `TBC` (proposed, not confirmed), never "about".
- IDs (`SYS-nn`, `ADR-nnnn`, `RSK-nn`, …) are never reused.
- An accepted ADR's decision is never edited. Write a new ADR that supersedes it.
- Nothing is fabbed, machined or bought until it passes `systems/reviews/fab-gate.md`.

## Rules for firmware

- Generated code (`firmware/sync-mcu/app/generated/`, `ros/src/arc_backend_car/include/arc_mavlink/`) is never edited by hand: change the schema in `systems/icd/` and run `tools/gen-interfaces.sh`.
- `firmware/vesc/` is GPL-3.0 (upstream's licence; ADR-0033); the rest of the repo is MIT. Don't copy code from it into other parts of the repo. Upstream updates go through `tools/vesc-upstream.sh update <ref>` in a PR of their own.

## Security

Don't open a public issue for a vulnerability. See [`SECURITY.md`](../SECURITY.md).
