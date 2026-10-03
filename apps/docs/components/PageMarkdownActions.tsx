'use client';

import { useState } from 'react';

/** Copy the page as Markdown (for pasting into an LLM), or open the .md version. */
export function PageMarkdownActions({ markdownUrl }: { markdownUrl: string }) {
  const [state, setState] = useState<'idle' | 'copied' | 'failed'>('idle');

  const copy = async () => {
    try {
      const response = await fetch(markdownUrl);
      if (!response.ok) throw new Error(response.statusText);
      await navigator.clipboard.writeText(await response.text());
      setState('copied');
    } catch {
      setState('failed');
    }
    setTimeout(() => setState('idle'), 2000);
  };

  const button =
    'rounded-md border px-2.5 py-1 text-xs font-medium text-fd-muted-foreground transition-colors hover:bg-fd-accent hover:text-fd-accent-foreground';

  return (
    <div className="not-prose -mt-2 mb-4 flex gap-2">
      <button type="button" onClick={copy} className={button}>
        {state === 'copied' ? 'Copied' : state === 'failed' ? 'Copy failed' : 'Copy as Markdown'}
      </button>
      <a href={markdownUrl} className={button}>
        View as Markdown
      </a>
    </div>
  );
}
