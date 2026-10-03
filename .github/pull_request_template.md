## What and why

<!-- What this changes and why. Link the issue it closes: "Closes #123". -->

## Traces to

<!-- IDs this PR implements or changes: SYS-nn, ADR-nnnn, ICD-<name>, RSK-nn. Write "none" for work that isn't on the car (website, tooling). -->

## How it was tested

<!-- Commands you ran, bench results, screenshots for UI changes. -->

## Checklist

- [ ] `npx nx affected -t lint check build` passes locally
- [ ] Docs updated if behaviour, setup or interfaces changed
- [ ] Changes a requirement, ICD or budget: links the ADR or issue that justifies it
- [ ] Changes an ICD: the owners on both sides have approved
- [ ] Touches a safety function (e-stop, watchdog, safety envelope): has its own tests and a second reviewer
