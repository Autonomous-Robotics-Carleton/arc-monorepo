'use client';

import { useRef, useCallback, type ReactNode } from 'react';

interface StackedLogosProps {
  logos: ReactNode[];
  /** Most columns to show; with more logos than this, each column cycles */
  maxColumns?: number;
  duration?: number;
  stagger?: number;
  className?: string;
}

// The appear keyframes show each item for a quarter of the cycle
const ITEMS_PER_CYCLE = 4;

/** Deal logos round-robin into columns, padding cycling columns to a full cycle. */
function buildColumns(logos: ReactNode[], maxColumns: number): ReactNode[][] {
  const columns = Math.max(1, Math.min(logos.length, maxColumns));
  const groups: ReactNode[][] = Array.from({ length: columns }, () => []);
  logos.forEach((logo, i) => groups[i % columns].push(logo));
  return groups.map((group) =>
    group.length > 1
      ? Array.from({ length: ITEMS_PER_CYCLE }, (_, i) => group[i % group.length])
      : group,
  );
}

export default function StackedLogos({
  logos,
  maxColumns = 4,
  duration = 30,
  stagger = 2,
  className = '',
}: StackedLogosProps) {
  const containerRef = useRef<HTMLDivElement>(null);
  const gridRef = useRef<HTMLDivElement>(null);

  const logoGroups = buildColumns(logos, maxColumns);
  const columns = logoGroups.length;

  const handleMouseMove = useCallback((e: React.MouseEvent<HTMLDivElement>) => {
    if (!containerRef.current || !gridRef.current) return;
    const rect = gridRef.current.getBoundingClientRect();
    containerRef.current.style.setProperty('--mouse-x', `${e.clientX - rect.left}px`);
    containerRef.current.style.setProperty('--mouse-y', `${e.clientY - rect.top}px`);
  }, []);

  return (
    <div
      ref={containerRef}
      className={`stacked-logos group relative w-full ${className}`}
      style={{
        '--duration': duration,
        '--items': ITEMS_PER_CYCLE,
        '--lists': columns,
        '--stagger': stagger,
      } as React.CSSProperties}
      onMouseMove={handleMouseMove}
    >
      <div
        ref={gridRef}
        className="relative mx-auto grid w-full"
        style={{ gridTemplateColumns: `repeat(${columns}, 1fr)` }}
      >
        {/* Mouse-follow glow — hidden by default, shown on group hover via CSS */}
        <div
          className="stacked-logos__glow pointer-events-none absolute inset-0 z-10"
          style={{
            opacity: 0,
            background: 'radial-gradient(350px circle at var(--mouse-x, 50%) var(--mouse-y, 50%), rgba(212,212,212,0.12), transparent 70%)',
          }}
        />
        {/* Mouse-follow border glow */}
        <div
          className="stacked-logos__border-glow pointer-events-none absolute inset-0 z-20"
          style={{
            opacity: 0,
            background: 'radial-gradient(500px circle at var(--mouse-x, 50%) var(--mouse-y, 50%), rgba(212,212,212,0.5), transparent 40%)',
            maskImage: `repeating-linear-gradient(to right, transparent, transparent calc(100% / ${columns} - 1px), black calc(100% / ${columns} - 1px), black calc(100% / ${columns})), linear-gradient(to bottom, black 0, black 1px, transparent 1px, transparent calc(100% - 1px), black calc(100% - 1px), black 100%)`,
            WebkitMaskImage: `repeating-linear-gradient(to right, transparent, transparent calc(100% / ${columns} - 1px), black calc(100% / ${columns} - 1px), black calc(100% / ${columns})), linear-gradient(to bottom, black 0, black 1px, transparent 1px, transparent calc(100% - 1px), black calc(100% - 1px), black 100%)`,
            maskComposite: 'add' as string,
          }}
        />

        {logoGroups.map((column, colIdx) => (
          <div
            key={colIdx}
            className="stacked-logos__cell relative grid"
            style={{ '--index': colIdx, gridTemplate: '1fr / 1fr' } as React.CSSProperties}
          >
            {/* Cell borders */}
            <div className="absolute top-0 right-0 bottom-0 w-px" style={{ backgroundColor: 'rgba(212,212,212,0.22)' }} />
            <div className="absolute right-0 bottom-0 left-0 h-px" style={{ backgroundColor: 'rgba(212,212,212,0.22)' }} />
            <div className="absolute top-0 right-0 left-0 h-px" style={{ backgroundColor: 'rgba(212,212,212,0.22)' }} />
            {colIdx === 0 && <div className="absolute top-0 bottom-0 left-0 w-px" style={{ backgroundColor: 'rgba(212,212,212,0.22)' }} />}

            {column.map((logo, itemIdx) => (
              <div
                key={itemIdx}
                className="stacked-logos__item col-start-1 row-start-1 grid place-items-center px-4 py-10 md:px-8 md:py-14"
                style={{ '--i': itemIdx } as React.CSSProperties}
              >
                <div
                  className={`flex h-11 w-full items-center justify-center ${
                    column.length > 1 ? 'stacked-logos__logo' : ''
                  }`}
                >
                  {logo}
                </div>
              </div>
            ))}
          </div>
        ))}
      </div>
    </div>
  );
}
