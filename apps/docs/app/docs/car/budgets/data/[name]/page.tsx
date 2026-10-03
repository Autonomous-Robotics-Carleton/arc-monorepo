import { readdirSync, readFileSync } from 'node:fs';
import { join } from 'node:path';
import type { Metadata } from 'next';
import { notFound } from 'next/navigation';
import { DocsBody, DocsDescription, DocsPage, DocsTitle } from 'fumadocs-ui/page';
import { parseCsv } from '@arc/systems-model';
import { linkIds, systemsRoot } from '@/lib/systems';

// One page per budget CSV in systems/budgets/, rendered as a table at build.
const budgets = join(systemsRoot, 'budgets');
const repoBlob = 'https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/blob/main/systems/budgets/';

export function generateStaticParams() {
  return readdirSync(budgets)
    .filter((file) => file.endsWith('.csv'))
    .map((file) => ({ name: file.replace(/\.csv$/, '') }));
}

export default async function BudgetPage(props: { params: Promise<{ name: string }> }) {
  const { name } = await props.params;
  const file = join(budgets, `${name}.csv`);
  let rows: string[][];
  try {
    rows = parseCsv(readFileSync(file, 'utf8'));
  } catch {
    notFound();
  }
  const [header, ...body] = rows;

  return (
    <DocsPage full>
      <DocsTitle>{name}.csv</DocsTitle>
      <DocsDescription>
        Budget data from <a href={`${repoBlob}${name}.csv`}>systems/budgets/{name}.csv</a>. Edit the CSV; this
        page is generated from it.
      </DocsDescription>
      <DocsBody>
        <table>
          <thead>
            <tr>
              {header.map((cell, i) => (
                <th key={i}>{cell.replaceAll('_', ' ')}</th>
              ))}
            </tr>
          </thead>
          <tbody>
            {body.map((row, r) => (
              <tr key={r}>
                {row.map((cell, c) => (
                  <td key={c}>
                    {linkIds(cell, file).map((part, p) =>
                      typeof part === 'string' ? (
                        part
                      ) : (
                        <a key={p} href={part.href} title={part.title}>
                          {part.id}
                        </a>
                      ),
                    )}
                  </td>
                ))}
              </tr>
            ))}
          </tbody>
        </table>
      </DocsBody>
    </DocsPage>
  );
}

export async function generateMetadata(props: { params: Promise<{ name: string }> }): Promise<Metadata> {
  const { name } = await props.params;
  return { title: `${name}.csv`, description: `Budget data from systems/budgets/${name}.csv` };
}
