'use client';

/**
 * Aceternity "Background Ripple Effect" — flat orthographic grid.
 * No isometric skew. Hover highlights nearby cells; click ripples outward.
 *
 * Uses direct DOM manipulation so mouse events never trigger React re-renders.
 */
import { useEffect, useRef } from 'react';

const CELL = 72;        // px — grid cell size
const HOVER_R = 1.5;   // cells — highlight radius around cursor
const RIPPLE_R = 5;    // cells — ripple spread radius on click
const RIPPLE_MS = 1200; // ms  — ripple animation duration

const BORDER   = 'rgba(212, 212, 212, 0.13)';
const HOVER_BG = 'rgba(212, 212, 212, 0.10)';

function dist(r1: number, c1: number, r2: number, c2: number) {
  return Math.sqrt((r1 - r2) ** 2 + (c1 - c2) ** 2);
}

function prefersReduced() {
  return (
    typeof window !== 'undefined' &&
    window.matchMedia('(prefers-reduced-motion: reduce)').matches
  );
}

export default function BackgroundBoxes({ className = '' }: { className?: string }) {
  const gridRef = useRef<HTMLDivElement>(null);

  useEffect(() => {
    const grid = gridRef.current;
    if (!grid) return;

    /* ── Build the DOM grid imperatively ── */
    const cols = Math.ceil(window.innerWidth  / CELL) + 1;
    const rows = Math.ceil(window.innerHeight / CELL) + 1;

    grid.style.display = 'grid';
    grid.style.gridTemplateColumns = `repeat(${cols}, ${CELL}px)`;
    grid.style.gridTemplateRows    = `repeat(${rows}, ${CELL}px)`;

    const cells: HTMLDivElement[] = [];
    for (let i = 0; i < rows * cols; i++) {
      const el = document.createElement('div');
      el.style.cssText = `border:1px solid ${BORDER}; background-color:transparent; transition:background-color 250ms ease;`;
      el.dataset.r = String(Math.floor(i / cols));
      el.dataset.c = String(i % cols);
      grid.appendChild(el);
      cells.push(el);
    }

    /* ── Hover: highlight cells near cursor ── */
    const onMove = (e: MouseEvent) => {
      if (prefersReduced()) return;
      const cr = Math.floor(e.clientY / CELL);
      const cc = Math.floor(e.clientX / CELL);
      for (const cell of cells) {
        if (cell.classList.contains('bp-rippling')) continue;
        const d = dist(+cell.dataset.r!, +cell.dataset.c!, cr, cc);
        cell.style.backgroundColor = d <= HOVER_R ? HOVER_BG : 'transparent';
      }
    };

    /* ── Click: ripple spreading outward ── */
    const onClick = (e: MouseEvent) => {
      if (prefersReduced()) return;
      const cr = Math.floor(e.clientY / CELL);
      const cc = Math.floor(e.clientX / CELL);
      for (const cell of cells) {
        const d = dist(+cell.dataset.r!, +cell.dataset.c!, cr, cc);
        if (d > RIPPLE_R) continue;
        const delay = Math.round(d * 80); // ms
        cell.style.setProperty('--rd', `${delay}ms`);
        // Force reflow so removing + re-adding the class restarts animation
        cell.classList.remove('bp-rippling');
        void (cell as HTMLElement).offsetWidth;
        cell.classList.add('bp-rippling');
        cell.addEventListener(
          'animationend',
          () => cell.classList.remove('bp-rippling'),
          { once: true },
        );
      }
    };

    window.addEventListener('mousemove', onMove, { passive: true });
    window.addEventListener('click',     onClick, { passive: true });

    return () => {
      window.removeEventListener('mousemove', onMove);
      window.removeEventListener('click',     onClick);
      grid.innerHTML = '';
    };
  }, []);

  return (
    <div
      className={`pointer-events-none relative w-full h-full overflow-hidden ${className}`}
      aria-hidden="true"
    >
      {/* Flat grid — built imperatively for zero re-render overhead */}
      <div ref={gridRef} className="absolute inset-0" />

      {/* Vignette — keeps edges dark for readability */}
      <div
        className="absolute inset-0 pointer-events-none"
        style={{
          background:
            'radial-gradient(ellipse at 50% 40%, transparent 35%, rgba(10, 10, 10, 0.60) 100%)',
        }}
      />

      {/* Paper noise */}
      <div className="bp-noise absolute inset-0 pointer-events-none" />
    </div>
  );
}
