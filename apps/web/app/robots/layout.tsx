import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Robots',
  description: 'ARC v1, our 1/10-scale four-wheel-drive autonomy research car: how it fits together, what it senses, and the code behind it.',
};

export default function Layout({ children }: { children: React.ReactNode }) {
  return children;
}
