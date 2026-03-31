'use client';

import { usePathname } from 'next/navigation';
import { useState } from 'react';
import ArcLogo from '@/components/ui/ArcLogo';
import NavIcon from '@/components/ui/NavIcon';
import TransitionLink from '@/components/ui/TransitionLink';

const NAV_ITEMS = [
  { label: 'HOME', href: '/' },
  { label: 'ROBOTS', href: '/robots' },
  { label: 'TEAM', href: '/team' },
  { label: 'GOVERNANCE', href: '/governance' },
  { label: 'CONTACT', href: '/contact' },
] as const;

export default function Header() {
  const pathname = usePathname();
  const [mobileOpen, setMobileOpen] = useState(false);

  return (
    <header className="fixed top-0 left-0 z-50 w-full border-b border-bp-line bp-glass-strong">
      <nav className="mx-auto flex max-w-[1440px] items-center justify-between px-6 py-3 md:px-10 lg:px-16">
        {/* Logo */}
        <TransitionLink href="/" aria-label="ARC Home">
          <ArcLogo className="h-10 w-auto text-white md:h-12" />
        </TransitionLink>

        {/* Desktop nav */}
        <ul className="hidden items-center gap-10 md:flex lg:gap-16">
          {NAV_ITEMS.map((item) => {
            const active = pathname === item.href;
            return (
              <li key={item.href}>
                <TransitionLink
                  href={item.href}
                  className={`group flex items-center gap-2.5 text-sm tracking-widest transition-colors duration-200 ${
                    active ? 'text-accent' : 'text-fg/60 hover:text-accent/80'
                  }`}
                >
                  <NavIcon filled={active} />
                  {item.label}
                </TransitionLink>
              </li>
            );
          })}
        </ul>

        {/* Mobile toggle */}
        <button
          className="flex flex-col gap-1.5 md:hidden"
          onClick={() => setMobileOpen(!mobileOpen)}
          aria-label="Toggle navigation"
          aria-expanded={mobileOpen}
        >
          <span
            className={`h-0.5 w-6 bg-accent transition-transform ${mobileOpen ? 'translate-y-2 rotate-45' : ''}`}
          />
          <span
            className={`h-0.5 w-6 bg-accent transition-opacity ${mobileOpen ? 'opacity-0' : ''}`}
          />
          <span
            className={`h-0.5 w-6 bg-accent transition-transform ${mobileOpen ? '-translate-y-2 -rotate-45' : ''}`}
          />
        </button>
      </nav>

      {/* Mobile menu */}
      {mobileOpen && (
        <div className="border-t border-bp-line bg-bp-blue-dark px-6 pb-8 pt-4 md:hidden">
          <ul className="flex flex-col gap-6">
            {NAV_ITEMS.map((item) => {
              const active = pathname === item.href;
              return (
                <li key={item.href}>
                  <TransitionLink
                    href={item.href}
                    className={`flex items-center gap-2.5 text-sm tracking-widest ${
                      active ? 'text-accent' : 'text-fg/60'
                    }`}
                    onClick={() => setMobileOpen(false)}
                  >
                    <NavIcon filled={active} />
                    {item.label}
                  </TransitionLink>
                </li>
              );
            })}
          </ul>
        </div>
      )}
    </header>
  );
}
