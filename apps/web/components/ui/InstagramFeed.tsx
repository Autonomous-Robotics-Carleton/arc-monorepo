'use client';

import { useEffect, useState } from 'react';
import BrandIcon from './BrandIcon';
import type { FeedPost } from '@/app/api/instagram/route';
import { INSTAGRAM_HANDLE, INSTAGRAM_URL } from '@/data/social';

type FeedState = { status: 'loading' } | { status: 'ready'; posts: FeedPost[] } | { status: 'empty' };

/** Latest @arcarleton posts, from the site's cached Behold feed (app/api/instagram) */
export default function InstagramFeed({ count = 6 }: { count?: number }) {
  const [state, setState] = useState<FeedState>({ status: 'loading' });

  useEffect(() => {
    let cancelled = false;
    fetch('/api/instagram')
      .then((r) => (r.ok ? r.json() : Promise.reject(new Error(String(r.status)))))
      .then((data: { posts: FeedPost[] }) => {
        const posts = data.posts.slice(0, count);
        if (!cancelled) setState(posts.length ? { status: 'ready', posts } : { status: 'empty' });
      })
      .catch(() => {
        if (!cancelled) setState({ status: 'empty' });
      });
    return () => {
      cancelled = true;
    };
  }, [count]);

  if (state.status === 'empty') {
    return (
      <a
        href={INSTAGRAM_URL}
        target="_blank"
        rel="noopener noreferrer"
        className="group flex flex-col items-start justify-between gap-6 border border-bp-line bg-black/70 p-8 transition-colors hover:border-accent/40 sm:flex-row sm:items-center md:p-10"
      >
        <div className="flex items-center gap-4">
          <BrandIcon brand="instagram" className="h-7 w-7 text-fg/60 transition-colors group-hover:text-arc-red" />
          <div>
            <p className="text-base text-fg/85">{INSTAGRAM_HANDLE}</p>
            <p className="mt-1 text-sm leading-relaxed text-fg/55">
              Build logs, test days and events. Follow along on Instagram.
            </p>
          </div>
        </div>
        <span className="shrink-0 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-colors group-hover:border-accent group-hover:bg-accent/10">
          FOLLOW
        </span>
      </a>
    );
  }

  const posts: (FeedPost | null)[] =
    state.status === 'ready' ? state.posts : Array.from({ length: count }, () => null);

  return (
    <div className="grid grid-cols-1 border-t border-l border-bp-line sm:grid-cols-2 lg:grid-cols-3">
      {posts.map((post, i) =>
        post ? (
          <a
            key={post.id}
            href={post.permalink}
            target="_blank"
            rel="noopener noreferrer"
            className="group flex flex-col border-r border-b border-bp-line bg-black/70 p-3"
          >
            <div className="aspect-square w-full overflow-hidden bg-bp-blue-light">
              {/* eslint-disable-next-line @next/next/no-img-element -- Behold's CDN already serves sized WebP */}
              <img
                src={post.image}
                alt={post.caption || 'Instagram post'}
                loading="lazy"
                className="h-full w-full object-cover transition-transform duration-500 group-hover:scale-[1.03]"
              />
            </div>
            {post.caption && (
              <p className="line-clamp-2 px-1 pt-3 text-sm leading-relaxed text-fg/60">{post.caption}</p>
            )}
            <p className="mt-auto flex items-center gap-2 px-1 pt-3 pb-1 text-xs text-fg/40 transition-colors group-hover:text-arc-red">
              <BrandIcon brand="instagram" className="h-3.5 w-3.5" />
              View on Instagram
            </p>
          </a>
        ) : (
          <div key={i} className="animate-pulse border-r border-b border-bp-line bg-black/70 p-3">
            <div className="aspect-square w-full bg-bp-blue-light" />
            <div className="h-14" />
          </div>
        ),
      )}
    </div>
  );
}
