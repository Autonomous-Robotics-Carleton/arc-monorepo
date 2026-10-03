import {
  defineCollections,
  defineConfig,
  defineDocs,
  frontmatterSchema,
  metaSchema,
} from 'fumadocs-mdx/config';
import { resolve } from 'node:path';
import { remarkSystemIds } from './lib/remark-system-ids';

// Pages come from several places in the repo; lib/source.ts mounts each
// collection at its place in the sidebar.

// Plain Markdown files kept readable on GitHub (CONTRIBUTING.md, systems/)
// have no frontmatter; their title comes from the first `# heading`.
const pageSchema = frontmatterSchema.extend({
  title: frontmatterSchema.shape.title.optional(),
});

// Handbook and landing pages, written for the site (MDX).
export const docs = defineDocs({
  dir: 'content/docs',
  docs: { schema: pageSchema },
  meta: { schema: metaSchema },
});

// Website docs, next to the website's code.
export const webDocs = defineDocs({
  dir: '../web/docs',
  docs: { schema: pageSchema },
  meta: { schema: metaSchema },
});

// Orin dev-kit setup notes, next to the platform code.
export const devKitDocs = defineDocs({
  dir: '../../platform/dev-kit',
  docs: { schema: pageSchema },
  meta: { schema: metaSchema },
});

// The car's systems engineering docs, the source of truth (Markdown).
// systems/README.md becomes "How these docs work" (see lib/source.ts).
export const systemsDocs = defineCollections({
  type: 'doc',
  dir: '../../systems',
  files: ['**/*.md', '!adr/0000-template.md'],
  schema: pageSchema,
});

// The contributing guide. Collections are watched in dev, so none may be
// rooted at the repo root: that would watch all of node_modules.
export const repoDocs = defineCollections({
  type: 'doc',
  dir: '../../.github',
  files: ['CONTRIBUTING.md'],
  schema: pageSchema,
});

// The page title is rendered by the layout, so a leading `# heading`
// (which Markdown files keep for GitHub) would show twice. Its text is
// exported as `headingTitle`, the title for pages without frontmatter.
type MdNode = { type: string; depth?: number; value?: string; children?: MdNode[]; name?: string; attributes?: unknown[] };

function textOf(node: MdNode): string {
  return node.value ?? node.children?.map(textOf).join('') ?? '';
}

function remarkLeadingH1() {
  return (tree: { children: MdNode[] }, file: { data: Record<string, unknown> }) => {
    const first = tree.children.find((node) => node.type !== 'yaml');
    if (first?.type === 'heading' && first.depth === 1) {
      file.data.headingTitle = textOf(first).trim();
      tree.children.splice(tree.children.indexOf(first), 1);
    }
  };
}

// GitHub alerts (> [!NOTE], > [!WARNING], …) render natively on GitHub; on
// the site they become callouts, so Markdown files need no JSX.
const alertTypes: Record<string, string> = {
  NOTE: 'info',
  TIP: 'info',
  IMPORTANT: 'info',
  WARNING: 'warn',
  CAUTION: 'error',
};

function remarkGithubAlerts() {
  const visit = (node: MdNode & { children?: MdNode[] }) => {
    node.children?.forEach((child, i) => {
      const paragraph = child.type === 'blockquote' ? child.children?.[0] : undefined;
      const text = paragraph?.type === 'paragraph' ? paragraph.children?.[0] : undefined;
      const match = text?.type === 'text' ? text.value?.match(/^\[!(\w+)\][ \t]*\n?/) : null;
      const type = match && alertTypes[match[1].toUpperCase()];
      if (!type || !text || !paragraph || !child.children) {
        visit(child);
        return;
      }
      text.value = text.value!.slice(match![0].length);
      if (!text.value) paragraph.children!.shift();
      const title = match![1][0].toUpperCase() + match![1].slice(1).toLowerCase();
      node.children![i] = {
        type: 'mdxJsxFlowElement',
        name: 'Callout',
        attributes: [
          { type: 'mdxJsxAttribute', name: 'type', value: type },
          { type: 'mdxJsxAttribute', name: 'title', value: title },
        ],
        children: paragraph.children!.length ? child.children : child.children.slice(1),
      } as MdNode;
    });
  };
  return visit;
}

export default defineConfig({
  mdxOptions: {
    remarkPlugins: [
      remarkLeadingH1,
      remarkGithubAlerts,
      [remarkSystemIds, { systemsRoot: resolve('../../systems') }],
    ],
    valueToExport: ['headingTitle'],
  },
});
