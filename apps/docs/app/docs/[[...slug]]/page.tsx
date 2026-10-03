import { source } from '@/lib/source';
import {
  DocsBody,
  DocsDescription,
  DocsPage,
  DocsTitle,
} from 'fumadocs-ui/page';
import type { Metadata } from 'next';
import { notFound } from 'next/navigation';
import defaultMdxComponents from 'fumadocs-ui/mdx';
import { Card } from 'fumadocs-ui/components/card';
import type { ComponentProps } from 'react';
import { getMDXComponents } from '@/mdx-components';
import { resolveHref } from '@/lib/links';
import { adrBacklinks } from '@/lib/systems';

export default async function Page(props: PageProps<'/docs/[[...slug]]'>) {
  const params = await props.params;
  const page = source.getPage(params.slug);
  if (!page) notFound();

  const MDXContent = page.data.body;
  const DefaultLink = defaultMdxComponents.a;
  const backlinks = adrBacklinks(page.absolutePath);

  return (
    <DocsPage toc={page.data.toc} full={page.data.full}>
      <DocsTitle>{page.data.title}</DocsTitle>
      <DocsDescription>{page.data.description}</DocsDescription>
      <DocsBody>
        <MDXContent
          components={getMDXComponents({
            // links are written as file paths, so they also work on GitHub
            a: (props: ComponentProps<'a'>) => (
              <DefaultLink {...props} href={resolveHref(props.href, page.absolutePath)} />
            ),
            Card: (props: ComponentProps<typeof Card>) => (
              <Card {...props} href={resolveHref(props.href, page.absolutePath)} />
            ),
          })}
        />
        {backlinks.length > 0 && (
          <>
            <h2 id="referenced-by">Referenced by</h2>
            <ul>
              {backlinks.map((link) => (
                <li key={link.url}>
                  <DefaultLink href={link.url}>{link.title}</DefaultLink>
                </li>
              ))}
            </ul>
          </>
        )}
      </DocsBody>
    </DocsPage>
  );
}

export async function generateStaticParams() {
  return source.generateParams();
}

export async function generateMetadata(
  props: PageProps<'/docs/[[...slug]]'>,
): Promise<Metadata> {
  const params = await props.params;
  const page = source.getPage(params.slug);
  if (!page) notFound();

  return {
    title: page.data.title,
    description: page.data.description,
  };
}
