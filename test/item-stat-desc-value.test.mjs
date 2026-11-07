import test from 'node:test';
import assert from 'node:assert/strict';
import { spawnSync } from 'node:child_process';
import { mkdtempSync, readFileSync, rmSync } from 'node:fs';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';

// Item property lines show a stat's value after ItemStatCost's ValShift (D2Client+0x546E0):
// a +25 life charm holds 6400 internally and must read "+25 to Life", not "+6400 to Life".
test('descValue drops ValShift and applies op 2..5 scaling', (t) => {
  if (spawnSync('g++', ['--version'], { stdio: 'ignore' }).status !== 0) { t.skip('g++ unavailable'); return; }
  const dir = mkdtempSync(join(tmpdir(), 'd2-item-stat-desc-value-'));
  try {
    const binary = join(dir, 'item-stat-desc-value');
    const built = spawnSync('g++', ['-std=c++20', '-Wall', '-Wextra', '-Werror', resolve('test/item-stat-desc-value.cpp'), '-o', binary], { encoding: 'utf8' });
    assert.equal(built.status, 0, built.stderr || built.stdout);
    const run = spawnSync(binary, [], { encoding: 'utf8' });
    assert.equal(run.status, 0, run.stderr || run.stdout);
  } finally { rmSync(dir, { recursive: true, force: true }); }
});

test('the item property formatter takes its number from descValue', () => {
  const src = readFileSync(resolve('src/core/monspawn/monspawn_check.cpp'), 'utf8');
  const from = src.indexOf('auto fmt = [&](const D2ItemStatCostTxt* r');
  const fmt = src.slice(from, src.indexOf('std::map<int, std::vector<const D2StatStrc*>> groups;', from));
  assert.ok(from > 0 && fmt.length > 0, 'formatter not found');
  assert.match(fmt, /int32_t v = d2::item_text::descValue\(st\.nValue,/);
  assert.doesNotMatch(fmt, /int32_t v = st\.nValue;/);
});
