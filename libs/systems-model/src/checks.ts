import { existsSync } from 'node:fs';
import { dirname, join, resolve } from 'node:path';
import { codeLines, tableRows } from './markdown.ts';
import { idKinds, type SystemsModel } from './model.ts';

export interface Problem {
  file: string;
  line: number;
  message: string;
}

const requiredAdrFields = ['Status', 'Date', 'Deciders', 'Traces to'];
const verificationMethod = /^[TAID](\s*(\+|,|then)\s*[TAID])*$/;

/** Relative Markdown links whose target file doesn't exist. */
function brokenLinks(model: SystemsModel): Problem[] {
  return model.files.flatMap((file) => {
    const source = model.read(file);
    const code = codeLines(source);
    return source.split('\n').flatMap((text, i) => {
      if (code.has(i + 1)) return [];
      return [...text.matchAll(/\]\(([^)\s]+)\)/g)].flatMap(([, target]) => {
        if (/^[a-z]+:/i.test(target) || target.startsWith('#')) return [];
        const path = resolve(dirname(join(model.root, file)), target.split('#')[0]);
        return existsSync(path) ? [] : [{ file, line: i + 1, message: `broken link: ${target}` }];
      });
    });
  });
}

/** IDs mentioned anywhere that aren't defined in their home file. */
function unknownIds(model: SystemsModel): Problem[] {
  return model.references
    .filter((ref) => !model.definitions.has(ref.id))
    .map((ref) => {
      const kind = ref.id.split('-')[0] as keyof typeof idKinds;
      return { ...ref, message: `${ref.id} isn't defined (${idKinds[kind].label}s live in ${idKinds[kind].file})` };
    });
}

function duplicateIds(model: SystemsModel): Problem[] {
  return model.duplicates.map((dup) => {
    const first = model.definitions.get(dup.id)!;
    return { file: dup.file, line: dup.line, message: `${dup.id} is already defined at ${first.file}:${first.line}` };
  });
}

/** ADR headers are complete and supersession is recorded on both sides. */
function adrConsistency(model: SystemsModel): Problem[] {
  const problems: Problem[] = [];
  for (const adr of model.adrs) {
    for (const field of requiredAdrFields) {
      if (!adr.fields[field]?.value) problems.push({ file: adr.file, line: 1, message: `${adr.id} has no **${field}:** line` });
    }
    if (adr.supersededBy) {
      const successor = model.adrs.find((a) => a.id === adr.supersededBy);
      const line = adr.fields.Status.line;
      if (!successor) problems.push({ file: adr.file, line, message: `${adr.id} is superseded by ${adr.supersededBy}, which doesn't exist` });
      else if (!successor.supersedes.includes(adr.id)) {
        problems.push({ file: successor.file, line: 1, message: `${successor.id} should list **Supersedes:** ${adr.id} (${adr.id} says it's superseded by ${successor.id})` });
      }
    }
    for (const old of adr.supersedes) {
      const predecessor = model.adrs.find((a) => a.id === old);
      if (predecessor && predecessor.supersededBy !== adr.id) {
        problems.push({ file: predecessor.file, line: predecessor.fields.Status?.line ?? 1, message: `${old} should have **Status:** Superseded by ${adr.id} (${adr.id} says it supersedes ${old})` });
      }
    }
  }
  return problems;
}

/** adr/README.md lists every ADR, with the same status as the ADR itself. */
function adrIndex(model: SystemsModel): Problem[] {
  const file = 'adr/README.md';
  if (!model.files.includes(file)) return [];
  const listed = new Map<string, { status: string; line: number }>();
  for (const row of tableRows(model.read(file))) {
    const number = row.cells[0]?.match(/\b(\d{4})\b/)?.[1];
    if (number) listed.set(`ADR-${number}`, { status: row.byHeader.Status ?? '', line: row.line });
  }
  const firstWord = (status: string) => status.match(/^[A-Za-z]+/)?.[0].toLowerCase();
  return model.adrs.flatMap((adr): Problem[] => {
    const entry = listed.get(adr.id);
    if (!entry) return [{ file, line: 1, message: `${adr.id} (${adr.file}) is missing from the index` }];
    if (firstWord(entry.status) !== firstWord(adr.status ?? '')) {
      return [{ file, line: entry.line, message: `${adr.id} is "${entry.status}" here but "${adr.status}" in ${adr.file}` }];
    }
    return [];
  });
}

/** Every live requirement has a method and appears in the verification plan. */
function requirementVerification(model: SystemsModel): Problem[] {
  const plan = 'verification/plan.md';
  const planned = new Set(model.references.filter((ref) => ref.file === plan).map((ref) => ref.id));
  return [...model.definitions.values()]
    .filter((def) => def.kind === 'SYS' && def.status !== 'Deleted')
    .flatMap((def): Problem[] => {
      const problems: Problem[] = [];
      const method = def.columns?.['Verif.'] ?? '';
      if (!verificationMethod.test(method)) {
        problems.push({ file: def.file, line: def.line, message: `${def.id} needs a verification method (T, A, I or D), found "${method}"` });
      }
      if (model.files.includes(plan) && !planned.has(def.id)) {
        problems.push({ file: def.file, line: def.line, message: `${def.id} isn't in ${plan}` });
      }
      return problems;
    });
}

/** Ranges like "ADRs 0001–0019" that stop short of the latest ADR. */
function staleAdrRanges(model: SystemsModel): Problem[] {
  const latest = model.adrs.map((adr) => adr.number).sort().at(-1);
  if (!latest) return [];
  return model.files.flatMap((file) =>
    model.read(file).split('\n').flatMap((text, i) =>
      [...text.matchAll(/ADRs?[ -](\d{4})\s*(?:–|-|to)\s*(?:ADR-)?(\d{4})/g)]
        .filter(([, from, to]) => from === '0001' && to < latest)
        .map(([range]) => ({ file, line: i + 1, message: `"${range}" stops short of the latest ADR (${latest})` })),
    ),
  );
}

export function checkSystems(model: SystemsModel): Problem[] {
  return [
    ...brokenLinks(model),
    ...unknownIds(model),
    ...duplicateIds(model),
    ...adrConsistency(model),
    ...adrIndex(model),
    ...requirementVerification(model),
    ...staleAdrRanges(model),
  ].sort((a, b) => a.file.localeCompare(b.file) || a.line - b.line);
}
