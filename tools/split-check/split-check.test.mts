import test from 'node:test';
import assert from 'node:assert/strict';
import * as fs from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { spawnSync } from 'node:child_process';
import { fileURLToPath } from 'node:url';
import { check, checkFile, escapeData, escapeProperty, glob } from './split-check.mts';
import * as rules from './rules.mts';

const flagged = (path: string, src: string) => checkFile(path, src, rules).map(v => v.line);

test('globs: * stays in a segment, ** crosses them', () => {
  assert.ok(glob('ros/src/arc_backend_*/**').test('ros/src/arc_backend_new/src/node.cpp'));
  assert.ok(!glob('ros/src/arc_backend_*/**').test('ros/src/arc_control/src/arc_backend_x.cpp'));
  assert.ok(glob('a/**/b.py').test('a/b.py'));
  assert.ok(glob('a/**/b.py').test('a/x/y/b.py'));
  assert.ok(!glob('a/*.py').test('a/x/b.py'));
});

test('Python: comparisons, conditions and tables keyed by target', () => {
  const src = [
    "if target == 'sim':", // 1
    "    pass",
    "x = 'gazebo' if world else 'gym'", // 3: a conditional expression
    "if is_sim:", // 4: an identifier
    "backend = {'sim': a, 'car': b}[target]", // 5: a table looked up by the target
    "LaunchConfigurationEquals('target', 'replay')", // 6
    "while mode != \"interface\":", // 7
  ].join('\n');
  assert.deepEqual(flagged('ros/src/arc_new/arc_new/node.py', src), [1, 3, 4, 5, 6, 7]);
});

test('tables keyed by target: a branch only when looked up by a value', () => {
  // dispatch tables: caught, on one line or several
  const dispatch = [
    "pkg = {'sim': a, 'car': b}[target]", // 1
    "pkg = {'sim': a, 'car': b}.get(target)", // 2
    'BACKENDS = {', // 3
    "    'gazebo': 'arc_backend_sim_gazebo',", // 4
    "    'gym': 'arc_backend_sim_gym',",
    '}',
    'pkg = BACKENDS[world]', // 7: no target name on this line, but the table is looked up
    'x = {', // 8
    "    'replay': a,", // 9
    '}.get(mode, None)',
  ].join('\n');
  assert.deepEqual(flagged('ros/src/arc_x/arc_x/node.py', dispatch), [1, 2, 4, 9]);
  assert.deepEqual(flagged('experiments/x/src/main.ts', "const pkg = { sim: a, car: b }[target];\nconst q = TABLE.get(k);"), [1]);
  assert.deepEqual(flagged('experiments/x/src/main.ts', "const T = {\n  'sim': a,\n};\nrun(T[target]);"), [2]);

  // fields that happen to be target names: not branches (#152's run record and launch arguments)
  const record = [
    'def run_record(target, sim, sensors, car, car_config_hash):',
    '    return {',
    "        'target': target,",
    "        'sim': sim,",
    "        'sensors': sensors,",
    "        'car': car,",
    '    }',
    'ARGS = {',
    "    'sim': ('', 'Run record: the simulator, if any'),",
    "    'car': ('twin', 'Run record: the car configuration'),",
    '}',
    'for name, (default, description) in ARGS.items():',
    '    pass',
    "x = ARGS['sim']", // indexed by a literal, not a value
    'y = [ARGS]',
  ].join('\n');
  assert.deepEqual(flagged('ros/src/arc_logging/arc_logging/recorder.py', record), []);
});

test('Python: comments, docstrings, loops, data and use_sim_time pass', () => {
  const src = [
    '"""Runs on every target:',
    "if target == 'sim' this docstring isn't code.",
    '"""',
    "# if target == 'car': a comment",
    "for sim in SIMS:",
    "config = {'world': 'gazebo'}", // data, not a branch
    "if node.get_parameter('use_sim_time').value:",
    "if use_sim_time:",
    "if car_config['wheelbase'] > 0:", // 'car' inside an identifier is an everyday word
    "print('the sim backend')",
  ].join('\n');
  assert.deepEqual(flagged('experiments/x/src/main.py', src), []);
});

