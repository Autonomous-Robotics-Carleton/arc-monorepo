import { siDiscord, siGithub, siGoogledrive, siInstagram, type SimpleIcon } from 'simple-icons';

export type Brand = 'discord' | 'instagram' | 'github' | 'drive';

const ICONS: Record<Brand, SimpleIcon> = {
  discord: siDiscord,
  instagram: siInstagram,
  github: siGithub,
  drive: siGoogledrive,
};

/** A service's logo (from Simple Icons), one colour: it takes the text colour around it */
export default function BrandIcon({ brand, className = 'h-4 w-4' }: { brand: Brand; className?: string }) {
  return (
    <svg viewBox="0 0 24 24" fill="currentColor" className={`shrink-0 ${className}`} aria-hidden="true">
      <path d={ICONS[brand].path} />
    </svg>
  );
}
