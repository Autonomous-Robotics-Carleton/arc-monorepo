import {
  configDocs,
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

// systems/ mounts at car/, except the pages one discipline reads, which go to
// that discipline's section (electrical/, mechanical/, software/). Folder
// READMEs become folder index pages, and the architecture overview is the car
// section's landing page. A page moved here needs a redirect in next.config.mjs.
const systemsPages: Record<string, string> = {
  'architecture.md': 'car/index.md',
  'handoff/electrical.md': 'electrical/index.md',
  'bom/electrical.md': 'electrical/bom.md',
  'bom/motherboard-wiring.md': 'electrical/motherboard-wiring.md',
  'bom/pcb-parts-selection.md': 'electrical/pcb-parts-selection.md',
  'bom/pcb-component-breakdown.md': 'electrical/pcb-component-breakdown.md',
  'bom/sensor-candidate-review.md': 'electrical/sensor-candidate-review.md',
  'bom/jetson-carrier-reference-parts.md': 'electrical/jetson-carrier-parts.md',
  'bom/total-electrical-cost.md': 'electrical/cost.md',
  'bom/jlc-pcba-cost-estimate.md': 'electrical/pcb-funding-estimate.md',
  'mechanical/layout-brief.md': 'mechanical/layout-brief.md',
  'mechanical/cad-workflow.md': 'mechanical/cad-workflow.md',
  'bom/mechanical.md': 'mechanical/bom.md',
  'handoff/software.md': 'software/handoff.md',
  'software/architecture.md': 'software/architecture.md',
  'software/simulation.md': 'software/simulation.md',
  'software/platform-plan.md': 'software/platform-plan.md',
  'software/setup-plan.md': 'software/setup-plan.md',
};

function systemsPath(path: string): string {
  return systemsPages[path] ?? `car/${path.replace(/(^|\/)README\.md$/, '$1index.md')}`;
}

// Sidebar titles for systems/ pages, by their path on the site. A page not
// listed is titled by its own `# heading`.
const systemsTitles: Record<string, string> = {
  'car/index.md': 'Overview',
  'car/requirements/system.md': 'System requirements',
  'car/risks.md': 'Risk register',
  'car/reviews/fab-gate.md': 'Fab gate checklist',
  'electrical/index.md': 'Electrical handoff',
  'electrical/bom.md': 'Electrical BOM',
  'electrical/motherboard-wiring.md': 'Motherboard wiring',
  'electrical/jetson-carrier-parts.md': 'Jetson carrier reference parts',
  'electrical/pcb-funding-estimate.md': 'PCB funding estimate',
  'electrical/cost.md': 'Electrical cost by build',
  'electrical/sensor-candidate-review.md': 'Sensor candidate review',
  'mechanical/bom.md': 'Mechanical BOM',
  'mechanical/layout-brief.md': 'Layout brief',
  'mechanical/cad-workflow.md': 'CAD workflow',
  'software/handoff.md': 'Software handoff',
  'software/architecture.md': 'Software architecture',
  'software/simulation.md': 'Simulation (digital twin)',
  'software/platform-plan.md': 'Platform plan',
  'software/setup-plan.md': 'Setup plan',
};

// Sidebars for the systems/ sections, kept here so systems/ holds no site
// config. Folders with a single page are flattened ("...folder"); "..." picks
// up new files. software/ has its own meta.json in content/docs/software/.
const systemsSidebar: Record<string, { title: string; pages?: string[] }> = {
  car: {
    title: 'The car',
    pages: [
      'index', 'builds', 'topology', '...requirements', '...verification', 'adr', 'icd', 'budgets',
      'risks', 'tests', 'reviews', '...',
    ],
  },
  'car/adr': { title: 'Decisions' },
  'car/icd': { title: 'Interfaces' },
  'car/budgets': { title: 'Budgets' },
  'car/tests': { title: 'Tests' },
  'car/reviews': { title: 'Reviews' },
  electrical: {
    title: 'Electrical',
    pages: [
      'index', 'bom', 'motherboard-wiring', 'pcb-parts-selection', 'pcb-component-breakdown',
      'sensor-candidate-review', 'jetson-carrier-parts', 'cost', 'pcb-funding-estimate', '...',
    ],
  },
  mechanical: { title: 'Mechanical', pages: ['layout-brief', 'cad-workflow', 'bom', '...'] },
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
  [platformDocs, { 'README.md': ['platform', 'Orin image'] }],
  [toolsDocs, { 'README.md': ['tools', 'Tools'] }],
  [configDocs, { 'cars/README.md': ['car-config', 'Car configuration'] }],
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

const systemsMeta: VirtualFile[] = Object.entries(systemsSidebar).map(([folder, data]) => ({
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
    const moved = { ...file, path: systemsPath(file.path) };
    const title = systemsTitles[moved.path];
    return title ? setTitle(moved, title) : moved;
  }),
  ...systemsMeta,
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
