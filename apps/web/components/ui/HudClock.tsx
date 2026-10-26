'use client';

import { useEffect, useState } from 'react';

/** Live HH:MM:SS, like the timestamp on a camera feed. Renders a fixed placeholder until mounted. */
export default function HudClock({ className = '' }: { className?: string }) {
  const [now, setNow] = useState<string | null>(null);

  useEffect(() => {
    const tick = () => setNow(new Date().toLocaleTimeString('en-GB', { hour12: false }));
    tick();
    const id = window.setInterval(tick, 1000);
    return () => window.clearInterval(id);
  }, []);

  return (
    <span className={`tabular-nums ${className}`} suppressHydrationWarning>
      {now ?? '--:--:--'}
    </span>
  );
}
