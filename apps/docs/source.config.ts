import {
  defineCollections,
  defineConfig,
  defineDocs,
  frontmatterSchema,
  metaSchema,
} from 'fumadocs-mdx/config';

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

// Single Markdown files from the repo root.
export const repoDocs = defineCollections({
  type: 'doc',
  dir: '../..',
  files: ['CONTRIBUTING.md', 'systems/README.md'],
  schema: pageSchema,
});

// The page title is rendered by the layout, so a leading `# heading`
// (which Markdown files keep for GitHub) would show twice. Its text is
// exported as `headingTitle`, the title for pages without frontmatter.
type MdNode = { type: string; depth?: number; value?: string; children?: MdNode[] };

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

export default defineConfig({
  mdxOptions: {
    remarkPlugins: [remarkLeadingH1],
    valueToExport: ['headingTitle'],
  },
});
