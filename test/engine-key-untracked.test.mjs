// tools/engine-key.sh must move when an IGNORED engine source changes (2026-10-31).
//
// link-d2.sh compiles `find src -name '*.cpp'`, and src/core/fn/* is gitignored except the tracked
// probes. The key used `git status` and `ls-files --exclude-standard`, so an edited or newly added
// ignored probe left the key unchanged and ci/build.sh and the batch prover reused a stale
// engine ("engine unchanged"). This builds a throwaway repo with the same shape and pins that a
// change to an ignored probe changes the key, and that the key is clean again once it is removed.
import { test, after } from 'node:test';
import assert from 'node:assert/strict';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, mkdirSync, writeFileSync, rmSync, copyFileSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, dirname } from 'node:path';
import { fileURLToPath } from 'node:url';

const ROOT = join(dirname(fileURLToPath(import.meta.url)), '..');
const work = mkdtempSync(join(tmpdir(), 'd2-key-untracked-'));
after(() => rmSync(work, { recursive: true, force: true }));

const bin = join(work, 'bin');
mkdirSync(bin);
writeFileSync(join(bin, 'docker'), '#!/bin/bash\necho sha256:' + 'c'.repeat(64) + '\n', { mode: 0o755 });

const repo = join(work, 'repo');
const git = (...a) => execFileSync('git', ['-C', repo, ...a], { encoding: 'utf8' });
mkdirSync(join(repo, 'src/core/fn'), { recursive: true });
mkdirSync(join(repo, 'wasm-build'));
mkdirSync(join(repo, 'third_party'));
mkdirSync(join(repo, 'ci'));
mkdirSync(join(repo, 'tools'));
execFileSync('git', ['init', '-q', repo]);
for (const [f, body] of [
  ['.gitignore', 'src/core/fn/*\n!src/core/fn/tracked.cpp\n'],
  ['src/core/main.cpp', 'int main(){return 0;}\n'],
  ['src/core/fn/tracked.cpp', 'int tracked(){return 1;}\n'],
  ['wasm-build/link-d2.sh', 'echo link\n'],
  ['third_party/README', 'x\n'],
  ['ci/build.sh', 'echo build\n'],
  ['Dockerfile.builder64', 'FROM scratch\n'],
  ['tools/compiler-cache.sh', 'echo cc\n'],
]) writeFileSync(join(repo, f), body);
copyFileSync(join(ROOT, 'tools/engine-key.sh'), join(repo, 'tools/engine-key.sh'));
git('add', '-A');
git('-c', 'user.email=t@t', '-c', 'user.name=t', 'commit', '-qm', 'base');

const key = () => execFileSync('bash', [join(repo, 'tools/engine-key.sh'), repo], {
  encoding: 'utf8',
  env: { PATH: `${bin}:${process.env.PATH}`, HOME: work, BUILDER_IMAGE: 'test-image', PWD: repo },
}).trim();

test('engine-key: an edited ignored probe changes the key; a new one does too; removing it restores the clean key', () => {
  const clean = key();
  assert.doesNotMatch(clean, /-dirty$/, 'a committed tree with no ignored sources is clean');

  const probe = join(repo, 'src/core/fn/probe_1234.cpp');
  writeFileSync(probe, 'int probe(){return 1;}\n');
  const added = key();
  assert.match(added, /-dirty$/, 'an ignored probe is an uncommitted engine input');
  assert.notEqual(added, clean, 'a new ignored probe must move the key');

  writeFileSync(probe, 'int probe(){return 2;}\n');
  const edited = key();
  assert.notEqual(edited, added, 'editing an ignored probe must move the key');

  assert.equal(key(), edited, 'the same probe bytes give the same key');
  rmSync(probe);
  assert.equal(key(), clean, 'removing the probe restores the committed key');
});
