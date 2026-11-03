// A belt potion's hover tooltip: the box under the pointer, the potion in it, the lines and the placement.
// The four boxes are Belts.txt's 800x600 HUD row (29x29, pitch 31), centre-bottom anchored at 800x600 (origin 0,0).
import assert from 'node:assert';
import { beltHoverPlan, beltSlotAt } from './belt-tooltip.mjs';
import { placeItemTip } from './item-tooltip.mjs';

const origin = { x: 0, y: 0 };
const boxes = [0, 1, 2, 3].map((i) => ({ left: 423 + 31 * i, right: 452 + 31 * i, top: 562, bottom: 591 }));
const tip = (item) => ({ name: 'Minor Healing Potion', props: ['Heals 1'], propColours: [0], quality: 2, sell: -1, sellLabel: 'Sell value: ' });
const items = [{ id: 77, mode: 2, gridX: 1, code: 'hp1', itemFlags: 0 }];

// The boxes' own edges: right and bottom are exclusive, as the belt click's test is.
assert.strictEqual(beltSlotAt(boxes, origin, 454, 562), 1);
assert.strictEqual(beltSlotAt(boxes, origin, 482, 590), 1);
assert.strictEqual(beltSlotAt(boxes, origin, 482, 591), -1);
assert.strictEqual(beltSlotAt(boxes, origin, 453, 570), -1);   // the 2-px gutter between box 0 and box 1
assert.strictEqual(beltSlotAt([], origin, 454, 562), -1);

// A potion in box 1: the plan names it, its box, and the engine's lines.
const plan = beltHoverPlan({ boxes, origin, x: 460, y: 570, items, tip });
assert.deepStrictEqual({ id: plan.id, slot: plan.slot, cell: plan.cell, lines: plan.lines, colours: plan.colours },
  { id: 77, slot: 1, cell: { x: 454, y: 562, w: 29, h: 29 }, lines: ['Minor Healing Potion', 'Heals 1'], colours: [0, 0] });

// The tooltip sits above the box, 2 px into it, centred on the box's middle (item-tooltip.mjs placeItemTip).
assert.deepStrictEqual(placeItemTip(plan.cell, 120, 32), { bx: 408, by: 532 });

// No potion in the box under the pointer, or no box under it: no plan, so nothing is drawn.
assert.strictEqual(beltHoverPlan({ boxes, origin, x: 490, y: 570, items, tip }), null);   // box 2 is empty
assert.strictEqual(beltHoverPlan({ boxes, origin, x: 453, y: 570, items, tip }), null);   // the gutter
assert.strictEqual(beltHoverPlan({ boxes, origin, x: 460, y: 570, items: [], tip }), null);

// The HUD origin moves the boxes with the view: a 1024x768 view (origin.x = 112) finds the same box.
const wide = { x: 112, y: 168 };
assert.strictEqual(beltHoverPlan({ boxes, origin: wide, x: 460 + 112, y: 570 + 168, items, tip }).slot, 1);

console.log('belt-tooltip: ok');