test('C: board symbols in the preprocessor, string compares; features and comments pass', () => {
  const src = [
    '#ifdef CONFIG_BOARD_NATIVE_SIM', // 1
    '#endif',
    '#if IS_ENABLED(CONFIG_ARCH_POSIX)', // 3
    '#endif',
    'if (strcmp(target, "car") == 0) {', // 5
    '}',
    '#ifdef CONFIG_NET_SOCKETS', // a feature, not a target
    '#endif',
    '/* native_sim only: if (board == "native_sim") */',
    '// if (target == "sim")',
    'const char *s = "if sim";',
    'bool on = in_simulation ? a : b;', // 12
  ].join('\n');
  assert.deepEqual(flagged('firmware/sync-mcu/app/src/x.c', src), [1, 3, 5, 12]);
});

test('C++ and TypeScript', () => {
  assert.deepEqual(flagged('ros/src/arc_x/src/node.cpp', 'if (mode_ == "replay") return;\nauto t = "replay";'), [1]);
  const ts = "if (target === 'sim') go();\nconst b = { sim: a, car: c }[t];\nconst u = x?.sim ?? y;\n";
  assert.deepEqual(flagged('experiments/x/src/main.ts', ts), [1, 2]);
});

test('CMake and Kconfig', () => {
  const cmake = [
    '# west build -b native_sim firmware/sync-mcu/app',
    'if(BOARD STREQUAL "native_sim")', // 2
    'endif()',
    'if(CONFIG_BOARD_ARC_SYNC)', // 4
    'endif()',
    'if(BUILD_TESTING)',
    'target_sources(app PRIVATE $<$<STREQUAL:${BOARD},nucleo_h723zg>:x.c>)', // 7
  ].join('\n');
  assert.deepEqual(flagged('firmware/sync-mcu/app/CMakeLists.txt', cmake), [2, 4, 7]);
  const kconfig = ['config ARC_X', '\tbool "x"', '\tdepends on BOARD_NATIVE_SIM', '\tdefault y if SOC_STM32H723XX', '\tdefault "127.0.0.1"'].join('\n');
  assert.deepEqual(flagged('firmware/sync-mcu/app/Kconfig', kconfig), [3, 4]);
});

test('shell and Dockerfiles', () => {
  const sh = [
    '# native_sim firmware',
    'fw=firmware/sync-mcu/build/native_sim/zephyr/zephyr.exe',
    'if [ "$TARGET" = car ]; then', // 3
    'fi',
    'case "$world" in',
    '  gazebo) run ;;', // 6
    'esac',
    '[[ $board == *nucleo_h723zg* ]] && flash', // 8
    'if [ -f car.yaml ]; then echo; fi',
  ].join('\n');
  assert.deepEqual(flagged('platform/scripts/start.sh', sh), [3, 6, 8]);
  assert.deepEqual(flagged('experiments/x/Dockerfile', 'RUN if [ "$TARGET" = sim ]; then x; fi'), [1]);
});

test('launch files in XML and YAML', () => {
  const xml = "<launch>\n<!-- if=\"$(eval '$(var target)' == 'sim')\" -->\n<group if=\"$(eval '$(var target)' == 'sim')\">\n</group>\n</launch>";
  assert.deepEqual(flagged('ros/src/arc_x/launch/x.launch.xml', xml), [3]);
  const yaml = "launch:\n- group:\n    if: \"$(eval '$(var target)' == 'car')\"\n    name: car\n";
  assert.deepEqual(flagged('ros/src/arc_x/launch/x.launch.yaml', yaml), [3]);
});

test('allowed paths: backends (new ones too), bringup, board files; new packages are checked', () => {
  const src = "if target == 'sim':\n    pass";
  for (const path of ['ros/src/arc_backend_car/x.py', 'ros/src/arc_backend_sim_new/x.py', 'ros/src/arc_bringup/launch/platform.launch.py', 'firmware/sync-mcu/boards/arc/arc_sync/x.py', 'tools/sim/x.py', 'firmware/vesc/bldc/x.py']) {
    assert.deepEqual(flagged(path, src), [], path);
  }
  for (const path of ['ros/src/arc_logging/arc_logging/node.py', 'ros/src/arc_backend.py', 'experiments/x/src/main.py', 'newdir/x.py']) {
    assert.deepEqual(flagged(path, src), [1], path);
  }
  assert.deepEqual(flagged('ros/src/arc_x/README.md', src), [], 'not code');
});

