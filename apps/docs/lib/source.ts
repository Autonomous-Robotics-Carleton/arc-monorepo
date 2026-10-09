import {
  devKitDocs,
  docs,
  experimentsDocs,
  firmwareDocs,
  libsDocs,
  platformDocs,
  repoDocs,
  rosDocs,
  systemsDocs,
  toolsDocs,
  webDocs,
} from '@/.source';
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
      '...bom', 'risks', 'tests', '...mechanical', 'software', '...handoff', '...reviews', '...',
    ],
  },
  'car/adr': { title: 'Decisions' },
  'car/icd': { title: 'Interfaces' },
  'car/budgets': { title: 'Budgets' },
  'car/tests': { title: 'Tests' },
  'car/software': { title: 'Software design', pages: ['architecture', 'setup-plan'] },
};

// Software READMEs (see source.config.ts) and their page under software/. Their
// own `# heading` is the folder name, as GitHub shows it, so the sidebar title
// is set here. A README not listed is named after its folder.
const softwareReadmes: [Parameters<typeof createMDXSource>[0], Record<string, [string, string]>][] = [
  [rosDocs, { 'README.md': ['ros', 'ROS workspace'] }],
  [
    firmwareDocs,
    {
      'README.md': ['firmware', 'Firmware'],
      'sync-mcu/README.md': ['sync-mcu', 'Sync MCU firmware'],
      'vesc/README.md': ['vesc', 'Motor controller firmware'],
    },
  ],
  [experimentsDocs, { 'README.md': ['experiments', 'Experiments'] }],
  [platformDocs, { 'README.md': ['platform', 'Platform'] }],
  [toolsDocs, { 'README.md': ['tools', 'Tools'] }],
  [libsDocs, { 'systems-model/README.md': ['systems-model', 'systems-model library'] }],
];

function softwareFiles(): VirtualFile[] {
  return softwareReadmes.flatMap(([collection, pages]) =>
    filesOf(createMDXSource(collection)).map((file) => {
      const fallback = file.path.replace(/\/?README\.md$/, '') || 'readme';
      const [name, title] = pages[file.path] ?? [fallback, undefined];
      const moved = { ...file, path: `software/${name}.md` };
      return title ? setTitle(moved, title) : moved;
    }),
  );
}

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
    if (file.path === 'README.md') return setTitle({ ...file, path: systemsReadme.path }, systemsReadme.title);
    const moved = { ...file, path: carPath(file.path) };
    return file.path === 'architecture.md' ? setTitle(moved, 'Overview') : moved;
  }),
  ...carMeta,
  ...softwareFiles(),
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
