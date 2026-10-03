import { readdirSync, readFileSync, statSync } from 'node:fs';
import { join, relative } from 'node:path';
import { codeLines, firstHeading, headerFields, plain, tableRows } from './markdown.ts';

/**
 * Where each kind of ID is defined. Table IDs are defined by a row in the
 * kind's home file whose `ID` column (or first column, in a table without
 * one) is exactly the ID. ADRs are defined by their own file, adr/NNNN-*.md.
 */
export const idKinds = {
  SYS: { file: 'requirements/system.md', label: 'System requirement' },
  LIM: { file: 'requirements/system.md', label: 'Accepted limit' },
  RSK: { file: 'risks.md', label: 'Risk' },
  E: { file: 'bom/electrical.md', label: 'Electrical BOM item' },
  ADR: { file: 'adr/', label: 'Decision record' },
} as const;

export type IdKind = keyof typeof idKinds;

/** Matches real IDs only (digits), not placeholders like SYS-nn or ADR-xxxx. */
export const idPattern = /\b(SYS|LIM|RSK|E|ADR)-(\d+)\b/g;

export interface Definition {
  id: string;
  kind: IdKind;
  /** Path relative to the systems/ root. */
  file: string;
  line: number;
  title: string;
  status?: string;
  /** For table IDs: the row's cells keyed by column header. */
  columns?: Record<string, string>;
}

export interface Reference {
  id: string;
  file: string;
  line: number;
}

export interface Adr extends Definition {
  number: string;
  fields: Record<string, { value: string; line: number }>;
  supersededBy?: string;
  supersedes: string[];
}

export interface SystemsModel {
  /** Absolute path of systems/. */
  root: string;
  /** Markdown files, relative to root. */
  files: string[];
  /** CSV data files (budgets), relative to root. */
  dataFiles: string[];
  definitions: Map<string, Definition>;
  /** Definitions of an ID that was already defined. */
  duplicates: Definition[];
  references: Reference[];
  adrs: Adr[];
  read(file: string): string;
}

function filesWith(extension: string, root: string, dir = root): string[] {
  return readdirSync(dir).flatMap((name) => {
    const path = join(dir, name);
    if (statSync(path).isDirectory()) return filesWith(extension, root, path);
    return name.endsWith(extension) ? [relative(root, path)] : [];
  });
}

/** Normalises an ID's number to its canonical width, e.g. ADR-12 -> ADR-0012. */
export function canonicalId(kind: string, number: string): string {
  return `${kind}-${number.padStart(kind === 'ADR' ? 4 : 2, '0')}`;
}

function parseAdr(file: string, source: string): Adr | undefined {
  const number = file.match(/^adr\/(\d{4})-/)?.[1];
  if (!number || number === '0000') return undefined;
  const fields = headerFields(source);
  const status = fields.Status?.value ?? '';
  const heading = firstHeading(source) ?? '';
  return {
    id: `ADR-${number}`,
    kind: 'ADR',
    number,
    file,
    line: 1,
    title: heading.replace(/^ADR-\d+:\s*/, ''),
    status,
    fields,
    supersededBy: status.match(/^Superseded by (ADR-\d{4})/)?.[1],
    // first sentence only: "Supersedes: ADR-0003. Reopens ADR-0002" supersedes 0003
    supersedes: [...(fields.Supersedes?.value.split(/\.\s/)[0] ?? '').matchAll(/ADR-\d{4}/g)].map((m) => m[0]),
  };
}

export function loadSystems(root: string): SystemsModel {
  const files = filesWith('.md', root).sort();
  const dataFiles = filesWith('.csv', root).sort();
  const cache = new Map<string, string>();
  const read = (file: string) => {
    if (!cache.has(file)) cache.set(file, readFileSync(join(root, file), 'utf8'));
    return cache.get(file)!;
  };

  const definitions = new Map<string, Definition>();
  const duplicates: Definition[] = [];
  const define = (definition: Definition) => {
    if (definitions.has(definition.id)) duplicates.push(definition);
    else definitions.set(definition.id, definition);
  };

  for (const [kind, home] of Object.entries(idKinds) as [IdKind, (typeof idKinds)[IdKind]][]) {
    if (kind === 'ADR' || !files.includes(home.file)) continue;
    for (const row of tableRows(read(home.file))) {
      const headers = Object.keys(row.byHeader);
      const column = headers.length ? headers.indexOf('ID') : 0;
      const id = plain(row.cells[column] ?? '');
      if (column < 0 || !new RegExp(`^${kind}-\\d+$`).test(id)) continue;
      define({
        id,
        kind,
        file: home.file,
        line: row.line,
        title: plain(row.cells[column + 1] ?? ''),
        status: row.byHeader.Status ? plain(row.byHeader.Status) : undefined,
        columns: row.byHeader,
      });
    }
  }

  const adrs = files.flatMap((file) => parseAdr(file, read(file)) ?? []);
  adrs.forEach(define);

  const references: Reference[] = [];
  for (const file of [...files, ...dataFiles]) {
    const source = read(file);
    const code = file.endsWith('.md') ? codeLines(source) : new Set<number>();
    source.split('\n').forEach((text, i) => {
      if (code.has(i + 1)) return;
      for (const match of text.matchAll(idPattern)) {
        references.push({ id: canonicalId(match[1], match[2]), file, line: i + 1 });
      }
    });
  }

  return { root, files, dataFiles, definitions, duplicates, references, adrs, read };
}

/** Files (other than the definition's own) that mention each ID. */
export function referencedBy(model: SystemsModel, id: string): string[] {
  const home = model.definitions.get(id)?.file;
  const files = model.references.filter((ref) => ref.id === id && ref.file !== home).map((ref) => ref.file);
  return [...new Set(files)].sort();
}
