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
| `content/docs/` | Handbook and index | MDX |
| `../../systems/` | `/docs/car` | Markdown |
| `../web/docs/` | Website docs | MDX |
| `../../platform/dev-kit/` | Handbook (Orin Nano dev kit) | MDX |
| `../../.github/CONTRIBUTING.md` | Contributing | Markdown |

Links between pages are relative file paths, so they also work on GitHub; IDs like SYS-04 are linked automatically (`lib/remark-system-ids.ts`). For LLMs: `/llms.txt`, `/llms-full.txt`, `/ids.json`, and every page as Markdown at its URL + `.md`. More in `AGENTS.md` (Docs site).

## Deployment

`apps/docs/Dockerfile` builds the image; CI publishes it from `main` and triggers the server's update hook.
