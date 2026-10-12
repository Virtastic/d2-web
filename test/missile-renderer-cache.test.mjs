import test from 'node:test';
import assert from 'node:assert/strict';
import { createMissileRenderer, missileTransDrawMode } from '../play/assets/missile-renderer.mjs';
import { PL2_MULTIPLICATIVE } from '../play/assets/frontend.mjs';

test('missile nTrans selects the local 1.13c draw mode', () => {
  assert.equal(missileTransDrawMode(0), 5, 'native normal/default mode');
  assert.equal(missileTransDrawMode(1), 3, 'PL2 additive');
  assert.equal(missileTransDrawMode(2), 4, 'PL2 multiplicative');
  assert.equal(missileTransDrawMode(3), 5, 'unsupported values use the native default mode');
});

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
  put(0, 32); // coded output size
  put(0, 2); // compression flags
  for (const width of [0, 1, 1, 0, 0, 0, 0]) put(width, 4);
  put(0, 0); put(1, 1); put(1, 1); put(0, 0); put(0, 0); put(0, 0); put(0, 0); put(0, 1);
  put(0, 20); // pixel mask stream length
  for (let i = 0; i < 256; i++) put(0, 1); // empty palette map
  put(4, 4); put(0, 4); // one unchanged-color pixel
  return bytes;
}

test('missile draws reuse the act palette tuple and preserve draw evidence and order', (t) => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (previousImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = previousImageData; });

  const reads = [];
  const read = (path) => {
    reads.push(path);
    if (path.endsWith('.dcc')) return onePixelDcc();
    if (path.endsWith('pal.dat')) return new Uint8Array(768);
    throw new Error('no PL2 fixture');
  };
  const operations = [];
  const documentObject = { createElement: () => ({ width: 0, height: 0, getContext: () => ({
    putImageData(image, x, y) { operations.push(['upload', image.width, image.height, x, y]); },
  }) }) };
  const ctx = { drawImage(...args) { operations.push(['drawImage', ...args.slice(1)]); } };
  const renderer = createMissileRenderer(read, documentObject, () => 'fixture');
  const missile = { classId: 62 };
  const first = renderer.draw(ctx, 10, 20, missile, 0, 0, 0, 0, 1);
  const firstOps = operations.splice(0);
  const second = renderer.draw(ctx, 10, 20, missile, 0, 0, 0, 0, 1);
  const secondOps = operations.splice(0);

  assert.deepEqual(second, first);
  assert.equal(first.drawMode, 5, 'an nTrans 0 blit reports native normal mode 5');
  assert.deepEqual(firstOps.map(([op]) => op), ['upload', 'drawImage']);
  assert.deepEqual(secondOps.map(([op]) => op), ['drawImage']);
  assert.deepEqual(reads.filter((path) => path.endsWith('pal.dat')), ['data\\global\\palette\\ACT1\\pal.dat']);
});

test('missile nTrans reaches the PL2 compositor as the recovered draw mode', (t) => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (previousImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = previousImageData; });

  const modes = [];
  const read = (path) => {
    if (path.endsWith('.dcc')) return onePixelDcc();
    if (path.endsWith('pal.dat')) return new Uint8Array(768);
    if (path.endsWith('pal.pl2')) return new Uint8Array(PL2_MULTIPLICATIVE + 65536);
    throw new Error(`unexpected asset ${path}`);
  };
  const renderer = createMissileRenderer(read, { createElement: () => ({ getContext: () => ({ putImageData() {} }) }) }, () => 'fixture');
  // Replace the real PL2 composition path with a recorder while retaining renderer dispatch.
  // A complete PL2 fixture makes createBlender succeed and allows both non-opaque branches to run.
  for (const trans of [1, 2]) {
    const ctx = { getImageData() { return { data: new Uint8ClampedArray(4) }; }, putImageData() {} };
    const result = renderer.draw(ctx, 0, 0, { classId: 62 }, 0, 0, 0, trans, 1);
    modes.push(result.drawMode);
  }
  assert.deepEqual(modes, [3, 4]);
});

test('missile decoder and raster caches stay bounded across asset and frame churn', (t) => {
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  t.after(() => { if (previousImageData === undefined) delete globalThis.ImageData; else globalThis.ImageData = previousImageData; });

  let dccReads = 0;
  const read = (path) => {
    if (path.endsWith('.dcc')) { dccReads++; return onePixelDcc(); }
    if (path.endsWith('pal.dat')) return new Uint8Array(768);
    throw new Error(`unexpected asset ${path}`);
  };
  const renderer = createMissileRenderer(read, { createElement: () => ({ width: 0, height: 0,
    getContext: () => ({ putImageData() {} }) }) }, (classId) => `missile-${classId}`);
  const ctx = { drawImage() {} };

  for (let classId = 1; classId <= 280; classId++) {
    assert.ok(renderer.draw(ctx, 0, 0, { classId }, 0, 0, 0, 0, 1));
  }
  assert.equal(renderer.decoded, 128, 'decoded DCCs are capped by entry count');
  assert.equal(renderer.rasters, 256, 'raster canvases are capped by entry count');
  const readsBeforeReload = dccReads;
  assert.ok(renderer.draw(ctx, 0, 0, { classId: 1 }, 0, 0, 0, 0, 1));
  assert.equal(dccReads, readsBeforeReload + 1, 'the evicted oldest DCC is decoded again on demand');
  assert.equal(renderer.decoded, 128);
  assert.equal(renderer.rasters, 256);
});
