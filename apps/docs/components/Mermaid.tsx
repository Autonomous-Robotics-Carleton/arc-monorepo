'use client';

import { use, useId, useSyncExternalStore } from 'react';
import { useTheme } from 'next-themes';

const noSubscription = () => () => {};

// Draws a ```mermaid block (turned into <Mermaid chart="…" /> by remarkMdxMermaid
// in source.config.ts), so diagrams written for GitHub render on the site too.
// Mermaid needs the browser, so it draws only there (false while prerendering),
// in the page's theme.
export function Mermaid({ chart }: { chart: string }) {
  const inBrowser = useSyncExternalStore(noSubscription, () => true, () => false);
  return inBrowser ? <MermaidDiagram chart={chart} /> : null;
}

// A diagram wider than its column shrinks to fit, but never below this share of
// its natural size: past that its text gets too small, so it scrolls instead.
const minScale = 0.7;

function fit(container: HTMLDivElement) {
  const svg = container.querySelector('svg');
  const natural = svg?.viewBox.baseVal.width;
  if (!svg || !natural) return;
  const scale = Math.max(Math.min(container.clientWidth / natural, 1), minScale);
  svg.style.width = `${natural * scale}px`;
  svg.style.height = 'auto';
}

// One promise per key, so re-renders reuse the library and each finished diagram.
const promises = new Map<string, Promise<unknown>>();

function once<T>(key: string, start: () => Promise<T>): Promise<T> {
  if (!promises.has(key)) promises.set(key, start());
  return promises.get(key) as Promise<T>;
}

function MermaidDiagram({ chart }: { chart: string }) {
  const id = useId().replace(/:/g, '');
  const { resolvedTheme } = useTheme();
  const theme = resolvedTheme === 'dark' ? 'dark' : 'neutral'; // neutral: no yellow groups
  const { default: mermaid } = use(once('mermaid', () => import('mermaid')));

  // Natural size: a diagram wider than the column scrolls instead of shrinking to unreadable text.
  const natural = { useMaxWidth: false };
  mermaid.initialize({
    startOnLoad: false,
    securityLevel: 'strict',
    fontFamily: 'inherit',
    theme,
    flowchart: natural,
    sequence: natural,
    state: natural,
    class: natural,
    er: natural,
    gantt: natural,
  });
  const { svg, bindFunctions } = use(once(`${theme}:${chart}`, () => mermaid.render(`mermaid-${id}`, chart)));

  return (
    <div
      // text-center, not flex: a centred flex child wider than its box is cut off on the left
      className="my-6 overflow-x-auto text-center [&_svg]:inline-block"
      ref={(container) => {
        if (!container) return;
        bindFunctions?.(container);
        fit(container);
      }}
      dangerouslySetInnerHTML={{ __html: svg }}
    />
  );
}
