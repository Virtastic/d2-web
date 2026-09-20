import test from 'node:test';
import assert from 'node:assert/strict';
import { BELT_LAYOUT } from '../play/assets/belt-renderer.mjs';
test('retail four-slot starter belt remains anchored over the control panel', () => {
  assert.deepEqual(BELT_LAYOUT, { x: 342, y: 561, cells: 4, cell: 31 });
});
