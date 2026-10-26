import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Governance',
  description: 'How Autonomous Robotics Carleton is run: purpose, membership, the executive and elections.',
};

export default function Layout({ children }: { children: React.ReactNode }) {
  return children;
}
