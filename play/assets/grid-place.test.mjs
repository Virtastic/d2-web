import test from 'node:test';
import assert from 'node:assert/strict';
import { overlappedItems, decideGridPlace } from './grid-place.mjs';

const it = (id, gridX, gridY, invWidth, invHeight, invPage = 0, mode = 0) => ({ id, gridX, gridY, invWidth, invHeight, invPage, mode });
const held = { id: 99, invWidth: 2, invHeight: 3, classId: 7 };
const items = [it(1, 0, 0, 1, 1), it(2, 4, 0, 2, 2), it(3, 6, 3, 1, 1), it(4, 0, 0, 1, 1, 4), it(5, 2, 2, 1, 1, 0, 1)];

test('no overlap puts', () => assert.deepEqual(decideGridPlace(items, 0, 8, 0, held), { action: 'put' }));
test('exactly one overlapped item swaps with it', () => {
  assert.deepEqual(decideGridPlace(items, 0, 0, 0, held), { action: 'swap', item: items[0] });
  // overlapped through the item's body (its lower right cell), not its corner cell
  assert.deepEqual(decideGridPlace(items, 0, 5, 1, { id: 99, invWidth: 1, invHeight: 1 }), { action: 'swap', item: items[1] });
});
test('two or more overlapped items refuse', () => {
  assert.deepEqual(decideGridPlace(items, 0, 5, 1, held), { action: 'refuse' });   // item 2 (5,1) and item 3 (6,3)
  assert.deepEqual(decideGridPlace(items, 0, 0, 0, { id: 99, invWidth: 5, invHeight: 1 }), { action: 'refuse' });   // items 1 and 2
});
test('other pages, worn items and the held item itself do not count', () => {
  assert.deepEqual(overlappedItems(items, 4, 0, 0, 1, 1).map((i) => i.id), [4]);
  assert.deepEqual(overlappedItems(items, 0, 2, 2, 1, 1), []);
  assert.deepEqual(overlappedItems([...items, it(99, 8, 0, 2, 3)], 0, 8, 0, 2, 3, 99), []);
});
test('a cube is never swapped out through the cube page', () => {
  const cube = [it(10, 0, 0, 1, 1, 3)];
  assert.equal(decideGridPlace(cube, 3, 0, 0, { id: 99, invWidth: 1, invHeight: 1, code: 'box' }).action, 'refuse');
  assert.equal(decideGridPlace(cube, 3, 0, 0, { id: 99, invWidth: 1, invHeight: 1, code: 'xyz' }).action, 'swap');
});
test('control: a wrong expectation fails', () => assert.throws(() => assert.equal(decideGridPlace(items, 0, 0, 0, held).action, 'put')));
