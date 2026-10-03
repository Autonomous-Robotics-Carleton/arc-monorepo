import { devKitDocs, docs, repoDocs, webDocs } from '@/.source';
import { loader, type Source, type VirtualFile } from 'fumadocs-core/source';
import { createMDXSource } from 'fumadocs-mdx';

// Where single repo files appear in the sidebar, and their title there.
const repoPages: Record<string, { path: string; title: string }> = {
  'CONTRIBUTING.md': { path: 'handbook/contributing.md', title: 'Contributing' },
  'systems/README.md': { path: 'how-these-docs-work.md', title: 'How these docs work' },
};

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
