import { devKitDocs, docs, repoDocs, systemsDocs, webDocs } from '@/.source';
import { loader, type Source, type VirtualFile } from 'fumadocs-core/source';
import { createMDXSource } from 'fumadocs-mdx';

// Where single repo files appear in the sidebar, and their title there.
// Keys are paths within their collection (.github/ for repoDocs, systems/ for systemsDocs).
const repoPages: Record<string, { path: string; title: string }> = {
  'CONTRIBUTING.md': { path: 'handbook/contributing.md', title: 'Contributing' },
};
const systemsReadme = { path: 'how-these-docs-work.md', title: 'How these docs work' };

// systems/ mounts at car/. Folder READMEs become folder index pages, and the
// architecture overview is the section's landing page.
function carPath(path: string): string {
  if (path === 'architecture.md') return 'car/index.md';
  return `car/${path.replace(/(^|\/)README\.md$/, '$1index.md')}`;
}

// Sidebar for car/, kept here so systems/ holds no site config. Folders with
// a single page are flattened ("...folder"); "..." picks up new files.
const carSidebar: Record<string, { title: string; pages?: string[] }> = {
  car: {
    title: 'The car',
    pages: [
      'index', 'topology', '...requirements', '...verification', 'adr', 'icd', 'budgets',
      '...bom', 'risks', 'tests', '...mechanical', '...handoff', '...reviews', '...',
    ],
  },
  'car/adr': { title: 'Decisions' },
  'car/icd': { title: 'Interfaces' },
  'car/budgets': { title: 'Budgets' },
  'car/tests': { title: 'Tests' },
};

const carMeta: VirtualFile[] = Object.entries(carSidebar).map(([folder, data]) => ({
  type: 'meta',
  path: `${folder}/meta.json`,
  data,
}));

// systems/ mounts at car/. Folder READMEs become folder index pages, and the
// architecture overview is the section's landing page.
function carPath(path: string): string {
  if (path === 'architecture.md') return 'car/index.md';
  return `car/${path.replace(/(^|\/)README\.md$/, '$1index.md')}`;
}

// Sidebar for car/, kept here so systems/ holds no site config. Folders with
// a single page are flattened ("...folder"); "..." picks up new files.
const carSidebar: Record<string, { title: string; pages?: string[] }> = {
  car: {
    title: 'The car',
    pages: [
      'index', 'topology', '...requirements', '...verification', 'adr', 'icd', 'budgets',
      '...bom', 'risks', 'tests', '...mechanical', '...handoff', '...reviews', '...',
    ],
  },
  'car/adr': { title: 'Decisions' },
  'car/icd': { title: 'Interfaces' },
  'car/budgets': { title: 'Budgets' },
  'car/tests': { title: 'Tests' },
};

const carMeta: VirtualFile[] = Object.entries(carSidebar).map(([folder, data]) => ({
  type: 'meta',
  path: `${folder}/meta.json`,
  data,
}));

// eslint-disable-next-line @typescript-eslint/no-explicit-any
type AnySource = Source<any>;

function filesOf(source: AnySource): VirtualFile[] {
  return typeof source.files === 'function' ? source.files() : source.files;
}

function mount(prefix: string, source: AnySource): VirtualFile[] {
  return filesOf(source).map((file) => ({ ...file, path: `${prefix}/${file.path}` }));
}

// Page data has lazy getters (e.g. `content` reads the source file), so copy
// descriptors rather than spreading, which would call them.
function setTitle(file: VirtualFile, title: string): VirtualFile {
  const data = Object.defineProperties({}, Object.getOwnPropertyDescriptors(file.data));
  return { ...file, data: Object.assign(data, { title }) };
}

// Markdown files without frontmatter take their title from the first `# heading`
// (exported as `headingTitle` by the remark plugin in source.config.ts).
function withTitle(file: VirtualFile): VirtualFile {
  const data = file.data as { title?: string; headingTitle?: string };
  if (file.type !== 'page' || data.title) return file;
  return setTitle(file, data.headingTitle ?? file.path);
}

const handbook = docs.toFumadocsSource();

const files: VirtualFile[] = [
  ...filesOf(handbook),
  ...mount('handbook/website', webDocs.toFumadocsSource()),
  ...mount('handbook/orin-dev-kit', devKitDocs.toFumadocsSource()),
  ...filesOf(createMDXSource(systemsDocs)).map((file) => {
    const moved = { ...file, path: carPath(file.path) };
    return file.path === 'architecture.md' ? setTitle(moved, 'Overview') : moved;
  }),
  ...carMeta,
  ...filesOf(createMDXSource(repoDocs)).map((file) => {
    const page = repoPages[file.path];
    return page ? setTitle({ ...file, path: page.path }, page.title) : file;
  }),
].map(withTitle);

export const source = loader({
  baseUrl: '/docs',
  // the other collections share the handbook's page and meta schema
  source: { ...handbook, files },
});
