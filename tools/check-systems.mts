#!/usr/bin/env node
// Checks systems/ for broken links, unknown or duplicate IDs, inconsistent
// ADRs and unverified requirements. Exits 1 on any problem.
// Run with `npx nx check systems`.

import { relative, resolve } from 'node:path';
import { checkSystems, loadSystems } from '../libs/systems-model/src/index.ts';

const root = resolve(process.argv[2] ?? 'systems');
const model = loadSystems(root);
const problems = checkSystems(model);
const shown = relative(process.cwd(), root) || '.';

if (problems.length) {
  for (const p of problems) console.error(`${shown}/${p.file}:${p.line}  ${p.message}`);
  console.error(`\n${problems.length} problem(s) in ${shown}/`);
  process.exit(1);
}
console.log(
  `${model.files.length} files, ${model.definitions.size} IDs, ${model.references.length} references in ${shown}/: no problems`,
);
