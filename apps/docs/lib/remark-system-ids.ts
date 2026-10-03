import { dirname, relative, resolve } from 'node:path';
// Relative import, not '@arc/systems-model': source.config.ts is bundled by
// fumadocs-mdx, which bundles relative imports but leaves packages external,
// and Node can't run a package's TypeScript from node_modules.
import { canonicalId, idPattern, loadSystems, plain } from '../../../libs/systems-model/src/index.ts';

type MdNode = {
  type: string;
  value?: string;
  url?: string;
  title?: string;
  children?: MdNode[];
  data?: { hProperties?: Record<string, unknown> };
};

// Never link inside these: nested links, code, or headings (already anchors).
const skip = new Set(['link', 'linkReference', 'inlineCode', 'code', 'heading', 'definition']);

function cellText(node: MdNode): string {
  return plain(node.value ?? node.children?.map(cellText).join('') ?? '');
}

/**
 * Links every ID mention (SYS-04, RSK-03, ADR-0012, …) to where it's defined,
 * with the definition as the link's tooltip, and gives each defining table
 * row an anchor (#sys-04) to land on.
 *
 * Links are written as relative file paths, so the site's link resolver turns
 * them into page URLs like any other link.
 */
export function remarkSystemIds({ systemsRoot }: { systemsRoot: string }) {
  return (tree: MdNode, file: { path?: string }) => {
    if (!file.path) return;
    const model = loadSystems(systemsRoot); // re-read per page so dev picks up edits
    const here = file.path;

    const target = (id: string) => {
      const def = model.definitions.get(id);
      if (!def) return undefined;
      const path = resolve(systemsRoot, def.file);
      const anchor = def.kind === 'ADR' ? '' : `#${id.toLowerCase()}`;
      const url = path === here ? anchor : relative(dirname(here), path) + anchor;
      const title = `${id}: ${def.title}`;
      return url ? { url, title: title.length > 140 ? `${title.slice(0, 139)}…` : title } : undefined;
    };

    const defined = new Set(
      [...model.definitions.values()].filter((d) => resolve(systemsRoot, d.file) === here).map((d) => d.id),
    );

    const visit = (node: MdNode) => {
      if (skip.has(node.type) || !node.children) return;

      if (node.type === 'table') {
        // a row defines an ID through the table's "ID" column (same rule as
        // @arc/systems-model): anchor it, and don't link the ID to itself
        const [header, ...rows] = node.children;
        const column = header.children?.findIndex((c) => cellText(c) === 'ID') ?? -1;
        visit(header);
        for (const row of rows) {
          const cell = row.children?.[column];
          if (cell && defined.has(cellText(cell))) {
            row.data = { ...row.data, hProperties: { ...row.data?.hProperties, id: cellText(cell).toLowerCase() } };
            row.children?.filter((c) => c !== cell).forEach(visit);
          } else visit(row);
        }
        return;
      }

      node.children = node.children.flatMap((child) => {
        if (child.type !== 'text' || !child.value) {
          visit(child);
          return [child];
        }
        const parts: MdNode[] = [];
        let last = 0;
        for (const match of child.value.matchAll(new RegExp(idPattern))) {
          const link = target(canonicalId(match[1], match[2]));
          if (!link) continue;
          if (match.index > last) parts.push({ type: 'text', value: child.value.slice(last, match.index) });
          parts.push({ type: 'link', url: link.url, title: link.title, children: [{ type: 'text', value: match[0] }] });
          last = match.index + match[0].length;
        }
        if (!parts.length) return [child];
        if (last < child.value.length) parts.push({ type: 'text', value: child.value.slice(last) });
        return parts;
      });
    };

    visit(tree);
  };
}
