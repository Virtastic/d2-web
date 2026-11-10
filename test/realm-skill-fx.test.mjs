import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

// A realm game starts every unit's skill from the server's two skill records; the rules that
// decode them and choose the start's sounds and overlay are checked on fixed bytes.
test('skill start records, monster actions, hit overlays and start effects', (t) => {
  if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { t.skip('g++ unavailable'); return; }
  const dir = mkdtempSync(join(tmpdir(), 'd2-realm-skill-fx-'));
  try {
    const binary = join(dir, 'realm-skill-fx');
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', resolve('test/realm-skill-fx.cpp'), '-o', binary], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr || built.stdout);
    const run = spawnSync(binary, [], { encoding: 'utf8' });
    assert.equal(run.status, 0, run.stderr || run.stdout);
  } finally { rmSync(dir, { recursive: true, force: true }); }
});

test('one skill start serves the selected player, other players and monsters', () => {
  const src = readFileSync(resolve('src/core/realm/d2gs_client_state.cpp'), 'utf8');
  // Both server records go to one handler, and it and the local start end in the same routine.
  assert.match(src, /pkt_or<apply_skill_start_packet, -2> \},\s*\/\/ 0x4c/);
  assert.match(src, /static int pkt_skill_start_at\([^\n]*\n[^\n]*apply_skill_start_packet\(packet, size\)/);
  const calls = src.match(/[^\w]client_skill_start\(/g) ?? [];
  assert.ok(calls.length >= 3, `expected the definition and two callers, saw ${calls.length}`);
  for (const name of ['apply_skill_start_packet', 'local_skill_start']) {
    const from = src.indexOf(`${name}(`), body = src.slice(from, src.indexOf('\n}\n', from));
    assert.ok(from > 0 && /client_skill_start\(/.test(body), `${name} does not use the shared start`);
  }
  assert.doesNotMatch(src, /remote_unit_skill_start/);
});
