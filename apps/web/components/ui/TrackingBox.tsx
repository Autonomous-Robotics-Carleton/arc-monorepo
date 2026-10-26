import type { ReactNode } from 'react';

/**
 * A computer-vision bounding box, as in the Instagram posts and trailer:
 * a thin frame with a mono label tab and a confidence readout.
 */
export default function TrackingBox({
  label,
  confidence,
  className = '',
  children,
}: {
  label: string;
  confidence?: number;
  className?: string;
  children?: ReactNode;
}) {
  return (
    <div className={`relative border border-arc-red/80 ${className}`}>
      <div className="absolute -top-px left-[-1px] -translate-y-full bg-arc-red px-1.5 py-0.5 font-mono text-[10px] leading-none tracking-wider text-white">
        {label}
        {confidence !== undefined && <span className="ml-1.5 opacity-75">{confidence.toFixed(2)}</span>}
      </div>
      {/* centre crosshair */}
      <div className="pointer-events-none absolute top-1/2 left-1/2 h-2 w-2 -translate-x-1/2 -translate-y-1/2 border border-arc-red/80" />
      {children}
    </div>
  );
}
