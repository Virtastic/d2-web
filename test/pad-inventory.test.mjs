// Inventory shortcuts for a pad: LT+A quick move, R3 belt fill, hold A to equip. A fake shell feeds the
// real controller; the assertions are the exact clicks it dispatches, in order.
import { test } from 'node:test';
import assert from 'node:assert/strict';
import { B, events, install, tap, down, up, poll, drain } from './pad-fixture.mjs';
import { quickMovePlan, beltFillPlan, makeHold, equipHeld } from '../play/assets/pad-panels.mjs';

let now = 1000;
performance.now = () => now;
const grid = (x0, y0, cols, rows) => { const o = {}; for (let y = 0; y < rows; y++) for (let x = 0; x < cols; x++) o[`${x},${y}`] = [x0 + x * 30, y0 + y * 30]; return o; };
const item = (id, gridX, gridY, w, h, extra = {}) => ({ id, gridX, gridY, invWidth: w, invHeight: h, invPage: 0, mode: 0, code: 'xxx', ...extra });
let cursor = 0, targets = {};
const shell = { screen: 'game', snapshot: { items: [], player: { x: 0, y: 0, hpMax: 1 } }, controlCodeFor: () => null, project: () => [0, 0],
  inventoryVisible: true, uiTargets: () => targets, get cursorItem() { return cursor; } };
await install(shell);
const pt = (x, y) => `pointerdown:${x + 0.5},${y + 0.5}`;
const downs = () => drain().filter((e) => e.startsWith('pointerdown'));
const cur = () => window.__d2Controller.evidence.cursor;
// Walk the highlight to a point with d-pad taps, trying each direction until the distance falls.
const goTo = (x, y) => {
  for (let i = 0; i < 80 && (cur().x !== x || cur().y !== y); i++) {
    const d = Math.hypot(cur().x - x, cur().y - y);
    for (const b of [B.LEFT, B.RIGHT, B.UP, B.DOWN]) { const from = { ...cur() }; tap(b); if (Math.hypot(cur().x - x, cur().y - y) < d) break; if (cur().x !== from.x || cur().y !== from.y) { /* wrong way: step back */ tap({ [B.LEFT]: B.RIGHT, [B.RIGHT]: B.LEFT, [B.UP]: B.DOWN, [B.DOWN]: B.UP }[b]); } }
  }
  assert.deepEqual([cur().x, cur().y], [x, y]);
  drain();
};
const open = (t, items) => { targets = t; shell.snapshot.items = items; cursor = 0; poll(); drain(); };

test('LT+A with the stash open lifts the item and puts it at the stash first free cell', () => {
  open({ inventoryGrid: grid(100, 100, 4, 3), stashGrid: grid(400, 100, 4, 3) }, [item(7, 0, 0, 1, 1)]);
  down(B.LT); tap(B.A); up(B.LT);
  assert.deepEqual(downs(), [pt(100, 100), pt(400, 100)]);
});

test('from the stash, a 2 wide item skips the occupied cell and lands at the pack first fit', () => {
  open({ inventoryGrid: grid(100, 100, 4, 3), stashGrid: grid(400, 100, 4, 3) }, [item(7, 0, 0, 1, 1), item(9, 0, 0, 2, 1, { invPage: 4 })]);
  goTo(415, 100);                                   // the stash item (2 wide, so its footprint middle)
  down(B.LT); tap(B.A); up(B.LT);
  const got = downs();
  assert.equal(got.length, 2, got.join());
  assert.equal(got[1], pt(145, 100));            // anchor (1,0): the middle of cells (1,0)-(2,0)
});

test('a pack that is full gets no clicks at all, and the press does nothing', () => {
  open({ inventoryGrid: grid(100, 100, 1, 1), stashGrid: grid(400, 100, 1, 1) }, [item(7, 0, 0, 1, 1), item(9, 0, 0, 1, 1, { invPage: 4 })]);
  const base = [{ key: 'invg:0,0', x: 100, y: 100 }, { key: 'stash:0,0', x: 400, y: 100 }];
  const items = shell.snapshot.items;
  assert.equal(quickMovePlan(base, {}, { items }, { key: 'stash:i9', x: 400, y: 100 }, 0), null);
  assert.equal(quickMovePlan(base, {}, { items }, { key: 'invg:i7', x: 100, y: 100 }, 7), null);   // an item already held: plain A
});

test('LT+A on a pack item with a vendor open is the engine\'s Ctrl-click sale, one press one click', () => {
  open({ inventoryGrid: grid(100, 100, 4, 3), shop: { 5: { at: [400, 100] } } }, [item(7, 0, 0, 1, 1)]);
  goTo(100, 100);
  down(B.LT); tap(B.A); up(B.LT);
  assert.deepEqual(downs(), [`pointerdown:100.5,100.5:ctrl`]);
});

test('plain A is still one plain click', () => {
  open({ inventoryGrid: grid(100, 100, 4, 3), stashGrid: grid(400, 100, 4, 3) }, [item(7, 0, 0, 1, 1)]);
  tap(B.A);
  assert.deepEqual(downs(), [pt(100, 100)]);
});

