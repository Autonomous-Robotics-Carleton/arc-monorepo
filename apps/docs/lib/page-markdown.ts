import { relative, resolve } from 'node:path';
import type { PageTree } from 'fumadocs-core/server';
import { resolveHref } from '@/lib/links';
import { source } from '@/lib/source';

// Plain-Markdown versions of the docs for LLMs and agents: /docs/<page>.md,
// /llms.txt and /llms-full.txt. All of it is generated at build time.

export const siteUrl = 'https://docs.arcarleton.ca';
const repoBlob = 'https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/blob/main/';
const repoRoot = resolve(process.cwd(), '../..'); // builds run in apps/docs

type Page = ReturnType<typeof source.getPages>[number];

export const absolute = (url: string) => (url.startsWith('/') ? siteUrl + url : url);

/** The URL of a page's Markdown version. */
export const markdownUrl = (url: string) => `${url}.md`;

function stripMdx(body: string): string {
  let inCode = false;
  return body
    .split('\n')
    .filter((line) => {
      if (/^\s*(```|~~~)/.test(line)) inCode = !inCode;
      return inCode || !/^import .+ from ['"].+['"];?\s*$/.test(line);
    })
    .join('\n');
}

/** A page as self-contained Markdown: title, links to the page and its source, body with absolute links. */
export function pageMarkdown(page: Page): string {
  const data = page.data as { title?: string; description?: string; content?: string };
  let body = (data.content ?? '').replace(/^---\n[\s\S]*?\n---\n/, ''); // frontmatter
  body = body.replace(/^\s*# .+\n/, ''); // the title is written below
  if (page.absolutePath?.endsWith('.mdx')) body = stripMdx(body);

  const link = (href: string) => absolute(resolveHref(href, page.absolutePath) ?? href);
  body = body
    .replace(/\]\(([^)\s]+)\)/g, (_, href: string) => `](${link(href)})`)
    .replace(/\bhref="([^"]+)"/g, (_, href: string) => `href="${link(href)}"`);

  const sourcePath = page.absolutePath ? relative(repoRoot, page.absolutePath) : undefined;
  return [
    `# ${data.title ?? page.url}`,
    data.description ? `> ${data.description}` : undefined,
    [`Page: ${absolute(page.url)}`, sourcePath ? `Source: ${repoBlob}${sourcePath}` : undefined]
      .filter(Boolean)
      .join('\n'),
    body.trim(),
  ]
    .filter(Boolean)
    .join('\n\n');
}

export interface Section {
  title: string;
  pages: Page[];
}

/** Pages grouped by the sidebar's separators, in sidebar order. */
export function sections(): Section[] {
  const byUrl = new Map(source.getPages().map((page) => [page.url, page]));
  const result: Section[] = [];
  let current: Section = { title: 'Docs', pages: [] };

  const walk = (nodes: PageTree.Node[]) => {
    for (const node of nodes) {
      if (node.type === 'separator') {
        if (current.pages.length) result.push(current);
        current = { title: String(node.name ?? ''), pages: [] };
      } else if (node.type === 'page') {
        const page = byUrl.get(node.url);
        if (page) current.pages.push(page);
      } else {
        const index = node.index && byUrl.get(node.index.url);
        if (index) current.pages.push(index);
        walk(node.children);
      }
    }
  };
  walk(source.pageTree.children);
  if (current.pages.length) result.push(current);
  return result;
}
