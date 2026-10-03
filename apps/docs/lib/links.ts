import { dirname, relative, resolve } from 'node:path';
import { source } from '@/lib/source';

const repoBlob = 'https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/blob/main/';
const repoRoot = resolve(process.cwd(), '../..'); // builds run in apps/docs

let pagesByFile: Map<string, string> | undefined;

/**
 * Resolves a link written in a page's source file. Relative links are
 * written as file paths so they also work on GitHub: one pointing at
 * another page goes to that page's URL, and one pointing at any other
 * repo file goes to the file on GitHub.
 */
export function resolveHref(href: string | undefined, fromFile: string | undefined) {
  if (!href || !fromFile || /^([a-z]+:|\/|#)/i.test(href)) return href;

  pagesByFile ??= new Map(
    source.getPages().flatMap((page) => (page.absolutePath ? [[page.absolutePath, page.url]] : [])),
  );

  const [path, hash] = href.split('#');
  const target = resolve(dirname(fromFile), path);
  const suffix = hash ? `#${hash}` : '';

  const pageUrl = pagesByFile.get(target);
  if (pageUrl) return pageUrl + suffix;

  const repoPath = relative(repoRoot, target);
  return repoPath.startsWith('..') ? href : repoBlob + repoPath + suffix;
}
