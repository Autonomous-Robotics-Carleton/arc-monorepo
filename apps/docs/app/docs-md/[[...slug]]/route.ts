import { notFound } from 'next/navigation';
import { pageMarkdown } from '@/lib/page-markdown';
import { source } from '@/lib/source';

// Serves /docs/<page>.md (rewritten here in next.config.mjs).
export const dynamic = 'force-static';

export function generateStaticParams() {
  return source.generateParams();
}

export async function GET(_request: Request, props: { params: Promise<{ slug?: string[] }> }) {
  const { slug } = await props.params;
  const page = source.getPage(slug);
  if (!page) notFound();
  return new Response(pageMarkdown(page), { headers: { 'Content-Type': 'text/markdown; charset=utf-8' } });
}
