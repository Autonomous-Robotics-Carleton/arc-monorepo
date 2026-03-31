'use client';

import { useEffect, useRef, useState } from 'react';
import ArcLogo from '@/components/ui/ArcLogo';
import CornerTicks from '@/components/ui/CornerTicks';
import { useTransition } from '@/context/TransitionContext';
import { gsap, prefersReducedMotion } from '@/lib/animations';

const GRID_STYLE = {
  backgroundImage: `
    repeating-linear-gradient(0deg,  rgba(212,212,212,0.07) 0px, rgba(212,212,212,0.07) 1px, transparent 1px, transparent 60px),
    repeating-linear-gradient(90deg, rgba(212,212,212,0.07) 0px, rgba(212,212,212,0.07) 1px, transparent 1px, transparent 60px)
  `,
} as React.CSSProperties;

export default function LoadingScreen() {
  const { isInitialLoad, completeInitialLoad } = useTransition();
  const [progress, setProgress] = useState(0);
  const [phase, setPhase] = useState<'logo' | 'loading' | 'complete'>('logo');
  const containerRef = useRef<HTMLDivElement>(null);
  const rafRef = useRef<number>(0);
  const startRef = useRef<number>(0);

  useEffect(() => {
    if (!isInitialLoad) return;

    if (prefersReducedMotion()) {
      completeInitialLoad();
      return;
    }

    const logoTimer = setTimeout(() => {
      setPhase('loading');
      startRef.current = performance.now();

      const COUNTER_DURATION = 1500;
      const animate = (now: number) => {
        const elapsed = now - startRef.current;
        const pct = Math.min(Math.round((elapsed / COUNTER_DURATION) * 100), 100);
        setProgress(pct);
        if (pct < 100) {
          rafRef.current = requestAnimationFrame(animate);
        } else {
          setTimeout(() => {
            setPhase('complete');
            gsap.to(containerRef.current, {
              yPercent: -100,
              duration: 0.8,
              ease: 'power2.out',
              onComplete: () => completeInitialLoad(),
            });
          }, 300);
        }
      };
      rafRef.current = requestAnimationFrame(animate);
    }, 800);

    return () => {
      clearTimeout(logoTimer);
      cancelAnimationFrame(rafRef.current);
    };
  }, [isInitialLoad, completeInitialLoad]);

  if (!isInitialLoad) return null;

  return (
    <div
      ref={containerRef}
      className="fixed inset-0 z-[101] flex items-center justify-center bg-bp-blue-dark"
      style={GRID_STYLE}
    >
      {/* Corner ticks */}
      <CornerTicks size={20} />

      {/* Original centre layout */}
      <div className="flex items-center gap-6 md:gap-10">
        <ArcLogo className="h-10 w-auto text-white md:h-14" />

        <div
          className={`overflow-hidden transition-all duration-500 ${
            phase === 'logo' ? 'max-w-0 opacity-0' : 'max-w-xs opacity-100'
          }`}
        >
          <p
            className="whitespace-nowrap text-sm tracking-widest text-fg md:text-base"
            style={{ fontFamily: 'var(--font-body)' }}
          >
            FULL SPEED. NO HANDS.
          </p>
          <p
            className="mt-1 flex justify-between text-xs tracking-widest text-accent/60 md:text-sm"
            style={{ fontFamily: 'var(--font-mono)' }}
          >
            <span>LOADING&hellip;</span>
            <span>{String(progress).padStart(3, '0')}%</span>
          </p>
        </div>
      </div>

      {/* Progress bar along bottom edge */}
      <div className="absolute bottom-0 left-0 h-px w-full bg-bp-line">
        <div
          className="h-full bg-accent/60 transition-none"
          style={{ width: `${progress}%` }}
        />
      </div>

      {/* Bottom-right label */}
      <p
        className="absolute bottom-4 right-6 font-mono text-xs tracking-widest text-fg/20"
      >
        ARC · CARLETON
      </p>
    </div>
  );
}
