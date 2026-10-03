import assert from 'node:assert/strict';
import { mkdirSync, mkdtempSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { dirname, join } from 'node:path';
import { test } from 'node:test';
import { checkSystems, loadSystems } from './index.ts';

const requirements = `# System Requirements

| ID | Requirement | Rationale | Verif. | Status |
| --- | --- | --- | --- | --- |
| SYS-01 | Go fast | Racing | T | Draft |
| SYS-02 | Stop | Safety | A then I | Draft |
| SYS-03 | *Deleted* | | — | Deleted |

| Spec item | Parent | Question |
| --- | --- | --- |
| Regulators | SYS-01 | Margin? |
`;

const plan = `# Verification plan

| Requirement | Method |
| --- | --- |
| SYS-01 | T |
| SYS-02 | A, I |
`;

const adr = (n: string, status: string, extra = '') => `# ADR-${n}: Decision ${n}

- **Status:** ${status}
- **Date:** 2026-10-03
- **Deciders:** Someone
- **Traces to:** SYS-01
${extra}
## Context
`;

const index = (rows: string) => `# Decision Records

| ADR | Decision | Status |
| --- | --- | --- |
${rows}
`;

/** A valid systems/ tree, with `files` overriding or adding files. */
function fixture(files: Record<string, string> = {}) {
  const root = mkdtempSync(join(tmpdir(), 'systems-'));
  const all: Record<string, string> = {
    'requirements/system.md': requirements,
    'verification/plan.md': plan,
    'adr/0001-one.md': adr('0001', 'Superseded by ADR-0002'),
    'adr/0002-two.md': adr('0002', 'Accepted', '- **Supersedes:** ADR-0001. Reopens nothing else.'),
    'adr/README.md': index('| [0001](0001-one.md) | One | Superseded by 0002 |\n| [0002](0002-two.md) | Two | Accepted |'),
    ...files,
  };
  for (const [path, content] of Object.entries(all)) {
    mkdirSync(dirname(join(root, path)), { recursive: true });
    writeFileSync(join(root, path), content);
  }
  return checkSystems(loadSystems(root)).map((p) => `${p.file}: ${p.message}`);
}

test('a consistent tree has no problems', () => {
  assert.deepEqual(fixture(), []);
});

test('only the ID column defines IDs', () => {
  // SYS-01 also appears in the Parent column of a second table: not a duplicate
  assert.equal(fixture().filter((p) => p.includes('already defined')).length, 0);
});

test('broken relative links', () => {
  assert.deepEqual(fixture({ 'risks.md': '# Risks\n\nSee [the plan](verification/nope.md).\n' }), [
    'risks.md: broken link: verification/nope.md',
  ]);
});

test('unknown IDs, but not placeholders or code', () => {
  const problems = fixture({
    'risks.md': '# Risks\n\nSee SYS-09 and ADR-0042. Template: SYS-nn, ADR-xxxx.\n\n```\nSYS-99\n```\n',
  });
  assert.equal(problems.length, 2);
  assert.match(problems[0], /SYS-09 isn't defined/);
  assert.match(problems[1], /ADR-0042 isn't defined/);
});

test('supersession must be recorded on both sides', () => {
  const problems = fixture({ 'adr/0002-two.md': adr('0002', 'Accepted') });
  assert.deepEqual(problems, ['adr/0002-two.md: ADR-0002 should list **Supersedes:** ADR-0001 (ADR-0001 says it\'s superseded by ADR-0002)']);
});

test('missing ADR header fields', () => {
  const problems = fixture({ 'adr/0002-two.md': '# ADR-0002: Two\n\n- **Status:** Accepted\n- **Supersedes:** ADR-0001\n' });
  assert.deepEqual(problems, [
    'adr/0002-two.md: ADR-0002 has no **Date:** line',
    'adr/0002-two.md: ADR-0002 has no **Deciders:** line',
    'adr/0002-two.md: ADR-0002 has no **Traces to:** line',
  ]);
});

test('ADR index lists every ADR with a matching status', () => {
  const problems = fixture({ 'adr/README.md': index('| [0001](0001-one.md) | One | Accepted |') });
  assert.deepEqual(problems, [
    'adr/README.md: ADR-0002 (adr/0002-two.md) is missing from the index',
    'adr/README.md: ADR-0001 is "Accepted" here but "Superseded by ADR-0002" in adr/0001-one.md',
  ]);
});

test('live requirements need a method and a place in the plan', () => {
  const problems = fixture({
    'requirements/system.md': requirements.replace('| SYS-02 | Stop | Safety | A then I |', '| SYS-02 | Stop | Safety | TBD |'),
    'verification/plan.md': plan.replace('| SYS-02 | A, I |\n', ''),
  });
  assert.deepEqual(problems, [
    'requirements/system.md: SYS-02 needs a verification method (T, A, I or D), found "TBD"',
    'requirements/system.md: SYS-02 isn\'t in verification/plan.md',
  ]);
});

test('ADR ranges that stop short of the latest ADR', () => {
  assert.deepEqual(fixture({ 'handoff.md': '# Handoff\n\nDecisions (ADRs 0001–0001).\n' }), [
    'handoff.md: "ADRs 0001–0001" stops short of the latest ADR (0002)',
  ]);
});
