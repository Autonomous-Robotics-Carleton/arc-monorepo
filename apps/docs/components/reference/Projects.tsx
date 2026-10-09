import { readdirSync, readFileSync } from 'node:fs';
import { join, relative, resolve } from 'node:path';

// The Nx projects and their targets, read from each project.json when the site
// is built. The workspace has no Nx plugins, so project.json is complete.

const repoRoot = resolve(process.cwd(), '../..'); // builds run in apps/docs
const skip = new Set(['node_modules', '.git', '.next', '.nx', 'build', 'install', 'log', 'bldc']);
const repoBlob = 'https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/blob/main/';

function projectFiles(dir: string, depth = 0): string[] {
  if (depth > 3) return [];
  return readdirSync(dir, { withFileTypes: true }).flatMap((entry) => {
    if (entry.isDirectory()) return skip.has(entry.name) ? [] : projectFiles(join(dir, entry.name), depth + 1);
    return entry.name === 'project.json' ? [join(dir, entry.name)] : [];
  });
}

interface Target {
  command?: string;
  dependsOn?: (string | { projects?: string[]; target: string })[];
  options?: { command?: string; commands?: string[]; cwd?: string };
}

interface Project {
  name: string;
  root: string;
  tags?: string[];
  implicitDependencies?: string[];
  targets?: Record<string, Target>;
}

function commandsOf(target: Target): string[] {
  return target.options?.commands ?? [target.command ?? target.options?.command ?? ''].filter(Boolean);
}

function dependsOnText(target: Target): string | undefined {
  const items = target.dependsOn?.map((dep) => {
    if (typeof dep === 'string') return dep.startsWith('^') ? `${dep.slice(1)} of its dependencies` : dep;
    return `${dep.target} of ${dep.projects?.join(', ') ?? 'its dependencies'}`;
  });
  return items?.length ? `after ${items.join(', ')}` : undefined;
}

/** Every Nx project: where it lives, where it runs, and what each target runs. */
export function NxProjects() {
  const projects: Project[] = projectFiles(repoRoot)
    .map((file) => ({
      ...(JSON.parse(readFileSync(file, 'utf8')) as Omit<Project, 'root'>),
      root: relative(repoRoot, join(file, '..')),
    }))
    .sort((a, b) => a.root.localeCompare(b.root));

  return (
    <>
      {projects.map((project) => {
        const container = project.tags?.includes('env:dev-container');
        return (
          <section key={project.name}>
            <h3 id={project.name}>
              <code>{project.name}</code>
            </h3>
            <p>
              <a href={`${repoBlob}${project.root}/project.json`}>
                <code>{project.root}/</code>
              </a>
              {container ? ': runs in the dev container (tag env:dev-container)' : ': runs on any machine with Node'}
              {project.implicitDependencies?.length
                ? `; depends on ${project.implicitDependencies.join(', ')}`
                : ''}
              .
            </p>
            <table>
              <thead>
                <tr>
                  <th>Target</th>
                  <th>Runs</th>
                </tr>
              </thead>
              <tbody>
                {Object.entries(project.targets ?? {}).map(([name, target]) => {
                  const cwd = target.options?.cwd;
                  const after = dependsOnText(target);
                  return (
                    <tr key={name}>
                      <td>
                        <code>
                          npx nx {name} {project.name}
                        </code>
                      </td>
                      <td>
                        {commandsOf(target).map((command) => (
                          <div key={command}>
                            <code>{command}</code>
                          </div>
                        ))}
                        {(cwd || after) && (
                          <div>
                            {[cwd && `in ${cwd}/`, after].filter(Boolean).join('; ')}
                          </div>
                        )}
                      </td>
                    </tr>
                  );
                })}
              </tbody>
            </table>
          </section>
        );
      })}
    </>
  );
}
