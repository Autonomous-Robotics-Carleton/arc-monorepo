import { resolve } from 'node:path';
import { absolute, markdownUrl, sections } from '@/lib/page-markdown';
import { systemsModel, systemsRoot } from '@/lib/systems';

// https://llmstxt.org: an index of the docs for LLMs, in sidebar order.
export const dynamic = 'force-static';

export function GET() {
  // ADR pages have no description; their status tells an agent whether to trust them
  const adrStatus = new Map(
    (systemsModel()?.adrs ?? []).map((adr) => [resolve(systemsRoot, adr.file), adr.status?.split(/[.(]/)[0].trim()]),
  );
  const lines = [
    '# ARC Docs',
    '',
    '> Autonomous Robotics Carleton builds a 1/10-scale 4WD autonomy research car. These docs cover the car (requirements, decisions, interfaces, budgets, risks; source of truth in systems/ of the arc-monorepo repo) and the team handbook.',
    '',
    'Every page is also available as Markdown at its URL plus `.md`. IDs like SYS-04, LIM-01, RSK-03, E-30 and ADR-0012 are defined once and referenced everywhere; ids.json maps each to its definition and references.',
  ];
  for (const section of sections()) {
    lines.push('', `## ${section.title}`, '');
    for (const page of section.pages) {
      const { title, description: given } = page.data as { title?: string; description?: string };
      const status = page.absolutePath ? adrStatus.get(page.absolutePath) : undefined;
      const description = given ?? (status ? `Status: ${status}` : undefined);
      lines.push(`- [${title ?? page.url}](${absolute(markdownUrl(page.url))})${description ? `: ${description}` : ''}`);
    }
  }
  lines.push(
    '',
    '## Optional',
    '',
    `- [All pages in one file](${absolute('/llms-full.txt')})`,
    `- [ID index](${absolute('/ids.json')}): every SYS, LIM, RSK, E and ADR ID with its title, status, page and references`,
    '- [Source repository](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo): see AGENTS.md at its root',
    '',
  );
  return new Response(lines.join('\n'), { headers: { 'Content-Type': 'text/plain; charset=utf-8' } });
}
