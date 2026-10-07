# ADR-0030: The VESC firmware fork lives in this repo as a git subtree of upstream

- **Status:** Accepted
- **Date:** 2026-10-07
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-05, SYS-24, SYS-33, ADR-0011, ADR-0012, ADR-0024, ADR-0028, RSK-12

## Context

The corner controllers run a fork of the VESC firmware (`vedderb/bldc`, about 100 MB of history; release branches up to `release_6_06`, with 7.00 in progress). Our changes are the e-stop brake routine (ADR-0012), CAN-FD telemetry (ADR-0011, ADR-0024), and a hardware config for the VESC fork board. Everything the car runs should live in this monorepo, built and tested by the same CI, while upstream fixes can still be brought in.

**The VESC firmware is licensed GPL-3.0 (or later).** This repo is MIT.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Git submodule | Small; upstream history separate | The code lives in another repo; easy to end up with a stale or missing checkout; not "everything in this repo" |
| **B. Git subtree, squashed** | The code is in this repo; upstream releases merge in with `git subtree pull --squash`; our changes are ordinary commits | Upstream merges can conflict with our changes; repo grows by one source snapshot per upstream update |
| C. One-time copy of the source | Simplest | Upstream fixes have to be re-applied by hand |
| D. Our own fork repo on GitHub | Familiar fork workflow | A second repo; not "everything in this repo" |

## Criteria

1. The code lives here and builds in the same CI (ADR-0028).
2. Upstream fixes can be merged without re-applying our changes by hand.
3. Our changes are visible as reviewable diffs.

## Decision

**Option B.**

- **Location:** upstream `vedderb/bldc` lives at `firmware/vesc/bldc/` as a squashed git subtree, pinned to a release branch chosen when scaffolding.
- **Our changes** are ordinary commits in that tree:
  - the ARC hardware config in `hwconf/`;
  - the e-stop routine;
  - FD telemetry.
- **The upstream version and update procedure** are recorded in `firmware/vesc/README.md`.

## Consequences

- **Licence boundary.** `firmware/vesc/` is GPL-3.0, under upstream's licence; the rest of the repo stays MIT. Our VESC changes are GPL too, which a public repo satisfies. The root README and `LICENSE` must say so, and code must not be copied from `firmware/vesc/` into MIT-licensed parts of the repo.
- **The repo grows** by one source snapshot (~tens of MB) per upstream update.
- **Upstream updates are deliberate PRs:** pull, resolve conflicts with our changes, rebuild, test.
- **Reopen if:** upstream merges become painful enough that a fork repo would be easier, or the licence boundary causes problems.