test('test code is skipped; the code it tests is still checked', () => {
  const src = "assert topics == {'sim': '/sim/scan'}\nif target == 'car':\n    pass";
  for (const path of ['ros/src/arc_logging/test/test_recorder.py', 'firmware/sync-mcu/tests/safety/src/main.c', 'test/x.py', 'ros/src/arc_x/arc_x/test_node.py', 'experiments/x/src/node_test.cpp']) {
    assert.deepEqual(flagged(path, src), [], path);
  }
  for (const path of ['ros/src/arc_logging/arc_logging/recorder.py', 'ros/src/arc_x/arc_x/testing.py', 'ros/src/arc_x/arc_x/latest.py']) {
    assert.deepEqual(flagged(path, src), [1, 2], path);
  }
});

test('annotations escape the file path and message', () => {
  assert.equal(escapeProperty('a%b,c:d\r\ne'), 'a%25b%2Cc%3Ad%0D%0Ae');
  assert.equal(escapeData('50% done: a,b\n'), '50%25 done: a,b%0A');
});

test('exceptions excuse the line they name; unmatched ones are reported as stale', () => {
  const r = { ...rules, EXCEPTIONS: [
    { path: 'a/x.py', contains: "== 'car'", followUp: '#1' },
    { path: 'a/y.py', contains: "== 'sim'", followUp: '#2' },
    { path: 'a/not-checked.py', contains: 'x', followUp: '#3' },
  ] };
  const result = check([
    { path: 'a/x.py', src: "if t == 'car':\n  pass\nif t == 'sim':\n  pass" },
    { path: 'a/y.py', src: 'pass' },
  ], r);
  assert.deepEqual(result.excused.map(v => [v.path, v.line]), [['a/x.py', 1]]);
  assert.deepEqual(result.violations.map(v => [v.path, v.line]), [['a/x.py', 3]]);
  assert.deepEqual(result.stale.map(e => e.path), ['a/y.py']);
});

test('the command: exit codes, file list from git, GitHub annotations', async t => {
  const root = await fs.mkdtemp(join(tmpdir(), 'arc-split-check-'));
  t.after(() => fs.rm(root, { recursive: true, force: true }));
  const script = fileURLToPath(new URL('./split-check.mts', import.meta.url));
  const run = (...args: string[]) => spawnSync(process.execPath, [script, '--root', root, ...args], { encoding: 'utf8', env: { ...process.env, GITHUB_ACTIONS: 'true' } });
  spawnSync('git', ['init', '-q'], { cwd: root });
  await fs.mkdir(join(root, 'ros/src/arc_new'), { recursive: true });
  await fs.writeFile(join(root, 'ros/src/arc_new/node.py'), 'if world:\n    pass\n');
  await fs.writeFile(join(root, '.gitignore'), 'ignored.py\n');
  await fs.writeFile(join(root, 'ignored.py'), "if target == 'sim':\n    pass\n");

  let r = run();
  assert.equal(r.status, 0, r.stderr);
  assert.match(r.stdout, /no violations; 1 files checked/);

  await fs.writeFile(join(root, 'ros/src/arc_new/node.py'), "if world:\n    pass\nelif target == 'car':\n    pass\n");
  r = run();
  assert.equal(r.status, 1);
  assert.match(r.stderr, /ros\/src\/arc_new\/node\.py:3: branches on the target \(car\)/);
  assert.match(r.stdout, /::error file=ros\/src\/arc_new\/node\.py,line=3,/);

  await fs.mkdir(join(root, 'experiments/a,b'), { recursive: true });
  await fs.writeFile(join(root, 'experiments/a,b/main.py'), "if target == 'sim':\n    pass\n");
  r = run('experiments/a,b/main.py');
  assert.match(r.stdout, /::error file=experiments\/a%2Cb\/main\.py,line=1,/);

  r = run('ignored.py');
  assert.equal(r.status, 1, 'explicit paths are checked even if git ignores them');
  assert.equal(run('--bogus').status, 2);
});
