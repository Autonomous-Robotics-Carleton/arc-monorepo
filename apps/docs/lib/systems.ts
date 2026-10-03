import { existsSync } from 'node:fs';
import { dirname, relative, resolve } from 'node:path';
import { canonicalId, idPattern, loadSystems, referencedBy, type SystemsModel } from '@arc/systems-model';
import { resolveHref } from '@/lib/links';
import { source } from '@/lib/source';

// Builds run in apps/docs. These helpers run at build time only: pages are
// prerendered, and the runtime image doesn't include systems/.
export const systemsRoot = resolve(process.cwd(), '../../systems');

let cached: SystemsModel | undefined;

export function systemsModel(): SystemsModel | undefined {
  if (!existsSync(systemsRoot)) return undefined;
  return (cached ??= loadSystems(systemsRoot));
}

export type IdPart = string | { id: string; href: string; title: string };

/** Splits text into plain parts and links for every defined ID. */
export function linkIds(text: string, fromFile: string): IdPart[] {
  const model = systemsModel();
  if (!model) return [text];
  const parts: IdPart[] = [];
  let last = 0;
  for (const match of text.matchAll(new RegExp(idPattern))) {
    const id = canonicalId(match[1], match[2]);
    const def = model.definitions.get(id);
    if (!def) continue;
    const anchor = def.kind === 'ADR' ? '' : `#${id.toLowerCase()}`;
    const target = relative(dirname(fromFile), resolve(systemsRoot, def.file)) + anchor;
    if (match.index > last) parts.push(text.slice(last, match.index));
    parts.push({ id: match[0], href: resolveHref(target, fromFile) ?? target, title: `${id}: ${def.title}` });
    last = match.index + match[0].length;
  }
  if (last < text.length) parts.push(text.slice(last));
  return parts;
}

/** Pages that mention the ADR defined by this file. */
export function adrBacklinks(absolutePath: string | undefined): { title: string; url: string }[] {
  const model = systemsModel();
  if (!model || !absolutePath) return [];
  const adr = model.adrs.find((a) => resolve(systemsRoot, a.file) === absolutePath);
  if (!adr) return [];
  const pages = source.getPages();
  return referencedBy(model, adr.id).flatMap((file) => {
    const page = pages.find((p) => p.absolutePath === resolve(systemsRoot, file));
    return page ? [{ title: page.data.title ?? file, url: page.url }] : [];
  });
}
