#!/usr/bin/env node
// The split check (ADR-0047, #176): fails if code above the split branches on
// which target it's on, i.e. a target name (rules.mts) appears in a branch
// condition outside the allowed paths. Line-based: it reads each line with
// comments removed and flags it when the line is a condition (if, case,
// ==, #ifdef, a launch condition, a table keyed by target...) and names a
// target. Exits 1 on a violation, 2 on a usage error.
//
//   node tools/split-check/split-check.mts            # every file git knows, from the repo root
//   node tools/split-check/split-check.mts <paths...> # only these files
//   npx nx check split-check

import { execFileSync } from 'node:child_process';
import { readFileSync, statSync } from 'node:fs';
import { basename, join } from 'node:path';
import * as rules from './rules.mts';

export type Rules = Pick<typeof rules, 'TARGET_NAMES' | 'IDENTIFIER_WORDS' | 'IDENTIFIER_EXEMPT' | 'ALLOWED' | 'EXCEPTIONS'>;
export type Violation = { path: string; line: number; text: string; names: string[] };
type Lang = 'c' | 'js' | 'python' | 'cmake' | 'kconfig' | 'shell' | 'launch-xml' | 'launch-yaml';

export function language(path: string): Lang | undefined {
  const name = basename(path);
  if (/\.(c|h|cc|cpp|cxx|hpp|hh|hxx|ino)$/.test(name)) return 'c';
  if (/\.(js|mjs|cjs|jsx|ts|mts|cts|tsx)$/.test(name)) return 'js';
  if (/\.py$/.test(name)) return 'python';
  if (name === 'CMakeLists.txt' || /\.cmake$/.test(name)) return 'cmake';
  if (/^Kconfig/.test(name)) return 'kconfig';
  if (/\.(sh|bash)$/.test(name) || /^(Dockerfile|Containerfile)/.test(name)) return 'shell';
  if (/\.launch\.xml$/.test(name)) return 'launch-xml';
  if (/\.launch\.ya?ml$/.test(name)) return 'launch-yaml';
  return undefined;
}

// `*` within a path segment, `**` across segments
export function glob(pattern: string): RegExp {
  let re = '';
  for (let i = 0; i < pattern.length; i++) {
    const c = pattern[i];
    if (c === '*' && pattern[i + 1] === '*') {
      i++;
      if (pattern[i + 1] === '/') { i++; re += '(?:.*/)?'; } else re += '.*';
    } else if (c === '*') re += '[^/]*';
    else if (c === '?') re += '[^/]';
    else re += c.replace(/[.+^${}()|[\]\\]/g, '\\$&');
  }
  return new RegExp(`^${re}$`);
}

export function allowedBy(path: string, r: Rules) {
  return r.ALLOWED.find(a => glob(a.path).test(path));
}

// The source with comments (and Python's triple-quoted strings, which are
// docstrings in practice) blanked out. Newlines are kept, so line numbers hold.
export function stripComments(src: string, lang: Lang): string {
  const out = src.split('');
  const blank = (from: number, to: number) => {
    for (let k = from; k < to; k++) if (out[k] !== '\n') out[k] = ' ';
  };
  const hashComments = lang === 'python' || lang === 'cmake' || lang === 'kconfig' || lang === 'shell' || lang === 'launch-yaml';
  const quotes = lang === 'js' ? '"\'`' : lang === 'launch-xml' ? '' : '"\'';
  let i = 0;
  while (i < src.length) {
    const c = src[i];
    if (lang === 'launch-xml' && src.startsWith('<!--', i)) {
      const end = src.indexOf('-->', i);
      const stop = end < 0 ? src.length : end + 3;
      blank(i, stop); i = stop;
    } else if ((lang === 'c' || lang === 'js') && src.startsWith('//', i)) {
      const end = src.indexOf('\n', i);
      const stop = end < 0 ? src.length : end;
      blank(i, stop); i = stop;
    } else if ((lang === 'c' || lang === 'js') && src.startsWith('/*', i)) {
      const end = src.indexOf('*/', i + 2);
      const stop = end < 0 ? src.length : end + 2;
      blank(i, stop); i = stop;
    } else if (lang === 'python' && (src.startsWith('"""', i) || src.startsWith("'''", i))) {
      const end = src.indexOf(src.slice(i, i + 3), i + 3);
      const stop = end < 0 ? src.length : end + 3;
      blank(i, stop); i = stop;
    } else if (hashComments && c === '#' && (lang === 'python' || i === 0 || /\s/.test(src[i - 1]))) {
      const end = src.indexOf('\n', i);
      const stop = end < 0 ? src.length : end;
      blank(i, stop); i = stop;
    } else if (quotes.includes(c) && !(lang === 'shell' && c === "'" && /\w/.test(src[i - 1] ?? ''))) {
      // skip over the string, keeping it
      let j = i + 1;
      while (j < src.length && src[j] !== c && (src[j] !== '\n' || c === '`')) j += src[j] === '\\' && lang !== 'shell' ? 2 : 1;
      i = j + 1;
    } else i++;
  }
  return out.join('');
}

