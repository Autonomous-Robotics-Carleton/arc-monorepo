import { createMDX } from 'fumadocs-mdx/next';
import { PHASE_PRODUCTION_SERVER } from 'next/constants.js';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));

/** @type {import('next').NextConfig} */
const config = {
  reactStrictMode: true,
  // workspace library shipped as TypeScript source; bundle it like app code
  transpilePackages: ['@arc/systems-model'],
  turbopack: {
    root: resolve(__dirname, '../..'),
  },
  // Pages from before the 2026-10 reorganisation
  async redirects() {
    const moved = {
      installation: 'handbook/dev-setup',
      'fusion-setup': 'handbook/fusion-setup',
      'get-started': 'handbook/contributing',
      'conventional-branch': 'handbook/contributing',
      'conventional-commits': 'handbook/contributing',
      'pull-request': 'handbook/contributing',
      jetson_discord_notifier: 'handbook/orin-dev-kit/discord-notifier',
      StartVNC_mdx_fixed: 'handbook/orin-dev-kit/auto-vnc',
      'website-overview': 'handbook/website/overview',
      'website-structure': 'handbook/website/structure',
      'website-design-system': 'handbook/website/design-system',
      'website-components': 'handbook/website/components',
      'website-content': 'handbook/website/content',
      'website-instagram': 'handbook/website/instagram',
      'tutorial-1': '',
      'tutorial-2': '',
      'tutorial-3': '',
      'knowledge-base': '',
      'linux-cheat': '',
      // systems/ pages moved out of car/ into their discipline's section (2026-10-25)
      'car/handoff/electrical': 'electrical',
      'car/bom/electrical': 'electrical/bom',
      'car/bom/motherboard-wiring': 'electrical/motherboard-wiring',
      'car/bom/pcb-parts-selection': 'electrical/pcb-parts-selection',
      'car/bom/pcb-component-breakdown': 'electrical/pcb-component-breakdown',
      'car/bom/sensor-candidate-review': 'electrical/sensor-candidate-review',
      'car/bom/jetson-carrier-reference-parts': 'electrical/jetson-carrier-parts',
      'car/bom/total-electrical-cost': 'electrical/cost',
      'car/bom/jlc-pcba-cost-estimate': 'electrical/pcb-funding-estimate',
      'car/mechanical/layout-brief': 'mechanical/layout-brief',
      'car/mechanical/cad-workflow': 'mechanical/cad-workflow',
      'car/bom/mechanical': 'mechanical/bom',
      'car/software/architecture': 'software/architecture',
      'car/software/setup-plan': 'software/setup-plan',
    };
    return [
      // the site is all docs for now; not permanent, so / can become a landing page later
      { source: '/', destination: '/docs', permanent: false },
      ...Object.entries(moved).flatMap(([from, to]) => [
        { source: `/docs/${from}`, destination: to ? `/docs/${to}` : '/docs', permanent: true },
        // the page as Markdown (rewrites below)
        ...(from.startsWith('car/') ? [{ source: `/docs/${from}.md`, destination: `/docs/${to}.md`, permanent: true }] : []),
      ]),
    ];
  },
  // /docs/<page>.md: each page as Markdown (app/docs-md)
  async rewrites() {
    return {
      beforeFiles: [
        { source: '/docs.md', destination: '/docs-md' },
        { source: '/docs/:path*.md', destination: '/docs-md/:path*' },
      ],
    };
  },
  images: {
    remotePatterns: [
      {
        protocol: 'https',
        hostname: 'github.com',
      },
      {
        protocol: 'https',
        hostname: 'user-attachments.githubusercontent.com', // GitHub issue/PR attachments
      },
      {
        protocol: 'https',
        hostname: 'avatars.githubusercontent.com', // GitHub avatars (fixes your error)
      },
    ],
  },
};

// `next start` serves the compiled build. Skipping fumadocs-mdx there stops it
// recompiling source.config.ts, which needs sources the runtime image doesn't have.
const nextConfig = (phase) => (phase === PHASE_PRODUCTION_SERVER ? config : createMDX()(config));

export default nextConfig;
