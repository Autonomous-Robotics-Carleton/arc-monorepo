#!/usr/bin/env node
// Looks up IDs in systems/: where each is defined, its title and status,
// and which files mention it. Prints JSON.
//
//   node tools/systems-ids.mts SYS-04 ADR-0012   # those IDs
//   node tools/systems-ids.mts RSK               # every ID of a kind
//   node tools/systems-ids.mts                   # everything

import { resolve } from 'node:path';
import { canonicalId, idIndex, loadSystems } from '../libs/systems-model/src/index.ts';

const queries = process.argv.slice(2).map((q) => {
  const match = q.toUpperCase().match(/^([A-Z]+)-(\d+)$/);
  return match ? canonicalId(match[1], match[2]) : q.toUpperCase();
});
const entries = idIndex(loadSystems(resolve('systems')));
const picked = queries.length
  ? entries.filter((e) => queries.includes(e.id) || queries.includes(e.kind))
  : entries;
const missing = queries.filter((q) => /-/.test(q) && !entries.some((e) => e.id === q));

console.log(JSON.stringify(picked, null, 2));
if (missing.length) {
  console.error(`not defined: ${missing.join(', ')}`);
  process.exit(1);
}