// Is this (comment-free) line a branch condition?
export function isCondition(line: string, lang: Lang): boolean {
  switch (lang) {
    case 'python':
      return /^\s*(if|elif|while|assert|match|case)\b/.test(line)
        || /\bif\b.*\belse\b/.test(line) // a conditional expression
        || /[=!]=/.test(line)
        || (/\bin\b/.test(line) && !/^\s*for\b/.test(line))
        || /\b(LaunchConfiguration(Not)?Equals|IfCondition|UnlessCondition|PythonExpression)\b|\bcondition\s*=/.test(line);
    case 'c':
    case 'js':
      return /^\s*#\s*(if|ifdef|ifndef|elif|elifdef|elifndef)\b/.test(line)
        || /\b(if|while|switch|case)\b/.test(line)
        || /[=!]==?/.test(line.replace(/[<>]=|=>/g, ''))
        || /(?<!\?)\?(?![.?])/.test(line) // a conditional expression, not ?. or ??
        || /\b(strn?cmp|strcasecmp|IS_ENABLED|IF_ENABLED|COND_CODE_[01])\b|\.(compare|equals|includes)\(/.test(line);
    case 'cmake':
      return /^\s*(if|elseif|while)\s*\(/i.test(line) || /\$<.*\b(STREQUAL|EQUAL|BOOL|IN_LIST)\b/.test(line);
    case 'kconfig':
      return /^\s*(if|depends\s+on|visible\s+if)\b/.test(line) || /\bif\b/.test(line);
    case 'shell':
      return /^\s*(if|elif|while|until|case)\b/.test(line)
        || /(^|\s)(\[\[?|test)\s/.test(line)
        || /^\s*\(?[\w"'*|. -]+\)(\s|$)/.test(line); // a case pattern
    case 'launch-xml':
      return /\b(if|unless)\s*=/.test(line);
    case 'launch-yaml':
      return /^\s*-?\s*(if|unless)\s*:/.test(line);
  }
}

const escape = (s: string) => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');

// The target names a line mentions: as a whole string literal, a bare word
// (shell, CMake, Kconfig, launch files), a word inside an identifier, or a
// Zephyr board, SoC or architecture symbol.
export function targetNames(line: string, lang: Lang, r: Rules): string[] {
  const found = new Set<string>();
  const names = r.TARGET_NAMES.map(escape).join('|');
  for (const m of line.matchAll(new RegExp(`(['"\`])\\s*(${names})\\s*\\1`, 'g'))) found.add(m[2]);
  if (lang === 'shell' || lang === 'cmake' || lang === 'kconfig' || lang.startsWith('launch')) {
    for (const m of line.matchAll(new RegExp(`(?<![\\w./-])(${names})(?![\\w./-])`, 'g'))) found.add(m[1]);
  }
  for (const m of line.matchAll(/\b(CONFIG_)?(BOARD_\w+|SOC_\w+|ARCH_POSIX|NATIVE_SIM\w*)\b/g)) found.add(m[0]);
  if (lang === 'cmake') for (const m of line.matchAll(/\$\{(BOARD\w*)\}|\b(BOARD|BOARD_TARGET|NORMALIZED_BOARD_TARGET)\b/g)) found.add(m[1] ?? m[2]);
  // identifiers, outside string literals
  const code = line.replace(/(['"`])(?:\\.|(?!\1).)*\1/g, '""');
  const words = new Set(r.IDENTIFIER_WORDS);
  for (const [id] of code.matchAll(/\b[A-Za-z_][A-Za-z0-9_]*\b/g)) {
    if (r.IDENTIFIER_EXEMPT.includes(id)) continue;
    const parts = id.replace(/([a-z0-9])([A-Z])/g, '$1_$2').toLowerCase().split('_');
    if (parts.some(p => words.has(p))) found.add(id);
  }
  return [...found];
}

// A table keyed by target name ({'sim': ..., 'car': ...}[target]) is a branch too
export function keyedByTarget(line: string, lang: Lang, r: Rules): string[] {
  if (lang !== 'python' && lang !== 'js') return [];
  const names = r.TARGET_NAMES.map(escape).join('|');
  const keys = [...line.matchAll(new RegExp(`(['"\`])(${names})\\1\\s*:(?!:)`, 'g'))].map(m => m[2]);
  if (lang === 'js') keys.push(...[...line.matchAll(new RegExp(`(?:^|[{,]\\s*)(${names})\\s*:(?!:)`, 'g'))].map(m => m[1]));
  return keys;
}

export function checkFile(path: string, src: string, r: Rules): Violation[] {
  const lang = language(path);
  if (!lang || allowedBy(path, r)) return [];
  const original = src.split('\n');
  return stripComments(src, lang).split('\n').flatMap((line, n) => {
    const names = [...new Set([...(isCondition(line, lang) ? targetNames(line, lang, r) : []), ...keyedByTarget(line, lang, r)])];
    return names.length ? [{ path, line: n + 1, text: original[n].trim(), names }] : [];
  });
}

export function check(files: { path: string; src: string }[], r: Rules = rules) {
  const violations: Violation[] = [];
  const excused: Violation[] = [];
  const used = new Set<(typeof r.EXCEPTIONS)[number]>();
  for (const f of files) {
    for (const v of checkFile(f.path, f.src, r)) {
      const exception = r.EXCEPTIONS.find(e => e.path === v.path && v.text.includes(e.contains));
      if (exception) { used.add(exception); excused.push(v); } else violations.push(v);
    }
  }
  const checkedPaths = new Set(files.map(f => f.path));
  // only report an exception as stale if its file was part of this run
  const stale = r.EXCEPTIONS.filter(e => !used.has(e) && checkedPaths.has(e.path));
  return { violations, excused, stale };
}

// GitHub workflow commands: a property value escapes % \r \n : and , ;
// the message escapes % \r \n
export const escapeData = (s: string) => s.replace(/%/g, '%25').replace(/\r/g, '%0D').replace(/\n/g, '%0A');
export const escapeProperty = (s: string) => escapeData(s).replace(/:/g, '%3A').replace(/,/g, '%2C');

function main(argv: string[]) {
  let root = '.';
  const paths: string[] = [];
  for (let i = 0; i < argv.length; i++) {
    if (argv[i] === '--root') root = argv[++i];
    else if (argv[i].startsWith('-')) {
      console.error(`unknown option ${argv[i]}\nusage: split-check.mts [--root <dir>] [paths...]`);
      return 2;
    } else paths.push(argv[i]);
  }
  // tracked and new files, not ignored ones
  const listed = paths.length ? paths : execFileSync('git', ['ls-files', '--cached', '--others', '--exclude-standard', '-z'], { cwd: root, encoding: 'utf8' }).split('\0').filter(Boolean);
  const files = listed.filter(p => language(p) && !allowedBy(p, rules)).flatMap(path => {
    const full = join(root, path);
    try {
      if (!statSync(full).isFile()) return [];
    } catch {
      return []; // deleted but still in the index
    }
    return [{ path, src: readFileSync(full, 'utf8') }];
  });
  const { violations, excused, stale } = check(files);
  const annotate = process.env.GITHUB_ACTIONS === 'true';
  for (const e of stale) console.warn(`warning: ${e.path}: the exception for "${e.contains}" matches nothing; remove it from tools/split-check/rules.mts`);
  for (const v of violations) {
    const message = `branches on the target (${v.names.join(', ')}): code above the split never asks which target it's on (ADR-0047)`;
    console.error(`${v.path}:${v.line}: ${message}\n    ${v.text}`);
    if (annotate) console.log(`::error file=${escapeProperty(v.path)},line=${v.line},title=Split check::${escapeData(message)}`);
  }
  const summary = `${files.length} files checked, ${excused.length} known exception(s)`;
  if (violations.length) {
    console.error(`\n${violations.length} violation(s); ${summary}. Move the target-specific part below the split (a board file, device model, target configuration or backend), or see tools/README.md.`);
    return 1;
  }
  console.log(`Split check: no violations; ${summary}`);
  return 0;
}

if (import.meta.main ?? process.argv[1]?.endsWith('split-check.mts')) process.exit(main(process.argv.slice(2)));
