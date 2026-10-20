import { test } from 'node:test';
import { mkdtempSync, writeFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
import { execFileSync } from 'node:child_process';

test('EH3 native registration, stack bounds, page identity and PE validation', () => {
  const dir = mkdtempSync(join(tmpdir(), 'd2-eh3-'));
  try {
    writeFileSync(join(dir, 'emscripten.h'), '#define EMSCRIPTEN_KEEPALIVE\n');
    execFileSync('c++', ['-std=c++20', '-Wall', '-Wextra', '-Werror',
      '-fsanitize=undefined', `-I${dir}`, 'test/native/d2game-eh3rn.cpp', '-o', join(dir, 'check')]);
    execFileSync(join(dir, 'check'));
  } finally { rmSync(dir, { recursive: true, force: true }); }
});
