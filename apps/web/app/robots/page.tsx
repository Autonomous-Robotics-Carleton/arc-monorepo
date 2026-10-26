'use client';

import Image from 'next/image';
import { BookOpen } from 'lucide-react';
import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import BrandIcon from '@/components/ui/BrandIcon';
import CornerTicks from '@/components/ui/CornerTicks';
import SectionLabel from '@/components/ui/SectionLabel';
import TrackingBox from '@/components/ui/TrackingBox';
import TransitionLink from '@/components/ui/TransitionLink';
import { useScrollReveal, useStaggerReveal } from '@/hooks/useScrollReveal';
import { BUILDS, COMPUTERS, SENSORS } from '@/data/car';
import { projects } from '@/data/projects';
import { DOCS_URL, GITHUB_REPO_URL } from '@/data/social';

const SAFETY = [
  {
    title: 'Policies never touch the motors',
    text: 'Learned and experimental code commands speed and steering only. Those pass through a safety envelope on the sync MCU that no policy can override.',
  },
  {
    title: 'Silence means stop',
    text: 'The operator laptop sends a heartbeat. If it stops, or the Orin stops commanding, the watchdog brakes the car.',
  },
  {
    title: 'A physical e-stop',
    text: 'A person at the track holds the e-stop. It reaches every motor controller directly, and each one ramps its own wheel to a stop.',
  },
  {
    title: 'Experiments can crash',
    text: 'Each experiment runs in its own container. One that hangs or hogs the Orin cannot stop the platform from driving, logging or stopping.',
  },
] as const;

