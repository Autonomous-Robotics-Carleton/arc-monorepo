/** Minimal Markdown helpers for the conventions systems/ uses. */

export interface TableRow {
  line: number; // 1-based
  cells: string[];
  /** Cells keyed by the table's header text. */
  byHeader: Record<string, string>;
}

/** Splits a `| a | b |` line into trimmed cells, ignoring escaped pipes. */
export function splitRow(line: string): string[] {
  const cells = line.trim().replace(/^\|/, '').replace(/\|$/, '').split(/(?<!\\)\|/);
  return cells.map((cell) => cell.trim());
}

/** Removes emphasis and code markers so a cell reads as plain text. */
export function plain(text: string): string {
  return text.replace(/\*\*|__|`/g, '').replace(/^\*(.*)\*$/, '$1').trim();
}

const separator = /^\|?\s*:?-{3,}:?\s*(\|\s*:?-{3,}:?\s*)*\|?\s*$/;

/** Every body row of every table in the file, with its header. */
export function tableRows(source: string): TableRow[] {
  const lines = source.split('\n');
  const rows: TableRow[] = [];
  let header: string[] | undefined;

  lines.forEach((line, i) => {
    if (!line.trimStart().startsWith('|')) {
      header = undefined;
      return;
    }
    if (separator.test(line.trim())) return;
    if (!header && separator.test(lines[i + 1]?.trim() ?? '')) {
      header = splitRow(line).map(plain);
      return;
    }
    const cells = splitRow(line);
    const byHeader: Record<string, string> = {};
    header?.forEach((name, column) => (byHeader[name] = cells[column] ?? ''));
    rows.push({ line: i + 1, cells, byHeader });
  });

  return rows;
}

/** `- **Key:** value` lines before the first `## ` heading. */
export function headerFields(source: string): Record<string, { value: string; line: number }> {
  const fields: Record<string, { value: string; line: number }> = {};
  for (const [i, line] of source.split('\n').entries()) {
    if (line.startsWith('## ')) break;
    const match = line.match(/^- \*\*([^*]+):\*\*\s*(.*)$/);
    if (match) fields[match[1].trim()] = { value: match[2].trim(), line: i + 1 };
  }
  return fields;
}

/** Text of the first `# ` heading. */
export function firstHeading(source: string): string | undefined {
  return source.match(/^# (.+)$/m)?.[1].trim();
}

/** Line numbers (1-based) that are inside fenced code blocks. */
export function codeLines(source: string): Set<number> {
  const inside = new Set<number>();
  let open = false;
  source.split('\n').forEach((line, i) => {
    if (/^\s*(```|~~~)/.test(line)) {
      open = !open;
      inside.add(i + 1);
    } else if (open) inside.add(i + 1);
  });
  return inside;
}
