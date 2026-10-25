import test from 'node:test';
import assert from 'node:assert/strict';
import * as fs from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join, resolve, relative, isAbsolute, basename } from 'node:path';
import { spawnSync } from 'node:child_process';
import { applySync, planSync, projects, type Config } from './sync.mts';

async function fixture(t: test.TestContext) {
  const root = await fs.mkdtemp(join(tmpdir(), 'arc-altium-sync-'));
  t.after(async () => {
    // The only recursive deletion is this newly created, verified test directory.
    const rel = relative(resolve(tmpdir()), resolve(root));
    assert.ok(!isAbsolute(rel) && rel && !rel.startsWith('..') && basename(root).startsWith('arc-altium-sync-'));
    await fs.rm(root, { recursive: true, force: true });
  });
  const source = join(root, 'source');
  const destination = join(root, projects.angle.directory);
  await fs.mkdir(source);
  await fs.writeFile(join(source, 'Original Name.PrjPcb'), '[Design]\n[Document1]\nDocumentPath=Sheet.SchDoc\n[Document2]\nDocumentPath=Board.PcbDoc\n');
  await fs.writeFile(join(source, 'Sheet.SchDoc'), 'schematic version one');
  await fs.writeFile(join(source, 'Board.PcbDoc'), 'board version one');
  const config: Config = { version: 1, projects: { angle: source } };
  return { root, source, destination, config, plan: (adopt = false, clean = false) => planSync(root, config, root, adopt, clean) };
}

test('preview is read-only; import preserves repo docs and includes source assets', async t => {
  const f = await fixture(t);
  await fs.mkdir(f.destination, { recursive: true });
  await fs.writeFile(join(f.destination, 'README.md'), 'repo instructions');
  await fs.writeFile(join(f.destination, '.gitattributes'), 'repo lfs attributes');
  await fs.writeFile(join(f.source, 'README.md'), 'source notes');
  for (const directory of ['images', '.git', 'History', '__Previews', 'Project Outputs for Original Name']) await fs.mkdir(join(f.source, directory));
  await fs.writeFile(join(f.source, 'images', 'logo.png'), 'image asset');
  await fs.writeFile(join(f.source, 'Original Name.PrjPcbVariants'), 'alternate-part symbol data');
  for (const name of ['Sheet.SchDocPreview', 'Board.PcbDocPreview', 'temporary.$$$']) await fs.writeFile(join(f.source, name), 'skip');
  for (const directory of ['.git', 'History', '__Previews', 'Project Outputs for Original Name']) await fs.writeFile(join(f.source, directory, 'skip.txt'), 'skip');
  const plans = await f.plan();
  assert.equal(plans[0].files.size, 5);
  assert.deepEqual((await fs.readdir(f.destination)).sort(), ['.gitattributes', 'README.md']);
  assert.ok(!await fs.stat(join(f.root, '.altium-sync')).catch(() => null));
  await applySync(f.root, plans);
  assert.equal(await fs.readFile(join(f.destination, 'README.md'), 'utf8'), 'repo instructions');
  assert.equal(await fs.readFile(join(f.destination, '.gitattributes'), 'utf8'), 'repo lfs attributes');
  assert.equal(await fs.readFile(join(f.destination, 'images', 'logo.png'), 'utf8'), 'image asset');
  assert.ok(await fs.stat(join(f.destination, `${projects.angle.name}.PrjPcb`)));
  assert.equal(await fs.readFile(join(f.destination, `${projects.angle.name}.PrjPcbVariants`), 'utf8'), 'alternate-part symbol data');
  assert.ok(!await fs.stat(join(f.destination, 'Original Name.PrjPcb')).catch(() => null));
  assert.equal((await f.plan())[0].changes.length, 0);
  assert.equal(await applySync(f.root, await f.plan()), null);
});

test('updates and previously owned deletions are backed up; unknown files survive', async t => {
  const f = await fixture(t);
  await fs.writeFile(join(f.source, 'support.csv'), 'old support data');
  await applySync(f.root, await f.plan());
  await fs.writeFile(join(f.destination, 'local-note.txt'), 'keep me');
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'schematic version two');
  await fs.unlink(join(f.source, 'support.csv'));
  const plans = await f.plan();
  assert.deepEqual(plans[0].changes.map(c => [c.action, c.file]), [['update', 'Sheet.SchDoc'], ['delete', 'support.csv']]);
  const backup = await applySync(f.root, plans);
  assert.equal(await fs.readFile(join(backup!, 'angle', 'Sheet.SchDoc'), 'utf8'), 'schematic version one');
  assert.equal(await fs.readFile(join(backup!, 'angle', 'support.csv'), 'utf8'), 'old support data');
  assert.equal(await fs.readFile(join(f.destination, 'local-note.txt'), 'utf8'), 'keep me');
  assert.ok(!await fs.stat(join(f.destination, 'support.csv')).catch(() => null));
});

