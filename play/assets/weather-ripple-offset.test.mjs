import test from 'node:test';
import assert from 'node:assert/strict';
import { createRippleRng } from './weather-ripple-offset.mjs';

test('ripple offsets follow the two-step diamond rule and are not a lattice', () => {
  // From lo=1, hi=0 the first step is exactly 0x6AC690C5, so the row is that value mod 80.
  assert.equal(createRippleRng(1, 0).offset().dy, 0x6ac690c5 % 80);
  const rng = createRippleRng(1, 0);
  const pts = Array.from({ length: 400 }, () => rng.offset());
  for (const p of pts) {
    const w = 80 - 2 * Math.abs(p.dy - 40);
    assert.ok(p.dy >= 0 && p.dy < 80 && p.dx >= -w && p.dx < Math.max(w, 1));
  }
  assert.ok(new Set(pts.map((p) => p.dx)).size > 60);
  assert.ok(new Set(pts.map((p) => p.dy)).size > 50);
  const again = createRippleRng(1, 0);
  assert.deepEqual(Array.from({ length: 400 }, () => again.offset()), pts);
});
