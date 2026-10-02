import { type ReactNode } from 'react';

const FILL = 'currentColor';

/** Altium Designer — official brand mark (via simple-icons) */
export function AltiumDesignerLogo({ className = '' }: { className?: string }) {
  return (
    <svg viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" className={className} aria-label="Altium Designer">
      <path
        d="M19.14 5.876a1.012 1.012 0 00-.442-.442L9.744.171c-.329-.226-.843-.226-1.203-.01L5.148 2.145c-.051.041-.102.082-.144.123a1.086 1.086 0 00-.288.72l.01 6.569-.02.215.062.123a.478.478 0 00.195.206.516.516 0 00.555.01L8.859 8.2a.573.573 0 00.175-.175l.082-.165V4.643l2.251 1.326 3.536 2.077a.413.413 0 01.164.185.442.442 0 01.062.226v7.052a.52.52 0 01-.072.257c-.041.072-.082.123-.154.154l-4.225 2.488-1.573.925v-3.228l1.953-1.172 1.049-.627.185-.175.021-.051a.542.542 0 00.062-.247V9.999a.51.51 0 00-.092-.288l-.062-.123-.144-.072c-.093-.041-.175-.041-.247-.041l-.175.01-6.363 3.865a1.129 1.129 0 00-.442.463 1.281 1.281 0 00-.144.607v6.559c0 .257.103.514.329.75.082.062.154.113.236.164l3.341 1.943c.186.113.381.164.597.164.216 0 .422-.051.596-.164l8.882-5.212c.195-.103.36-.267.442-.432.113-.185.164-.401.164-.617V6.483a1.236 1.236 0 00-.153-.607zM8.387 7.624L5.447 9.32V2.988c0-.072.031-.154.092-.216l.216-.123 2.632 1.563v3.412zm-2.951 6.795c0-.093.021-.185.062-.278a.409.409 0 01.175-.175l5.973-3.629v3.392l-.956.576-2.313 1.388-2.94 1.778v-3.052zm0 6.559v-2.663l2.94-1.768v3.218l-2.632 1.552-.103-.062c-.051-.031-.093-.051-.103-.062-.061-.071-.102-.143-.102-.215zm13.128-3.403a.518.518 0 01-.072.257.342.342 0 01-.165.154l-8.892 5.222a.405.405 0 01-.452 0l-2.508-1.47 4.575-2.693v-.01l4.215-2.478a.998.998 0 00.432-.442 1.13 1.13 0 00.175-.606V8.457c0-.216-.062-.421-.165-.596a1.189 1.189 0 00-.432-.442l-3.536-2.077-3.352-1.974-1.923-1.141L8.911.788a.446.446 0 01.452 0l8.985 5.294a.319.319 0 01.154.154.517.517 0 01.062.247v11.092z"
        fill={FILL}
      />
    </svg>
  );
}

/** KEFC (Kostiuk Engineering Fund) — no public brand mark exists, so a bracketed
 * spec-plate wordmark is used to match the site's blueprint/technical aesthetic. */
export function KEFCLogo({ className = '' }: { className?: string }) {
  return (
    <svg viewBox="0 0 160 48" fill="none" xmlns="http://www.w3.org/2000/svg" className={className} aria-label="KEFC">
      {/* Corner ticks */}
      <path d="M1 9V1h8" stroke={FILL} strokeWidth="1.5" />
      <path d="M159 9V1h-8" stroke={FILL} strokeWidth="1.5" />
      <path d="M1 39v8h8" stroke={FILL} strokeWidth="1.5" />
      <path d="M159 39v8h-8" stroke={FILL} strokeWidth="1.5" />
      <text
        x="80"
        y="26"
        textAnchor="middle"
        dominantBaseline="central"
        fill={FILL}
        style={{ fontFamily: 'var(--font-mono)', fontWeight: 700, fontSize: '15px', letterSpacing: '0.28em' }}
      >
        KEFC
      </text>
    </svg>
  );
}

/** Notion — rounded square with cut-out "N" shape */
export function NotionLogo({ className = '' }: { className?: string }) {
  return (
    <svg viewBox="0 0 100 100" fill="none" xmlns="http://www.w3.org/2000/svg" className={className} aria-label="Notion">
      <path
        d="M6.017 4.313l55.333-4.087c6.797-.583 8.543-.19 12.817 2.917l17.663 12.443c2.913 2.14 3.883 2.723 3.883 5.053v68.243c0 4.277-1.553 6.807-6.99 7.193L24.467 99.967c-4.08.193-6.023-.39-8.16-3.113L3.3 79.94c-2.333-3.113-3.3-5.443-3.3-8.167V11.113c0-3.497 1.553-6.413 6.017-6.8z"
        fill={FILL}
      />
      <path
        d="M61.35 0.227l-55.333 4.087C1.553 4.7 0 7.617 0 11.113v60.66c0 2.723.967 5.053 3.3 8.167l13.007 16.913c2.137 2.723 4.08 3.307 8.16 3.113l64.257-3.89c5.433-.387 6.99-2.917 6.99-7.193V17.64c0-1.937-.58-2.723-2.467-4.117L76.167 1.14C72.193-1.673 70.447-2.063 63.65-1.48l-2.3.18V.227z"
        fill={FILL}
      />
      <path
        d="M28.45 16.7c-5.1.387-6.267.477-9.167-1.913L12.107 9.18C11.14 8.407 10.75 7.83 10.75 7.07c0-1.163.777-1.747 2.72-1.94l49.98-3.657c4.47-.383 6.7 1.167 8.44 2.527l8.367 6.08c.387.29.967 1.163.967 1.747 0 1.163-.967 1.94-2.527 2.047l-51.867 3.11v-.283z"
        fill="var(--color-bp-blue)"
      />
      <path
        d="M22.477 88.817V31.393c0-2.527.777-3.693 3.107-3.883l56.053-3.303c2.137-.193 3.11 1.163 3.11 3.69v56.843c0 2.527-.39 4.67-3.887 4.86l-53.76 3.11c-3.5.193-4.623-1.163-4.623-3.887v-2.003-.003z"
        fill="var(--color-bp-blue)"
      />
      <path
        d="M71.24 34.5c.387 1.74 0 3.5-1.747 3.693l-2.72.58v42.023c-2.333 1.257-4.47 1.94-6.22 1.94-2.917 0-3.69-.97-5.83-3.5L38.637 51.76v25.683l5.63 1.26s0 3.5-4.86 3.5l-13.397.77c-.387-.77 0-2.723 1.357-3.11l3.497-.967V42.573l-4.86-.387c-.387-1.74.58-4.277 3.3-4.47l14.367-.967 17.273 26.457V39.74l-4.667-.58c-.387-2.14 1.163-3.69 3.11-3.887l13.5-.773z"
        fill={FILL}
      />
    </svg>
  );
}

/** Styled text logo for brands without SVG */
function TextLogo({ name, className = '' }: { name: string; className?: string }) {
  return (
    <span
      className={`text-sm font-bold uppercase tracking-[0.15em] ${className}`}
      style={{ fontFamily: 'var(--font-heading)', color: 'currentColor' }}
    >
      {name}
    </span>
  );
}

/** All sponsor logos indexed by name */
export function getSponsorLogo(name: string): ReactNode {
  const logoClass = 'h-full w-auto fill-current';
  switch (name) {
    case 'Notion':
      return <NotionLogo className={logoClass} />;
    case 'Altium Designer':
      return <AltiumDesignerLogo className={logoClass} />;
    case 'KEFC':
      return <KEFCLogo className={logoClass} />;
    default:
      return <TextLogo name={name} />;
  }
}
