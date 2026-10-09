# ADR-0037: Our VESC firmware code lives in `firmware/vesc/arc/`, joined to upstream by small hooks

- **Status:** Accepted
- **Date:** 2026-10-08
- **Deciders:** Shrikar Vempati
- **Traces to:** SYS-33, ADR-0028, ADR-0033, ADR-0034, ADR-0035, RSK-20
- **Amends:** ADR-0033 (where our changes live)

## Context

ADR-0033 says our VESC changes are ordinary commits in `firmware/vesc/bldc/`, upstream's tree. In practice our additions (the e-stop routine, UART telemetry, the speed limit, and the command timeout from ADR-0035) were started as modules in `firmware/vesc/arc/`, with host tests built by CMake (`nx test vesc`), and `bldc/` is still unmodified. Nothing recorded that choice.

## Options

| Option | Pros | Cons |
| --- | --- | --- |
| A. Our code inside `bldc/`, as ADR-0033 says | One tree; follows upstream's layout | Mixed in with upstream's files; more conflicts on each three-way update; hard to test off the target |
| **B. Our code in `arc/`, joined to the build by small hooks in `bldc/`** | Our code is easy to find, review and test on the host; upstream updates conflict only at the hooks | Hook edits in upstream's Makefile and call sites to keep working across updates |

## Criteria

1. Upstream updates stay cheap (ADR-0033's criterion 3).
2. Safety functions in our code get their own tests (ADR-0012, ADR-0035), off the target.

## Decision

**Option B.**

- **`firmware/vesc/arc/`** holds our modules and their host tests.
- **`firmware/vesc/bldc/`** gets only the edits that join them to the build: the Makefile and the call sites. Those are ordinary commits, as ADR-0033 says.
- **Licence:** `arc/` is under `firmware/vesc/`, so it's GPL-3.0 like the rest of it (ADR-0033).

## Consequences

- Each module joins the firmware build as it's implemented; until then it's a fail-safe stub with host tests.
- An upstream update that conflicts at a hook is resolved in that update's PR (ADR-0033).
- **Reopen if:** the hooks grow large enough that the modules can't be tested on the host.
