import test from 'node:test';
import assert from 'node:assert/strict';
import { labelItemAt, nearestGroundItem, pickupNeedsWalk } from '../play/item-pick.mjs';

const it = (id, x, y, mode = 3) => ({ id, x, y, mode, invPage: 255 });
const ev = [{ id: 1, box: { x: 0, y: 0, width: 50, height: 10 } }, { id: 2, box: { x: 40, y: 0, width: 50, height: 10 } }];

test('topmost (last drawn) label wins the overlap', () => {
  assert.equal(labelItemAt(ev, [it(1, 0, 0), it(2, 0, 0)], 45, 5).id, 2);
  assert.equal(labelItemAt(ev, [it(1, 0, 0), it(2, 0, 0)], 10, 5).id, 1);
});
test('label of an item already picked up is skipped; outside the box misses', () => {
  assert.equal(labelItemAt(ev, [it(1, 0, 0), it(2, 0, 0, 4)], 45, 5).id, 1);
  assert.equal(labelItemAt(ev, [it(1, 0, 0)], 50, 5), null);
});
test('nearest ground item wins, equal distance goes to the lower id; far undrawn item is ignored', () => {
  const a = it(5, 10, 10), b = it(3, 12, 10), far = it(9, 40, 40);
  assert.equal(nearestGroundItem([a, b], { x: 11, y: 10 }, new Set()).item.id, 3);
  assert.equal(nearestGroundItem([a, b], { x: 10, y: 10 }, new Set()).item.id, 5);
  assert.equal(nearestGroundItem([far], { x: 0, y: 0 }, new Set()), null);
  assert.equal(nearestGroundItem([far], { x: 0, y: 0 }, new Set([far])).item.id, 9);
});
test('only a refused pickup needs the shell walk', () => {
  assert.deepEqual([0, 1, 2].map(pickupNeedsWalk), [true, false, false]);
});
