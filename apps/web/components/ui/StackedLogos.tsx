'use client';

import { useRef, useCallback, type ReactNode } from 'react';

interface StackedLogosProps {
  /** Each inner array is one column of logos that cycle */
  logoGroups: ReactNode[][];
  columns?: number;
  duration?: number;
  stagger?: number;
  className?: string;
}

export default function StackedLogos({
  logoGroups,
  duration = 30,
  stagger = 2,
  className = '',
}: StackedLogosProps) {
  const containerRef = useRef<HTMLDivElement>(null);
  const gridRef = useRef<HTMLDivElement>(null);

  const columns = logoGroups.length;
  const itemsPerCol = logoGroups[0]?.length || 1;

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
        '--items': itemsPerCol,
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

        {logoGroups.map((logos, colIdx) => (
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

            {logos.map((logo, itemIdx) => (
              <div
                key={itemIdx}
                className="stacked-logos__item col-start-1 row-start-1 grid place-items-center px-4 py-10 md:px-8 md:py-14"
                style={{ '--i': itemIdx } as React.CSSProperties}
              >
                <div className="stacked-logos__logo flex h-8 w-full items-center justify-center [&>svg]:h-full [&>svg]:w-auto">
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
