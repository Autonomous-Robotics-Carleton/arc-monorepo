# docs

The docs site, [docs.arcarleton.ca](https://docs.arcarleton.ca): Fumadocs on Next.js.

```bash
npx nx dev docs     # http://localhost:3000
npx nx build docs
```

Don't run `nx build docs` while `nx dev docs` is running: both write `.next`.

## Where pages come from

Declared in `source.config.ts` and combined in `lib/source.ts`; edit the source, not a copy:

| Source | On the site | Format |
| --- | --- | --- |
| `content/docs/` | The index, the handbook and the car's Topology page (`car/topology.mdx`, which embeds `systems/topology.html` from `/embed/topology`) | MDX |
| `../../systems/` | `/docs/car` (its README is `/docs/how-these-docs-work`; the ADR template is left out) | Markdown |
| `../web/docs/` | `/docs/handbook/website` | MDX |
| `../../platform/dev-kit/` | `/docs/handbook/orin-dev-kit` | MDX |
| `../../.github/CONTRIBUTING.md` | `/docs/handbook/contributing` | Markdown |

The car section's sidebar order and folder titles are set in code (`carSidebar` in `lib/source.ts`), not in `meta.json`: a new folder under `systems/` needs an entry there. The budget CSVs get generated pages (`app/docs/car/budgets/data/`).

Links between pages are relative file paths, so they also work on GitHub; IDs like SYS-04 are linked automatically (`lib/remark-system-ids.ts`). For LLMs: `/llms.txt`, `/llms-full.txt`, `/ids.json`, and every page as Markdown at its URL + `.md`. More in `AGENTS.md` (Docs site).

## Deployment

`apps/docs/Dockerfile` builds the image; CI publishes it from `main` and triggers the server's update hook.
