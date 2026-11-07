import test from 'node:test';
import assert from 'node:assert/strict';
import { readFileSync } from 'node:fs';
// Band count is call overhead only: pin the rule so a shadow's pixels never depend on it.
const src = readFileSync(new URL('../play/assets/unit-shadow-renderer.mjs', import.meta.url), 'utf8');
test('shadow readback bands: one per 8 rows, between 1 and 4', () => {
  assert.match(src, /BANDS = Math\.min\(4, Math\.max\(1, rows >> 3\)\)/);
  const bands = (rows) => Math.min(4, Math.max(1, rows >> 3));
  assert.deepEqual([1, 7, 8, 16, 24, 31, 32, 99].map(bands), [1, 1, 1, 2, 3, 3, 4, 4]);
});
