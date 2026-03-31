'use client';

import Image from 'next/image';
import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import ArcLogo from '@/components/ui/ArcLogo';
import BackgroundBoxes from '@/components/ui/BackgroundBoxes';
import CornerTicks from '@/components/ui/CornerTicks';
import StackedLogos from '@/components/ui/StackedLogos';
import { getSponsorLogo } from '@/components/ui/SponsorLogos';
import { sponsors } from '@/data/sponsors';

function buildLogoGroups(columns: number) {
  const logos = sponsors.map((s) => getSponsorLogo(s.name));
  return Array.from({ length: columns }, () => logos);
}

export default function HomePage() {
  const logoGroups = buildLogoGroups(4);

  return (
    <>
      {/* ── Full-page interactive background ── */}
      <div className="fixed inset-0 -z-10 overflow-hidden pointer-events-none">
        <BackgroundBoxes />
      </div>

      <Header />

      <main>
        {/* ── Hero ── */}
        <section className="relative flex min-h-screen items-center overflow-hidden px-6 md:px-10 lg:px-16">
          <div className="mx-auto flex w-full max-w-[1440px] items-center pt-16">

            {/* Left — identity */}
            <div className="relative z-10 flex flex-col gap-6 md:max-w-[480px] lg:max-w-[560px]">
              <ArcLogo className="h-16 w-auto text-white md:h-20 lg:h-24" />
              <p className="font-mono text-xs tracking-[0.3em] text-fg/40 uppercase">
                Autonomous Racing at Carleton
              </p>
              <p className="text-sm leading-relaxed text-fg/60 md:text-base">
                A student-run engineering club building the future of autonomous
                racing vehicles.
              </p>
              <div className="flex flex-wrap items-center gap-4 pt-2">
                <a
                  href="/robots"
                  className="border border-accent/40 px-6 py-3 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                >
                  OUR ROBOTS →
                </a>
                <a
                  href="/team"
                  className="border border-bp-line px-6 py-3 text-xs tracking-widest text-fg/60 transition-all duration-200 hover:border-accent/40 hover:text-accent/80"
                >
                  MEET THE TEAM
                </a>
              </div>
            </div>

            {/* Right — car PNG */}
            <div className="pointer-events-none absolute right-0 top-1/2 w-[55%] -translate-y-1/2 select-none md:w-[52%] lg:w-[58%]">
              <Image
                src="/car.png"
                alt="ARC autonomous vehicle"
                width={1280}
                height={860}
                className="w-full"
                style={{ mixBlendMode: 'screen', filter: 'saturate(0) brightness(1.05) contrast(1.15)' }}
                priority
                draggable={false}
              />
            </div>

          </div>
        </section>

        {/* ── Identity Section ── */}
        <section className="relative px-6 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px] py-16 md:py-20">
            <div className="relative flex flex-col gap-6 md:flex-row md:items-start md:gap-16 lg:gap-24">
              <CornerTicks label="01" />
              <h2 className="shrink-0 text-3xl md:text-4xl lg:text-5xl">
                IDENTITY
              </h2>
              <p className="max-w-xl text-sm leading-relaxed text-fg/70 md:text-base">
                ARC is a student-run engineering club at Carleton University focused
                on building autonomous robotics systems. We give students hands-on
                experience in designing, building, and testing robots while
                fostering interdisciplinary collaboration across engineering,
                computer science, and design disciplines.
              </p>
            </div>
          </div>
        </section>

        {/* ── Mission Section ── */}
        <section className="relative px-6 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px] py-16 md:py-20">
            <div className="relative flex flex-col gap-6 md:flex-row md:items-start md:gap-16 lg:gap-24">
              <CornerTicks label="02" />
              <h2 className="shrink-0 text-3xl md:text-4xl lg:text-5xl">
                MISSION
              </h2>
              <p className="max-w-xl text-sm leading-relaxed text-fg/70 md:text-base">
                Our mission is to create a collaborative and inclusive space for
                students to explore and contribute to the future of autonomous
                vehicles: developing practical skills, valuable experience,
                meaningful connections, and cutting edge projects along the way.
              </p>
            </div>
          </div>
        </section>

        {/* ── Sponsors Section ── */}
        <section className="relative px-6 pb-24 md:px-10 md:pb-32 lg:px-16 lg:pb-40">
          <div className="mx-auto max-w-[1440px]">
            <div className="relative border border-bp-line p-8 md:p-12 bp-glass">
              <CornerTicks label="03" />
              <h2 className="text-3xl md:text-4xl lg:text-5xl">SPONSORS</h2>
              <div className="mt-12 text-fg/50 md:mt-16">
                <StackedLogos
                  logoGroups={logoGroups}
                  columns={4}
                  duration={28}
                  stagger={3}
                />
              </div>
              <div className="mt-10 flex flex-col items-start justify-between gap-4 border-t border-bp-line pt-8 sm:flex-row sm:items-center">
                <p className="text-xs tracking-widest text-fg/40">
                  Interested in supporting ARC?
                </p>
                <a
                  href="/contact"
                  className="border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                >
                  BECOME A SPONSOR →
                </a>
              </div>
            </div>
          </div>
        </section>
      </main>

      <Footer />

      {/* ── Bottom Marquee ── */}
      <div className="overflow-hidden border-t border-bp-line bg-bp-blue-dark py-4">
        <div className="animate-marquee flex whitespace-nowrap">
          {Array.from({ length: 8 }).map((_, i) => (
            <span key={i} className="inline-flex items-center">
              <span className="mx-8 text-4xl tracking-wider text-accent/15 md:text-5xl lg:text-6xl">
                AHEAD OF THE CURVE
              </span>
              <span className="text-2xl md:text-3xl" style={{ color: '#444444' }}>·</span>
            </span>
          ))}
        </div>
      </div>
    </>
  );
}
