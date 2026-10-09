import test from 'node:test';
import assert from 'node:assert/strict';
import { createObjectRenderer, parseObjectPresentations } from '../play/assets/object-renderer.mjs';

test('object presentation records retain retail token, mode, frame, and component mask', () => {
  const bytes = new Uint8Array(40), v = new DataView(bytes.buffer);
  v.setUint32(0, 9, true); v.setUint32(4, 12, true); v.setUint32(8, 0x00357931, true); v.setUint32(12, 2, true);
  v.setInt32(16, -3, true); v.setInt32(20, 8, true); v.setUint32(24, 0x300, true); v.setUint32(28, 0x800, true);
  v.setUint32(32, 0x102, true); v.setUint16(36, 128, true); v.setUint8(38, 4); v.setUint8(39, 1);
  assert.deepEqual(parseObjectPresentations(bytes, 1)[0], { id: 9, classId: 12, token: '1y5', mode: 2, x: -3, y: 8,
    framePrecise: 0x300, frameCountPrecise: 0x800, componentMask: 0x102, animSpeed: 128, direction: 4, selectable: true });
  assert.throws(() => parseObjectPresentations(bytes.subarray(1), 1), /truncated/);
});

class BitWriter {
  constructor() { this.bits = []; }
  write(value, count) { for (let i = 0; i < count; i++) this.bits.push((value >>> i) & 1); }
  bytes() { const out = new Uint8Array(Math.ceil(this.bits.length / 8)); this.bits.forEach((v, i) => out[i >>> 3] |= v << (i & 7)); return out; }
}

function dccFixture() {
  const body = new BitWriter();
  body.write(12, 32); body.write(0, 2);
  for (const value of [0, 2, 2, 0, 0, 0, 0]) body.write(value, 4);
  body.write(3, 2); body.write(2, 2); body.write(0, 1); body.write(0, 20);
  for (let i = 0; i < 256; i++) body.write(i === 0 || i === 7 ? 1 : 0, 1);
  body.write(1, 4); body.write(0, 4);
  for (let i = 0; i < 6; i++) body.write(0, 1);
  const encoded = body.bytes(), out = new Uint8Array(19 + encoded.length), view = new DataView(out.buffer);
  out.set([0x74, 6, 1], 0); view.setUint32(3, 1, true); view.setUint32(7, 1, true);
  view.setUint32(11, encoded.length, true); view.setUint32(15, 19, true); out.set(encoded, 19);
  return out;
}

function rendererHarness({ blended = true, rendererOptions = {} } = {}) {
  const cof = (hasBlend) => { const bytes = new Uint8Array(39);
    bytes.set([1, 1, 1], 0); bytes[24] = 128;
    bytes.set([1, 0, 1, hasBlend ? 1 : 0, hasBlend ? 3 : 0, 0x48, 0x54, 0x48, 0], 28); bytes[38] = 1;
    return bytes;
  };
  const dcc = dccFixture(), palette = new Uint8Array(768), cofReads = new Map();
  for (let i = 0; i < 256; i++) palette.set([i, i, i], i * 3);
  const read = (path) => {
    if (path.endsWith('.COF')) {
      const token = path.split('\\')[3]; cofReads.set(token, (cofReads.get(token) || 0) + 1);
      return cof(path.includes('\\BLD\\'));
    }
    if (path.endsWith('.DCC')) return dcc;
    if (path.endsWith('pal.dat')) return palette;
    throw new Error(`missing fixture ${path}`); // no PL2: effect layer must use raster fallback
  };
  let created = 0;
  const documentObject = { createElement(tag) {
    assert.equal(tag, 'canvas'); created++;
    const canvas = { width: 0, height: 0, pixels: null, getContext() { return {
      putImageData(image) { canvas.pixels = Uint8ClampedArray.from(image.data); },
    }; } };
    return canvas;
  } };
  const oldImageData = globalThis.ImageData;
  globalThis.ImageData = class { constructor(data) { this.data = data; } };
  try {
    const renderer = createObjectRenderer(read, documentObject, rendererOptions), draws = [];
    const ctx = { drawImage(canvas, x, y) { draws.push({ canvas, x, y, pixels: Array.from(canvas.pixels) }); } };
    return { renderer, draws, ctx, created: () => created, cofReads,
      object: { token: blended ? 'BLD' : 'TST', mode: 0, direction: 0, componentMask: 2, framePrecise: 0 } };
  } catch (error) {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
    throw error;
  }
}

