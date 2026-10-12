import test from 'node:test';
import assert from 'node:assert/strict';
import { createPlayerRenderer } from '../play/assets/unit-renderer.mjs';
import { decodeDcc } from '../play/assets/unit-formats.mjs';

function cofFixture() {
  const out = new Uint8Array(28 + 18 + 2 + 4);
  out.set([2, 2, 1], 0); out[24] = 128;
  out.set([1, 1, 1, 0, 2, 0x48, 0x54, 0x48, 0], 28);
  out.set([5, 0, 0, 1, 3, 0, 0, 0, 0], 37);
  out.set([0, 1], 46); out.set([1, 5, 5, 1], 48);
  return out;
}

class BitWriter {
  constructor() { this.bits = []; }
  write(value, count) { for (let i = 0; i < count; i++) this.bits.push((value >>> i) & 1); }
  bytes() { const out = new Uint8Array(Math.ceil(this.bits.length / 8)); this.bits.forEach((v, i) => out[i >>> 3] |= v << (i & 7)); return out; }
}

function dccFixture() {
  const body = new BitWriter();
  body.write(12, 32); body.write(0, 2);
  for (const index of [0, 2, 2, 0, 0, 0, 0]) body.write(index, 4);
  body.write(3, 2); body.write(2, 2); body.write(0, 1);
  body.write(0, 20);
  for (let i = 0; i < 256; i++) body.write(i === 0 || i === 7 ? 1 : 0, 1);
  body.write(1, 4); body.write(0, 4);
  for (let i = 0; i < 6; i++) body.write(0, 1);
  const encoded = body.bytes(), out = new Uint8Array(19 + encoded.length), view = new DataView(out.buffer);
  out.set([0x74, 6, 1], 0); view.setUint32(3, 1, true); view.setUint32(7, 1, true);
  view.setUint32(11, encoded.length, true); view.setUint32(15, 19, true); out.set(encoded, 19);
  return out;
}

function retainedDataDccFixture() {
  const optionalSize = 4096, body = new BitWriter();
  body.write(12, 32); body.write(0, 2);
  // A single 1x1 frame with 4 KiB of optional frame data and a full palette map.
  for (const width of [0, 2, 2, 0, 0, 15, 0]) body.write(width, 4);
  body.write(0, 0); body.write(1, 2); body.write(1, 2);
  body.write(0, 0); body.write(0, 0); body.write(optionalSize, 32);
  body.write(0, 0); body.write(0, 1);
  while (body.bits.length % 8) body.write(0, 1);
  for (let i = 0; i < optionalSize; i++) body.write(0, 8);
  body.write(0, 20); // pixel mask stream
  for (let i = 0; i < 256; i++) body.write(1, 1);
  body.write(1, 4); body.write(0, 4);
  const encoded = body.bytes(), out = new Uint8Array(19 + encoded.length), view = new DataView(out.buffer);
  out.set([0x74, 6, 1], 0); view.setUint32(3, 1, true); view.setUint32(7, 1, true);
  view.setUint32(11, encoded.length, true); view.setUint32(15, 19, true); out.set(encoded, 19);
  return out;
}

test('player renderer evicts old decoded modes and raster frames after equipment churn', () => {
  const variants = 280;
  const armTypes = ['token'];
  const armorRows = ['code\tcomponent\ttorso'];
  for (let i = 0; i < variants; i++) {
    const suffix = String(i).padStart(3, '0');
    armTypes.push(`v${suffix}`);
    armorRows.push(`a${suffix}\t1\t${i}`);
  }
  const tables = new Map([
    ['data\\global\\excel\\armor.txt', new TextEncoder().encode(armorRows.join('\n'))],
    ['data\\global\\excel\\weapons.txt', new TextEncoder().encode('code\tcomponent\talternategfx\t2handed\n')],
    ['data\\global\\excel\\ArmType.txt', new TextEncoder().encode(armTypes.join('\n'))],
  ]);
  const cof = cofFixture(), dcc = dccFixture(), palette = new Uint8Array(768);
  const reads = new Map();
  const read = (path) => {
    reads.set(path, (reads.get(path) || 0) + 1);
    if (tables.has(path)) return tables.get(path);
    if (path.endsWith('.COF')) return cof;
    if (path.endsWith('.DCC')) return dcc;
    if (path.endsWith('pal.dat')) return palette;
    throw new Error(`unexpected player asset read: ${path}`);
  };
  let canvases = 0;
  const documentObject = { createElement(tag) {
    assert.equal(tag, 'canvas'); canvases++;
    return { width: 0, height: 0, getContext: () => ({ putImageData() {} }) };
  } };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    const renderer = createPlayerRenderer(read, documentObject);
    const context = { drawImage() {} };
    const drawVariant = (i) => renderer.draw(context, 0, 0, { playerClass: 0, mode: 1, direction: 0,
      framePrecise: 0, act: 1, weaponClass: 'hth', items: [{ code: `a${String(i).padStart(3, '0')}`, mode: 1 }] });
    let firstEvidence;
    for (let i = 0; i < variants; i++) {
      const evidence = drawVariant(i);
      assert.ok(evidence);
      if (i === 0) firstEvidence = evidence;
    }

    assert.equal(renderer.decodedModes, 128);
    assert.equal(renderer.rasterizedFrames, 256);
    const firstDcc = 'data\\global\\CHARS\\AM\\TR\\AMTRv000NUHTH.DCC';
    const readsBefore = reads.get(firstDcc);
    const canvasesBefore = canvases;
    assert.deepEqual(drawVariant(0), firstEvidence, 'eviction preserves the requested sprite and draw evidence');
    assert.equal(reads.get(firstDcc), readsBefore + 1, 'the evicted player DCC is loaded again on demand');
    assert.equal(canvases, canvasesBefore + 1, 'the evicted raster is rebuilt on demand');
    assert.equal(renderer.decodedModes, 128);
    assert.equal(renderer.rasterizedFrames, 256);
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});

