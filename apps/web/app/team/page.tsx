'use client';

import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import CornerTicks from '@/components/ui/CornerTicks';
import Button from '@/components/ui/Button';
import InstagramFeed from '@/components/ui/InstagramFeed';
import { useScrollReveal, useStaggerReveal } from '@/hooks/useScrollReveal';
import { teamMembers } from '@/data/team';

// Set your Behold feed ID here once you've set up behold.so
const BEHOLD_FEED_ID = '';

export default function TeamPage() {
  const gridRef = useStaggerReveal<HTMLDivElement>('[data-reveal-item]', {
    stagger: 0.08,
  });
  const instagramRef = useScrollReveal<HTMLDivElement>({ y: 30, delay: 0.1 });
  const ctaRef = useScrollReveal<HTMLDivElement>({ delay: 0.2 });

  return (
    <>
      <Header />
      <main>
        {/* Exec grid */}
        <section className="relative px-6 pt-28 pb-24 md:px-10 lg:px-16">
          <BlueprintGrid variant="hero" />
          <div className="mx-auto max-w-[1440px]">
            <p className="mb-8 text-xs tracking-widest text-fg/40 pt-8">— EXECS</p>
            <div
              ref={gridRef}
              className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4"
            >
              {teamMembers.map((member) => (
                <div
                  key={member.name}
                  data-reveal-item
                  className="group relative overflow-hidden border border-bp-line transition-all duration-300 hover:border-accent/30 bp-glow-hover bp-glass"
                >
                  <CornerTicks size={8} />
                  {/* Photo — full width, portrait ratio */}
                  <div className="aspect-[3/4] w-full bg-bp-blue-light" />
                  {/* Info */}
                  <div className="border-t border-bp-line p-5">
                    <h3 className="text-base font-bold italic">{member.name}</h3>
                    <p className="mt-0.5 text-xs tracking-wider text-fg/50">{member.role}</p>
                  </div>
                </div>
              ))}
            </div>
          </div>
        </section>

        {/* Instagram feed */}
        <section className="relative px-6 pb-24 md:px-10 lg:px-16">
          <BlueprintGrid variant="section" />
          <div className="mx-auto max-w-[1440px]">
            <div ref={instagramRef} className="mb-8 flex items-end justify-between">
              <div>
                <p className="text-xs tracking-widest text-fg/40">— FOLLOW OUR BUILD</p>
                <p className="mt-1 text-sm text-fg/30 font-mono">@arcarleton</p>
              </div>
              <a
                href="https://instagram.com/arcarleton"
                target="_blank"
                rel="noopener noreferrer"
                className="border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
              >
                VIEW ON INSTAGRAM →
              </a>
            </div>
            <InstagramFeed feedId={BEHOLD_FEED_ID || undefined} />
          </div>
        </section>

        {/* Join CTA */}
        <section className="relative px-6 pb-32 md:px-10 lg:px-16">
          <div
            ref={ctaRef}
            className="relative mx-auto max-w-[1440px] border border-bp-line p-10 text-center md:p-16 bp-glow bp-glass"
          >
            <CornerTicks size={14} />
            <h2 className="text-3xl md:text-5xl">JOIN THE TEAM</h2>
            <p className="mx-auto mt-4 max-w-md text-base text-fg/60">
              We&apos;re always looking for passionate people. Whether you&apos;re into
              software, hardware, or design — there&apos;s a place for you.
            </p>
            <div className="mt-8">
              <Button href="/contact" variant="outline">
                Get in Touch
              </Button>
            </div>
          </div>
        </section>
      </main>
      <Footer />
    </>
  );
}
