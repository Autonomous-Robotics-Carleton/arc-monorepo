'use client';

import {
  createContext,
  useContext,
  useState,
  useCallback,
  useEffect,
  type ReactNode,
} from 'react';
import { useRouter, usePathname } from 'next/navigation';
import ArcLogo from '@/components/ui/ArcLogo';
import CornerTicks from '@/components/ui/CornerTicks';

type TransitionPhase = 'idle' | 'covering' | 'covered' | 'revealing';

interface TransitionContextValue {
  phase: TransitionPhase;
  isInitialLoad: boolean;
  navigateTo: (href: string) => void;
  completeInitialLoad: () => void;
}

const TransitionContext = createContext<TransitionContextValue | null>(null);

export function useTransition() {
  const ctx = useContext(TransitionContext);
  if (!ctx) throw new Error('useTransition must be used within TransitionProvider');
  return ctx;
}

interface TransitionProviderProps {
  children: ReactNode;
}

const GRID_STYLE = {
  backgroundImage: `
    repeating-linear-gradient(0deg,  rgba(212,212,212,0.07) 0px, rgba(212,212,212,0.07) 1px, transparent 1px, transparent 60px),
    repeating-linear-gradient(90deg, rgba(212,212,212,0.07) 0px, rgba(212,212,212,0.07) 1px, transparent 1px, transparent 60px)
  `,
} as React.CSSProperties;

export function TransitionProvider({ children }: TransitionProviderProps) {
  const router = useRouter();
  const pathname = usePathname();
  const [phase, setPhase] = useState<TransitionPhase>('idle');
  const [isInitialLoad, setIsInitialLoad] = useState(true);
  const [targetHref, setTargetHref] = useState<string | null>(null);

  const completeInitialLoad = useCallback(() => {
    if (!isInitialLoad) return;
    setIsInitialLoad(false);
    setPhase('idle');
  }, [isInitialLoad]);

  const navigateTo = useCallback(
    (href: string) => {
      const normalizedHref = href.replace(/\/$/, '') || '/';
      const normalizedPathname = pathname.replace(/\/$/, '') || '/';
      if (normalizedHref === normalizedPathname) return;
      if (phase !== 'idle') return;
      if (typeof window !== 'undefined' && window.matchMedia('(prefers-reduced-motion: reduce)').matches) {
        router.push(href);
        return;
      }
      setTargetHref(href);
      setPhase('covering');
    },
    [pathname, phase, router]
  );

  useEffect(() => {
    if (phase !== 'covering' || !targetHref) return;
    const timer = setTimeout(() => {
      setPhase('covered');
      router.push(targetHref);
    }, 800);
    return () => clearTimeout(timer);
  }, [phase, targetHref, router]);

  useEffect(() => {
    if (phase !== 'covered' || !targetHref) return;
    const normalizedTarget = targetHref.replace(/\/$/, '') || '/';
    const normalizedPathname = pathname.replace(/\/$/, '') || '/';
    if (normalizedPathname === normalizedTarget) {
      const timer = setTimeout(() => {
        setPhase('revealing');
        setTargetHref(null);
      }, 100);
      return () => clearTimeout(timer);
    }
  }, [phase, pathname, targetHref]);

  useEffect(() => {
    if (phase !== 'revealing') return;
    const timer = setTimeout(() => setPhase('idle'), 800);
    return () => clearTimeout(timer);
  }, [phase]);

  const isVisible = phase === 'covering' || phase === 'covered' || phase === 'revealing';
  const isDown = phase === 'covering' || phase === 'covered';

  return (
    <TransitionContext.Provider
      value={{ phase, isInitialLoad, navigateTo, completeInitialLoad }}
    >
      {children}

      {/* Transition overlay */}
      <div
        style={{
          position: 'fixed',
          inset: 0,
          zIndex: 100,
          backgroundColor: 'var(--color-bp-blue-dark)',
          transform: isDown ? 'translateY(0%)' : 'translateY(-100%)',
          transition: 'transform 800ms',
          transitionTimingFunction: phase === 'covering' ? 'ease-in' : 'ease-out',
          pointerEvents: isVisible ? 'auto' : 'none',
          ...GRID_STYLE,
        }}
        aria-hidden="true"
      >
        {/* Corner ticks */}
        <CornerTicks size={20} />

        {/* Centred logo */}
        <div style={{
          position: 'absolute',
          inset: 0,
          display: 'flex',
          alignItems: 'center',
          justifyContent: 'center',
        }}>
          <ArcLogo className="h-10 w-auto text-white md:h-14" />
        </div>

        {/* Scan line — sweeps down while covering */}
        {phase === 'covering' && (
          <div
            className="bp-scanning pointer-events-none absolute left-0 right-0 h-px bg-accent/40"
            style={{ top: 0 }}
          />
        )}

        {/* Bottom label */}
        <p
          style={{
            position: 'absolute',
            bottom: '1rem',
            right: '1.5rem',
            fontFamily: 'var(--font-mono)',
            fontSize: '0.65rem',
            letterSpacing: '0.2em',
            color: 'rgba(240,240,240,0.2)',
          }}
        >
          ARC · CARLETON
        </p>
      </div>
    </TransitionContext.Provider>
  );
}
