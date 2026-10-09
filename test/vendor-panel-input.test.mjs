import test from 'node:test';
import assert from 'node:assert/strict';
import { vendorButtonHitRect } from '../play/assets/vendor-panel.mjs';

test('vendor action hit regions follow the recovered 1.13c client bounds', () => {
  const slots = [195, 248, 300, 352].map((x) => vendorButtonHitRect({ buttonX: [x] }, 0));
  assert.deepEqual(slots[0], { x: 197, y: 432, w: 44, h: 43 });
  assert.deepEqual(slots[1], { x: 250, y: 432, w: 44, h: 43 });
  assert.deepEqual(slots[2], { x: 302, y: 432, w: 44, h: 43 });
  assert.deepEqual(slots[3], { x: 354, y: 432, w: 44, h: 43 });
  const hit = (rect, x, y) => x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;
  assert.equal(hit(slots[0], 197, 432), true); // first accepted integer coordinate
  assert.equal(hit(slots[0], 240, 474), true); // last accepted integer coordinate
  assert.equal(hit(slots[0], 196, 432), false); // native x bound is strict
  assert.equal(hit(slots[0], 241, 474), false);
  assert.equal(hit(slots[0], 197, 431), false); // native y bound is strict
  assert.equal(hit(slots[0], 197, 475), false);
  assert.equal(hit(slots[1], 250, 450), true);
  assert.equal(hit(slots[3], 354, 450), true);
  assert.equal(hit(slots[3], 398, 450), false);
});
