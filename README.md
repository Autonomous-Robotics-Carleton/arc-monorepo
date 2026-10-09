# 🚗 ARC — Autonomous Robotics Carleton  

<p align="center">
  <img src="https://img.shields.io/github/last-commit/Autonomous-Robotics-Carleton/arc-monorepo?color=blue&style=for-the-badge" />
  <img src="https://img.shields.io/github/contributors/Autonomous-Robotics-Carleton/arc-monorepo?color=green&style=for-the-badge" />
  <img src="https://img.shields.io/github/issues/Autonomous-Robotics-Carleton/arc-monorepo?color=orange&style=for-the-badge" />
  <img src="https://img.shields.io/github/issues-pr/Autonomous-Robotics-Carleton/arc-monorepo?color=purple&style=for-the-badge" />
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" />
</p>

---

<p align="center">
  <img width="3308" height="1350" alt="ARC INSTA BANNER" src="https://github.com/user-attachments/assets/4b4c9acd-6771-452b-a9fa-c6f2cd5e4346" />
</p>

---

Welcome to the **monorepo** for **ARC (Autonomous Robotics Carleton)**!  
This project houses everything for our **autonomous car**: its systems engineering, the code that runs on it, its hardware designs, plus our public website and documentation hub.  

We’re keeping this project **fully open source**, so current and future members — and the wider robotics community — can learn, contribute, and grow with us.  

---

## 📖 What’s Inside  

| Folder | What lives there |
| --- | --- |
| [`systems/`](systems/README.md) | Source of truth for the car: requirements, decisions (ADRs), interfaces, budgets, risks |
| [`ros/`](ros/README.md) | ROS 2 workspace for the Jetson Orin NX |
| [`firmware/`](firmware/README.md) | Sync MCU (Zephyr) and VESC firmware |
| [`experiments/`](experiments/README.md) | Team experiments, run in containers beside the platform |
| [`hardware/`](hardware/README.md) | Board designs and mechanical exports |
| [`platform/`](platform/README.md) | Jetson image, kernel, containers, system services |
| [`tools/`](tools/README.md) | Developer scripts: `systems/` checks, interface code generation, VESC vendoring, firmware-in-the-loop test |
| [`.devcontainer/`](.devcontainer/) | The dev container every firmware and ROS build runs in (CI uses the same image) |
| [`libs/`](libs/README.md) | Shared TypeScript packages |
| `apps/` | The public website and the docs site (below) |

### 🌐 `apps/web` — Public Website  
The ARC marketing and showcase site built with **Next.js 15**, **Tailwind CSS v4**, and **GSAP** animations.  
- 🏎 **Home page** — interactive hero, mission and sponsors  
- 📐 **Blueprint page** — animated exploded-view car visualization  
- 👥 **Robots & team pages** — showcasing members and ongoing work  

