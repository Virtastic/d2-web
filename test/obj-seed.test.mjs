import { test } from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, writeFileSync, readFileSync, existsSync, utimesSync, readdirSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const root = join(import.meta.dirname, '..');
const lib = join(root, 'wasm-build/obj-seed.sh');
const seedInto = (seed, obj) => spawnSync('bash', ['-c', 'source "$1"; d2_obj_seed "$2" "$3"', 'x', lib, seed, obj], { encoding: 'utf8' });

// One object triple; the digest is written `lag` seconds after the object (negative = before it).
const triple = (dir, name, lag = 1, body = 'obj') => {
  writeFileSync(join(dir, `${name}.o`), body);
  writeFileSync(join(dir, `${name}.o.d`), 'dep');
  writeFileSync(join(dir, `${name}.o.inputs`), 'digest');
  utimesSync(join(dir, `${name}.o`), 1000, 1000);
  utimesSync(join(dir, `${name}.o.inputs`), 1000 + lag, 1000 + lag);
};
const dirs = () => {
  const t = mkdtempSync(join(tmpdir(), 'objseed-'));
  const seed = join(t, 'seed'), obj = join(t, 'obj');
  mkdirSync(seed); mkdirSync(obj);
  return { seed, obj };
};

test('an empty object dir takes every complete object of the seed, as copies', () => {
  const { seed, obj } = dirs();
  triple(seed, 'a'); triple(seed, 'b');
  const r = seedInto(seed, obj);
  assert.match(r.stdout, /seeded 2 objects/);
  for (const f of ['a.o', 'a.o.d', 'a.o.inputs', 'b.o', 'b.o.d', 'b.o.inputs']) assert.ok(existsSync(join(obj, f)), f);
  writeFileSync(join(obj, 'a.o'), 'rebuilt here');
  assert.equal(readFileSync(join(seed, 'a.o'), 'utf8'), 'obj', 'a compile in the new tree wrote into the seed');
});

test('an object the seed tree was still compiling is dropped, not trusted', () => {
  const { seed, obj } = dirs();
  triple(seed, 'done');
  triple(seed, 'midway', -5);          // object newer than its digest: a compile in flight
  writeFileSync(join(seed, 'nodigest.o'), 'x'); writeFileSync(join(seed, 'nodigest.o.d'), 'x');
  writeFileSync(join(seed, 'orphan.o.inputs'), 'x');
  seedInto(seed, obj);
  assert.deepEqual(readdirSync(obj).sort(), ['done.o', 'done.o.d', 'done.o.inputs']);
});

test('a dir that already holds objects, or a missing seed, is left alone', () => {
  const { seed, obj } = dirs();
  triple(seed, 'a'); triple(obj, 'mine');
  assert.equal(seedInto(seed, obj).stdout, '');
  assert.deepEqual(readdirSync(obj).sort(), ['mine.o', 'mine.o.d', 'mine.o.inputs']);
  const e = dirs();
  assert.equal(seedInto(join(e.seed, 'absent'), e.obj).status, 0);
  assert.deepEqual(readdirSync(e.obj), []);
});

test('the build mounts the main checkout objects and the link step seeds from them', () => {
  assert.match(readFileSync(join(root, 'ci/build.sh'), 'utf8'), /-v "\$LOCK_DIR\/obj:\/d2-obj-seed:ro"/);
  assert.match(readFileSync(join(root, 'wasm-build/link-d2.sh'), 'utf8'), /d2_obj_seed "\/d2-obj-seed\/\$\(basename "\$OBJDIR"\)" "\$OBJDIR"/);
});

test('the object dir is named by flags, compiler and compile recipe, never by the link script', () => {
  const objdir = readFileSync(join(root, 'wasm-build/link-d2.sh'), 'utf8').split('\n').find((l) => l.startsWith('OBJDIR='));
  assert.ok(objdir.includes('"$CXXFLAGS" "$TOOLCHAIN_KEY" "$(sha256sum < "$(dirname "$0")/probe-skip.sh")"'), objdir);
  assert.doesNotMatch(readFileSync(join(root, 'tools/compiler-cache.sh'), 'utf8'), /sha256sum[^\n]*link-d2\.sh/);
});
