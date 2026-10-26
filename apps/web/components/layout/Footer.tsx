import BrandIcon from '@/components/ui/BrandIcon';
import { SOCIAL_LINKS } from '@/data/social';


export default function Footer() {
  return (
    <footer className="border-t border-bp-line bg-bp-blue-dark px-6 py-12 md:px-10 lg:px-16">
      <div className="mx-auto flex max-w-[1440px] flex-col items-center gap-6 md:flex-row md:justify-between">
        <p className="text-xs text-fg/40">
          &copy; {new Date().getFullYear()} ARC &mdash; Autonomous Robotics Carleton
        </p>
        <ul className="flex items-center gap-6">
          {SOCIAL_LINKS.map((link) => (
            <li key={link.label}>
              <a
                href={link.href}
                target="_blank"
                rel="noopener noreferrer"
                className="flex items-center gap-2 text-xs text-fg/40 transition-colors hover:text-accent"
              >
                <BrandIcon brand={link.icon} className="h-3.5 w-3.5" />
                {link.label}
              </a>
            </li>
          ))}
        </ul>
      </div>
    </footer>
  );
}
