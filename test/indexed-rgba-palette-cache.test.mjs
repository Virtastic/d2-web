import assert from 'node:assert/strict';
import { test } from 'node:test';
import { indexedToRgba } from '../play/assets/unit-renderer.mjs';
test('packed palette cache sees in-place edits (aligned and unaligned views)', () => {
  const n = 1024, indexed = Uint8Array.from({ length: n }, (_, i) => i & 255), opaque = new Uint8Array(n).fill(1);
  for (const off of [0, 1]) {
    const palette = new Uint8Array(new ArrayBuffer(1032), off, 1024).fill(7);
    assert.equal(indexedToRgba(indexed, opaque, palette)[4], 7);
    palette[1023] = 9; palette[4] = 200;
    const out = indexedToRgba(indexed, opaque, palette);
    assert.equal(out[4], 200); assert.equal(out[255 * 4], 7);
  }
});
