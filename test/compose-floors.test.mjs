// composeFloors (play/assets/session-terrain.mjs) writes the floor pass as one putImageData. It must leave exactly the
// pixels a black fillRect + one source-over drawImage per binary-alpha floor, in order, leave -- clipped at every edge.
import test from 'node:test';
import assert from 'node:assert/strict';

globalThis.ImageData ??= class { constructor(w, h) { this.width = w; this.height = h; this.data = new Uint8ClampedArray(w * h * 4); } };
const { opaqueRuns, composeFloors } = await import('../play/assets/session-terrain.mjs');

const tile = (w, h, seed) => {   // a diamond with holes, opaque pixels a per-tile colour
  const px = new Uint32Array(w * h);
  for (let y = 0; y < h; y++) for (let x = 0; x < w; x++) {
    const inside = Math.abs(x - w / 2) / (w / 2) + Math.abs(y - h / 2) / (h / 2) <= 1;
    if (inside && (x * 7 + y * 3 + seed) % 11) px[y * w + x] = (0xff000000 | (seed * 0x10101 + x * 0x100 + y)) >>> 0;
  }
  return px;
};

test('composeFloors equals fill + in-order source-over of binary-alpha floors, clipped at all four edges', () => {
  const W = 300, H = 170, BLACK = 0xff000000;
  const floors = [[-50, -30], [100, 40], [140, 60], [250, 120], [20, 130], [-159, 0], [299, 169], [400, 10]].map(([x, y], i) => {
    const w = 160, h = 79 + (i % 2) * 9, px = tile(w, h, i + 1);
    return { r: opaqueRuns(px, w, h), x, y, px, w, h };
  });
  const want = new Uint32Array(W * H).fill(BLACK);
  for (const f of floors) for (let y = 0; y < f.h; y++) for (let x = 0; x < f.w; x++) {
    const v = f.px[y * f.w + x], X = f.x + x, Y = f.y + y;
    if (v >>> 24 && X >= 0 && X < W && Y >= 0 && Y < H) want[Y * W + X] = v;
  }
  let put = null;
  composeFloors({ putImageData(img, x, y) { put = { img, x, y }; } }, W, H, BLACK, floors);
  assert.deepEqual([put.x, put.y, put.img.width, put.img.height], [0, 0, W, H]);
  assert.deepEqual(new Uint32Array(put.img.data.buffer), want);
});

test('opaqueRuns refuses a partially transparent pixel', () => {
  const px = tile(16, 8, 3); px[20] = 0x80112233;
  assert.equal(opaqueRuns(px, 16, 8), null);
});
