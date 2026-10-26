export default function NavIcon({
  filled = false,
  className = '',
}: {
  filled?: boolean;
  className?: string;
}) {
  return (
    <span
      className={`inline-block h-2.5 w-2.5 border border-accent/40 transition-colors duration-200 ${filled ? 'bg-accent shadow-[0_0_6px_rgba(214,42,32,0.5)]' : ''} ${className}`}
      aria-hidden="true"
    />
  );
}
