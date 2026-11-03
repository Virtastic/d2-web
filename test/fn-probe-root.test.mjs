// The FN probe gate resolves headers in the checkout being committed, even when its checker is a symlink into another
// store (the private store links tools/re into each checkout). A checker resolved from its own realpath checks the store.
import test from 'node:test';
import assert from 'node:assert';
import { execFileSync, spawnSync } from 'node:child_process';
import fs from 'node:fs';
import os from 'node:os';
import path from 'node:path';
import { fileURLToPath } from 'node:url';

const CHECKER = fileURLToPath(new URL('../tools/re/check-fn-probes.mjs', import.meta.url));
const SOURCE = fs.existsSync(CHECKER) ? fs.realpathSync(CHECKER) : null;

function git(cwd, args) { return execFileSync('git', args, { cwd, encoding: 'utf8', stdio: ['ignore', 'pipe', 'pipe'] }); }

// A repo whose tools/re/check-fn-probes.mjs is a symlink into a separate store directory, as in a linked checkout.
function makeLinkedRepo(root, sources) {
  const store = path.join(root, 'store');
  const repo = path.join(root, 'repo');
  fs.mkdirSync(path.join(store, 'tools/re'), { recursive: true });
  fs.copyFileSync(SOURCE, path.join(store, 'tools/re/check-fn-probes.mjs'));
  fs.mkdirSync(path.join(repo, 'tools/re'), { recursive: true });
  fs.symlinkSync(path.join(store, 'tools/re/check-fn-probes.mjs'), path.join(repo, 'tools/re/check-fn-probes.mjs'));
  fs.mkdirSync(path.join(repo, 'src/core/d2client'), { recursive: true });
  for (const [name, body] of Object.entries(sources)) fs.writeFileSync(path.join(repo, 'src/core/d2client', name), body);
  git(repo, ['init', '-q']);
  git(repo, ['add', 'src']);
  return repo;
}

function runStaged(repo) {
  const env = { ...process.env };
  delete env.CHECK_FN_ROOT; // the hook sets it; the bare script must find the checkout by itself
  return spawnSync(process.execPath, [path.join(repo, 'tools/re/check-fn-probes.mjs'), '--staged'], { cwd: repo, encoding: 'utf8', env });
}

test('fn-probe gate: a header next to the probe in a linked checkout resolves (not "not found")', { skip: SOURCE ? false : 'tools/re is private; checker not linked here' }, (t) => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'fnprobe-root-'));
  t.after(() => fs.rmSync(root, { recursive: true, force: true }));
  const repo = makeLinkedRepo(root, {
    'h.h': 'int value();\n',
    'c.cpp': '#include "h.h"\nint value() { return 1; }\n',
  });
  const r = runStaged(repo);
  assert.strictEqual(r.status, 0, `expected clean, got: ${r.stderr}`);
  assert.match(r.stdout, /2 file\(s\) clean/);
});

test('fn-probe gate: a missing header still fails in a linked checkout', { skip: SOURCE ? false : 'tools/re is private; checker not linked here' }, (t) => {
  const root = fs.mkdtempSync(path.join(os.tmpdir(), 'fnprobe-root-'));
  t.after(() => fs.rmSync(root, { recursive: true, force: true }));
  const repo = makeLinkedRepo(root, {
    'c.cpp': '#include "nope.h"\nint value() { return 1; }\n',
  });
  const r = runStaged(repo);
  assert.strictEqual(r.status, 1);
  assert.match(r.stderr, /#include "nope\.h" not found/);
});
