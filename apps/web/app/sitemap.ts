import type { MetadataRoute } from 'next';
import { SITE_URL } from '@/data/social';

export default function sitemap(): MetadataRoute.Sitemap {
  return ['', '/robots', '/team', '/governance', '/contact', '/blueprint'].map((path) => ({
    url: `${SITE_URL}${path}`,
  }));
}
