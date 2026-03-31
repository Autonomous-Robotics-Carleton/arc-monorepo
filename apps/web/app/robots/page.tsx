'use client';

import { useState } from 'react';
import Header from '@/components/layout/Header';
import Footer from '@/components/layout/Footer';
import BlueprintGrid from '@/components/ui/BlueprintGrid';
import CornerTicks from '@/components/ui/CornerTicks';
import { useStaggerReveal } from '@/hooks/useScrollReveal';
import { projects, type Project } from '@/data/projects';

function ProjectModal({ project, onClose }: { project: Project; onClose: () => void }) {
  return (
    <div
      className="fixed inset-0 z-50 flex items-center justify-center p-6"
      onClick={onClose}
    >
      {/* Backdrop */}
      <div className="absolute inset-0 bg-bg/80 backdrop-blur-sm" />

      {/* Panel */}
      <div
        className="relative w-full max-w-2xl border border-bp-line p-8 bp-glass-strong"
        onClick={(e) => e.stopPropagation()}
      >
        <CornerTicks size={12} />

        {/* Header row */}
        <div className="mb-6 flex items-start justify-between gap-4">
          <p className="text-xs tracking-widest text-fg/40">
            {project.tags.join(' / ')}
          </p>
          <button
            onClick={onClose}
            className="text-xs tracking-widest text-fg/40 transition-colors hover:text-accent"
          >
            CLOSE
          </button>
        </div>

        {/* Image placeholder */}
        <div className="mb-6 aspect-video w-full bg-bp-blue-light" />

        {/* Content */}
        <h2 className="text-2xl md:text-3xl">{project.title}</h2>
        <p className="mt-3 text-sm leading-relaxed text-fg/60">
          {project.description}
        </p>

        {/* Tags */}
        <div className="mt-4 flex flex-wrap gap-2">
          {project.tags.map((tag) => (
            <span
              key={tag}
              className="border border-accent/15 px-3 py-1 text-xs uppercase tracking-wider text-accent/50"
            >
              {tag}
            </span>
          ))}
        </div>

        {/* GitHub */}
        {project.github && (
          <div className="mt-6 border-t border-bp-line pt-6">
            <a
              href={project.github}
              target="_blank"
              rel="noopener noreferrer"
              className="inline-flex items-center gap-2.5 border border-accent/40 px-5 py-2.5 text-xs tracking-widest text-accent transition-all duration-200 hover:border-accent hover:bg-accent/10"
            >
              VIEW ON GITHUB
            </a>
          </div>
        )}
      </div>
    </div>
  );
}

export default function RobotsPage() {
  const [activeProject, setActiveProject] = useState<Project | null>(null);
  const gridRef = useStaggerReveal<HTMLDivElement>('[data-reveal-item]', {
    stagger: 0.12,
  });

  return (
    <>
      <Header />
      <main>
        <section className="relative px-6 pt-28 pb-32 md:px-10 lg:px-16">
          <BlueprintGrid variant="hero" />
          <div
            ref={gridRef}
            className="mx-auto grid max-w-[1440px] grid-cols-1 gap-8 pt-8 md:grid-cols-2"
          >
            {projects.map((project) => (
              <article
                key={project.id}
                data-reveal-item
                className="group relative cursor-pointer border border-bp-line p-8 transition-all duration-300 hover:border-accent/30 bp-glow-hover bp-glass"
                onClick={() => setActiveProject(project)}
              >
                <CornerTicks size={10} />
                <div className="mb-6 aspect-video w-full bg-bp-blue-light" />
                <h2 className="text-2xl md:text-3xl">{project.title}</h2>
                <p className="mt-3 text-sm leading-relaxed text-fg/60">
                  {project.description}
                </p>
                <div className="mt-4 flex flex-wrap gap-2">
                  {project.tags.map((tag) => (
                    <span
                      key={tag}
                      className="border border-accent/15 px-3 py-1 text-xs uppercase tracking-wider text-accent/50"
                    >
                      {tag}
                    </span>
                  ))}
                </div>
              </article>
            ))}
          </div>
        </section>
      </main>

      <Footer />

      {activeProject && (
        <ProjectModal
          project={activeProject}
          onClose={() => setActiveProject(null)}
        />
      )}
    </>
  );
}
