import assert from 'node:assert/strict';
import { hoveredLabel, labelItemAt } from '../play/item-pick.mjs';
import { altHoverBox, nearestPaletteIndex, ALT_HOVER_RGB } from '../play/assets/hover-lifebar.mjs';
const ev = [{ id: 1, box: { x: 0, y: 0, width: 60, height: 10 } }, { id: 2, box: { x: 20, y: 0, width: 60, height: 10 } }];
const items = [1, 2].map((id) => ({ id, mode: 3, invPage: 255 }));
// label hover: the click's pick (topmost label) names the label to highlight
assert.equal(hoveredLabel(ev, labelItemAt(ev, items, 5, 5)), ev[0]);
assert.equal(hoveredLabel(ev, labelItemAt(ev, items, 30, 5)), ev[1]);       // overlap: topmost only
assert.equal(hoveredLabel(ev, labelItemAt(ev, items, 200, 5)), null);
assert.equal(hoveredLabel(ev, labelItemAt(ev, [items[0]], 70, 5)), null);   // picked-up item: no highlight
// sprite hover: the pointer is off every label; the click pick still returns the item (shell: resolvePointerTarget().item)
assert.equal(labelItemAt(ev, items, 200, 200), null);
assert.equal(hoveredLabel(ev, items[1]), ev[1]);                            // sprite hit on item 2 highlights its label
assert.equal(hoveredLabel(ev, { id: 9 }), null);                            // no label drawn for it
// control: a deliberately wrong pick must NOT yield the sprite's label
assert.notEqual(hoveredLabel(ev, items[0]), ev[1]);
// retail colour: D2Win #10190(0,0x40,0x80) over a palette, mode 5 (D2Client+0x58FB0)
assert.deepEqual([...ALT_HOVER_RGB], [0, 0x40, 0x80]);
const pal = new Uint8Array(1024); for (let i = 0; i < 256; i++) pal.set([i, i, i, 0], i * 4);
pal.set([0, 0x40, 0x80, 0], 77 * 4);
assert.equal(nearestPaletteIndex(pal, 0, 0x40, 0x80), 77);
assert.deepEqual(altHoverBox(ev[0].box, pal), [[0, 0, 60, 10, 77, 5]]);
// control: the old approximation (90,110,255) is not the retail colour
assert.notEqual(nearestPaletteIndex(pal, 90, 110, 255), 77);
