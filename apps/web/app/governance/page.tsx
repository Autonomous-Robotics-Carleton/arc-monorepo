'use client';

import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import VerticalLine from '@/components/ui/VerticalLine';
import NavIcon from '@/components/ui/NavIcon';
import { useScrollReveal, useStaggerReveal } from '@/hooks/useScrollReveal';

const ROWS = [
  {
    id: 'constitution',
    label: 'CONSTITUTION',
    description: 'Club founding documents and bylaws.',
    action: 'OPEN IN DRIVE →',
    href: '#',
  },
  {
    id: 'minutes',
    label: 'MEETING MINUTES',
    description: 'Most recent general meeting notes.',
    action: 'OPEN NOTES →',
    href: '#',
  },
  {
    id: 'status',
    label: 'CURRENT STATUS',
    description: 'Live club status — almost running soon.',
    action: 'PLACEHOLDER',
    href: null,
  },
] as const;

export default function GovernancePage() {
  const headingRef = useScrollReveal<HTMLDivElement>({ y: 30 });
  const rowsRef = useStaggerReveal<HTMLDivElement>('[data-row]', { stagger: 0.1 });

  return (
    <>
      <Header />
      <main>
        <section className="relative flex min-h-screen items-start px-6 pt-28 md:px-10 lg:px-16">
          <BlueprintGrid variant="hero" />
          <div className="mx-auto w-full max-w-[1440px] py-16">

            {/* Heading */}
            <div className="flex gap-6 pb-16">
              <VerticalLine className="hidden h-[200px] shrink-0 md:flex" />
              <div ref={headingRef}>
                <h1 className="text-5xl leading-tight md:text-7xl">GOVERNANCE</h1>
                <p className="mt-4 text-sm leading-relaxed text-fg/50">
                  Club documents and governance records.
                </p>
              </div>
            </div>

            {/* Rows */}
            <div ref={rowsRef} className="relative border border-bp-line px-6 lg:px-8 bp-glass">
              {ROWS.map((row) => (
                <div key={row.id} data-row className="border-t border-bp-line py-10">
                  <div className="flex flex-col gap-6 sm:flex-row sm:items-center sm:justify-between">
                    {/* Label */}
                    <div className="flex items-center gap-2.5">
                      <NavIcon />
                      <span className="text-sm tracking-widest">{row.label}</span>
                    </div>

                    {/* Description + CTA */}
                    <div className="flex flex-col items-start gap-4 sm:flex-row sm:items-center sm:gap-12">
                      <p className="text-sm leading-relaxed text-fg/50 sm:max-w-[300px]">
                        {row.description}
                      </p>
                      {row.href ? (
                        <a
                          href={row.href}
                          target="_blank"
                          rel="noopener noreferrer"
                          className="shrink-0 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                        >
                          {row.action}
                        </a>
                      ) : (
                        <span className="shrink-0 border border-bp-line px-5 py-2.5 text-xs tracking-widest text-fg/20">
                          {row.action}
                        </span>
                      )}
                    </div>
                  </div>
                </div>
              ))}
            </div>

          </div>
        </section>
      </main>
      <Footer />
    </>
  );
}
