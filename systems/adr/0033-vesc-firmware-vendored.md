# ADR-0033: The VESC firmware is vendored as an upstream snapshot, updated by script

- **Status:** Accepted, amended by ADR-0034 (our changes are now the e-stop routine, UART telemetry and the speed limit, on upstream's A50S target; no ARC hardware config or FD telemetry) and ADR-0037 (our code lives in `firmware/vesc/arc/`, joined to `bldc/` by small hooks)
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-24, SYS-33, ADR-0011, ADR-0012, ADR-0024, ADR-0028, RSK-12
- **Supersedes:** ADR-0030. The goal (the VESC firmware fork lives in this repo) and the GPL-3.0 boundary carry over; the mechanism changes.

## Context

ADR-0030 put the VESC firmware (upstream `vedderb/bldc`, GPL-3.0) in `firmware/vesc/` as a squashed `git subtree`. A subtree arrives through merge commits, and later `git subtree pull`s depend on that merge history. This repo merges every PR with "Rebase and merge" (or squash) and has no merge commits on `main` (CONTRIBUTING, AGENTS.md). Rebasing a subtree import would replay upstream's files at the repo root. The two can't both hold.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Keep the subtree; allow merge commits for VESC updates only | ADR-0030 unchanged; standard git tooling | A standing exception to the merge rules, which is easy to get wrong in the GitHub UI and destructive when it goes wrong |
| **B. Vendored snapshot, updated by a script** | Linear history; ordinary commits only; one merge rule for every PR | A small script to maintain |
| C. Git submodule | Standard; small | The code lives in another repo, against the goal |

## Criteria

1. The code lives in this repo (ADR-0028, ADR-0030's goal).
2. One merge rule for every PR: no exceptions.
3. Upstream fixes can be brought in without re-applying our changes by hand.

## Decision

**Option B.**

- **Where things live:**
  - `firmware/vesc/bldc/`: upstream's source plus our changes;
  - `firmware/vesc/UPSTREAM`: the upstream repository, ref and commit our copy is based on.
- **Import:** `tools/vesc-upstream.sh import <ref>` copies upstream at `<ref>` into `firmware/vesc/bldc/` and records it. One ordinary commit.
- **Update:** `tools/vesc-upstream.sh update <ref>` takes upstream's changes between the recorded commit and `<ref>` and applies them over our copy with a three-way merge. Our changes survive, and conflicts are marked for hand-resolution. One ordinary commit, in its own PR.
- **Our changes** are ordinary commits in `firmware/vesc/bldc/`:
  - the ARC hardware config;
  - the e-stop routine;
  - FD telemetry.

## Consequences

- **Licence boundary (from ADR-0030):**
  - `firmware/vesc/` is GPL-3.0, under upstream's licence; the rest of the repo stays MIT.
  - Our VESC changes are GPL too, which a public repo satisfies.
  - The root README and `LICENSE` say so.
  - Code must not be copied from `firmware/vesc/` into MIT-licensed parts of the repo.
- **The repo grows** by the size of upstream's source on import, and by upstream's changes on each update.
- **Each upstream update is a PR of its own:** update, resolve conflicts, rebuild, test.
- **Reopen if:** the script's three-way updates become unworkable.
