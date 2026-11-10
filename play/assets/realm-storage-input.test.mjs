import test from 'node:test';
import assert from 'node:assert/strict';
import { realmStorageClick } from './realm-storage-input.mjs';
const calls = [];
const ri = { lift: (g) => calls.push(['lift', g]), swapGrid: (...a) => calls.push(['swap', ...a]), putInGrid: (...a) => calls.push(['put', ...a]) };
const renderer = (over = {}) => ({ contains: () => true, itemAt: () => null, cellAt: () => ({ x: 2, y: 3 }), transmuteAt: () => false, ...over });
const ctx = (over = {}) => ({ width: 800, height: 600, items: [], cursorId: 0, input: ri, send: (b) => calls.push(['send', ...b]),
  inventory: { visible: true, renderer: renderer() }, stash: { open: false }, cube: { open: false }, snapshot: {}, ...over });
test('a stash click lifts, puts on page 4; the cube puts on page 3 and transmutes', () => {
  calls.length = 0;
  realmStorageClick(ctx({ stash: { open: true, layout: {}, renderer: renderer({ itemAt: () => ({ id: 7 }) }) } }), 1, 1);
  realmStorageClick(ctx({ cursorId: 9, items: [{ id: 9, page: 0, x: 0, y: 0, invWidth: 1, invHeight: 1 }], stash: { open: true, layout: {}, renderer: renderer() } }), 1, 1);
  realmStorageClick(ctx({ cursorId: 9, items: [{ id: 9, invWidth: 1, invHeight: 1 }], cube: { open: true, layout: {}, renderer: renderer() } }), 1, 1);
  realmStorageClick(ctx({ cube: { open: true, layout: {}, renderer: renderer({ transmuteAt: () => true }) } }), 1, 1);
  assert.deepEqual(calls, [['lift', 7], ['put', 9, 2, 3, 4], ['put', 9, 2, 3, 3], ['send', 0x4f, 24, 0, 0, 0, 0, 0]]);
});
test('shift-click on a carried item sends it to the belt; other buttons are not ours', () => {
  calls.length = 0;
  assert.equal(realmStorageClick(ctx({ inventory: { visible: true, renderer: renderer({ itemAt: () => ({ id: 5 }) }) } }), 1, 1, { shift: true }), true);
  assert.deepEqual(calls, [['send', 0x63, 5, 0, 0, 0]]);
  assert.equal(realmStorageClick(ctx(), 1, 1, { button: 2 }), false);
});
test('the trade screen: my grid lifts and puts on page 2, the rest of the panel is its buttons', () => {
  calls.length = 0;
  const press = (x, y, held) => calls.push(['press', x, y, held]);
  const trade = (over) => ({ open: true, layout: {}, press, renderer: renderer(over) });
  assert.equal(realmStorageClick(ctx({ trade: trade({ itemAt: () => ({ id: 7 }) }) }), 1, 1), true);
  assert.equal(realmStorageClick(ctx({ cursorId: 9, items: [{ id: 9, invWidth: 1, invHeight: 1 }], trade: trade() }), 1, 1), true);
  assert.equal(realmStorageClick(ctx({ trade: trade({ cellAt: () => null }) }), 30, 40), true);
  assert.equal(realmStorageClick(ctx({ cursorId: 9, trade: trade({ cellAt: () => null }) }), 30, 40), true);
  assert.deepEqual(calls, [['lift', 7], ['put', 9, 2, 3, 2], ['press', 30, 40, { cursorItem: false }], ['press', 30, 40, { cursorItem: true }]]);
  calls.length = 0;
  realmStorageClick(ctx({ trade: { ...trade({ contains: () => false }) }, inventory: { visible: false } }), 1, 1);
  assert.deepEqual(calls, []);
});
