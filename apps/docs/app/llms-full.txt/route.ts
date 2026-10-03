import { pageMarkdown, sections } from '@/lib/page-markdown';

// Every page as Markdown in one file, in sidebar order.
export const dynamic = 'force-static';

export function GET() {
  const pages = sections().flatMap((section) => section.pages);
  return new Response(pages.map(pageMarkdown).join('\n\n---\n\n') + '\n', {
    headers: { 'Content-Type': 'text/plain; charset=utf-8' },
  });
}