test('local edits and local deletions cannot be overwritten by later sync', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  await fs.writeFile(join(f.destination, 'Sheet.SchDoc'), 'my unsaved work');
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'cloud change');
  await assert.rejects(f.plan(), /local edit\/deletion/);
  await assert.rejects(f.plan(true), /local edit\/deletion/);
  await fs.writeFile(join(f.destination, 'Sheet.SchDoc'), 'schematic version one');
  await fs.unlink(join(f.destination, 'Sheet.SchDoc'));
  await assert.rejects(f.plan(), /local edit\/deletion/);
});

test('first adoption needs explicit opt-in and saves replaced files', async t => {
  const f = await fixture(t);
  await fs.mkdir(f.destination, { recursive: true });
  await fs.writeFile(join(f.destination, 'Sheet.SchDoc'), 'previous scaffold');
  await assert.rejects(f.plan(), /--adopt-existing/);
  const backup = await applySync(f.root, await f.plan(true));
  assert.equal(await fs.readFile(join(backup!, 'angle', 'Sheet.SchDoc'), 'utf8'), 'previous scaffold');
});

test('missing documents, traversal, overlap, credentials and LFS pointers fail before writes', async t => {
  const f = await fixture(t);
  const project = join(f.source, 'Original Name.PrjPcb');
  const original = await fs.readFile(project);
  await fs.writeFile(project, '[Document1]\nDocumentPath=../outside.SchDoc\n[Document2]\nDocumentPath=Board.PcbDoc\n');
  await assert.rejects(f.plan(), /Unsafe relative path/);
  await fs.writeFile(project, '[Document1]\nDocumentPath=Missing.SchDoc\n[Document2]\nDocumentPath=Board.PcbDoc\n');
  await assert.rejects(f.plan(), /missing or excluded/);
  await fs.writeFile(project, original);
  await fs.writeFile(join(f.source, 'private.key'), 'do not copy');
  await assert.rejects(f.plan(), /credential\/key/);
  await fs.unlink(join(f.source, 'private.key'));
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'version https://git-lfs.github.com/spec/v1\noid sha256:123\n');
  await assert.rejects(f.plan(), /unresolved Git LFS pointer/);
  await assert.rejects(planSync(f.root, { version: 1, projects: { angle: f.destination } }, f.root), /ENOENT|overlap/);
  assert.ok(!await fs.stat(f.destination).catch(() => null));
});

test('managed source identity must match the configured cloud project', async t => {
  const f = await fixture(t);
  const project = join(f.source, 'Original Name.PrjPcb');
  const original = await fs.readFile(project, 'utf8');
  await fs.writeFile(project, `${original}ManagedProjectGUID=${projects.imu.guid}\n`);
  await assert.rejects(f.plan(), /source belongs to cloud project.*check the source mapping/);
  await fs.writeFile(project, `${original}ManagedProjectGUID={${projects.angle.guid.toLowerCase()}}\n`);
  assert.equal((await f.plan()).length, 1);
  await fs.writeFile(project, `${original}ManagedProjectGUID=\n`);
  assert.equal((await f.plan()).length, 1);
});

test('normalizing a project name cannot overwrite another variant sidecar', async t => {
  const f = await fixture(t);
  await fs.writeFile(join(f.source, 'Original Name.PrjPcbVariants'), 'source variants');
  await fs.writeFile(join(f.source, `${projects.angle.name}.PrjPcbVariants`), 'different variants');
  await assert.rejects(f.plan(), /variant sidecar collides/);
  assert.ok(!await fs.stat(f.destination).catch(() => null));
});

test('source junctions and destination junctions cannot redirect writes', async t => {
  const f = await fixture(t);
  const external = join(f.root, 'outside');
  await fs.mkdir(external);
  await fs.symlink(external, join(f.source, 'assets'), process.platform === 'win32' ? 'junction' : 'dir');
  await assert.rejects(f.plan(), /symlink\/junction/);
  await fs.unlink(join(f.source, 'assets'));
  await fs.mkdir(join(f.root, 'hardware', 'sensor-boards', 'angle'), { recursive: true });
  await fs.symlink(external, f.destination, process.platform === 'win32' ? 'junction' : 'dir');
  await assert.rejects(f.plan(), /Symlink\/junction/);
  assert.deepEqual(await fs.readdir(external), []);
});

