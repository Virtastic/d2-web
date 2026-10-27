import test from 'node:test';
import assert from 'node:assert/strict';
import { hireThumbOffset } from '../play/assets/npc-menu.mjs';

test('hire window scroll thumb follows the scroll position', () => {
  assert.equal(hireThumbOffset(0, 14, 10), 0);
  const mid = hireThumbOffset(3, 14, 10), end = hireThumbOffset(7, 14, 10);
  assert.ok(mid > 0 && end > mid);
  assert.equal(hireThumbOffset(0, 5, 21), 0);
});
