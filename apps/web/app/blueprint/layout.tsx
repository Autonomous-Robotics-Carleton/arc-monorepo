import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Blueprint',
  description: "Take ARC's car apart, one scroll at a time.",
};

export default function Layout({ children }: { children: React.ReactNode }) {
  return children;
}
