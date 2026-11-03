import test from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync, spawnSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, writeFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const tree = (files) => {
  const root = mkdtempSync(join(tmpdir(), 'link-check-'));
  mkdirSync(join(root, 'src/core'), { recursive: true });
  for (const [n, body] of Object.entries(files)) writeFileSync(join(root, 'src/core', n), body);
  return root;
};
const run = (root, ...a) => spawnSync(process.execPath, ['tools/link-check.mjs', '--root', root, '--no-baseline', ...a], { encoding: 'utf8' });

test('clean tree passes', () => {
  const r = run(tree({ 'a.cpp': 'extern "C" int f(int x) { return x; }\n', 'b.cpp': 'extern "C" int f(int);\nint g() { return f(1); }\n' }));
  assert.equal(r.status, 0, r.stdout);
});

test('duplicate definition is refused', () => {
  const r = run(tree({ 'a.cpp': 'extern "C" int dup(void) { return 1; }\n', 'b.cpp': 'extern "C" int dup(void) { return 2; }\n' }));
  assert.equal(r.status, 1);
  assert.match(r.stdout, /LINK-DUP.*dup/);
});

test('extern C vs C++ linkage mismatch is refused', () => {
  const r = run(tree({ 'a.cpp': 'int fx(int a) { return a; }\n', 'b.cpp': 'extern "C" int fx(int);\nint h() { return fx(1); }\n' }));
  assert.equal(r.status, 1);
  assert.match(r.stdout, /LINK-LINKAGE.*fx/);
});

test('undefined data reference is refused', () => {
  const r = run(tree({ 'a.cpp': 'extern int g_missing_var;\nint h() { return g_missing_var; }\n' }));
  assert.equal(r.status, 1);
  assert.match(r.stdout, /LINK-UNDEF.*g_missing_var/);
});

test('a #define rename keeps a vendored copy from counting as a duplicate', () => {
  const r = run(tree({ 'a.cpp': 'extern "C" int ren(void) { return 1; }\n', 'b.cpp': '#define ren ren_vendored\nextern "C" int ren(void) { return 2; }\n' }));
  assert.equal(r.status, 0, r.stdout);
});

test('baseline accepts a known finding and still refuses a new one', () => {
  const root = tree({ 'a.cpp': 'extern int g_missing_var;\nint h() { return g_missing_var; }\n' });
  const w = spawnSync(process.execPath, ['tools/link-check.mjs', '--root', root, '--write-baseline'], { encoding: 'utf8' });
  assert.equal(w.status, 0, w.stderr);
  mkdirSync(join(root, 'tools'), { recursive: true });
  writeFileSync(join(root, 'tools/link-check.baseline'), w.stdout);
  assert.equal(spawnSync(process.execPath, ['tools/link-check.mjs', '--root', root], { encoding: 'utf8' }).status, 0);
  writeFileSync(join(root, 'src/core/c.cpp'), 'extern "C" int d(void) { return 1; }\n');
  writeFileSync(join(root, 'src/core/e.cpp'), 'extern "C" int d(void) { return 2; }\n');
  assert.equal(spawnSync(process.execPath, ['tools/link-check.mjs', '--root', root], { encoding: 'utf8' }).status, 1);
});

test('the real tree has no duplicate definitions', () => {
  const r = spawnSync(process.execPath, ['tools/link-check.mjs'], { encoding: 'utf8' });
  assert.equal(r.status, 0, r.stdout.slice(0, 2000));
});

test('quarantine names only engine objects outside the keep list', () => {
  const log = join(mkdtempSync(join(tmpdir(), 'quar-')), 'l.log');
  writeFileSync(log, [
    'wasm-ld: error: undefined symbol: a',
    '>>> referenced by build-wasm/obj/ab/src_core_other_x.o:(f)',
    'wasm-ld: error: undefined symbol: b',
    '>>> referenced by build-wasm/obj/ab/src_core_mine_y.o:(f)',
  ].join('\n'));
  const out = execFileSync('bash', ['-c', `. wasm-build/probe-skip.sh; D2_QUARANTINE_KEEP="src/core/mine_y.cpp" probe_engine_objs_quarantinable ${log}`], { encoding: 'utf8' });
  assert.equal(out, 'src_core_other_x.o\n');
});
