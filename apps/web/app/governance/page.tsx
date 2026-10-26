'use client';

import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import { Mail } from 'lucide-react';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import BrandIcon from '@/components/ui/BrandIcon';
import { DOCUMENTS, ELECTIONS, EXECUTIVE, MEETINGS, MEMBERSHIP, PURPOSE } from '@/data/governance';
import { EMAIL } from '@/data/social';

const documentsHref = `mailto:${EMAIL}?subject=${encodeURIComponent('ARC constitution / meeting minutes')}`;

export default function GovernancePage() {
  return (
    <>
      <Header />
      <main>
        <BlueprintGrid variant="hero" />

        {/* ── Intro ── */}
        <section className="relative px-6 pt-28 pb-16 md:px-10 md:pt-36 lg:px-16">
          <div className="mx-auto flex max-w-[1440px] flex-col gap-7">
            <h1 className="text-[clamp(2.75rem,8vw,7rem)]">Governance</h1>
            <p className="max-w-2xl text-base leading-relaxed text-fg/70 md:text-lg">
              ARC is a Carleton University Students&apos; Association (CUSA) club. Its constitution sets out
              what the club is for, who can join, who runs it and how they&apos;re chosen. Here&apos;s the
              short version.
            </p>
          </div>
        </section>

        {/* ── Purpose ── */}
        <section className="relative px-6 py-16 md:px-10 lg:px-16">
          <div className="mx-auto grid max-w-[1440px] gap-10 lg:grid-cols-[minmax(0,0.8fr)_minmax(0,1.2fr)] lg:gap-20">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">What ARC is for</h2>
            <ul className="border-t border-bp-line">
              {PURPOSE.map((p) => (
                <li key={p} className="border-b border-bp-line py-5 text-base leading-relaxed text-fg/75">
                  {p}
                </li>
              ))}
            </ul>
          </div>
        </section>

        {/* ── Membership ── */}
        <section className="relative px-6 py-16 md:px-10 lg:px-16">
          <div className="mx-auto grid max-w-[1440px] gap-10 lg:grid-cols-[minmax(0,0.8fr)_minmax(0,1.2fr)] lg:gap-20">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">Who can join</h2>
            <dl className="border-t border-bp-line">
              {MEMBERSHIP.map((m) => (
                <div key={m.label} className="grid grid-cols-1 gap-1 border-b border-bp-line py-5 sm:grid-cols-[10rem_minmax(0,1fr)] sm:gap-6">
                  <dt className="text-sm text-fg/45">{m.label}</dt>
                  <dd className="text-base text-fg/80">{m.value}</dd>
                </div>
              ))}
            </dl>
          </div>
        </section>

        {/* ── Executive ── */}
        <section className="relative px-6 py-16 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">Who runs it</h2>
            <div className="mt-10 grid grid-cols-1 border-t border-l border-bp-line sm:grid-cols-2 lg:grid-cols-3">
              {EXECUTIVE.map((e) => (
                <div key={e.role} className="border-r border-b border-bp-line bg-black/70 p-6">
                  <h3 className="text-lg">{e.role}</h3>
                  <p className="mt-3 text-sm leading-relaxed text-fg/60">{e.duties}</p>
                </div>
              ))}
            </div>
          </div>
        </section>

        {/* ── Meetings ── */}
        <section className="relative px-6 py-16 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">How the year runs</h2>
            <div className="mt-10 grid grid-cols-1 border-t border-l border-bp-line md:grid-cols-3">
              {MEETINGS.map((m) => (
                <div key={m.title} className="border-r border-b border-bp-line bg-black/70 p-6">
                  <h3 className="text-lg">{m.title}</h3>
                  <p className="mt-3 text-sm leading-relaxed text-fg/60">{m.text}</p>
                </div>
              ))}
            </div>
          </div>
        </section>

        {/* ── Elections ── */}
        <section className="relative px-6 py-16 md:px-10 lg:px-16">
          <div className="mx-auto grid max-w-[1440px] gap-10 lg:grid-cols-[minmax(0,0.8fr)_minmax(0,1.2fr)] lg:gap-20">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">Elections</h2>
            <ul className="border-t border-bp-line">
              {ELECTIONS.map((e) => (
                <li key={e} className="border-b border-bp-line py-5 text-base leading-relaxed text-fg/75">
                  {e}
                </li>
              ))}
            </ul>
          </div>
        </section>

        {/* ── Documents ── */}
        <section className="relative px-6 pt-16 pb-32 md:px-10 lg:px-16">
          <div className="mx-auto max-w-[1440px]">
            <h2 className="text-[clamp(2rem,4vw,3.5rem)]">Documents</h2>
            <div className="mt-10 border-t border-bp-line">
              {DOCUMENTS.map((d) => (
                <a
                  key={d.title}
                  href={d.href}
                  target="_blank"
                  rel="noopener noreferrer"
                  className="group flex flex-col gap-4 border-b border-bp-line py-6 transition-colors hover:bg-white/[0.02] sm:flex-row sm:items-center sm:justify-between"
                >
                  <div className="flex items-center gap-4">
                    <BrandIcon brand="drive" className="h-5 w-5 text-fg/50 transition-colors group-hover:text-arc-red" />
                    <div>
                      <h3 className="text-lg">{d.title}</h3>
                      <p className="mt-1 text-sm text-fg/55">{d.text}</p>
                    </div>
                  </div>
                  <span className="shrink-0 self-start border border-accent/60 px-5 py-3 text-xs tracking-[0.2em] text-accent transition-colors group-hover:border-accent group-hover:bg-accent/10 sm:self-auto">
                    OPEN IN DRIVE
                  </span>
                </a>
              ))}
            </div>
            <a
              href={documentsHref}
              className="mt-6 inline-flex items-center gap-2 text-sm text-fg/50 transition-colors hover:text-arc-red"
            >
              <Mail className="h-4 w-4" aria-hidden="true" />
              Can&apos;t open them? Email us for a copy.
            </a>
          </div>
        </section>
      </main>
      <Footer />
    </>
  );
}