export default function RobotsPage() {
  const heroRef = useStaggerReveal<HTMLDivElement>('[data-hero]', { stagger: 0.1, y: 24 });
  const chainRef = useStaggerReveal<HTMLDivElement>('[data-node]', { stagger: 0.1 });
  const sensorsRef = useScrollReveal<HTMLDivElement>({ y: 30 });
  const buildsRef = useScrollReveal<HTMLDivElement>({ y: 30 });
  const safetyRef = useStaggerReveal<HTMLDivElement>('[data-safety]', { stagger: 0.08 });
  const subsRef = useStaggerReveal<HTMLDivElement>('[data-sub]', { stagger: 0.06 });

  return (
    <>
      <Header />
      <main>
        {/* ── Hero ── */}
        <section className="relative px-6 pt-28 pb-20 md:px-10 md:pt-32 lg:px-16">
          <BlueprintGrid variant="hero" />
          <div ref={heroRef} className="mx-auto grid max-w-[1440px] items-center gap-12 lg:grid-cols-2 lg:gap-20">
            <div className="flex flex-col gap-7">
              <p data-hero className="font-mono text-xs tracking-[0.3em] text-arc-red uppercase">
                {'//'} Robots · In design
              </p>
              <h1 data-hero className="text-[clamp(3.5rem,10vw,9rem)]">
                ARC <span className="text-arc-red">V1</span>
              </h1>
              <p data-hero className="max-w-xl text-base leading-relaxed text-fg/70 md:text-lg">
                Our own 1/10-scale four-wheel-drive research car, designed from the requirements up. It&apos;s
                a sandbox for autonomy: the sensors are already on it, so a new idea needs code, not a new
                build.
              </p>
              <div data-hero className="flex flex-wrap gap-3 font-mono text-[11px] tracking-[0.2em]">
                {['1/10 SCALE', '4WD · 4 MOTORS', 'TARGET ≥ 9 M/S', 'BUILT TWICE: V1.0 → V1.1'].map((chip) => (
                  <span key={chip} className="border border-bp-line px-3 py-1.5 text-fg/60">
                    {chip}
                  </span>
                ))}
              </div>
            </div>

            <div data-hero className="relative border border-bp-line bg-black/60 p-8 md:p-12">
              <CornerTicks size={14} />
              <p className="absolute top-3 left-4 font-mono text-[10px] tracking-[0.25em] text-fg/40">
                OBJ 01 · TRACKING
              </p>
              <TransitionLink href="/blueprint" aria-label="Take the car apart" className="group block">
                <TrackingBox label="ARC_V1" confidence={0.98} className="mt-6">
                  <Image
                    src="/car.png"
                    alt="Wireframe render of an ARC car"
                    width={1376}
                    height={768}
                    className="w-full transition-transform duration-500 group-hover:scale-[1.02]"
                    style={{ mixBlendMode: 'screen', filter: 'saturate(0) brightness(1.1) contrast(1.15)' }}
                    priority
                    draggable={false}
                  />
                </TrackingBox>
                <p className="mt-4 text-right font-mono text-[11px] tracking-[0.2em] text-fg/50 transition-colors group-hover:text-arc-red">
                  TAKE IT APART →
                </p>
              </TransitionLink>
            </div>
          </div>
        </section>

        {/* ── 01 How it fits together ── */}
        <section className="relative px-6 py-24 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="flex flex-col gap-6">
              <SectionLabel index="01">How it fits together</SectionLabel>
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">
                Four computers,
                <br />
                one chain of command
              </h2>
            </div>
            <div ref={chainRef} className="mt-12 grid grid-cols-1 gap-4 md:grid-cols-2 xl:grid-cols-4 xl:gap-0">
              {COMPUTERS.map((c, i) => (
                <div key={c.name} data-node className="relative flex xl:pr-16">
                  <div className="relative flex w-full flex-col border border-bp-line p-6 bp-glass">
                    <CornerTicks size={8} />
                    <p className="font-mono text-xs text-arc-red">{String(i + 1).padStart(2, '0')}</p>
                    <h3 className="mt-3 text-xl">{c.name}</h3>
                    <p className="mt-2 font-mono text-[11px] leading-relaxed tracking-wider text-fg/50">{c.hardware}</p>
                    <p className="mt-4 text-sm leading-relaxed text-fg/65">{c.job}</p>
                  </div>
                  {/* link to the next computer: a tag under the card, or a labelled line between cards on xl */}
                  {c.link && (
                    <>
                      <p className="absolute -bottom-3 left-6 z-10 bg-black px-2 font-mono text-[10px] tracking-[0.2em] text-arc-red uppercase xl:hidden">
                        ↓ {c.link}
                      </p>
                      <div className="absolute top-1/2 right-0 hidden w-16 -translate-y-1/2 xl:block" aria-hidden="true">
                        <p className="mb-1.5 text-center font-mono text-[9px] tracking-[0.15em] text-arc-red uppercase">{c.link}</p>
                        <div className="h-px w-full bg-arc-red/70" />
                      </div>
                    </>
                  )}
                </div>
              ))}
            </div>
          </div>
        </section>

        {/* ── 02 Sensors ── */}
        <section className="relative px-6 py-24 md:px-10 lg:px-16">
          <div ref={sensorsRef} className="mx-auto grid max-w-[1440px] gap-12 lg:grid-cols-[minmax(0,0.8fr)_minmax(0,1.2fr)] lg:gap-20">
            <div className="flex flex-col gap-6">
              <SectionLabel index="02">What it senses</SectionLabel>
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">Sensors already on board</h2>
              <p className="text-base leading-relaxed text-fg/65">
                Every sample is timestamped in one clock on the sync MCU, so data from the LiDAR, cameras and
                IMUs lines up exactly. Every run is logged: learned methods need the data.
              </p>
            </div>
            <dl className="border-t border-bp-line">
              {SENSORS.map((s) => (
                <div key={s.name} className="grid grid-cols-1 gap-1 border-b border-bp-line py-4 sm:grid-cols-[10rem_minmax(0,1fr)] sm:gap-6">
                  <dt className="font-mono text-[11px] tracking-[0.2em] text-fg/45 uppercase">{s.name}</dt>
                  <dd>
                    <p className="text-sm text-fg">{s.part}</p>
                    <p className="mt-0.5 text-sm text-fg/50">{s.detail}</p>
                  </dd>
                </div>
              ))}
            </dl>
          </div>
        </section>

        {/* ── 03 Two builds ── */}
        <section className="relative px-6 py-24 md:px-10 lg:px-16">
          <div ref={buildsRef} className="mx-auto max-w-[1440px]">
            <div className="flex flex-col gap-6 md:flex-row md:items-end md:justify-between">
              <div className="flex flex-col gap-6">
                <SectionLabel index="03">Two builds</SectionLabel>
                <h2 className="text-[clamp(2rem,4.5vw,4rem)]">v1.0 now, v1.1 next</h2>
              </div>
              <p className="max-w-md text-sm leading-relaxed text-fg/60 md:text-base">
                v1.0 runs on compute and a LiDAR borrowed from our first car. v1.1 is the full design. Nothing
                bought for v1.0 is wasted: the boards and chassis are built for v1.1 from the start.
              </p>
            </div>
            <div className="mt-10 overflow-x-auto border border-bp-line bp-glass">
              <table className="w-full min-w-[560px] text-left text-sm">
                <thead>
                  <tr className="border-b border-bp-line font-mono text-[11px] tracking-[0.2em]">
                    <th className="px-5 py-4 font-normal text-fg/40" />
                    <th className="px-5 py-4 font-normal text-fg/70">V1.0</th>
                    <th className="px-5 py-4 font-normal text-arc-red">V1.1</th>
                  </tr>
                </thead>
                <tbody>
                  {BUILDS.map((b) => (
                    <tr key={b.item} className="border-b border-bp-line last:border-b-0">
                      <th scope="row" className="px-5 py-4 font-mono text-[11px] font-normal tracking-[0.2em] text-fg/45 uppercase">
                        {b.item}
                      </th>
                      <td className="px-5 py-4 text-fg/70">{b.v10}</td>
                      <td className="px-5 py-4 text-fg">{b.v11}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </section>

        {/* ── 04 Safety ── */}
        <section className="relative px-6 py-24 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="flex flex-col gap-6">
              <SectionLabel index="04">Built to fail safely</SectionLabel>
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">
                Hardware is never
                <br />
                <span className="text-arc-red">the reason</span>
              </h2>
              <p className="max-w-2xl text-base leading-relaxed text-fg/65">
                When an experiment fails, the cause should be the code under test, not the car. So the car
                assumes every experiment is new and untested.
              </p>
            </div>
            <div ref={safetyRef} className="mt-12 grid grid-cols-1 border-t border-l border-bp-line sm:grid-cols-2 lg:grid-cols-4">
              {SAFETY.map((s) => (
                <div key={s.title} data-safety className="border-r border-b border-bp-line p-6 bp-glass">
                  <h3 className="text-base leading-tight">{s.title}</h3>
                  <p className="mt-3 text-sm leading-relaxed text-fg/60">{s.text}</p>
                </div>
              ))}
            </div>
          </div>
        </section>

        {/* ── 05 Subsystems ── */}
        <section className="relative px-6 py-24 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="flex flex-col gap-6 md:flex-row md:items-end md:justify-between">
              <div className="flex flex-col gap-6">
                <SectionLabel index="05">Open source</SectionLabel>
                <h2 className="text-[clamp(2rem,4.5vw,4rem)]">Read the whole car</h2>
              </div>
              <p className="max-w-md text-sm leading-relaxed text-fg/60 md:text-base">
                Every requirement, decision, board and line of code is in one public repo.
              </p>
            </div>
            <div ref={subsRef} className="mt-10 border-t border-bp-line">
              {projects.map((p) => (
                <a
                  key={p.id}
                  data-sub
                  href={p.github}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="group grid grid-cols-1 gap-3 border-b border-bp-line py-6 transition-colors hover:bg-white/[0.02] md:grid-cols-[16rem_minmax(0,1fr)_auto] md:items-center md:gap-10"
                >
                  <h3 className="text-xl transition-colors group-hover:text-arc-red">{p.title}</h3>
                  <div>
                    <p className="text-sm leading-relaxed text-fg/60">{p.description}</p>
                    <div className="mt-3 flex flex-wrap gap-2">
                      {p.tags.map((tag) => (
                        <span key={tag} className="border border-bp-line px-2 py-1 font-mono text-[10px] tracking-wider text-fg/45 uppercase">
                          {tag}
                        </span>
                      ))}
                    </div>
                  </div>
                  <span className="flex items-center gap-2 font-mono text-[11px] tracking-[0.2em] text-fg/40 transition-colors group-hover:text-arc-red">
                    <BrandIcon brand="github" className="h-4 w-4" />
                    CODE
                  </span>
                </a>
              ))}
            </div>
            <div className="mt-10 flex flex-wrap gap-4">
              <a
                href={GITHUB_REPO_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="bp-focus inline-flex items-center gap-2.5 bg-arc-red px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-white transition-colors hover:bg-white hover:text-black"
              >
                <BrandIcon brand="github" />
                VIEW THE REPO
              </a>
              <a
                href={DOCS_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="bp-focus inline-flex items-center gap-2.5 border border-white/30 px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-white transition-colors hover:border-white hover:bg-white/5"
              >
                <BookOpen className="h-4 w-4" aria-hidden="true" />
                READ THE DOCS
              </a>
            </div>
          </div>
        </section>

        {/* ── 06 First car ── */}
        <section className="relative px-6 pb-32 md:px-10 lg:px-16">
          <div className="relative mx-auto flex max-w-[1440px] flex-col gap-6 border border-bp-line p-8 md:flex-row md:items-center md:justify-between md:p-12 bp-glass">
            <CornerTicks size={12} />
            <div className="flex flex-col gap-4">
              <SectionLabel index="06">Where it started</SectionLabel>
              <h2 className="text-[clamp(1.75rem,3.5vw,3rem)]">Our first car</h2>
            </div>
            <p className="max-w-xl text-sm leading-relaxed text-fg/65 md:text-base">
              An F1TENTH-style car on a Traxxas Slash 4x4 chassis, with a Jetson Orin Nano and a Hokuyo
              UST-20LX. It&apos;s the car we learned on, and it lends its compute and LiDAR to ARC v1.0.
            </p>
          </div>
        </section>
      </main>
      <Footer />
    </>
  );
}
