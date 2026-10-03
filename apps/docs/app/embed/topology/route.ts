import { readFileSync } from 'node:fs';
import { resolve } from 'node:path';

// Rendered once at build: the runtime image doesn't include systems/.
export const dynamic = 'force-static';

export function GET() {
  const html = readFileSync(resolve(process.cwd(), '../../systems/topology.html'), 'utf8');
  return new Response(html, { headers: { 'Content-Type': 'text/html; charset=utf-8' } });
}
