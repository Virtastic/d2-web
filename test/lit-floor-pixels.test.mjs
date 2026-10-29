import { test } from 'node:test';
import assert from 'node:assert/strict';
import { litFloorPixels } from '../play/assets/session-terrain.mjs';

// The per-pixel form litFloor used before pf1 (allocating, branchy), kept here as the reference.
const ISO_SKIP = [14, 12, 10, 8, 6, 4, 2, 0, 2, 4, 6, 8, 10, 12, 14], ISO_RUN = [4, 8, 12, 16, 20, 24, 28, 32, 28, 24, 20, 16, 12, 8, 4];
function reference(px, blocks, grid, t, pal, tileX, tileY) {
  const idx = new Uint8Array(px.indices), opq = px.opaque;
  const P = (i, j) => grid[Math.min(7, j) * 8 + Math.min(7, i)];
  for (const b of blocks) {
    const x0 = b.gridX, y0 = b.gridY;
    const A = P(x0 + 1, y0 + 1), B = P(x0 + 2, y0 + 1), C = P(x0 + 1, y0 + 2), D = P(x0 + 2, y0 + 2);
    const flat = Math.abs(B - A) + Math.abs(C - A) + Math.abs(D - B) < 10;
    const E = [(P(x0, y0 + 2) + C + A + P(x0, y0 + 1)) >> 2, (P(x0 + 2, y0) + B + P(x0 + 1, y0) + A) >> 2,
      (P(x0 + 3, y0 + 2) + C + P(x0 + 3, y0 + 1) + B) >> 2, (P(x0 + 1, y0 + 3) + P(x0 + 2, y0 + 3) + D + C) >> 2];
    const iso = b.format === 1;
    for (let r = 0; r < 15; r++) {
      const topclip = iso && tileY !== undefined ? Math.max(0, -(tileY + b.y - px.originY)) : 0;
      if (iso && r < topclip) continue;
      const rr = r - topclip;
      const L = ((E[0] << 4) + rr * (E[3] - E[0])) >> 7, R = ((E[1] << 4) + rr * (E[2] - E[1])) >> 7;
      const skip = iso ? ISO_SKIP[r] : 0, run = iso ? ISO_RUN[r] : 32;
      const leftclip = tileX === undefined ? 0 : Math.max(0, Math.min(run, 0 - (tileX + b.x - px.originX + skip)));
      for (let col = 0; col < run; col++) {
        const x = skip + col, q = (b.y + r - px.originY) * px.width + (b.x + x - px.originX);
        if (b.y + r - px.originY >= px.height || !b.pixels.written[r * 32 + x]) continue;
        const lvl = flat ? A >> 3 : L + (((R - L) * (x - leftclip)) >> 5);
        idx[q] = t[1024 + lvl * 256 + b.pixels.indices[r * 32 + x]];
      }
    }
  }
  const out = new Uint32Array(idx.length);
  for (let q = 0; q < idx.length; q++) if (opq[q]) out[q] = pal[idx[q]];
  return out;
}

test('litFloorPixels is byte-identical to the per-pixel reference on random floors', () => {
  let s = 12345; const rnd = (n) => (s = (s * 1103515245 + 12345) >>> 0, (s >>> 8) % n);
  const t = new Uint8Array(1024 + 64 * 256); for (let i = 0; i < t.length; i++) t[i] = rnd(256);
  const pal = new Uint32Array(256); for (let i = 0; i < 256; i++) pal[i] = (0xff000000 | rnd(1 << 24)) >>> 0;
  for (let n = 0; n < 60; n++) {
    const width = 96 + rnd(3) * 32, height = 48 + rnd(3) * 16, originX = rnd(40), originY = rnd(30);
    const px = { width, height, originX, originY, indices: Uint8Array.from({ length: width * height }, () => rnd(256)), opaque: Uint8Array.from({ length: width * height }, () => rnd(4) ? 1 : 0) };
    const blocks = Array.from({ length: 1 + rnd(6) }, () => ({
      gridX: rnd(6), gridY: rnd(6), x: rnd(width - 32), y: rnd(height - 15) - rnd(8), format: rnd(2),
      pixels: { written: Uint8Array.from({ length: 480 }, () => rnd(5) ? 1 : 0), indices: Uint8Array.from({ length: 480 }, () => rnd(256)) } }));
    const grid = Array.from({ length: 64 }, () => (n % 2 ? 20 + rnd(6) : rnd(64)) & 0x3f);   // flat-ish and gouraud grids
    for (const [tx, ty] of [[undefined, undefined], [100, 100], [-20, 50], [50, -6]]) {
      const out = new Uint32Array(width * height).fill(0x12345678);   // dirty, as the reused litScratch is
      litFloorPixels(px, blocks, grid, t, pal, tx, ty, out);
      assert.deepEqual(out, reference(px, blocks, grid, t, pal, tx, ty), `case ${n} at ${tx},${ty}`);
    }
  }
});
