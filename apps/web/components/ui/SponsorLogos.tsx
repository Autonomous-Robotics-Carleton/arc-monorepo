import Image from 'next/image';
import type { ReactNode } from 'react';
import type { Sponsor } from '@/data/sponsors';

/** A sponsor's logo, at its size relative to the others on the KEFC slide */
export function getSponsorLogo(sponsor: Sponsor): ReactNode {
  return (
    <Image
      src={sponsor.logo}
      alt={sponsor.name}
      width={sponsor.width}
      height={sponsor.height}
      style={{ height: `${(sponsor.height / 65) * 2.75}rem`, width: 'auto' }}
      draggable={false}
    />
  );
}
