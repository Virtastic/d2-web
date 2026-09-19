import { test } from 'node:test';
import assert from 'node:assert/strict';
import { hashString, buildMpq } from '../tools/mkmpq.mjs';

// The two table keys are published constants (StormLib SBaseCommon.cpp); if the crypt table or
// the hash loop is off by anything, these are the first things to go.
test('MPQ hash keys match StormLib', () => {
  assert.equal(hashString('(hash table)', 3), 0xC3AF3770);
  assert.equal(hashString('(block table)', 3), 0xEC83B3A3);
});

test('buildMpq lays out header/data/tables', () => {
  const data = new TextEncoder().encode('hello');
  const mpq = buildMpq([{ name: 'a.txt', data }]);
  const dv = new DataView(mpq.buffer);
  assert.equal(String.fromCharCode(...mpq.subarray(0, 3)), 'MPQ');
  assert.equal(dv.getUint32(8, true), mpq.length);        // archive size
  assert.equal(dv.getUint32(16, true), 32 + 5);           // hash table right after data
  assert.equal(dv.getUint32(20, true), 32 + 5 + 4 * 16);  // block table after 4 hash entries
  assert.equal(new TextDecoder().decode(mpq.subarray(32, 37)), 'hello');
});

// Encrypt/decrypt are the same loop with the key chain fed by the PLAINTEXT dword; a writer
// that fed it the ciphertext (the first version of this file) produced tables StormLib read as
// garbage. Known-answer: StormLib's own tables decrypt to exactly this.
test('table encryption is StormLib-compatible', async () => {
  const { buildMpq: b, hashString: h } = await import('../tools/mkmpq.mjs');
  const mpq = b([{ name: 'a', data: new Uint8Array(1) }]);
  const dv = new DataView(mpq.buffer);
  const hashPos = dv.getUint32(16, true);
  const t = new Uint32Array(mpq.buffer.slice(hashPos, hashPos + 64));
  // decrypt (StormLib DecryptMpqBlock)
  let s1 = h('(hash table)', 3), s2 = 0xEEEEEEEE;
  const { CRYPT } = await import('../tools/mkmpq.mjs');
  for (let i = 0; i < t.length; i++) {
    s2 = (s2 + CRYPT[0x400 + (s1 & 0xFF)]) >>> 0;
    t[i] = (t[i] ^ ((s1 + s2) >>> 0)) >>> 0;
    s1 = ((((~s1) << 0x15) + 0x11111111) | (s1 >>> 0x0B)) >>> 0;
    s2 = (t[i] + s2 + (s2 << 5) + 3) >>> 0;
  }
  const slot = h('a', 0) & 3;
  assert.equal(t[slot * 4], h('a', 1)); assert.equal(t[slot * 4 + 1], h('a', 2)); assert.equal(t[slot * 4 + 3], 0);
  for (let i = 0; i < 4; i++) if (i !== slot) assert.equal(t[i * 4 + 3], 0xFFFFFFFF);
});
