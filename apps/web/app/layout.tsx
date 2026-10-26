import type { Metadata, Viewport } from 'next';
import { Archivo, Inter, JetBrains_Mono } from 'next/font/google';
import './globals.css';
import ClientProviders from '@/components/providers/ClientProviders';
import { SITE_URL } from '@/data/social';

// Variable width axis: headings use it at 125% (expanded)
const archivo = Archivo({
  subsets: ['latin'],
  axes: ['wdth'],
  variable: '--font-archivo',
  display: 'swap',
});

const inter = Inter({
  subsets: ['latin'],
  variable: '--font-inter',
  display: 'swap',
});

const jetbrainsMono = JetBrains_Mono({
  subsets: ['latin'],
  variable: '--font-jetbrains',
  display: 'swap',
});

const description =
  'Autonomous Robotics Carleton: a student engineering club at Carleton University building 1/10-scale cars that drive themselves.';

export const metadata: Metadata = {
  metadataBase: new URL(SITE_URL),
  title: {
    default: 'ARC | Autonomous Robotics Carleton',
    template: '%s | ARC',
  },
  description,
  openGraph: {
    type: 'website',
    siteName: 'Autonomous Robotics Carleton',
    title: 'ARC | Autonomous Robotics Carleton',
    description,
    url: '/',
  },
  twitter: { card: 'summary_large_image' },
};

export const viewport: Viewport = { themeColor: '#000000' };

export default function RootLayout({
  children,
}: {
  children: React.ReactNode;
}) {
  return (
    <html
      lang="en"
      className={`${archivo.variable} ${inter.variable} ${jetbrainsMono.variable}`}
    >
      <body>
        <ClientProviders>{children}</ClientProviders>
      </body>
    </html>
  );
}