test('player decoded cache accounts optional DCC data and palette maps for byte eviction', () => {
  const armTypes = ['token', 'v000', 'v001'];
  const armorRows = ['code\tcomponent\ttorso', 'a000\t1\t0', 'a001\t1\t1'];
  const tables = new Map([
    ['data\\global\\excel\\armor.txt', new TextEncoder().encode(armorRows.join('\n'))],
    ['data\\global\\excel\\weapons.txt', new TextEncoder().encode('code\tcomponent\talternategfx\t2handed\n')],
    ['data\\global\\excel\\ArmType.txt', new TextEncoder().encode(armTypes.join('\n'))],
  ]);
  const cof = cofFixture(), dcc = retainedDataDccFixture(), palette = new Uint8Array(768);
  const decodedDirection = decodeDcc(dcc).directionRecords[0];
  assert.equal(decodedDirection.frames[0].optionalData.byteLength, 4096, 'fixture retains optional DCC frame bytes');
  assert.equal(decodedDirection.paletteEntries.length, 256, 'fixture retains every palette-map entry');
  const reads = new Map();
  const read = (path) => {
    reads.set(path, (reads.get(path) || 0) + 1);
    if (tables.has(path)) return tables.get(path);
    if (path.endsWith('.COF')) return cof;
    if (path.endsWith('.DCC')) return dcc;
    if (path.endsWith('pal.dat')) return palette;
    throw new Error(`unexpected player asset read: ${path}`);
  };
  const previousImageData = globalThis.ImageData;
  globalThis.ImageData = class ImageData { constructor(data, width, height) { Object.assign(this, { data, width, height }); } };
  try {
    // At this limit, dropping either optionalData or paletteEntries would let
    // both decoded variants fit, so the reload assertion covers both charges.
    const byteLimit = 13_000;
    const renderer = createPlayerRenderer(read, { createElement: () => ({ width: 0, height: 0, getContext: () => ({ putImageData() {} }) }) },
      { decodeCacheByteLimit: byteLimit });
    const context = { drawImage() {} };
    const drawVariant = (i) => renderer.draw(context, 0, 0, { playerClass: 0, mode: 1, direction: 0,
      framePrecise: 0, act: 1, weaponClass: 'hth', items: [{ code: `a00${i}`, mode: 1 }] });
    const firstDcc = 'data\\global\\CHARS\\AM\\TR\\AMTRv000NUHTH.DCC';
    drawVariant(0);
    assert.equal(renderer.decodedModes, 1, 'one decoded mode fits the byte limit');
    assert.ok(renderer.decodedBytes > 0 && renderer.decodedBytes <= byteLimit);
    drawVariant(1);
    assert.equal(renderer.decodedModes, 1, `the older mode is evicted when retained bytes exceed the limit (${renderer.decodedBytes} / ${byteLimit})`);
    assert.ok(renderer.decodedBytes <= byteLimit);
    const readsBeforeReload = reads.get(firstDcc);
    drawVariant(0);
    assert.equal(reads.get(firstDcc), readsBeforeReload + 1, 'the byte-evicted DCC is decoded again on demand');
    assert.equal(renderer.decodedModes, 1);
    assert.ok(renderer.decodedBytes <= byteLimit);
  } finally {
    if (previousImageData === undefined) delete globalThis.ImageData;
    else globalThis.ImageData = previousImageData;
  }
});
