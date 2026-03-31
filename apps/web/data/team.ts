export type TeamMember = {
  name: string;
  role: string;
  image?: string;
};

export const teamMembers: TeamMember[] = [
  { name: 'Member One',   role: 'President' },
  { name: 'Member Two',   role: 'VP Finance & Administration' },
  { name: 'Member Three', role: 'VP External' },
  { name: 'Member Four',  role: 'VP Media' },
  { name: 'Member Five',  role: 'Software Lead' },
  { name: 'Member Six',   role: 'Electrical Lead' },
  { name: 'Member Seven', role: 'Mechanical Lead' },
];