test('tampered deletion manifest is rejected; source change invalidates a preview', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  const manifestPath = join(f.destination, 'altium-sync.json');
  const manifest = JSON.parse(await fs.readFile(manifestPath, 'utf8'));
  manifest.files['../outside.txt'] = 'a'.repeat(64);
  await fs.writeFile(manifestPath, JSON.stringify(manifest));
  await assert.rejects(f.plan(), /Unsafe relative path/);
  delete manifest.files['../outside.txt'];
  await fs.writeFile(manifestPath, JSON.stringify(manifest));
  const plans = await f.plan();
  await fs.writeFile(join(f.source, 'Board.PcbDoc'), 'new source while sync was running');
  await assert.rejects(applySync(f.root, plans), /source changed during sync/);
  assert.equal(await fs.readFile(join(f.destination, 'Board.PcbDoc'), 'utf8'), 'board version one');
});

test('case-only destination names and redirected backup folders are rejected', async t => {
  const f = await fixture(t);
  await fs.mkdir(f.destination, { recursive: true });
  await fs.writeFile(join(f.destination, 'sheet.SchDoc'), 'case variant');
  await assert.rejects(f.plan(true), /Case-only destination rename/);
  await fs.unlink(join(f.destination, 'sheet.SchDoc'));
  const plans = await f.plan();
  const control = join(f.root, '.altium-sync');
  const outside = join(f.root, 'outside');
  await fs.mkdir(control);
  await fs.mkdir(outside);
  await fs.symlink(outside, join(control, 'backups'), process.platform === 'win32' ? 'junction' : 'dir');
  await assert.rejects(applySync(f.root, plans), /Symlink\/junction/);
  assert.deepEqual(await fs.readdir(outside), []);
  assert.deepEqual(await fs.readdir(f.destination), []);
});

test('a partially failed overwrite is restored along with earlier writes', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  const oldManifest = await fs.readFile(join(f.destination, 'altium-sync.json'));
  await fs.writeFile(join(f.source, 'Board.PcbDoc'), 'board version two');
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'schematic version two');
  await fs.writeFile(join(f.source, 'Added.txt'), 'new support asset');
  const writeError = new Error('simulated disk error after partial schematic write');
  let failed = false;
  const io = {
    writeFile: async (path: string, data: Buffer) => {
      if (!failed && path === join(f.destination, 'Sheet.SchDoc')) {
        failed = true;
        await fs.writeFile(path, data.subarray(0, 5));
        throw writeError;
      }
      await fs.writeFile(path, data);
    },
    unlink: (path: string) => fs.unlink(path),
  };
  await assert.rejects(applySync(f.root, await f.plan(), io), error => error === writeError);
  assert.equal(await fs.readFile(join(f.destination, 'Board.PcbDoc'), 'utf8'), 'board version one');
  assert.equal(await fs.readFile(join(f.destination, 'Sheet.SchDoc'), 'utf8'), 'schematic version one');
  assert.ok(!await fs.stat(join(f.destination, 'Added.txt')).catch(() => null));
  assert.deepEqual(await fs.readFile(join(f.destination, 'altium-sync.json')), oldManifest);
  assert.ok(!await fs.stat(join(f.root, '.altium-sync', 'lock')).catch(() => null));
  await applySync(f.root, await f.plan());
  assert.equal(await fs.readFile(join(f.destination, 'Sheet.SchDoc'), 'utf8'), 'schematic version two');
});

test('a partially created file is removed when its write rejects', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  await fs.writeFile(join(f.source, 'Added.txt'), 'new support asset');
  const writeError = new Error('simulated partial file creation');
  const io = {
    writeFile: async (path: string, data: Buffer) => {
      if (path === join(f.destination, 'Added.txt')) { await fs.writeFile(path, data.subarray(0, 3)); throw writeError; }
      await fs.writeFile(path, data);
    },
    unlink: (path: string) => fs.unlink(path),
  };
  await assert.rejects(applySync(f.root, await f.plan(), io), error => error === writeError);
  assert.ok(!await fs.stat(join(f.destination, 'Added.txt')).catch(() => null));
  assert.equal((await f.plan())[0].changes.length, 1);
});

