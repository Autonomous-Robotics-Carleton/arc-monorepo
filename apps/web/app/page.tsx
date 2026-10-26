'use client';

import Image from 'next/image';
import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BackgroundBoxes from '@/components/ui/BackgroundBoxes';
import BrandIcon from '@/components/ui/BrandIcon';
import CornerTicks from '@/components/ui/CornerTicks';
import InstagramFeed from '@/components/ui/InstagramFeed';
import StackedLogos from '@/components/ui/StackedLogos';
import TrackingBox from '@/components/ui/TrackingBox';
import TransitionLink from '@/components/ui/TransitionLink';
import CamFeed from '@/components/sections/CamFeed';
import { getSponsorLogo } from '@/components/ui/SponsorLogos';
import { useStaggerReveal } from '@/hooks/useScrollReveal';
import { sponsors } from '@/data/sponsors';
import { teamMembers } from '@/data/team';
import { DISCORD_URL, INSTAGRAM_HANDLE, INSTAGRAM_URL } from '@/data/social';

// Facts under the hero, from systems/ (SYS-01 is a draft target)
const FACTS = [
  { label: 'Scale', value: '1/10' },
  { label: 'Drive', value: '4WD, 4 motors' },
  { label: 'Top speed target', value: '≥ 9 m/s' },
  { label: 'Stack', value: 'ROS 2 Jazzy' },
] as const;

// From the club's Drive (FALL 25 / PICTURES AND VIDEO / GIT Event)
const PHOTOS = [
  { src: '/photos/workshop-talk.jpg', alt: 'A member presenting Git vs GitHub in a lecture hall', caption: 'Teaching a Git workshop' },
  { src: '/photos/lab-night.jpg', alt: 'A member laughing mid-conversation in the lab, beside a 3D printer', caption: 'In the lab' },
  { src: '/photos/pairing.jpg', alt: 'Two members working through a problem together on a laptop', caption: 'Working through it together' },
] as const;

// v1.0 → v1.1 where the two builds differ (systems/builds.md, ADR-0046)
const CAR_SPECS = [
  { label: 'Compute', value: 'Jetson Orin Nano → Orin NX' },
  { label: 'LiDAR', value: 'Hokuyo UST-20LX → UST-10LX' },
  { label: 'Cameras', value: 'Global-shutter stereo pair' },
  { label: 'Drive', value: '4 × A50S motor controllers' },
  { label: 'Time base', value: 'STM32H723 sync MCU' },
  { label: 'Software', value: 'ROS 2, sim to car' },
] as const;

// The three technical subteams in the constitution (W26, Article III), each run by a Design Lead
const TEAMS = [
  {
    name: 'Software',
    role: 'Software Lead',
    text: 'The ROS 2 stack: perception, state estimation, planning and control, plus the simulators that let it run before the car does.',
    tags: ['ROS 2', 'C++', 'Python', 'Simulation'],
  },
  {
    name: 'Electrical',
    role: 'Electrical Lead',
    text: 'Custom boards for power, sensing and time sync, the firmware on them, and the wiring that holds the car together.',
    tags: ['Altium', 'Zephyr', 'Power', 'CAN'],
  },
  {
    name: 'Mechanical',
    role: 'Mechanical Lead',
    text: 'The chassis, suspension corners and sensor mounts: designed in CAD, then machined and printed.',
    tags: ['CAD', 'Suspension', '3D printing'],
  },
] as const;

const leadFor = (role: string) => teamMembers.find((m) => m.role === role)?.name;

