import { createMDX } from 'fumadocs-mdx/next';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const withMDX = createMDX();

/** @type {import('next').NextConfig} */
const config = {
  reactStrictMode: true,
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
    };
    return Object.entries(moved).map(([from, to]) => ({
      source: `/docs/${from}`,
      destination: to ? `/docs/${to}` : '/docs',
      permanent: true,
    }));
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

export default withMDX(config);
