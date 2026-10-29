// The split check (ADR-0047) against code written to get past it, and code
// it must leave alone. Written apart from split-check.test.mts's worked
// cases: these fail when a rule is dropped or an allowed path grows over code
// that runs on the car.
import test from 'node:test';
import assert from 'node:assert/strict';
import { allowedBy, checkFile } from './split-check.mts';
import * as rules from './rules.mts';

const flags = (path: string, src: string) => checkFile(path, src, rules).length > 0;

// Written out here, not read from rules.mts, so dropping one from the rules fails.
// Targets, worlds and modes: ADR-0047 and systems/software/simulation.md; boards: ADR-0047.
const TARGETS = ['sim', 'devhw', 'car', 'gazebo', 'webots', 'gym', 'replay', 'interface',
  'native_sim', 'nucleo_h723zg', 'arc_sync'];
const WORDS = ['sim', 'simulation', 'simulated', 'simulator', 'devhw', 'gazebo', 'webots', 'gym',
  'replay', 'nucleo'];

// Code above the split: every one of these is checked.
const ABOVE = [
  'ros/src/arc_control/arc_control/node.py',
  'ros/src/arc_logging/arc_logging/node.py',
  'ros/src/arc_safety/src/watchdog.cpp',
  'ros/src/arc_new_package/launch/x.launch.py',
  'firmware/sync-mcu/app/src/main.c',
  'firmware/sync-mcu/app/CMakeLists.txt',
  'firmware/vesc/arc/src/estop.c',
  'platform/services/start.sh',
  'experiments/lap/src/policy.py',
];

test('code above the split is never in an allowed path', () => {
  for (const path of ABOVE) assert.equal(allowedBy(path, rules), undefined, path);
});

test('every target name is caught in a comparison', () => {
  for (const name of TARGETS) {
    assert.ok(flags(ABOVE[0], `if target == '${name}':`), `python '${name}'`);
    assert.ok(flags('ros/src/arc_safety/src/watchdog.cpp', `if (target_ == "${name}") {`), `C++ "${name}"`);
    assert.ok(flags('platform/services/start.sh', `if [ "$TARGET" = ${name} ]; then`), `shell ${name}`);
  }
});

test('every identifier word is caught in a condition', () => {
  for (const word of WORDS) {
    assert.ok(flags(ABOVE[0], `if is_${word}:`), `is_${word}`);
    const camel = word[0].toUpperCase() + word.slice(1);
    assert.ok(flags('experiments/x/src/main.ts', `if (on${camel}) {}`), `on${camel}`);
  }
});

test('ways of asking for the target that are caught', () => {
  const caught: [string, string][] = [
    ['firmware/sync-mcu/app/src/x.c', '#ifdef CONFIG_BOARD_NATIVE_SIM'],
    ['firmware/sync-mcu/app/src/x.c', '#if defined(CONFIG_BOARD_ARC_SYNC)'],
    ['firmware/sync-mcu/app/src/x.c', 'if (IS_ENABLED(CONFIG_BOARD_NUCLEO_H723ZG)) {'],
    ['firmware/sync-mcu/app/src/x.c', '#ifdef CONFIG_ARCH_POSIX'],
    ['firmware/sync-mcu/app/src/x.c', 'if (strcmp(CONFIG_BOARD, "native_sim") == 0) {'],
    ['firmware/sync-mcu/app/CMakeLists.txt', 'if(BOARD STREQUAL native_sim)'],
    ['firmware/sync-mcu/app/CMakeLists.txt', 'if(CONFIG_BOARD_ARC_SYNC)'],
    ['firmware/sync-mcu/app/Kconfig', '\tdepends on BOARD_NATIVE_SIM'],
    [ABOVE[0], "if os.environ.get('ARC_TARGET') == 'car':"],
    [ABOVE[0], "if self.get_parameter('target').value in ('sim', 'replay'):"],
    [ABOVE[0], "match target:\n    case 'car':\n        pass"],
    [ABOVE[0], "backend = {'gazebo': a, 'webots': b}.get(world)"],
    ['ros/src/arc_safety/src/watchdog.cpp', 'const bool on_car = get_parameter("target").as_string() == "car";'],
    ['ros/src/arc_safety/src/watchdog.cpp', 'auto t = target_ == "gym" ? fast : slow;'],
    ['ros/src/arc_new_package/launch/x.launch.py',
      "condition=IfCondition(PythonExpression([\"'\", LaunchConfiguration('target'), \"' == 'car'\"]))"],
    ['ros/src/arc_new_package/launch/x.launch.xml', '<node pkg="a" exec="b" if="$(eval \'$(var target)\' == \'car\')"/>'],
    ['platform/services/start.sh', 'case "$TARGET" in sim) ;; esac'],
    ['platform/services/start.sh', '[[ $ARC_TARGET == car ]] && start'],
  ];
  for (const [path, src] of caught) assert.ok(flags(path, src), `${path}: ${src}`);
});

test('code that only looks like it asks is left alone', () => {
  const clean: [string, string][] = [
    [ABOVE[0], "self.declare_parameter('use_sim_time', False)"],
    [ABOVE[0], '# runs the same in sim and on the car'],
    [ABOVE[0], 'car_config = load(car)'],
    [ABOVE[0], "log.info('the car interface is up')"],
    [ABOVE[0], 'gymnasium_like = 1'],
    [ABOVE[0], "record = {'target': target, 'car': car}"],
    ['ros/src/arc_safety/src/watchdog.cpp', '// "sim" in a comment'],
    ['ros/src/arc_safety/src/watchdog.cpp', 'similar = simple + 1;'],
    ['ros/src/arc_safety/src/watchdog.cpp', 'if (car_speed > limit) {'],
  ];
  for (const [path, src] of clean) assert.ok(!flags(path, src), `${path}: ${src}`);
});

test('places ADR-0047 allows: board files, configurations, backends, bringup, tests', () => {
  const allowed = [
    'firmware/sync-mcu/boards/arc/arc_sync/board.c',
    'firmware/sync-mcu/app/boards/native_sim.conf',
    'config/cars/nominal.yaml',
    'ros/src/arc_backend_car/src/backend.cpp',
    'ros/src/arc_bringup/launch/platform.launch.py',
    'ros/src/arc_control/test/test_node.py',
  ];
  for (const path of allowed) assert.notEqual(allowedBy(path, rules), undefined, path);
  // and a target branch there passes
  assert.ok(!flags('ros/src/arc_bringup/launch/platform.launch.py', "if target == 'car':"));
});

test('known gap: a target name inside a longer string', { todo: 'startswith("/sim") and similar aren\'t caught; flagging them would also flag every /sim topic name' }, () => {
  assert.ok(flags(ABOVE[0], "if node.get_namespace().startswith('/sim'):"));
});
