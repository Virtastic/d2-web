import { test } from 'node:test';
import { execFileSync } from 'node:child_process';
import { mkdtempSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';
test('Storm break-chain registration preserves list, mode, message identity and region dependencies', () => {
  const dir = mkdtempSync(join(tmpdir(), 'storm-event-'));
  try {
    const binary = join(dir, 'check');
    execFileSync('c++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', '-pedantic',
      '-isystem', 'src/core/d2moo/compat', 'test/native/storm-event-3b1e0.cpp', 'src/core/storm/storm_349e0.cpp', '-o', binary]);
    execFileSync(binary);
  } finally { rmSync(dir, { recursive: true, force: true }); }
});
