import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

test('quest NPC message tables name the 1.13c speakers and menu flags', (t) => {
  if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { t.skip('g++ unavailable'); return; }
  const dir = mkdtempSync(join(tmpdir(), 'quest-npc-message-tables-'));
  try {
    const binary = join(dir, 'check');
    for (const [command, args] of [
      ['g++', ['-std=c++20', '-w', ...["-Wno-unused-parameter", "-fms-extensions", "-fwrapv", "-Wno-volatile", "-Wno-attributes", "-Wno-unknown-pragmas", "-Wno-invalid-offsetof", "-Wno-unused-value", "-Wno-multichar","-ffunction-sections", "-fdata-sections", "-Isrc", "-Isrc/core/d2moo/compat", "-Isrc/core/d2moo/include", "-Isrc/core/d2moo/include/D2Game", "-Isrc/core/d2moo/squall", "-Itest/native/stub", "-Wl,--gc-sections"], 'test/native/quest-npc-message-tables.cpp', '-o', binary]],
      [binary, []],
    ]) {
      const result = spawnSync(command, args, { encoding: 'utf8' });
      assert.equal(result.status, 0, result.stdout + result.stderr);
    }
  } finally { rmSync(dir, { recursive: true, force: true }); }
});
