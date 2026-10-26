import { CONSTITUTION_URL, MINUTES_URL } from '@/data/social';

// From the ARC constitution, W26 (Drive: ARC CONSTITUTION W26 v1). Paraphrased: the
// constitution is the authority, so update this when it's amended.

export const PURPOSE = [
  'Design, build and test autonomous vehicles and robotic systems for recognized student competitions.',
  'Put members’ learning first: the focus is the learning process, not only the vehicle.',
  'Give Carleton students hands-on experience with industry-standard engineering practices, tools and methods.',
  'Make robotics accessible to the whole student body, with first-year students a priority.',
  'Keep strong relationships with the Faculty of Engineering and Design and the Department of Systems and Computer Engineering.',
] as const;

export const MEMBERSHIP = [
  { label: 'Who can join', value: 'Every Carleton University student, through the yearly registration form' },
  { label: 'Term', value: 'May 1 to April 30' },
  { label: 'Subteams', value: 'Electrical, Mechanical and Software, each led by a Design Lead' },
] as const;

export const EXECUTIVE = [
  {
    role: 'President',
    duties: 'Holds the vision for the robot and keeps the subteams on schedule. Manages the relationship with the Faculty of Engineering and Design, represents ARC to CUSA, CSES and outside bodies, and approves purchases.',
  },
  {
    role: 'VP Finance & Administration',
    duties: 'Keeps the ledger, runs funding applications like KEFC, turns purchase requests into orders, and looks after documents, membership, onboarding, room bookings and safety training.',
  },
  {
    role: 'VP External',
    duties: 'Brings in and looks after sponsors and industry contacts, keeps the sponsorship package current, and organizes events with other teams and clubs.',
  },
  {
    role: 'VP Media',
    duties: "Runs the website and socials, makes the promotional material, and records the team's progress in photo and video.",
  },
  {
    role: 'Design Leads',
    duties: 'One each for Electrical, Mechanical and Software. They drive their subsystem, train new members, manage their budget, and keep documentation and version control in order.',
  },
  {
    role: 'Junior Technical Leads',
    duties: 'Help the Design Leads run tests, teach general members, and handle integration so the leads can focus on the big picture. Non-voting.',
  },
] as const;

export const MEETINGS = [
  {
    title: 'Work meetings',
    text: 'Informal sessions with space to build. This is where most of the work happens.',
  },
  {
    title: 'Executive meetings',
    text: 'Weekly. Club business, and how the subsystems fit together.',
  },
  {
    title: 'Design reviews',
    text: 'Three a year, with feedback from faculty and industry: conceptual (early fall), preliminary (late fall) and critical (late winter).',
  },
] as const;

export const ELECTIONS = [
  'Elections happen once a year, at the Critical Design Review at the end of the winter term, run by a neutral Chief Returning Officer.',
  'The President and the three VPs are elected by a simple majority of the members present. Nominations happen on the spot, and anyone can nominate themselves.',
  'Design Leads aren’t voted on: a committee of the outgoing and incoming executives and past Design Leads appoints them.',
  'Whoever’s elected shadows the current executive until the year ends on April 30.',
] as const;

export const DOCUMENTS = [
  {
    title: 'Constitution',
    text: 'The full W26 constitution.',
    href: CONSTITUTION_URL,
  },
  {
    title: 'Meeting minutes',
    text: "Minutes and exec reports from each week's executive meeting, fall 2026.",
    href: MINUTES_URL,
  },
] as const;
