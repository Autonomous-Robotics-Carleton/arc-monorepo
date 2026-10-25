#!/usr/bin/env node
// Import local Altium 365 source checkouts/snapshots without touching Git history.
import { createHash, randomUUID } from 'node:crypto';
import { spawnSync } from 'node:child_process';
import * as fs from 'node:fs/promises';
import { existsSync } from 'node:fs';
import { dirname, isAbsolute, relative, resolve, sep } from 'node:path';
import { fileURLToPath } from 'node:url';

export const projects = {
  motherboard: { name: 'ARC-Motherboard', directory: 'hardware/motherboard/altium', guid: '435E9B62-9567-4537-918E-7E4772E07F96' },
  angle: { name: 'ARC-Sensor-Angle-AS5047P', directory: 'hardware/sensor-boards/angle/altium', guid: 'B0EECA79-5052-4095-B203-15090846669F' },
  imu: { name: 'ARC-Sensor-IMU-ISM330DHCX', directory: 'hardware/sensor-boards/imu/altium', guid: '2A72933B-BA15-4B7E-AB7B-3331F4D6727C' },
  tof: { name: 'ARC-Sensor-ToF', directory: 'hardware/sensor-boards/tof/altium', guid: '1CE3083D-9559-41D0-BFD7-DBE654C9101D' },
} as const;
export type ProjectId = keyof typeof projects;
export const workspace = 'https://americas-carleton-university-autonomous-robotics-carle.365.altium.com';
const manifestName = 'altium-sync.json';
const protectedNames = new Set(['readme.md', '.gitattributes', manifestName]);
const digest = (data: Uint8Array) => createHash('sha256').update(data).digest('hex');
type Files = Map<string, { path: string; hash: string }>;
type Manifest = { version: 1; project: ProjectId; source: string; revision?: string; files: Record<string, string> };
type Change = { action: 'add' | 'update' | 'delete'; file: string; before: string | null; after: string | null };
export type Plan = { id: ProjectId; source: string; destination: string; files: Files; manifest: Manifest; previous: Manifest | null; manifestBefore: string | null; changes: Change[]; requireClean: boolean };
export type Config = { version: 1; projects: Partial<Record<ProjectId, string>> };

