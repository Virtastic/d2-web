import test from 'node:test';
import assert from 'node:assert/strict';
import { itemDrawMode, drawItemCel, ETHEREAL_ITEM_FLAG } from '../play/assets/item-draw-mode.mjs';

test('only the ethereal flag selects the alpha mode', () => {
  assert.equal(itemDrawMode(0), 5);
  assert.equal(itemDrawMode(0x800), 5);          // socketed alone stays opaque
  assert.equal(itemDrawMode(0x10 | 0x800000), 5);
  assert.equal(itemDrawMode(ETHEREAL_ITEM_FLAG), 1);
  assert.equal(itemDrawMode(0x400010 | 0x800), 1);
  assert.equal(itemDrawMode(0xffffffff), 1);      // unsigned read of a high-bit mask
});

test('an ethereal item blits through the blender with mode 1, a plain one with drawImage', () => {
  const frame = { width: 2, height: 1, indices: Uint8Array.of(3, 4), opaque: Uint8Array.of(1, 1) };
  const calls = [];
  const ctx = { drawImage: (...a) => calls.push(['img', ...a]) };
  const blend = (c, f, l, t, m) => calls.push(['blend', l, t, m, Array.from(f.indices)]);
  assert.equal(drawItemCel(ctx, 'IMG', frame, 10, 20, 0x800, blend), 5);
  assert.equal(drawItemCel(ctx, 'IMG', frame, 10, 20, 0x400000, blend), 1);
  assert.equal(drawItemCel(ctx, 'IMG', frame, 10, 20, 0x400000, blend, (i) => i + 1), 1);
  assert.equal(drawItemCel(ctx, 'IMG', frame, 1, 2, 0x400000, null), 5);   // no PL2 for the act: stays opaque
  assert.deepEqual(calls, [['img', 'IMG', 10, 20], ['blend', 10, 20, 1, [3, 4]], ['blend', 10, 20, 1, [4, 5]], ['img', 'IMG', 1, 2]]);
});
