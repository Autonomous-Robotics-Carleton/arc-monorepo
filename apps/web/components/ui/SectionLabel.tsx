/** Section marker: "01 / WHO WE ARE" in mono, with the index in ARC red. */
export default function SectionLabel({ index, children }: { index: string; children: string }) {
  return (
    <p className="flex items-center gap-3 font-mono text-xs tracking-[0.25em] text-fg/50 uppercase">
      <span className="text-arc-red">{index}</span>
      <span className="h-px w-8 bg-bp-line" />
      {children}
    </p>
  );
}
