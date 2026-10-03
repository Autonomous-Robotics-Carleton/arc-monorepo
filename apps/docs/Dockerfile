# --- Base stage: install deps with pnpm ---
FROM node:22-alpine AS deps
WORKDIR /app

RUN corepack enable

# Copy workspace config + app manifest for install
COPY package.json pnpm-workspace.yaml pnpm-lock.yaml ./
COPY apps/docs/package.json apps/docs/source.config.ts ./apps/docs/

RUN pnpm install --frozen-lockfile

# --- Build stage ---
FROM node:22-alpine AS builder
WORKDIR /app
ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1

RUN corepack enable

COPY --from=deps /app ./
COPY apps/docs ./apps/docs
# Pages the site renders from elsewhere in the repo (see source.config.ts)
COPY CONTRIBUTING.md ./
COPY systems ./systems
COPY platform/dev-kit ./platform/dev-kit
COPY apps/web/docs ./apps/web/docs

RUN pnpm --filter arc-docs build

# Deploy creates a standalone copy with real (not symlinked) node_modules
RUN pnpm --filter arc-docs deploy --legacy /deploy --prod
RUN cp -r /app/apps/docs/.next /deploy/.next
RUN cp -r /app/apps/docs/public /deploy/public

# --- Runtime stage ---
FROM node:22-alpine AS runner
WORKDIR /app
ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1
ENV PORT=3000

RUN addgroup -g 1001 nodejs \
  && adduser -u 1001 -G nodejs -s /bin/sh -D nextjs

COPY --from=builder /deploy ./

# fumadocs-mdx regenerates .source/ when `next start` runs
RUN mkdir .source && chown nextjs:nodejs .source

USER nextjs
EXPOSE 3000

CMD ["npm", "run", "start"]
