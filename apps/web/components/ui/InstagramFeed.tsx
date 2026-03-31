'use client';

import { useEffect, useState } from 'react';
import CornerTicks from './CornerTicks';

type BeholdPost = {
  id: string;
  mediaUrl: string;
  thumbnailUrl?: string;
  permalink: string;
  caption?: string;
};

const PLACEHOLDER_COUNT = 6;

export default function InstagramFeed({ feedId }: { feedId?: string }) {
  const [posts, setPosts] = useState<BeholdPost[]>([]);

  useEffect(() => {
    if (!feedId) return;
    fetch(`https://feeds.behold.so/${feedId}`)
      .then((r) => r.json())
      .then((data) => setPosts(data.slice(0, PLACEHOLDER_COUNT)))
      .catch(() => {});
  }, [feedId]);

  const items: (BeholdPost | null)[] =
    posts.length > 0
      ? posts
      : Array.from({ length: PLACEHOLDER_COUNT }, () => null);

  return (
    <div className="grid grid-cols-1 gap-4 sm:grid-cols-2 lg:grid-cols-3">
      {items.map((post, i) => (
        <div
          key={post?.id ?? i}
          className="group relative border border-bp-line transition-all duration-300 hover:border-accent/30 bp-glow-hover bp-glass"
        >
          <CornerTicks size={8} />
          {/* Image area */}
          <div className="aspect-square w-full overflow-hidden bg-bp-blue-light">
            {post?.mediaUrl && (
              // eslint-disable-next-line @next/next/no-img-element
              <img
                src={post.thumbnailUrl ?? post.mediaUrl}
                alt={post.caption ?? 'Instagram post'}
                className="h-full w-full object-cover transition-transform duration-500 group-hover:scale-105"
              />
            )}
          </div>
          {/* Caption */}
          {post?.caption && (
            <p className="line-clamp-2 px-4 pt-3 text-xs leading-relaxed text-fg/50">
              {post.caption}
            </p>
          )}
          {/* Link */}
          <div className="px-4 py-3">
            {post?.permalink ? (
              <a
                href={post.permalink}
                target="_blank"
                rel="noopener noreferrer"
                className="text-xs tracking-widest text-fg/40 transition-colors hover:text-accent"
              >
                VIEW POST →
              </a>
            ) : (
              <span className="text-xs tracking-widest text-fg/20">
                VIEW POST →
              </span>
            )}
          </div>
        </div>
      ))}
    </div>
  );
}
