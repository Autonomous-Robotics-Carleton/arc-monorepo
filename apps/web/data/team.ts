export type TeamMember = {
  name: string;
  role: string;
  image?: string;
};

export const teamMembers: TeamMember[] = [
  { name: 'Jon Froese',            role: 'President' },
  { name: 'Shrikar Vempati',       role: 'VP Finance & Administration' },
  { name: 'Khang Doan',            role: 'VP Media' },
  { name: 'Dane Lindhart Smith',   role: 'VP External' },
  { name: 'Kunshpreet Baweja',     role: 'Electrical Lead' },
  { name: 'Maryam Jama',           role: 'Mechanical Lead' },
  { name: 'Shrikar Vempati',       role: 'Software Lead' },
];
