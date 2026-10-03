"use client";

import Link from "next/link";
import Image from "next/image";
import { usePathname } from "next/navigation";
import { useSidebar } from "fumadocs-ui/provider";
import logo from "@/public/arc-logo.svg";

const navLinks = [{ href: "/docs", label: "Docs" }];

const externalLinks = [
  {
    href: "https://github.com/Autonomous-Robotics-Carleton/arc-monorepo",
    label: "GitHub",
    icon: (
      <svg viewBox="0 0 24 24" width="20" height="20" fill="currentColor">
        <path d="M12 0C5.37 0 0 5.37 0 12c0 5.31 3.435 9.795 8.205 11.385.6.105.825-.255.825-.57 0-.285-.015-1.23-.015-2.235-3.015.555-3.795-.735-4.035-1.41-.135-.345-.72-1.41-1.23-1.695-.42-.225-1.02-.78-.015-.795.945-.015 1.62.87 1.845 1.23 1.08 1.815 2.805 1.305 3.495.99.105-.78.42-1.305.765-1.605-2.67-.3-5.46-1.335-5.46-5.925 0-1.305.465-2.385 1.23-3.225-.12-.3-.54-1.53.12-3.18 0 0 1.005-.315 3.3 1.23.96-.27 1.98-.405 3-.405s2.04.135 3 .405c2.295-1.56 3.3-1.23 3.3-1.23.66 1.65.24 2.88.12 3.18.765.84 1.23 1.905 1.23 3.225 0 4.605-2.805 5.625-5.475 5.925.435.375.81 1.095.81 2.22 0 1.605-.015 2.895-.015 3.3 0 .315.225.69.825.57A12.02 12.02 0 0024 12c0-6.63-5.37-12-12-12z" />
      </svg>
    ),
  },
];

function SidebarToggle() {
  const { collapsed, setCollapsed } = useSidebar();

  return (
    <button
      type="button"
      aria-label={collapsed ? "Expand Sidebar" : "Collapse Sidebar"}
      onClick={() => setCollapsed(!collapsed)}
      className="hidden md:flex items-center justify-center w-9 h-9 rounded-md text-fd-muted-foreground hover:text-fd-foreground hover:bg-fd-accent transition-colors"
    >
      <svg
        xmlns="http://www.w3.org/2000/svg"
        width="20"
        height="20"
        viewBox="0 0 24 24"
        fill="none"
        stroke="currentColor"
        strokeWidth="2"
        strokeLinecap="round"
        strokeLinejoin="round"
      >
        <rect width="18" height="18" x="3" y="3" rx="2" />
        <path d="M9 3v18" />
      </svg>
    </button>
  );
}

export function Navbar() {
  const pathname = usePathname();

  return (
    <header className="sticky top-0 z-50 w-full border-b border-fd-border bg-fd-background/95 backdrop-blur supports-[backdrop-filter]:bg-fd-background/60">
      <div className="flex h-14 items-center px-4 md:px-6">
        {/* Logo and site name */}
        <Link href="/" className="flex items-center gap-2 mr-4">
          <Image
            src={logo}
            alt="ARC Logo"
            width={28}
            height={28}
            className="rounded"
          />
          <span className="font-semibold text-fd-foreground">Arc Docs</span>
        </Link>

        {/* Sidebar toggle - after logo */}
        <SidebarToggle />

        {/* Nav links */}
        <nav className="hidden md:flex items-center gap-6 text-sm ml-6">
          {navLinks.map((link) => (
            <Link
              key={link.href}
              href={link.href}
              className={`transition-colors hover:text-fd-foreground ${
                pathname?.startsWith(link.href)
                  ? "text-fd-foreground font-medium"
                  : "text-fd-muted-foreground"
              }`}
            >
              {link.label}
            </Link>
          ))}
        </nav>

        {/* Spacer */}
        <div className="flex-1" />

        {/* External links */}
        <div className="flex items-center gap-4">
          {externalLinks.map((link) => (
            <a
              key={link.href}
              href={link.href}
              target="_blank"
              rel="noopener noreferrer"
              className="text-fd-muted-foreground hover:text-fd-foreground transition-colors"
              title={link.label}
            >
              {link.icon}
            </a>
          ))}
        </div>
      </div>
    </header>
  );
}
