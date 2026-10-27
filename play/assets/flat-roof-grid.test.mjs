import assert from 'node:assert/strict';
import { flatRoofGrid } from './session-terrain.mjs';
for (const a of [0, 133, 146]) {
  const g = flatRoofGrid(a);
  assert.deepEqual([...g], new Array(64).fill(a));
  assert.equal(g.key, new Array(64).fill(a).join(','));
}
assert.equal(flatRoofGrid(146), flatRoofGrid(146));
console.log('flat-roof-grid ok');
