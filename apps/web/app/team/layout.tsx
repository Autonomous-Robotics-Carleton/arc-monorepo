import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Team',
  description: 'The execs and leads who run Autonomous Robotics Carleton.',
};

export default function Layout({ children }: { children: React.ReactNode }) {
  return children;
}
