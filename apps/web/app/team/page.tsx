'use client';

import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import CornerTicks from '@/components/ui/CornerTicks';
import Button from '@/components/ui/Button';
import BrandIcon from '@/components/ui/BrandIcon';
import InstagramFeed from '@/components/ui/InstagramFeed';
import SectionLabel from '@/components/ui/SectionLabel';
import { useScrollReveal, useStaggerReveal } from '@/hooks/useScrollReveal';
import { teamMembers } from '@/data/team';
import { DISCORD_URL, INSTAGRAM_HANDLE, INSTAGRAM_URL } from '@/data/social';

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
            <div className="mb-8 pt-8">
              <SectionLabel index="01">Execs &amp; leads</SectionLabel>
            </div>
            <div
              ref={gridRef}
              className="grid grid-cols-1 gap-6 sm:grid-cols-2 lg:grid-cols-3 xl:grid-cols-4"
            >
              {teamMembers.map((member) => (
                <div
                  key={`${member.name}-${member.role}`}
                  data-reveal-item
                  className="group relative overflow-hidden border border-bp-line transition-all duration-300 hover:border-accent/30 bp-glow-hover bp-glass"
                >
                  <CornerTicks size={8} />
                  {/* Photo — full width, portrait ratio */}
                  <div className="aspect-[3/4] w-full bg-bp-blue-light" />
                  {/* Info */}
                  <div className="border-t border-bp-line p-5">
                    <h3 className="text-base">{member.name}</h3>
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
                <p className="mt-1 text-sm text-fg/30 font-mono">{INSTAGRAM_HANDLE}</p>
              </div>
              <a
                href={INSTAGRAM_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="inline-flex items-center gap-2.5 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
              >
                <BrandIcon brand="instagram" />
                VIEW ON INSTAGRAM
              </a>
            </div>
            <InstagramFeed />
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
            <div className="mt-8 flex flex-wrap justify-center gap-4">
              <Button href={DISCORD_URL}>
                <BrandIcon brand="discord" />
                Join the Discord
              </Button>
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
