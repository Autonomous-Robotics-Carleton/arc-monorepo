import { join, relative } from 'node:path';
import { idIndex } from '@arc/systems-model';
import { resolveHref } from '@/lib/links';
import { absolute } from '@/lib/page-markdown';
import { systemsModel, systemsRoot } from '@/lib/systems';

// Every ID defined in systems/, with its page on this site. Built at build time.
export const dynamic = 'force-static';

export function GET() {
  const model = systemsModel();
  if (!model) return Response.json({ error: 'systems/ not available' }, { status: 500 });
  const from = join(systemsRoot, 'README.md');

  const ids = idIndex(model).map((entry) => {
    const file = join(systemsRoot, '..', entry.definedAt.replace(/:\d+$/, ''));
    const anchor = entry.kind === 'ADR' ? '' : `#${entry.id.toLowerCase()}`;
    const url = resolveHref(relative(systemsRoot, file) + anchor, from);
    return { ...entry, url: url ? absolute(url) : undefined };
  });
  return Response.json({ source: 'https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/tree/main/systems', ids });
}
