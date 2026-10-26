'use client';

import { useEffect, useRef, useState } from 'react';
import CornerTicks from '@/components/ui/CornerTicks';
import HudClock from '@/components/ui/HudClock';

/**
 * The launch trailer as a camera feed: a 4:5 panel (the video's Instagram format)
 * framed with a HUD. Reduced-motion visitors get the poster frame only.
 */
export default function CamFeed({ className = '' }: { className?: string }) {
  const videoRef = useRef<HTMLVideoElement>(null);
  const [motion, setMotion] = useState(true);

  useEffect(() => {
    const reduce = window.matchMedia('(prefers-reduced-motion: reduce)');
    const apply = () => {
      setMotion(!reduce.matches);
      if (reduce.matches) videoRef.current?.pause();
      else videoRef.current?.play().catch(() => {});
    };
    apply();
    reduce.addEventListener('change', apply);
    return () => reduce.removeEventListener('change', apply);
  }, []);

  return (
    <figure className={`relative ${className}`}>
      {/* HUD: top */}
      <div className="mb-2 flex items-center justify-between font-mono text-[11px] tracking-[0.2em] text-fg/60">
        <span>
          CAM 01 · <HudClock />
        </span>
        <span className="flex items-center gap-2 text-arc-red">
          <span className={`h-2 w-2 rounded-full bg-arc-red ${motion ? 'animate-pulse' : ''}`} />
          {motion ? 'REC' : 'PAUSED'}
        </span>
      </div>

      <div className="relative aspect-[4/5] w-full overflow-hidden border border-bp-line bg-black">
        <video
          ref={videoRef}
          className="absolute inset-0 h-full w-full object-cover"
          src="/media/trailer.mp4"
          poster="/media/trailer-poster.jpg"
          muted
          loop
          playsInline
          autoPlay={motion}
          preload="metadata"
          aria-label="ARC launch trailer"
        />
        {/* scanlines */}
        <div
          className="pointer-events-none absolute inset-0 opacity-40 mix-blend-multiply"
          style={{
            backgroundImage:
              'repeating-linear-gradient(0deg, rgba(0,0,0,0.5) 0px, rgba(0,0,0,0.5) 1px, transparent 1px, transparent 3px)',
          }}
        />
        <CornerTicks size={18} />
      </div>

      {/* HUD: bottom */}
      <figcaption className="mt-2 flex items-center justify-between font-mono text-[11px] tracking-[0.2em] text-fg/40">
        <span>MACKENZIE 4463</span>
        <span>45°23&apos;03.8&quot;N 75°41&apos;52.1&quot;W</span>
      </figcaption>
    </figure>
  );
}
