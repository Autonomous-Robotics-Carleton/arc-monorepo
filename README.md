# 🚗 ARC — Autonomous Robotics Carleton  

<p align="center">
  <img src="https://img.shields.io/github/last-commit/Autonomous-Robotics-Carleton/2026?color=blue&style=for-the-badge" />
  <img src="https://img.shields.io/github/contributors/Autonomous-Robotics-Carleton/2026?color=green&style=for-the-badge" />
  <img src="https://img.shields.io/github/issues/Autonomous-Robotics-Carleton/2026?color=orange&style=for-the-badge" />
  <img src="https://img.shields.io/github/issues-pr/Autonomous-Robotics-Carleton/2026?color=purple&style=for-the-badge" />
  <img src="https://img.shields.io/badge/License-MIT-yellow.svg?style=for-the-badge" />
</p>

---

<p align="center">
  <img width="3308" height="1350" alt="ARC INSTA BANNER" src="https://github.com/user-attachments/assets/4b4c9acd-6771-452b-a9fa-c6f2cd5e4346" />
</p>

---

Welcome to the **monorepo** for **ARC (Autonomous Robotics Carleton)**!  
This project houses our public-facing website and documentation hub — everything related to building, configuring, and showcasing our **autonomous car** as we prepare for competitions.  

We’re keeping this project **fully open source**, so current and future members — and the wider robotics community — can learn, contribute, and grow with us.  

---

## 📖 What’s Inside  

This repository contains two apps:

### 🌐 `apps/web` — Public Website  
The ARC marketing and showcase site built with **Next.js 15**, **Tailwind CSS v4**, and **GSAP** animations.  
- 🏎 **Home page** — interactive hero, sponsors, and team highlights  
- 📐 **Blueprint page** — animated exploded-view car visualization  
- 👥 **Projects & team pages** — showcasing members and ongoing work  

### 📚 `apps/docs` — Documentation Hub  
The technical docs site built with **Fumadocs** + **Next.js**.  
- ✅ **Setup guides** — step-by-step instructions for getting the ARC car up and running.  
- 🛠 **Configuration docs** — details on software, hardware, and environment settings.  
- 📚 **Knowledge base** — collected learnings and resources as the project evolves.  
- 🏎 **Race preparation logs** — documenting our progress on the road to competition.  

---

## 🚀 Getting Started  

1. Clone this repository:  
   ```bash
   git clone https://github.com/Autonomous-Robotics-Carleton/2026.git
   cd 2026
---

This is an **Nx monorepo**. Documentation source code lives in **`apps/docs/`**.

---

# 🔧 Local Development (for Contributors)

Contributors **do NOT need Docker**.
Docker is used only in production via CI/CD.

---

## 📌 Prerequisites

* **Node.js 20+**
* **Git**

```bash
corepack enable        # activates pnpm (version pinned in package.json)
pnpm install           # installs all workspace dependencies
```

---

## ▶️ Run the Dev Servers

```bash
npx nx dev docs        # Docs site → http://localhost:3000
npx nx dev web         # Public website → http://localhost:3001
```

---

## 🧪 Lint + Build

```bash
# Docs
npx nx build docs
npx nx lint docs

# Website
npx nx build web
npx nx lint web
```

---

## 2️⃣ Make Your Changes

### Docs (`apps/docs`)

Content lives in:

```
apps/docs/content/
```

UI + logic:

```
apps/docs/app/
apps/docs/lib/
```

### Website (`apps/web`)

Pages live in:

```
apps/web/app/
```

Components + UI:

```
apps/web/components/
apps/web/lib/
```

---

## 3️⃣ Test Locally

```bash
pnpm install
npx nx dev docs
```
---

# 🧪 CI/CD Pipeline

This repository uses **GitHub Actions + GitHub Container Registry (GHCR)**.

### 🔹 For every Pull Request:

* Installs dependencies
* Lints the docs
* Builds the site
* Tests Docker build

### 🔹 For every merge to `main`:

* Builds the production Docker image
* Pushes it to GHCR:

  * `ghcr.io/autonomous-robotics-carleton/2026:latest`
  * `ghcr.io/autonomous-robotics-carleton/2026:<commit-sha>`
* ARC infrastructure auto-deploys the new version to **arcarleton.ca** via Watchtower

Contributors never touch Docker.

---

# 🏗 Project Structure

```
2026/                             # Nx monorepo root
├── apps/
│   ├── docs/                 # Fumadocs / Next.js docs app
│   │   ├── app/              # Next.js App Router
│   │   ├── content/          # MDX documentation pages
│   │   ├── public/           # Static assets
│   │   ├── lib/              # Utility functions
│   │   ├── components/       # React components
│   │   ├── next.config.mjs
│   │   └── package.json
│   └── web/                  # Next.js public website
│       ├── app/              # Next.js App Router (pages)
│       ├── components/       # UI + layout components
│       ├── lib/              # Animation utilities
│       ├── hooks/            # Custom React hooks
│       ├── public/           # Static assets (images, video)
│       ├── next.config.mjs
│       └── package.json
├── nx.json                   # Nx workspace config
├── pnpm-workspace.yaml       # pnpm workspace config
├── .github/workflows/        # CI & Docker build pipelines
└── README.md
```

---

# ➕ Adding a New App or Library

This repo uses **Nx** + **pnpm workspaces**. All apps live in `apps/`, shared code in `libs/`.

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
| `npx nx run-many -t build` | Build all projects |

---

# 📘 License

This project is licensed under the **MIT License**.

---

# 🎉 Thanks for Contributing!

Whether you're fixing typos, writing docs, or creating new tutorials —
**your work helps drive ARC forward.**

If you have questions, open an issue or reach out to the ARC Team!