### 📚 `apps/docs` — Documentation Hub  
**[docs.arcarleton.ca](https://docs.arcarleton.ca)**, built with **Fumadocs** + **Next.js**.  
- 🏎 **The car** — everything in `systems/`, with every requirement, risk and decision ID linked to its definition  
- 🧰 **Handbook** — dev setup, contributing, Fusion, the Orin dev kit and the club website  
- 🤖 **For LLMs** — `/llms.txt`, `/llms-full.txt`, `/ids.json`, and every page as Markdown at its URL + `.md`  

---

## 🚀 Getting Started

```bash
git clone https://github.com/Autonomous-Robotics-Carleton/arc-monorepo.git
cd arc-monorepo
corepack enable && pnpm install
npx nx dev docs        # Docs site → http://localhost:3000
npx nx dev web         # Website  → http://localhost:3001
```

Requires Node.js 22 (`.nvmrc`). Firmware and ROS build in the dev container: see **Handbook → Dev setup** on the docs site. Branching, commits, PRs and the rules for `systems/` are in **[CONTRIBUTING.md](.github/CONTRIBUTING.md)**. Coding agents: start with **[AGENTS.md](AGENTS.md)**.

**Looking for something to work on?** Open work is in the issues, indexed in [#110](https://github.com/Autonomous-Robotics-Carleton/arc-monorepo/issues/110).

---

# 🧪 CI/CD Pipeline

**GitHub Actions** runs `nx affected`, so each PR only lints, tests, checks and builds the projects its changes touch. A change in `systems/` also marks the projects that depend on it (`docs`, `sync-mcu`, `vesc`, `ros`, and through them `firmware-in-loop`).

On every merge to `main` that affects the docs:

* Builds the production Docker image and pushes it to GHCR:
  * `ghcr.io/autonomous-robotics-carleton/2026:latest`
  * `ghcr.io/autonomous-robotics-carleton/2026:<commit-sha>`
* ARC infrastructure auto-deploys it to **docs.arcarleton.ca** via Watchtower

Firmware and ROS (`sync-mcu`, `vesc`, `ros`, `firmware-in-loop`) build and test inside the dev container image, `ghcr.io/autonomous-robotics-carleton/arc-dev`. CI tags it by the hash of its inputs and builds and smoke-tests a new one when they change; the affected firmware and ROS projects are tested in it (a change to `.devcontainer/` alone affects none, so run them yourself). `main` publishes it as `:latest`.

Website and docs contributors never touch Docker; firmware and ROS contributors use the dev container.

---

# 🏗 Project Structure

```
arc-monorepo/                 # Nx monorepo root
├── systems/                  # Requirements, ADRs, ICDs, budgets, risks
├── ros/                      # ROS 2 workspace (Orin)
├── firmware/                 # Sync MCU and VESC firmware
├── experiments/              # Team experiments (containers)
├── hardware/                 # Board designs, mechanical exports
├── platform/                 # Jetson image and services
├── tools/                    # Repo scripts (systems/ checks, codegen, VESC vendoring, firmware-in-loop)
├── .devcontainer/            # Dev container for firmware and ROS (CI uses it too)
├── libs/
│   └── systems-model/        # Parser and checks for systems/
├── apps/
│   ├── docs/                 # Fumadocs / Next.js docs app
│   │   ├── app/              # Next.js App Router
│   │   ├── content/          # Handbook pages (MDX); other pages come from systems/, apps/web/docs/, platform/dev-kit/
│   │   ├── public/           # Static assets
│   │   ├── lib/              # Utility functions
│   │   ├── components/       # React components
│   │   ├── next.config.mjs
│   │   └── package.json
│   └── web/                  # Next.js public website
│       ├── docs/             # Website docs (shown on the docs site)
│       ├── app/              # Next.js App Router (pages)
│       ├── components/       # UI + layout components
│       ├── context/          # React context (page transitions)
│       ├── data/             # Site content: sponsors, projects, team
│       ├── lib/              # Animation utilities
│       ├── hooks/            # Custom React hooks
│       ├── public/           # Static assets (images, video)
│       ├── next.config.mjs
│       └── package.json
├── nx.json                   # Nx workspace config
├── pnpm-workspace.yaml       # pnpm workspace config
├── .github/                  # CI workflows, templates, CONTRIBUTING.md, CODEOWNERS
├── AGENTS.md                 # Guidance for coding agents (CLAUDE.md points here)
└── README.md
```

---

# ➕ Adding a New App or Library

This repo uses **Nx** + **pnpm workspaces**. Web apps live in `apps/` and shared TypeScript in `libs/`; the car's projects are in `firmware/`, `ros/`, `systems/` and `tools/`.

## Add a new app

1. Create the app directory with its own `package.json`:

```bash
mkdir -p apps/myapp
```

2. Add a `package.json` inside it:

```json
{
  "name": "arc-myapp",
  "version": "0.0.0",
  "private": true,
  "scripts": {
    "dev": "next dev",
    "build": "next build",
    "start": "next start"
  }
}
```

3. Add a `project.json` to register it with Nx:

```json
{
  "name": "myapp",
  "$schema": "../../node_modules/nx/schemas/project-schema.json",
  "projectType": "application",
  "targets": {
    "dev": { "command": "next dev", "options": { "cwd": "apps/myapp" } },
    "build": { "command": "next build", "options": { "cwd": "apps/myapp" }, "outputs": ["{projectRoot}/.next"] },
    "start": { "command": "next start", "options": { "cwd": "apps/myapp" }, "dependsOn": ["build"] }
  }
}
```

4. Install dependencies and run:

```bash
pnpm install
npx nx dev myapp
```

## Add a shared library

1. Create the lib:

```bash
mkdir -p libs/config
```

2. Add a `libs/config/package.json`:

```json
{
  "name": "@arc/config",
  "version": "0.0.0",
  "private": true,
  "main": "index.ts"
}
```

3. Use it from any app by adding to that app's `package.json`:

```json
{
  "dependencies": {
    "@arc/config": "workspace:*"
  }
}
```

4. Run `pnpm install` and import as usual:

```ts
import { something } from '@arc/config';
```

## Useful Nx commands

| Command | What it does |
|---|---|
| `npx nx dev <app>` | Run dev server for an app |
| `npx nx build <app>` | Build an app |
| `npx nx lint <app>` | Lint an app |
| `npx nx show projects` | List all registered projects |
| `npx nx graph` | Open interactive dependency graph |
| `npx nx run-many -t build` | Build all projects (firmware and ROS only in the dev container) |
| `npx nx affected -t lint test check build` | What CI runs: only projects your changes touch |
| `npx nx check systems` | Check `systems/`: links, IDs, ADRs, verification coverage |
| `node tools/systems-ids.mts SYS-04` | Look up an ID: definition, status, references |
| `npx nx build sync-mcu` / `test sync-mcu` | Sync MCU firmware: three boards; tests on `native_sim` (dev container) |
| `npx nx build vesc` / `test vesc` | Motor controller firmware (A50S); host tests for our modules (dev container) |
| `npx nx build ros` / `test ros` | ROS 2 workspace; launch tests per backend and the sync-link round trip (dev container) |
| `npx nx test firmware-in-loop` | Sync MCU firmware on `native_sim` and ROS link up over UDP (dev container) |
| `tools/gen-interfaces.sh` | Regenerate code from `systems/icd/` (`--check` in CI) |

---

# 📘 License

This project is licensed under the **MIT License**, except `firmware/vesc/`: the VESC firmware is upstream's GPL-3.0, and so are our changes to it (ADR-0033). Don't copy code from there into the rest of the repo.

---

# 🎉 Thanks for Contributing!

Whether you're fixing typos, writing docs, or creating new tutorials —
**your work helps drive ARC forward.**

If you have questions, open an issue or reach out to the ARC Team!
