import test from 'node:test';
import assert from 'node:assert/strict';
import { createMissileRenderer } from '../play/assets/missile-renderer.mjs';

// Hero overlays (Inner Sight EOZCast / HardenArmor) draw with the current act's palette.
// Before this fix the hero draw sites passed act 1 for every act.

function onePixelDcc() {
  const bytes = new Uint8Array(64);
  bytes[0] = 0x74; bytes[1] = 6; bytes[2] = 1;
  const view = new DataView(bytes.buffer);
  view.setUint32(3, 1, true); view.setUint32(7, 1, true); view.setUint32(11, bytes.length, true);
  view.setUint32(15, 19, true);
  let bit = 19 * 8;
  const put = (value, count) => {
    for (let i = 0; i < count; i++, bit++) if ((value >>> i) & 1) bytes[bit >>> 3] |= 1 << (bit & 7);
  };
  put(0, 32); put(0, 2);
  for (const width of [0, 1, 1, 0, 0, 0, 0]) put(width, 4);
  put(0, 0); put(1, 1); put(1, 1); put(0, 0); put(0, 0); put(0, 0); put(0, 0); put(0, 1);
  put(0, 20);
  for (let i = 0; i < 256; i++) put(0, 1);
  put(4, 4); put(0, 4);
  return bytes;
}

test('hero overlay records take the palette of the act they are drawn in', (t) => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (previousImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = previousImageData; });

  const reads = [];
  const read = (path) => {
    reads.push(path);
    if (path.endsWith('.dcc')) return onePixelDcc();
    if (path.endsWith('pal.dat')) return new Uint8Array(768);
    throw new Error(`unexpected asset ${path}`);
  };
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) };
  const ctx = { drawImage() {} };
  const renderer = createMissileRenderer(read, documentObject, (f) => f, 'overlays');
  const overlay = { classId: 'EOZCast' };

  const act1 = renderer.draw(ctx, 0, 0, overlay, 0, 0, 0, 1, 1);
  const act2 = renderer.draw(ctx, 0, 0, overlay, 0, 0, 0, 1, 2);

  assert.notEqual(act2.palette, act1.palette, 'act 2 must not reuse the act 1 palette');
  assert.equal(act1.drawMode, 3, 'Trans 3 overlay (passed as trans 1) draws additive, mode 3');
  assert.equal(act2.drawMode, 3);
  assert.ok(reads.some((p) => p.endsWith('ACT2\\pal.dat')), 'act 2 palette file is read');
});