export default function HomePage() {
  // One entrance animation, on the hero; the rest of the page doesn't fade in on scroll
  const heroRef = useStaggerReveal<HTMLDivElement>('[data-hero]', { stagger: 0.1, y: 24 });

  return (
    <>
      {/* Full-page interactive background */}
      <div className="pointer-events-none fixed inset-0 -z-10 overflow-hidden">
        <BackgroundBoxes />
      </div>

      <Header />

      <main>
        {/* ── Hero ── */}
        <section className="relative px-6 pt-28 pb-12 md:px-10 md:pt-32 lg:px-16">
          <div
            ref={heroRef}
            className="mx-auto grid max-w-[1440px] items-center gap-12 lg:min-h-[calc(100vh-11rem)] lg:grid-cols-[minmax(0,1fr)_minmax(0,400px)] lg:gap-16"
          >
            <div className="@container flex flex-col gap-8">
              {/* "AUTONOMOUS" is ~9.75em wide in the expanded face: size to the column */}
              <h1 data-hero className="text-[min(10cqw,7rem)]">
                Autonomous
                <br />
                Robotics
                <br />
                <span className="text-arc-red">Carleton</span>
              </h1>
              <p data-hero className="max-w-xl text-base leading-relaxed text-fg/70 md:text-lg">
                A student engineering club at Carleton University. We design and build 1/10-scale cars that
                drive themselves: the chassis, the boards, the firmware and the software that makes the
                decisions.
              </p>
              <div data-hero className="flex flex-wrap gap-4">
                <a
                  href={DISCORD_URL}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="bp-focus inline-flex items-center gap-2.5 bg-arc-red px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-white transition-colors hover:bg-white hover:text-black"
                >
                  <BrandIcon brand="discord" />
                  JOIN THE DISCORD
                </a>
                <TransitionLink
                  href="/robots"
                  className="bp-focus border border-white/30 px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-white transition-colors hover:border-white hover:bg-white/5"
                >
                  SEE THE CAR
                </TransitionLink>
              </div>
            </div>

            <div data-hero className="mx-auto w-full max-w-[400px]">
              <CamFeed />
            </div>
          </div>

          <dl className="mx-auto mt-12 grid max-w-[1440px] grid-cols-2 border border-bp-line bg-black/70 md:grid-cols-4">
            {FACTS.map((f, i) => (
              <div
                key={f.label}
                className={`border-bp-line px-5 py-4 ${i % 2 ? 'border-l' : ''} ${i >= 2 ? 'border-t md:border-t-0' : ''} ${i === 2 ? 'md:border-l' : ''}`}
              >
                <dt className="text-xs text-fg/45">{f.label}</dt>
                <dd className="mt-1 font-mono text-sm text-fg">{f.value}</dd>
              </div>
            ))}
          </dl>
        </section>

        {/* ── Tagline band ── */}
        <div className="overflow-hidden bg-arc-red-fill py-3" aria-hidden="true">
          <div className="animate-marquee flex whitespace-nowrap">
            {Array.from({ length: 10 }).map((_, i) => (
              <span
                key={i}
                className="mx-6 text-2xl font-black tracking-tight text-black uppercase md:text-3xl"
                style={{ fontFamily: 'var(--font-heading)', fontStretch: '125%' }}
              >
                Ahead of the curve ·
              </span>
            ))}
          </div>
        </div>

        {/* ── Who we are ── */}
        <section className="relative px-6 py-24 md:px-10 md:py-32 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="grid gap-10 lg:grid-cols-2 lg:gap-24">
              <h2 className="text-[clamp(2rem,5vw,4.5rem)]">
                Full speed.
                <br />
                No hands.
              </h2>
              <div className="flex flex-col gap-5 text-base leading-relaxed text-fg/70 md:text-lg lg:pt-2">
                <p>
                  ARC is a student-run engineering club at Carleton University. We design, build and test
                  autonomous vehicles, and bring engineering, computer science and design students together
                  to do it.
                </p>
                <p>
                  Students rarely get hands-on time with real robotics and autonomy. ARC is where they get
                  it: real hardware, real code, and people who&apos;ll show you how.
                </p>
              </div>
            </div>

            <div className="mt-16 grid grid-cols-1 border-t border-l border-bp-line sm:grid-cols-3">
              {PHOTOS.map((p) => (
                <figure key={p.src} className="border-r border-b border-bp-line bg-black/70 p-3">
                  <div className="relative aspect-[4/3] w-full overflow-hidden">
                    <Image src={p.src} alt={p.alt} fill sizes="(min-width: 640px) 33vw, 100vw" className="object-cover" />
                  </div>
                  <figcaption className="px-1 pt-3 pb-1 text-sm text-fg/60">{p.caption}</figcaption>
                </figure>
              ))}
            </div>
          </div>
        </section>

        {/* ── The car ── */}
        <section className="relative px-6 py-24 md:px-10 md:py-32 lg:px-16">
          <div className="mx-auto grid max-w-[1440px] items-center gap-12 lg:grid-cols-2 lg:gap-20">
            <div className="relative order-2 lg:order-1">
              <div className="relative border border-bp-line bg-black/70 p-8 md:p-12">
                <CornerTicks size={14} />
                <TrackingBox label="ARC_V1">
                  <Image
                    src="/car.png"
                    alt="Wireframe render of an ARC car"
                    width={1376}
                    height={768}
                    className="w-full"
                    style={{ mixBlendMode: 'screen', filter: 'saturate(0) brightness(1.1) contrast(1.15)' }}
                    draggable={false}
                  />
                </TrackingBox>
              </div>
            </div>

            <div className="order-1 flex flex-col gap-6 lg:order-2">
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">
                A sandbox
                <br />
                for autonomy
              </h2>
              <p className="text-base leading-relaxed text-fg/70 md:text-lg">
                ARC v1 is our own 1/10-scale 4WD research car. The sensors are already on it, so trying a
                new idea takes code, not a new build. Classical autonomy comes first (localization,
                planning, MPC), then learned and embodied AI.
              </p>
              <dl className="grid grid-cols-1 border-t border-bp-line sm:grid-cols-2">
                {CAR_SPECS.map((s) => (
                  <div key={s.label} className="flex items-baseline justify-between gap-4 border-b border-bp-line py-3 sm:pr-6">
                    <dt className="text-xs text-fg/45">{s.label}</dt>
                    <dd className="text-right text-sm text-fg/85">{s.value}</dd>
                  </div>
                ))}
              </dl>
              <div className="flex flex-wrap gap-4 pt-2">
                <TransitionLink
                  href="/robots"
                  className="bp-focus border border-accent/60 px-5 py-3 text-xs tracking-[0.2em] text-accent transition-colors hover:border-accent hover:bg-accent/10"
                >
                  HOW IT WORKS
                </TransitionLink>
                <TransitionLink
                  href="/blueprint"
                  className="bp-focus border border-bp-line px-5 py-3 text-xs tracking-[0.2em] text-fg/60 transition-colors hover:border-white/40 hover:text-white"
                >
                  TAKE IT APART
                </TransitionLink>
              </div>
            </div>
          </div>
        </section>

        {/* ── Teams ── */}
        <section className="relative px-6 py-24 md:px-10 md:py-32 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="flex flex-col gap-6 md:flex-row md:items-end md:justify-between">
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">
                Three teams.
                <br />
                One car.
              </h2>
              <p className="max-w-md text-sm leading-relaxed text-fg/60 md:text-base">
                No experience needed. Pick the part you want to learn, and the leads will get you going.
              </p>
            </div>

            <div className="mt-12 grid grid-cols-1 border-t border-l border-bp-line md:grid-cols-3">
              {TEAMS.map((t) => (
                <article key={t.name} className="flex flex-col border-r border-b border-bp-line bg-black/70 p-6">
                  <h3 className="text-2xl">{t.name}</h3>
                  <p className="mt-4 flex-1 text-sm leading-relaxed text-fg/60">{t.text}</p>
                  <p className="mt-5 text-xs text-fg/40">{t.tags.join(', ')}</p>
                  {leadFor(t.role) && (
                    <p className="mt-5 border-t border-bp-line pt-4 text-sm text-fg/50">
                      Design Lead: <span className="text-fg/85">{leadFor(t.role)}</span>
                    </p>
                  )}
                </article>
              ))}
            </div>
            <p className="mt-8 max-w-2xl text-base leading-relaxed text-fg/60">
              Not an engineer? Media, external relations and finance keep the club running, and they need
              people too.
            </p>
          </div>
        </section>

        {/* ── Instagram ── */}
        <section className="relative px-6 py-24 md:px-10 md:py-32 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <div className="mb-10 flex flex-col gap-6 md:flex-row md:items-end md:justify-between">
              <h2 className="text-[clamp(2rem,4.5vw,4rem)]">Follow the build</h2>
              <a
                href={INSTAGRAM_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="bp-focus inline-flex items-center gap-2.5 self-start border border-accent/60 px-5 py-3 text-xs tracking-[0.2em] text-accent transition-colors hover:border-accent hover:bg-accent/10 md:self-auto"
              >
                <BrandIcon brand="instagram" />
                {INSTAGRAM_HANDLE.toUpperCase()} ON INSTAGRAM
              </a>
            </div>
            <InstagramFeed count={3} />
          </div>
        </section>

        {/* ── Sponsors ── */}
        <section className="relative px-6 py-24 md:px-10 md:py-32 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <h2 className="text-[clamp(1.5rem,3vw,2.5rem)]">Supported by</h2>
            <div className="mt-10 text-fg/60">
              <StackedLogos logos={sponsors.map(getSponsorLogo)} duration={28} stagger={3} />
            </div>
            <div className="mt-6 flex flex-col items-start justify-between gap-4 sm:flex-row sm:items-center">
              <p className="text-sm text-fg/50">Want your logo on the car?</p>
              <TransitionLink
                href="/contact"
                className="bp-focus border border-accent/60 px-5 py-3 text-xs tracking-[0.2em] text-accent transition-colors hover:border-accent hover:bg-accent/10"
              >
                BECOME A SPONSOR
              </TransitionLink>
            </div>
          </div>
        </section>

        {/* ── Join ── */}
        <section className="relative px-6 pb-24 md:px-10 md:pb-32 lg:px-16">
          <div className="relative mx-auto flex max-w-[1440px] flex-col gap-10 overflow-hidden bg-arc-red-fill p-8 md:p-14 lg:flex-row lg:items-end lg:justify-between">
            <div className="flex flex-col gap-6">
              <h2 className="text-[clamp(2.5rem,7vw,6rem)] text-black">Join ARC</h2>
              <p className="max-w-lg text-base leading-relaxed text-black/80 md:text-lg">
                Open to every Carleton student, in any program and any year, first-years especially. Come
                find us in Mackenzie 4463, or say hi on Discord first.
              </p>
            </div>
            <div className="flex flex-wrap gap-4">
              <a
                href={DISCORD_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="inline-flex items-center gap-2.5 bg-black px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-white transition-colors hover:bg-white hover:text-black"
              >
                <BrandIcon brand="discord" />
                JOIN THE DISCORD
              </a>
              <a
                href={INSTAGRAM_URL}
                target="_blank"
                rel="noopener noreferrer"
                className="inline-flex items-center gap-2.5 border border-black/60 px-6 py-3.5 text-xs font-semibold tracking-[0.2em] text-black transition-colors hover:border-black hover:bg-black/10"
              >
                <BrandIcon brand="instagram" />
                FOLLOW {INSTAGRAM_HANDLE.toUpperCase()}
              </a>
            </div>
          </div>
        </section>
      </main>

      <Footer />
    </>
  );
}
