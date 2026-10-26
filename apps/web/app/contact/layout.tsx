import type { Metadata } from 'next';

export const metadata: Metadata = {
  title: 'Contact',
  description: 'Get in touch with Autonomous Robotics Carleton: email, Discord, and where to find us at Carleton.',
};

export default function Layout({ children }: { children: React.ReactNode }) {
  return children;
}
