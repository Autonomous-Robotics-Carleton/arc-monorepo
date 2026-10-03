#!/usr/bin/env node
// Checks systems/ for references that point at nothing:
//   - relative Markdown links whose target file doesn't exist
//   - ADR-nnnn mentions with no matching file in systems/adr/
// Exits 1 if any are found. Run with `npx nx check systems`.

import { readdirSync, readFileSync, existsSync, statSync } from 'node:fs';
import { join, dirname, relative, resolve } from 'node:path';

const root = resolve(process.argv[2] ?? 'systems');

function walk(dir) {
  return readdirSync(dir).flatMap((name) => {
    const path = join(dir, name);
    return statSync(path).isDirectory() ? walk(path) : [path];
  });
}

const files = walk(root).filter((f) => f.endsWith('.md'));
const adrNumbers = new Set(
  readdirSync(join(root, 'adr'))
    .map((name) => name.match(/^(\d{4})-/)?.[1])
    .filter(Boolean),
);

const problems = [];

for (const file of files) {
  const lines = readFileSync(file, 'utf8').split('\n');
  lines.forEach((line, i) => {
    const where = `${relative(process.cwd(), file)}:${i + 1}`;

    for (const [, target] of line.matchAll(/\]\(([^)\s]+)\)/g)) {
      if (/^[a-z]+:/i.test(target) || target.startsWith('#')) continue;
      const path = resolve(dirname(file), target.split('#')[0]);
      if (!existsSync(path)) problems.push(`${where}  broken link: ${target}`);
    }

    for (const [id, number] of line.matchAll(/\bADR-(\d{4})\b/g)) {
      if (!adrNumbers.has(number)) problems.push(`${where}  unknown decision: ${id}`);
    }
  });
}

if (problems.length) {
  console.error(problems.join('\n'));
  console.error(`\n${problems.length} problem(s) in ${relative(process.cwd(), root)}/`);
  process.exit(1);
}
console.log(`${files.length} files in ${relative(process.cwd(), root)}/ checked, no problems`);
