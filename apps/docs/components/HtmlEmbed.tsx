'use client';

import { useEffect, useRef } from 'react';

/**
 * Embeds a standalone same-origin HTML page. The iframe grows to fit its
 * content and follows the site's light/dark toggle through the page's
 * `data-theme` attribute.
 */
export function HtmlEmbed({ src, title }: { src: string; title: string }) {
  const ref = useRef<HTMLIFrameElement>(null);

  useEffect(() => {
    const frame = ref.current;
    if (!frame) return;

    const sync = () => {
      const doc = frame.contentDocument;
      if (!doc?.documentElement) return;
      const dark = document.documentElement.classList.contains('dark');
      doc.documentElement.dataset.theme = dark ? 'dark' : 'light';
      frame.style.height = `${doc.documentElement.scrollHeight}px`;
    };

    frame.addEventListener('load', sync);
    sync();
    const observer = new MutationObserver(sync);
    observer.observe(document.documentElement, { attributes: true, attributeFilter: ['class'] });
    return () => {
      frame.removeEventListener('load', sync);
      observer.disconnect();
    };
  }, []);

  return (
    <iframe
      ref={ref}
      src={src}
      title={title}
      className="w-full rounded-lg border"
      style={{ height: '80vh' }}
    />
  );
}
