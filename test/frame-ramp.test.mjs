// F238: the display ramp's 16-bit LUT path must be byte-identical to the per-channel byte loop it
// replaced. The canonical table lives in play/gameplay-shell.mjs (`DISPLAY_RAMP`, built from
// RETAIL_RAMP_6BIT); this file pins the PACKING -- a pixel is the low (R,G) pair and the high
// (B, alpha) pair through 65536-entry Uint16 tables -- against the byte loop, on the real table and
// on a random one, for every value and for random pixels. It cannot import the shell (it runs DOM
// code at load), so if the byte loop and this test ever disagree on the real table, the frame rows
// are the integration guard; this is the algorithm guard, and it carries its own deliberate failure.
import { test } from 'node:test';
import assert from 'node:assert/strict';

const RETAIL_RAMP_6BIT = [
  0, 6, 12, 18, 24, 31, 37, 43, 49, 55, 62, 68, 74, 80, 86, 93, 99, 105, 111, 117, 123, 130,
  136, 142, 148, 154, 161, 167, 173, 179, 185, 191, 198, 204, 210, 216, 222, 228, 235, 241,
  247, 253, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254, 254,
  254, 254, 254, 254, 254, 254,
];
const DISPLAY_RAMP = Uint8ClampedArray.from({ length: 256 }, (_, v) =>
  v === 255 ? 255 : v % 4 === 0 ? RETAIL_RAMP_6BIT[v >> 2] : Math.min(254, Math.floor(v * 155 / 100)));

const LE = new Uint8Array(new Uint32Array([1]).buffer)[0] === 1;

function tables(ramp) {
  const lo = new Uint16Array(65536), hi = new Uint16Array(65536);
  for (let x = 0; x < 65536; x++) { lo[x] = ramp[x & 0xff] | (ramp[(x >> 8) & 0xff] << 8); hi[x] = ramp[x & 0xff] | (x & 0xff00); }
  return { lo, hi };
}
// The byte loop: three 8-bit lookups per pixel, byte 3 untouched (play/gameplay-shell.mjs before F238).
function byBytes(ramp, src) {
  const d = new Uint8ClampedArray(src);
  for (let i = 0; i < d.length; i += 4) { d[i] = ramp[d[i]]; d[i + 1] = ramp[d[i + 1]]; d[i + 2] = ramp[d[i + 2]]; }
  return d;
}
// The 16-bit path F238 ships (little-endian only; the shell falls back to the byte loop otherwise).
function byWords(ramp, src) {
  const { lo, hi } = tables(ramp), d = new Uint8ClampedArray(src), d32 = new Uint32Array(d.buffer);
  for (let i = 0; i < d32.length; i++) { const p = d32[i]; d32[i] = lo[p & 0xffff] | (hi[p >>> 16] << 16); }
  return d;
}
const eq = (a, b) => { if (a.length !== b.length) return false; for (let i = 0; i < a.length; i++) if (a[i] !== b[i]) return false; return true; };

test('the 16-bit ramp path reproduces the byte loop', { skip: !LE && 'little-endian only' }, () => {
  // Every (R,G,B,alpha) combination is 2^32; the pair boundaries plus every single-channel value and
  // random pixels cover the packing: the low table sees all 65536 (R,G) pairs, the high table all
  // 65536 (B,alpha) pairs.
  const pair = new Uint8ClampedArray(65536 * 4);
  for (let x = 0; x < 65536; x++) { pair[x * 4] = x & 0xff; pair[x * 4 + 1] = (x >> 8) & 0xff; pair[x * 4 + 2] = x & 0xff; pair[x * 4 + 3] = (x >> 8) & 0xff; }
  assert.ok(eq(byBytes(DISPLAY_RAMP, pair), byWords(DISPLAY_RAMP, pair)), 'all 16-bit pairs must round-trip identically');

  const rnd = new Uint8ClampedArray(40000);
  for (let i = 0; i < rnd.length; i++) rnd[i] = (Math.random() * 256) | 0;
  assert.ok(eq(byBytes(DISPLAY_RAMP, rnd), byWords(DISPLAY_RAMP, rnd)), 'random pixels must round-trip identically');
});

test('the identity holds for any 8-bit LUT, and a wrong one is caught', { skip: !LE && 'little-endian only' }, () => {
  const other = Uint8ClampedArray.from({ length: 256 }, (_, v) => (v * 37 + 11) & 0xff);
  const src = new Uint8ClampedArray(40000);
  for (let i = 0; i < src.length; i++) src[i] = (Math.random() * 256) | 0;
  assert.ok(eq(byBytes(other, src), byWords(other, src)));
  // CONTROL: a deliberately wrong table must NOT match, so a comparison that cannot fail is caught.
  const wrong = Uint8ClampedArray.from(other); wrong[1] = (wrong[1] + 1) & 0xff;
  assert.ok(!eq(byWords(other, src), byWords(wrong, src)), 'the check must be able to fail');
});
