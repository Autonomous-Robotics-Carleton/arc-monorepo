// Sponsors as shown on the last slide of the KEFC Fall 2026 deck (Drive: ARC KEFC Fall 2026 - Slides).
// Logos are cut from that slide: white on transparent, and their pixel heights keep the slide's
// relative sizing (the tallest is 65px, shown at 2.75rem).
export type Sponsor = {
  name: string;
  logo: string;
  width: number;
  height: number;
};

export const sponsors: Sponsor[] = [
  { name: 'Altium', logo: '/sponsors/altium.png', width: 162, height: 36 },
  { name: 'Notion', logo: '/sponsors/notion.png', width: 145, height: 54 },
  { name: 'KEFC', logo: '/sponsors/kefc.png', width: 77, height: 65 },
  {
    name: 'Carleton University Faculty of Engineering and Design',
    logo: '/sponsors/carleton-fed.png',
    width: 343,
    height: 50,
  },
];