test('R3 in the inventory shift-clicks each potion, in grid order, and nothing else', () => {
  open({ inventoryGrid: grid(100, 100, 4, 3) }, [item(7, 0, 0, 1, 1, { code: 'hp1' }), item(8, 2, 1, 1, 1, { code: 'mp2' }), item(9, 1, 0, 1, 1, { code: 'tsc' })]);
  tap(B.R3);
  assert.deepEqual(downs(), [`pointerdown:100.5,100.5:shift`, `pointerdown:160.5,130.5:shift`]);
  assert.equal(beltFillPlan(null, {}, [], 0), null);
  assert.deepEqual(beltFillPlan([{ key: 'invg:i7', x: 1, y: 1 }], {}, [item(7, 0, 0, 1, 1, { code: 0x20317068 })], 0), [{ x: 1, y: 1, shift: true }]);   // the snapshot's numeric code
  assert.equal(beltFillPlan([{ key: 'invg:i7', x: 1, y: 1 }], { shop: {} }, [item(7, 0, 0, 1, 1, { code: 'hp1' })], 0), null);   // vendor open
  assert.equal(beltFillPlan([{ key: 'invg:i7', x: 1, y: 1 }], {}, [item(7, 0, 0, 1, 1, { code: 'hp1' })], 5), null);              // item on the cursor
});

test('R3 outside an inventory clicks nothing', () => {
  open({ select: { a: [10, 10] } }, []);
  tap(B.R3);
  assert.deepEqual(downs(), []);
});

test('makeHold reports the key the press began on once, after 400 ms', () => {
  const h = makeHold();
  assert.equal(h(true, 0, 'a'), null); assert.equal(h(true, 399, 'b'), null);
  assert.equal(h(true, 400, 'b'), 'a'); assert.equal(h(true, 900, 'b'), null);
  assert.equal(h(false, 1000, 'b'), null); assert.equal(h(true, 1100, 'c'), null); assert.equal(h(true, 1500, 'c'), 'c');
});

// The engine refuses a slot the item does not fit: the fake cursor takes item 7 on the first click at the
// item's cell and gives it up (to `swapped`, 0 for an empty slot) on a click at `fits`.
let model = null;
const push = events.push.bind(events);
events.push = (e) => {
  if (model && e.type === 'pointerdown') {
    const at = `${e.clientX - 0.5},${e.clientY - 0.5}`;
    if (cursor === 0 && at === '100,100') cursor = 7;
    else if (cursor === 7 && at === model.fits) cursor = model.swapped;
  }
  return push(e);
};
const fakeCursor = (fits, swapped) => { model = { fits, swapped }; };
const slots = { invs: { 3: [300, 50], 1: [300, 10], 2: [340, 10] } };

test('holding A ~400 ms on an item lifts it, then tries the empty worn slots and equips at the first that takes it', () => {
  fakeCursor('300,10', 0);
  open({ inventoryGrid: grid(100, 100, 4, 3), inventorySlots: slots.invs }, [item(7, 0, 0, 1, 1)]);
  goTo(100, 100); now = 5000; down(B.A);
  assert.deepEqual(downs(), [pt(100, 100)]);          // the press lifts at once, as before
  now = 5300; poll(); assert.deepEqual(downs(), []);
  now = 5400; poll();
  assert.deepEqual(downs(), [pt(300, 50), pt(300, 10)]);   // slot 3 refused, slot 1 took it
  now = 5900; poll(); assert.deepEqual(downs(), []);        // once per hold
  up(B.A); drain();
});

test('with every fitting slot worn, the hold swaps and tries the empty slots first', () => {
  fakeCursor('300,10', 20);
  open({ inventoryGrid: grid(100, 100, 4, 3), inventorySlots: slots.invs }, [item(7, 0, 0, 1, 1), item(20, -1, -1, 1, 1, { mode: 1, bodyLoc: 1 })]);
  goTo(100, 100); now = 8000; down(B.A); drain();
  now = 8400; poll();
  assert.deepEqual(downs(), [pt(300, 50), pt(340, 10), pt(300, 10)]);   // empties 3 and 2 refused, worn slot 1 swapped
  up(B.A); drain();
});

test('a tap of A is not a hold, and a hold that fits nowhere leaves the item on the cursor', () => {
  fakeCursor('nowhere', 0);
  open({ inventoryGrid: grid(100, 100, 4, 3), inventorySlots: slots.invs }, [item(7, 0, 0, 1, 1)]);
  goTo(100, 100); now = 9000; down(B.A); now = 9100; up(B.A); now = 9600; poll();
  assert.deepEqual(downs(), [pt(100, 100)]);
  cursor = 0; goTo(100, 100); now = 10000; down(B.A); drain(); now = 10400; poll();
  assert.equal(downs().length, 3);
  assert.equal(equipHeld(7, [], [], () => {}, () => 7), null);
  up(B.A);
});
