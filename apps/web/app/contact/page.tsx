'use client';

import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import VerticalLine from '@/components/ui/VerticalLine';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import { BookOpen, Mail, MapPin } from 'lucide-react';
import BrandIcon from '@/components/ui/BrandIcon';
import ContactTypography from '@/components/ui/ContactTypography';
import { useScrollReveal } from '@/hooks/useScrollReveal';
import { DISCORD_URL, DOCS_URL, EMAIL, GITHUB_REPO_URL, SOCIAL_LINKS } from '@/data/social';

export default function ContactPage() {
  const leftRef = useScrollReveal<HTMLDivElement>({ y: 30 });
  const rightRef = useScrollReveal<HTMLDivElement>({ y: 30, delay: 0.15 });

  return (
    <>
      <Header />
      <main>
        <section className="relative flex min-h-screen items-start px-6 pt-28 md:px-10 lg:px-16">
          <BlueprintGrid variant="hero" />
          <div className="mx-auto flex w-full max-w-[1440px] flex-col gap-12 py-16 lg:flex-row lg:gap-24">
            {/* Left */}
            <div className="flex gap-6 lg:w-2/5">
              <VerticalLine className="hidden h-[400px] shrink-0 md:flex" />
              <div ref={leftRef}>
                <ContactTypography className="h-auto w-full max-w-[280px] md:max-w-[320px] lg:max-w-[353px]" />
              </div>
            </div>

            {/* Right */}
            <div ref={rightRef} className="flex-1 border border-bp-line px-6 py-2 lg:px-8 bp-glass">

              {/* Email */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <a
                    href={`mailto:${EMAIL}`}
                    className="flex items-center gap-2.5 text-sm tracking-widest transition-colors hover:text-accent"
                  >
                    <Mail className="h-4 w-4 text-arc-red" aria-hidden="true" />
                    {EMAIL.toUpperCase()}
                  </a>
                  <p className="max-w-[240px] text-right text-sm leading-relaxed text-fg/60">
                    Feel free to reach out if you want to collaborate with us, or
                    simply have a chat!
                  </p>
                </div>
              </div>

              {/* Discord */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <a
                    href={DISCORD_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="flex items-center gap-2.5 text-sm tracking-widest transition-colors hover:text-accent"
                  >
                    <BrandIcon brand="discord" className="h-4 w-4 text-arc-red" />
                    DISCORD
                  </a>
                  <a
                    href={DISCORD_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="shrink-0 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                  >
                    JOIN THE SERVER →
                  </a>
                </div>
              </div>

              {/* Location */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <a
                    href="https://maps.google.com/?q=1125+Colonel+By+Dr+Ottawa+ON+K1S+5B6"
                    target="_blank"
                    rel="noopener noreferrer"
                    className="flex items-center gap-2.5 text-sm tracking-widest transition-colors hover:text-accent"
                  >
                    <MapPin className="h-4 w-4 text-arc-red" aria-hidden="true" />
                    VIEW ON GOOGLE MAPS
                  </a>
                  <div className="text-right text-sm leading-relaxed text-fg/60">
                    <p>Mackenzie Building 4463</p>
                    <p>1125 Colonel By Dr, Ottawa, ON K1S 5B6</p>
                  </div>
                </div>
              </div>

              {/* Follow us */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <span className="flex items-center gap-2.5 text-sm tracking-widest text-fg/60">
                    FOLLOW US
                  </span>
                  <ul className="flex items-center gap-6 text-right">
                    {SOCIAL_LINKS.map((link) => (
                      <li key={link.label}>
                        <a
                          href={link.href}
                          target="_blank"
                          rel="noopener noreferrer"
                          className="flex items-center gap-2 text-sm tracking-widest text-fg/40 transition-colors hover:text-accent"
                        >
                          <BrandIcon brand={link.icon} className="h-4 w-4" />
                          {link.label}
                        </a>
                      </li>
                    ))}
                  </ul>
                </div>
              </div>

              {/* GitHub */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <a
                    href={GITHUB_REPO_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="flex items-center gap-2.5 text-sm tracking-widest transition-colors hover:text-accent"
                  >
                    <BrandIcon brand="github" className="h-4 w-4 text-arc-red" />
                    GITHUB
                  </a>
                  <a
                    href={GITHUB_REPO_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="shrink-0 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                  >
                    VIEW REPO →
                  </a>
                </div>
              </div>

              {/* Docs */}
              <div className="border-t border-bp-line py-8">
                <div className="flex flex-col justify-between gap-4 sm:flex-row sm:items-start">
                  <a
                    href={DOCS_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="flex items-center gap-2.5 text-sm tracking-widest transition-colors hover:text-accent"
                  >
                    <BookOpen className="h-4 w-4 text-arc-red" aria-hidden="true" />
                    DOCS
                  </a>
                  <a
                    href={DOCS_URL}
                    target="_blank"
                    rel="noopener noreferrer"
                    className="shrink-0 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
                  >
                    OPEN DOCS →
                  </a>
                </div>
              </div>

            </div>
          </div>
        </section>
      </main>
      <Footer />
    </>
  );
}