test('object renderer reuses bounded blended-layer rasters without allocating an unused composed canvas', () => {
  const oldImageData = globalThis.ImageData;
  const harness = rendererHarness({ blended: true });
  try {
    const { renderer, draws, ctx, object } = harness;
    const first = renderer.draw(ctx, 10, 20, object, 1), firstDraw = draws.at(-1);
    const second = renderer.draw(ctx, 10, 20, object, 1), secondDraw = draws.at(-1);
    assert.equal(first.layers.length, 1);
    assert.equal(first.layers[0].mode, 5, 'missing PL2 reports the raster fallback mode');
    assert.deepEqual(second.layers, first.layers);
    assert.equal(secondDraw.canvas, firstDraw.canvas, 'the actual renderer reuses the cached layer canvas');
    assert.deepEqual([secondDraw.x, secondDraw.y, secondDraw.pixels], [firstDraw.x, firstDraw.y, firstDraw.pixels]);
    assert.equal(draws.length, 2, 'one layer draw is preserved per render in the same order');
    assert.equal(harness.created(), 1, 'the blended path allocates only its fallback layer canvas');
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});

test('object blend eligibility is scanned when its animation is decoded, not on repeated draws', () => {
  const oldImageData = globalThis.ImageData, oldSome = Array.prototype.some;
  const harness = rendererHarness({ blended: true });
  let scans = 0;
  try {
    const { renderer, ctx, object } = harness;
    Array.prototype.some = function (...args) { scans++; return oldSome.apply(this, args); };
    const first = renderer.draw(ctx, 10, 20, object, 1);
    const scansAfterDecode = scans;
    const second = renderer.draw(ctx, 10, 20, object, 1);
    assert.ok(first && second);
    assert.ok(scansAfterDecode > 0, 'decoding determines whether this COF has PL2 layers');
    assert.equal(scans, scansAfterDecode, 'a cached animation does not rescan its COF layer list per draw');
  } finally {
    Array.prototype.some = oldSome;
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});

test('object renderer caches composed and blended rasters in one bounded FIFO', () => {
  const oldImageData = globalThis.ImageData;
  // The same DCC fixture supplies equal-sized composed and per-layer rasters. A cap of one raster
  // tests both the entry-count bound and byte bound while alternating the two renderer paths.
  const harness = rendererHarness({ blended: false, rendererOptions: { rasterLimit: 1, rasterByteLimit: 64 } });
  try {
    const { renderer, draws, ctx, object } = harness;
    const first = renderer.draw(ctx, 10, 20, object, 1), composedCanvas = draws.at(-1).canvas;
    const second = renderer.draw(ctx, 10, 20, object, 1), repeatedCanvas = draws.at(-1).canvas;
    assert.equal(repeatedCanvas, composedCanvas, 'repeated composed draws reuse their canvas');
    assert.deepEqual(second, first, 'the raster change preserves object evidence');
    const composedOutput = [draws.at(-1).x, draws.at(-1).y, draws.at(-1).pixels];

    // The same renderer and cache now take the blended-layer path under another complete animation
    // key. Inserting that layer evicts the composed raster at the shared single-entry bound.
    const blendedObject = { ...object, token: 'BLD' };
    const blendedFirst = renderer.draw(ctx, 10, 20, blendedObject, 1), blendedCanvas = draws.at(-1).canvas;
    const blendedSecond = renderer.draw(ctx, 10, 20, blendedObject, 1);
    assert.equal(draws.at(-1).canvas, blendedCanvas, 'the blended fallback remains reusable');
    assert.deepEqual(blendedSecond.layers, blendedFirst.layers, 'layer evidence is stable');
    assert.deepEqual([draws.at(-1).x, draws.at(-1).y, draws.at(-1).pixels],
      [draws[2].x, draws[2].y, draws[2].pixels], 'blended output remains stable');
    assert.equal(harness.created(), 2, 'the blended fallback evicted and replaced the composed entry');
    assert.deepEqual(composedOutput, [draws[0].x, draws[0].y, draws[0].pixels]);
    const composedAgain = renderer.draw(ctx, 10, 20, object, 1);
    assert.equal(harness.created(), 3, 'the composed raster was evicted by the blended entry');
    assert.deepEqual(composedAgain, first);
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});

test('object renderer bounds decoded animation retention with FIFO eviction', () => {
  const oldImageData = globalThis.ImageData;
  const harness = rendererHarness({ blended: false, rendererOptions: { animationLimit: 1 } });
  try {
    const { renderer, draws, ctx, object, cofReads } = harness;
    const first = renderer.draw(ctx, 10, 20, object, 1);
    const other = renderer.draw(ctx, 10, 20, { ...object, token: 'BLD' }, 1);
    assert.ok(first && other);
    assert.equal(renderer.decoded, 1, 'decoded animation entries stay within the configured limit');

    const decodedAgain = renderer.draw(ctx, 10, 20, object, 1);
    assert.deepEqual(decodedAgain, first, 'eviction and re-decode preserve rendered object evidence');
    assert.equal(cofReads.get('TST'), 2, 'the oldest animation was evicted and decoded again');
    assert.equal(renderer.decoded, 1);
    assert.equal(draws.length, 3);
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});

test('object renderer bounds failed animation retention with FIFO eviction', () => {
  const oldImageData = globalThis.ImageData;
  const harness = rendererHarness({ rendererOptions: { failureLimit: 1 } });
  try {
    const { renderer, ctx, cofReads } = harness;
    const firstFailure = { token: 'TST', mode: 0, direction: 0, componentMask: 0, framePrecise: 0 };
    const secondFailure = { ...firstFailure, token: 'BAD' };
    assert.equal(renderer.draw(ctx, 0, 0, firstFailure, 1), null);
    assert.equal(renderer.draw(ctx, 0, 0, secondFailure, 1), null);
    assert.equal(renderer.failures, 1, 'failed keys stay within the configured limit');

    assert.equal(renderer.draw(ctx, 0, 0, firstFailure, 1), null);
    assert.equal(cofReads.get('TST'), 2, 'the oldest failure was evicted and its key was retried');
    assert.equal(renderer.failures, 1);
  } finally {
    if (oldImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = oldImageData;
  }
});
