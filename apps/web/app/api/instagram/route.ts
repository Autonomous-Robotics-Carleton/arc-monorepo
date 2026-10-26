import { BEHOLD_FEED_ID } from '@/data/social';

// Behold's free plan counts every feed request as a view (1,200 a month), so the feed is fetched
// here on the server and cached for 6 hours (~120 requests a month) instead of once per visitor.
// Dynamic, so builds (CI, Docker) never call Behold; the fetch below is still cached for 6 hours.
export const dynamic = 'force-dynamic';

type BeholdSize = { mediaUrl: string; width: number; height: number };

type BeholdPost = {
  id: string;
  permalink: string;
  mediaType?: 'IMAGE' | 'VIDEO' | 'CAROUSEL_ALBUM';
  mediaUrl: string; // the raw image, or the video file for videos
  thumbnailUrl?: string; // videos only
  sizes?: Partial<Record<'small' | 'medium' | 'large' | 'full', BeholdSize>>;
  caption?: string;
  prunedCaption?: string; // caption without hashtags
};

/** What the site's cards need from a post */
export type FeedPost = { id: string; permalink: string; image: string; caption: string };

// Behold's v2 feeds return { ...profile, posts: [...] }; older feeds return the array itself
function parseFeed(data: unknown): BeholdPost[] {
  if (Array.isArray(data)) return data;
  if (data && typeof data === 'object' && Array.isArray((data as { posts?: unknown }).posts)) {
    return (data as { posts: BeholdPost[] }).posts;
  }
  return [];
}

function toFeedPost(post: BeholdPost): FeedPost | null {
  const image =
    post.sizes?.medium?.mediaUrl ?? post.thumbnailUrl ?? (post.mediaType === 'VIDEO' ? undefined : post.mediaUrl);
  if (!image) return null;
  return { id: post.id, permalink: post.permalink, image, caption: post.prunedCaption || post.caption || '' };
}

export async function GET() {
  if (!BEHOLD_FEED_ID) return Response.json({ posts: [] });
  try {
    const res = await fetch(`https://feeds.behold.so/${BEHOLD_FEED_ID}`, { next: { revalidate: 21600 } });
    if (!res.ok) throw new Error(`Behold returned ${res.status}`);
    const posts = parseFeed(await res.json())
      .map(toFeedPost)
      .filter((p): p is FeedPost => p !== null);
    return Response.json({ posts });
  } catch {
    return Response.json({ posts: [] });
  }
}