test('rollback continues after a restoration failure and reports original and recovery errors', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  const backupRoot = join(f.root, '.altium-sync', 'backups');
  const previousRuns = new Set(await fs.readdir(backupRoot));
  const oldManifest = await fs.readFile(join(f.destination, 'altium-sync.json'));
  await fs.writeFile(join(f.source, 'Board.PcbDoc'), 'board version two');
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'schematic version two');
  await fs.writeFile(join(f.source, 'Added.txt'), 'new support asset');
  const writeError = new Error('simulated write failure');
  const restoreError = new Error('simulated restoration failure');
  let failed = false;
  const restored: string[] = [];
  const io = {
    writeFile: async (path: string, data: Buffer) => {
      if (failed) { restored.push(basename(path)); if (path === join(f.destination, 'Sheet.SchDoc')) throw restoreError; }
      if (!failed && path === join(f.destination, 'Sheet.SchDoc')) {
        failed = true;
        await fs.writeFile(path, data.subarray(0, 5));
        throw writeError;
      }
      await fs.writeFile(path, data);
    },
    unlink: async (path: string) => { if (failed) restored.push(basename(path)); await fs.unlink(path); },
  };
  await assert.rejects(applySync(f.root, await f.plan(), io), error => {
    assert.ok(error instanceof AggregateError);
    assert.equal(error.errors[0], writeError);
    assert.equal(error.errors.length, 2);
    assert.equal(error.errors[1].cause, restoreError);
    assert.match(error.errors[1].message, /Sheet\.SchDoc/);
    assert.match(error.message, /1 file\(s\) could not be restored\. Recovery copies:/);
    return true;
  });
  assert.deepEqual(restored, ['Sheet.SchDoc', 'Board.PcbDoc', 'Added.txt']);
  assert.equal(await fs.readFile(join(f.destination, 'Board.PcbDoc'), 'utf8'), 'board version one');
  assert.ok(!await fs.stat(join(f.destination, 'Added.txt')).catch(() => null));
  assert.deepEqual(await fs.readFile(join(f.destination, 'altium-sync.json')), oldManifest);
  const newRuns = (await fs.readdir(backupRoot)).filter(name => !previousRuns.has(name));
  assert.equal(newRuns.length, 1);
  assert.equal(await fs.readFile(join(backupRoot, newRuns[0], 'angle', 'Sheet.SchDoc'), 'utf8'), 'schematic version one');
});

test('lock cleanup failure cannot hide the original sync failure or recovery location', async t => {
  const f = await fixture(t);
  await applySync(f.root, await f.plan());
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'schematic version two');
  const writeError = new Error('simulated original write failure');
  let failed = false;
  const lockPath = join(f.root, '.altium-sync', 'lock');
  const io = {
    writeFile: async (path: string, data: Buffer) => {
      if (!failed && path === join(f.destination, 'Sheet.SchDoc')) {
        failed = true;
        await fs.rename(lockPath, `${lockPath}.moved-by-test`);
        await fs.mkdir(lockPath);
        throw writeError;
      }
      await fs.writeFile(path, data);
    },
    unlink: (path: string) => fs.unlink(path),
  };
  await assert.rejects(applySync(f.root, await f.plan(), io), error => {
    assert.ok(error instanceof AggregateError);
    assert.equal(error.errors[0], writeError);
    assert.equal(error.cause, writeError);
    assert.match(error.message, /original write failure.*Lock cleanup also failed.*Recovery copies:/);
    assert.match(error.errors[1].message, /Could not remove/);
    return true;
  });
  assert.equal(await fs.readFile(join(f.destination, 'Sheet.SchDoc'), 'utf8'), 'schematic version one');
});

test('--require-clean rejects dirty Git checkouts and records clean source HEAD', async t => {
  const f = await fixture(t);
  const git = (...args: string[]) => {
    const result = spawnSync('git', ['-C', f.source, ...args], { encoding: 'utf8', shell: false });
    assert.equal(result.status, 0, result.stderr);
    return result.stdout.trim();
  };
  git('init', '-q');
  await assert.rejects(f.plan(false, true), /uncommitted changes/);
  git('add', '.');
  git('-c', 'user.name=ARC Sync Test', '-c', 'user.email=sync-test@example.invalid', '-c', 'commit.gpgsign=false', 'commit', '-qm', 'fixture');
  const plans = await f.plan(false, true);
  assert.equal(plans[0].manifest.revision, git('rev-parse', 'HEAD'));
  await fs.writeFile(join(f.source, 'Sheet.SchDoc'), 'dirty checkout');
  await assert.rejects(applySync(f.root, plans), /uncommitted changes/);
});
