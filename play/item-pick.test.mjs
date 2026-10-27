import assert from 'node:assert/strict';
import { altSync, labelItemAt, clickModifiers, nearestGroundItem } from './item-pick.mjs';
const items = [{ id: 1, mode: 3, invPage: 255, x: 10, y: 10 }, { id: 2, mode: 3, invPage: 255, x: 10, y: 10 }, { id: 3, mode: 4, invPage: 255 }];
const ev = [{ id: 1, box: { x: 0, y: 0, width: 50, height: 10 } }, { id: 2, box: { x: 20, y: 0, width: 50, height: 10 } }, { id: 3, box: { x: 0, y: 0, width: 50, height: 10 } }];
// Alt held, click events at fixed points: topmost (last drawn) label wins; non-ground item skipped; outside = null.
assert.equal(labelItemAt(ev, items, 5, 5).id, 1);      // only label 1 (3 is not on the ground)
assert.equal(labelItemAt(ev, items, 30, 5).id, 2);     // overlap 1/2 -> last drawn
assert.equal(labelItemAt(ev, items, 70, 5), null);     // box is half-open
assert.equal(labelItemAt(ev, items, 5, 10), null);
assert.equal(labelItemAt(ev, [items[0]], 30, 5).id, 1); // picked-up item skipped
// Alt not held: caller never consults labels; ground pick falls back to the disc.
assert.equal(nearestGroundItem(items, { x: 10, y: 11 }, new Set()).item.id, 1);
// retail modifier byte: Alt(show items)=4, run hold=8, both=12
assert.deepEqual([clickModifiers(0, 0), clickModifiers(0, 1), clickModifiers(1, 0), clickModifiers(1, 1)], [0, 4, 8, 12]);
// click carries Alt state: stale held state is corrected, agreement or unbound leaves it
assert.deepEqual([altSync(true, true, false), altSync(true, false, true), altSync(true, true, true), altSync(false, true, false)], [true, false, null, null]);
console.log('ok');
