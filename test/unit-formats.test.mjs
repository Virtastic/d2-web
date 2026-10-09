import test from 'node:test';
import assert from 'node:assert/strict';
import { cofLayerRecord, composeCofFrame, decodeCof, decodeDcc } from '../play/assets/unit-formats.mjs';

function cofFixture() {
  // 2 layers, 2 frames, 1 direction: header + body prelude + layers + events + priorities.
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
  // var0=0, width=2 bits, height=2 bits, x/y/optional/coded=0 bits.
  for (const index of [0, 2, 2, 0, 0, 0, 0]) body.write(index, 4);
  body.write(3, 2); body.write(2, 2); body.write(0, 1); // 3x2 frame, top-down
  body.write(0, 20); // pixel-mask stream has zero bits
  for (let i = 0; i < 256; i++) body.write(i === 0 || i === 7 ? 1 : 0, 1);
  body.write(1, 4); body.write(0, 4); // one nonzero palette-stack entry, then terminator
  for (let i = 0; i < 6; i++) body.write(0, 1); // select stack entry 0 for the 3x2 cell
  const encoded = body.bytes();
  const out = new Uint8Array(19 + encoded.length);
  const view = new DataView(out.buffer);
  out.set([0x74, 6, 1], 0); view.setUint32(3, 1, true); view.setUint32(7, 1, true);
  view.setUint32(11, encoded.length, true); view.setUint32(15, 19, true); out.set(encoded, 19);
  return out;
}

test('COF decodes layers, events, and per-frame draw priorities', () => {
  const cof = decodeCof(cofFixture());
  assert.deepEqual([cof.layers, cof.framesPerDirection, cof.directions, cof.speed], [2, 2, 1, 128]);
  assert.equal(cof.layerRecords[0].weaponClass, 'HTH');
  assert.equal(cofLayerRecord(cof, cof.layerRecords[1].component), cof.layerRecords[1]);
  assert.equal(Object.keys(cof).includes('layerByComponent'), false, 'render lookup cache is not part of the decoded public shape');
  const first = { component: 5 }, duplicate = { component: 5 };
  assert.equal(cofLayerRecord({ layerRecords: [first, duplicate] }, 5), first, 'uncached COFs preserve array-find first-match behavior');
  assert.equal(cofLayerRecord({ layerRecords: [first, duplicate], layerByComponent: new Map([[5, first]]) }, 5), first,
    'cached COFs preserve the first duplicate record');
  assert.deepEqual(Array.from(cof.events), [0, 1]);
  assert.deepEqual(cof.priority, [[[1, 5], [5, 1]]]);
});

test('COF rejects truncation and priorities for absent components', () => {
  assert.throws(() => decodeCof(cofFixture().subarray(0, 40)), /size .* layout size/);
  const bad = cofFixture(); bad[48] = 9;
  assert.throws(() => decodeCof(bad), /absent component 9/);
});

test('DCC decodes strict container, frame, palette, and stream metadata', () => {
  const dcc = decodeDcc(dccFixture());
  assert.deepEqual([dcc.version, dcc.directions, dcc.framesPerDirection], [6, 1, 1]);
  assert.deepEqual(dcc.directionRecords[0].paletteEntries, [0, 7]);
  assert.deepEqual(dcc.directionRecords[0].frames[0], {
    variable0: 0, width: 3, height: 2, xOffset: 0, yOffset: 0,
    optionalBytes: 0, codedBytes: 0, bottomUp: false, cellsAcross: 1, cellsDown: 1,
    indexed: Uint8Array.of(7, 7, 7, 7, 7, 7), opaque: Uint8Array.of(1, 1, 1, 1, 1, 1),
  });
  assert.deepEqual(dcc.directionRecords[0].box, { minX: 0, minY: -1, maxX: 3, maxY: 1 });
});

test('DCC rejects signature, offsets, and truncated bitstreams', () => {
  const signature = dccFixture(); signature[0] = 0;
  assert.throws(() => decodeDcc(signature), /signature/);
  const offset = dccFixture(); new DataView(offset.buffer).setUint32(15, 18, true);
  assert.throws(() => decodeDcc(offset), /direction 0 offset/);
  assert.throws(() => decodeDcc(dccFixture().subarray(0, -1)), /truncated DCC direction 0/);
  assert.throws(() => decodeDcc(dccFixture(), { directions: [1] }), /outside 0\.\.0/);
});

test('COF composition applies its component priority and preserves transparency', () => {
  const cof = decodeCof(cofFixture());
  const layer = (xOffset, value) => ({ directions: 1, framesPerDirection: 2, directionRecords: [{ frames: [
    { xOffset, yOffset: 0, width: 2, height: 1, bottomUp: false, indexed: Uint8Array.of(value, value), opaque: Uint8Array.of(1, 1) },
    { xOffset, yOffset: 0, width: 2, height: 1, bottomUp: false, indexed: Uint8Array.of(value, value), opaque: Uint8Array.of(1, 1) },
  ] }] });
  const composed = composeCofFrame(cof, 0, 0, new Map([[1, layer(0, 3)], [5, layer(1, 9)]]));
  assert.deepEqual([composed.xOffset, composed.yOffset, composed.width, composed.height], [0, 0, 3, 1]);
  assert.deepEqual(Array.from(composed.indexed), [3, 9, 9]);
  assert.deepEqual(Array.from(composed.opaque), [1, 1, 1]);
});
