export const SITE_URL = 'https://arcarleton.ca'; // TBC: the domain CSES serves the site on
export const INSTAGRAM_URL = 'https://instagram.com/arcarleton';
export const INSTAGRAM_HANDLE = '@arcarleton';
export const DISCORD_URL = 'https://discord.gg/7Vcy79VDqV';
export const GITHUB_ORG_URL = 'https://github.com/Autonomous-Robotics-Carleton';
export const GITHUB_REPO_URL = `${GITHUB_ORG_URL}/arc-monorepo`;
export const LINKEDIN_URL = ''; // TODO: the club's LinkedIn page; add it to SOCIAL_LINKS once it exists
export const EMAIL = 'contact@arcarleton.ca';
export const DOCS_URL = 'https://docs.arcarleton.ca';

// Behold.so feed for @arcarleton (the "ARC" feed in the Behold dashboard; see docs/instagram.mdx).
// Read on the server by app/api/instagram. Empty shows a follow-us panel instead of posts.
export const BEHOLD_FEED_ID = 'xoLErkzH8KcqYxiD79aO';

export const SOCIAL_LINKS = [
  { label: 'Instagram', href: INSTAGRAM_URL, icon: 'instagram' },
  { label: 'Discord', href: DISCORD_URL, icon: 'discord' },
  { label: 'GitHub', href: GITHUB_ORG_URL, icon: 'github' },
] as const;

// Club documents in the ARC shared drive. Anyone opening them needs view access:
// set each to "Anyone with the link can view" in Drive, or visitors get a request-access page.
export const CONSTITUTION_URL = 'https://docs.google.com/document/d/1z9AmYuL1joLx6j9R_Rj6ulLbbQR0vW5DfG2PzHBKvtk/view';
export const MINUTES_URL = 'https://drive.google.com/drive/folders/1DEaHyFJ5IjTycTsccgi_82XLGZBNRNkJ';
