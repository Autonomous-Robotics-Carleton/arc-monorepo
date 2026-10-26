# --- Base stage: install deps with pnpm ---
FROM node:22-alpine AS deps
WORKDIR /app

RUN corepack enable

# Copy workspace config + app manifest for install
COPY package.json pnpm-workspace.yaml pnpm-lock.yaml ./
COPY apps/web/package.json ./apps/web/

RUN pnpm install --frozen-lockfile --filter arc-web

# --- Build stage ---
FROM node:22-alpine AS builder
WORKDIR /app
ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1

RUN corepack enable

COPY --from=deps /app ./
COPY apps/web ./apps/web

RUN pnpm --filter arc-web build

# Deploy creates a standalone copy with real (not symlinked) node_modules
RUN pnpm --filter arc-web deploy --legacy /deploy --prod
RUN cp -r /app/apps/web/.next /deploy/.next
RUN cp -r /app/apps/web/public /deploy/public

# --- Runtime stage ---
FROM node:22-alpine AS runner
WORKDIR /app
ENV NODE_ENV=production
ENV NEXT_TELEMETRY_DISABLED=1
ENV PORT=3000

RUN addgroup -g 1001 nodejs \
  && adduser -u 1001 -G nodejs -s /bin/sh -D nextjs

COPY --from=builder --chown=nextjs:nodejs /deploy ./

USER nextjs
EXPOSE 3000

CMD ["npm", "run", "start"]