function fail(message: string): never { throw new Error(message); }
function inside(root: string, path: string): boolean {
  const r = relative(root, path);
  return r === '' || (!isAbsolute(r) && r !== '..' && !r.startsWith(`..${sep}`));
}
function safeRelative(value: string): string {
  const path = value.replaceAll('\\', '/');
  if (!path || path.startsWith('/') || /^[A-Za-z]:/.test(path) || path.includes('\0') || path.split('/').some(p => !p || p === '.' || p === '..' || /[<>:"|?*]/.test(p) || /[. ]$/.test(p) || /^(?:con|prn|aux|nul|com[1-9]|lpt[1-9])(?:\.|$)/i.test(p))) {
    fail(`Unsafe relative path: ${JSON.stringify(value)}`);
  }
  return path;
}
async function read(path: string): Promise<Buffer | null> {
  try { return await fs.readFile(path); } catch (error) {
    if ((error as NodeJS.ErrnoException).code === 'ENOENT') return null;
    throw error;
  }
}
async function noSymlinks(root: string, path: string): Promise<void> {
  if (!inside(root, path)) fail(`Path escapes permitted directory: ${path}`);
  for (const part of [root, ...relative(root, path).split(sep).filter(Boolean).map((_, i, parts) => resolve(root, ...parts.slice(0, i + 1)))]) {
    try { if ((await fs.lstat(part)).isSymbolicLink()) fail(`Symlink/junction is not a sync target: ${part}`); }
    catch (error) { if ((error as NodeJS.ErrnoException).code !== 'ENOENT') throw error; }
  }
}
async function noCaseRename(root: string, path: string): Promise<void> {
  let directory = root;
  for (const part of relative(root, path).split(sep)) {
    let names: string[];
    try { names = await fs.readdir(directory); } catch (error) {
      if ((error as NodeJS.ErrnoException).code === 'ENOENT') return;
      throw error;
    }
    const match = names.find(name => name.toLowerCase() === part.toLowerCase());
    if (match && match !== part) fail(`Case-only destination rename needs reconciliation: ${resolve(directory, match)} -> ${part}`);
    directory = resolve(directory, part);
  }
}
function excluded(parts: string[]): boolean {
  const lower = parts.map(p => p.toLowerCase());
  const name = lower.at(-1)!;
  return lower.some(p => p.startsWith('.') || p === 'history' || p === '__previews' || p === '__history' || /^project (outputs|logs)(?: |$)/.test(p)) ||
    ['thumbs.db', 'desktop.ini'].includes(name) || /(?:\.bak|\.log|\.lck|\.lock|\.tmp|\.prjpcbstructure|\.prjpcbautosave|\.schdocpreview|\.pcbdocpreview|\.\$\$\$|~)$/.test(name);
}
async function collect(source: string): Promise<Files> {
  const files: Files = new Map();
  const folded = new Set<string>();
  let bytes = 0;
  async function walk(directory: string, parts: string[]): Promise<void> {
    for (const entry of await fs.readdir(directory, { withFileTypes: true })) {
      const pathParts = [...parts, entry.name];
      if (excluded(pathParts)) continue;
      const name = safeRelative(pathParts.join('/'));
      if (parts.length === 0 && protectedNames.has(name.toLowerCase())) continue;
      if (entry.isSymbolicLink()) fail(`Source contains a symlink/junction: ${name}`);
      if (entry.isDirectory()) { await walk(resolve(directory, entry.name), pathParts); continue; }
      if (!entry.isFile()) fail(`Source contains a non-regular file: ${name}`);
      if (/\.(?:pem|key|pfx|p12|credentials)$/i.test(name)) fail(`Source contains credential/key material: ${name}`);
      if (folded.has(name.toLowerCase())) fail(`Case-colliding source path: ${name}`);
      folded.add(name.toLowerCase());
      const path = resolve(directory, entry.name);
      const stat = await fs.stat(path);
      bytes += stat.size;
      if (files.size >= 20_000 || bytes > 1024 ** 3) fail('Source exceeds 20,000 files or 1 GiB; check that the project folder is correct');
      const data = (await read(path))!;
      if (data.subarray(0, 100).toString('utf8').startsWith('version https://git-lfs.github.com/spec/v1')) fail(`Source has an unresolved Git LFS pointer: ${name}`);
      files.set(name, { path, hash: digest(data) });
    }
  }
  await walk(source, []);
  return files;
}
function decodeProject(data: Buffer): string {
  if (data[0] === 0xff && data[1] === 0xfe) return data.subarray(2).toString('utf16le');
  return data.toString('utf8').replace(/^\uFEFF/, '');
}
async function verifyProject(id: ProjectId, files: Files): Promise<void> {
  const allProjects = [...files].filter(([name]) => /\.prjpcb$/i.test(name));
  if (allProjects.length !== 1 || allProjects[0][0].includes('/')) fail(`${id}: expected exactly one PCB project at the source folder root`);
  const entry = allProjects[0];
  const text = decodeProject((await read(entry[1].path))!);
  // Desktop checkouts identify their cloud project. An unlinked source snapshot
  // may legitimately have a blank GUID; its config mapping then selects the board.
  for (const match of text.matchAll(/^ManagedProjectGUID[ \t]*=[ \t]*(.*)$/gim)) {
    const guid = match[1].trim().replace(/^\{(.*)\}$/, '$1').toUpperCase();
    if (guid && guid !== projects[id].guid) fail(`${id}: source belongs to cloud project ${guid}, expected ${projects[id].guid}; check the source mapping`);
  }
  const references = [...text.matchAll(/^DocumentPath=(.+)$/gm)].map(m => safeRelative(m[1].trim().replace(/^"(.*)"$/, '$1')));
  if (!references.some(p => /\.schdoc$/i.test(p)) || !references.some(p => /\.pcbdoc$/i.test(p))) fail(`${id}: project must reference a schematic and PCB document`);
  const names = new Set([...files.keys()].map(name => name.toLowerCase()));
  for (const name of references) if (!names.has(name.toLowerCase())) fail(`${id}: missing or excluded project document ${name}`);
  // Cloud display-name changes need not rename the local .PrjPcb. Keep one stable
  // monorepo entrypoint while preserving the project contents and document paths.
  files.delete(entry[0]);
  files.set(`${projects[id].name}.PrjPcb`, entry[1]);
  const originalStem = entry[0].replace(/\.prjpcb$/i, '');
  const variants = [...files].find(([name]) => name.toLowerCase() === `${originalStem}.PrjPcbVariants`.toLowerCase());
  if (variants) {
    const canonicalName = `${projects[id].name}.PrjPcbVariants`;
    if ([...files.keys()].some(name => name !== variants[0] && name.toLowerCase() === canonicalName.toLowerCase())) fail(`${id}: renamed variant sidecar collides with ${canonicalName}`);
    files.delete(variants[0]); files.set(canonicalName, variants[1]);
  }
}
async function sourceRevision(source: string, requireClean: boolean): Promise<string | undefined> {
  if (!requireClean) return undefined;
  // An extracted snapshot inside this monorepo is not an Altium Git checkout.
  if (!existsSync(resolve(source, '.git'))) fail('--require-clean needs a Git checkout; an extracted snapshot has no Git status');
  const status = spawnSync('git', ['-C', source, 'status', '--porcelain=v1', '--untracked-files=normal'], { encoding: 'utf8', shell: false });
  if (status.status !== 0) fail(`Cannot inspect Altium source checkout: ${String(status.stderr || status.error?.message || 'Git inspection failed').trim()}`);
  if (status.stdout.trim()) fail('Altium source checkout has uncommitted changes. Save to Server, update it, then retry; sync imports saved revisions only.');
  const revision = spawnSync('git', ['-C', source, 'rev-parse', 'HEAD'], { encoding: 'utf8', shell: false });
  if (revision.status !== 0 || !/^[0-9a-f]{40,64}$/i.test(revision.stdout.trim())) fail('Cannot identify saved source checkout revision');
  return revision.stdout.trim();
}
function parseManifest(data: Buffer, id: ProjectId): Manifest {
  const value = JSON.parse(data.toString('utf8')) as Manifest;
  if (value.version !== 1 || value.project !== id || !value.files || Array.isArray(value.files) || typeof value.files !== 'object') fail(`${id}: invalid sync manifest`);
  const seen = new Set<string>();
  for (const [name, hash] of Object.entries(value.files)) {
    safeRelative(name);
    if (protectedNames.has(name.toLowerCase()) || excluded(name.split('/')) || seen.has(name.toLowerCase()) || typeof hash !== 'string' || !/^[0-9a-f]{64}$/.test(hash)) fail(`${id}: unsafe sync manifest entry ${name}`);
    seen.add(name.toLowerCase());
  }
  return value;
}

export async function planSync(rootPath: string, config: Config, configDirectory: string, adoptExisting = false, requireClean = false): Promise<Plan[]> {
  const root = await fs.realpath(rootPath);
  if (config.version !== 1 || !config.projects || typeof config.projects !== 'object' || Array.isArray(config.projects) || !Object.keys(config.projects).length) fail('Config must have version 1 and a nonempty projects mapping');
  const plans: Plan[] = [];
  for (const [key, sourceValue] of Object.entries(config.projects)) {
    if (!Object.hasOwn(projects, key) || typeof sourceValue !== 'string' || !sourceValue) fail(`Unknown project or invalid source directory: ${key}`);
    const id = key as ProjectId;
    const source = await fs.realpath(resolve(configDirectory, sourceValue));
    const destination = resolve(root, projects[id].directory);
    if (inside(source, destination) || inside(destination, source)) fail(`${id}: source and destination overlap`);
    await noSymlinks(root, destination);
    const revision = await sourceRevision(source, requireClean);
    const files = await collect(source);
    await verifyProject(id, files);
    const manifestPath = resolve(destination, manifestName);
    await noSymlinks(destination, manifestPath);
    const previousBytes = await read(manifestPath);
    const previous = previousBytes ? parseManifest(previousBytes, id) : null;
    const changes: Change[] = [];
    const allNames = [...new Set([...files.keys(), ...Object.keys(previous?.files ?? {})])].sort();
    const seenNames = new Set<string>();
    for (const file of allNames) {
      if (seenNames.has(file.toLowerCase())) fail(`${id}/${file}: case-only rename needs reconciliation before syncing`);
      seenNames.add(file.toLowerCase());
      const target = resolve(destination, file);
      await noSymlinks(destination, target);
      await noCaseRename(destination, target);
      const existing = await read(target);
      const before = existing ? digest(existing) : null;
      const after = files.get(file)?.hash ?? null;
      const baseline = previous?.files[file] ?? null;
      if (before === after) continue;
      if (baseline !== null && before !== baseline) fail(`${id}/${file}: local edit/deletion differs from the last sync; preserve or reconcile it before syncing`);
      if (baseline === null && before !== null && (!adoptExisting || previous !== null)) fail(`${id}/${file}: existing file is not managed by sync. Review it, then use --adopt-existing for the first import (a backup is kept).`);
      changes.push({ action: after === null ? 'delete' : before === null ? 'add' : 'update', file, before, after });
    }
    const manifest: Manifest = {
      version: 1, project: id, source: `${workspace}/designs/${projects[id].guid}#design`,
      ...(revision ? { revision } : {}), files: Object.fromEntries([...files].sort(([a], [b]) => a.localeCompare(b)).map(([name, file]) => [name, file.hash])),
    };
    plans.push({ id, source, destination, files, manifest, previous, manifestBefore: previousBytes ? digest(previousBytes) : null, changes, requireClean });
  }
  return plans;
}

const serialized = (manifest: Manifest) => `${JSON.stringify(manifest, null, 2)}\n`;
// Target mutations are injectable so tests can reproduce partial writes and
// restoration errors without changing how backups and validation are performed.
type MutationIO = { writeFile(path: string, data: Buffer): Promise<void>; unlink(path: string): Promise<void> };
const filesystemMutations: MutationIO = { writeFile: (path, data) => fs.writeFile(path, data), unlink: path => fs.unlink(path) };
export async function applySync(rootPath: string, plans: Plan[], mutationIO: MutationIO = filesystemMutations): Promise<string | null> {
  const root = await fs.realpath(rootPath);
  const control = resolve(root, '.altium-sync');
  await noSymlinks(root, control);
  await fs.mkdir(control, { recursive: true });
  const lockPath = resolve(control, 'lock');
  let lock: fs.FileHandle;
  try { lock = await fs.open(lockPath, 'wx'); } catch { fail('Another Altium sync is running, or .altium-sync/lock is stale. Check before removing the lock.'); }
  const run = resolve(control, 'backups', `${new Date().toISOString().replaceAll(':', '-')}-${randomUUID()}`);
  const operations: { path: string; before: Buffer | null; after: Buffer | null; backup: string }[] = [];
  let syncFailure: unknown;
  try {
    // Read and validate every source/destination before changing any CAD file.
    for (const plan of plans) {
      if (plan.destination !== resolve(root, projects[plan.id].directory)) fail('Plan targets another repository');
      await noSymlinks(root, plan.destination);
      if (await sourceRevision(plan.source, plan.requireClean) !== plan.manifest.revision) fail(`${plan.id}: source revision changed during sync; retry`);
      for (const [file, item] of plan.files) {
        await noSymlinks(plan.source, item.path);
        if (digest((await read(item.path)) ?? Buffer.alloc(0)) !== item.hash) fail(`${plan.id}/${file}: source changed during sync; retry after saving`);
      }
      for (const change of plan.changes) {
        const path = resolve(plan.destination, safeRelative(change.file));
        await noSymlinks(plan.destination, path);
        const before = await read(path);
        if ((before ? digest(before) : null) !== change.before) fail(`${plan.id}/${change.file}: destination changed during sync; retry`);
        const after = change.after ? await read(plan.files.get(change.file)!.path) : null;
        if ((after ? digest(after) : null) !== change.after) fail(`${plan.id}/${change.file}: source changed during sync; retry`);
        operations.push({ path, before, after, backup: resolve(run, plan.id, change.file) });
      }
      const path = resolve(plan.destination, manifestName);
      const before = await read(path);
      if ((before ? digest(before) : null) !== plan.manifestBefore) fail(`${plan.id}: sync manifest changed; retry`);
      const after = Buffer.from(serialized(plan.manifest));
      if (!before?.equals(after)) operations.push({ path, before, after, backup: resolve(run, plan.id, manifestName) });
    }
    if (!operations.length) return null;
    await noSymlinks(control, run);
    await fs.mkdir(run, { recursive: true });
    // Backups and transaction record survive a failed/aborted run.
    for (const operation of operations) if (operation.before) {
      await fs.mkdir(dirname(operation.backup), { recursive: true });
      await fs.writeFile(operation.backup, operation.before, { flag: 'wx' });
    }
    await fs.writeFile(resolve(run, 'transaction.json'), `${JSON.stringify(operations.map(o => ({ destination: relative(root, o.path).replaceAll('\\', '/'), existed: o.before !== null })), null, 2)}\n`, { flag: 'wx' });
    const attempted: typeof operations = [];
    try {
      for (const operation of operations) {
        // A rejected write can still have truncated or partially written its
        // target. Include that operation in rollback before the first mutation.
        attempted.push(operation);
        if (operation.after) { await fs.mkdir(dirname(operation.path), { recursive: true }); await mutationIO.writeFile(operation.path, operation.after); }
        else await mutationIO.unlink(operation.path);
      }
    } catch (error) {
      const restorationErrors: Error[] = [];
      for (const operation of attempted.reverse()) {
        try {
          if (operation.before) await mutationIO.writeFile(operation.path, operation.before);
          else {
            try { await mutationIO.unlink(operation.path); }
            catch (restoreError) { if ((restoreError as NodeJS.ErrnoException).code !== 'ENOENT') throw restoreError; }
          }
        } catch (restoreError) {
          restorationErrors.push(new Error(`Could not restore ${relative(root, operation.path)}: ${(restoreError as Error).message}`, { cause: restoreError }));
        }
      }
      if (restorationErrors.length) throw new AggregateError([error, ...restorationErrors], `Sync failed and ${restorationErrors.length} file(s) could not be restored. Recovery copies: ${relative(root, run)}`, { cause: error });
      throw error;
    }
    return run;
  } catch (error) {
    syncFailure = error;
    throw error;
  } finally {
    const cleanupErrors: Error[] = [];
    try { await lock.close(); } catch (error) { cleanupErrors.push(new Error(`Could not close sync lock: ${(error as Error).message}`, { cause: error })); }
    try { await fs.unlink(lockPath); } catch (error) { cleanupErrors.push(new Error(`Could not remove ${relative(root, lockPath)}: ${(error as Error).message}`, { cause: error })); }
    if (cleanupErrors.length) {
      const recovery = existsSync(run) ? ` Recovery copies: ${relative(root, run)}` : '';
      throw new AggregateError(syncFailure === undefined ? cleanupErrors : [syncFailure, ...cleanupErrors], `${syncFailure === undefined ? 'Sync completed, but lock cleanup failed.' : `${(syncFailure as Error).message} Lock cleanup also failed.`}${recovery}`, { cause: syncFailure });
    }
  }
}

export async function main(args: string[]): Promise<void> {
  let configPath = '.altium-sync/config.json';
  let apply = false, dryRun = false, adopt = false, requireClean = false;
  for (let i = 0; i < args.length; i++) {
    if (args[i] === '--apply') apply = true;
    else if (args[i] === '--dry-run') dryRun = true;
    else if (args[i] === '--adopt-existing') adopt = true;
    else if (args[i] === '--require-clean') requireClean = true;
    else if (args[i] === '--config' && args[i + 1]) configPath = args[++i];
    else if (args[i] === '--help') {
      console.log('Usage: node tools/altium-sync/sync.mts [--config .altium-sync/config.json] [--dry-run | --apply] [--adopt-existing] [--require-clean]\nDefault is a preview. Read tools/altium-sync/README.md before the first import. No Git staging, commits, pushes, credentials or remote operations.');
      return;
    } else fail(`Unknown or incomplete option: ${args[i]}`);
  }
  if (apply && dryRun) fail('Choose --apply or --dry-run');
  const root = resolve(dirname(fileURLToPath(import.meta.url)), '../..');
  const absoluteConfig = resolve(root, configPath);
  const config = JSON.parse(await fs.readFile(absoluteConfig, 'utf8')) as Config;
  const plans = await planSync(root, config, dirname(absoluteConfig), adopt, requireClean);
  for (const plan of plans) {
    console.log(`${projects[plan.id].name}: ${plan.files.size} source files, ${plan.changes.length} change(s)${plan.manifest.revision ? `, source ${plan.manifest.revision.slice(0, 12)}` : ', local source files (Update/Save to Server in Altium first)'}`);
    for (const change of plan.changes) console.log(`  ${change.action.padEnd(6)} ${projects[plan.id].directory}/${change.file}`);
    if (!plan.previous) console.log('  create source/file manifest');
  }
  if (!apply) { console.log('\nPreview only. Add --apply to write these files; Git is unchanged.'); return; }
  const backup = await applySync(root, plans);
  console.log(backup ? `\nSynced. Recovery copies: ${relative(root, backup)}\nReview git diff/status, then commit and push when ready.` : '\nAlready synchronized. Git is unchanged.');
}

if (process.argv[1] && resolve(process.argv[1]) === fileURLToPath(import.meta.url)) {
  main(process.argv.slice(2)).catch(error => { console.error(`altium-sync: ${error.message}`); process.exitCode = 1; });
}
