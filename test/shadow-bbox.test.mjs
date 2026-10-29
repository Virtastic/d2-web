import test from 'node:test';
import assert from 'node:assert/strict';
import { createSessionTerrain, worldToScreen } from '../play/assets/session-terrain.mjs';

const PAL = 'data\\global\\palette\\ACT1\\pal.dat', LIB = 'data\\global\\tiles\\s.dt1';
const W = 64, H = 48, TW = 20, TH = 12;
// A shadow cel with a small opaque patch in the middle of a mostly transparent rectangle.
const indices = new Uint8Array(TW * TH), opaque = new Uint8Array(TW * TH);
for (let y = 4; y < 8; y++) for (let x = 6; x < 13; x++) { indices[y * TW + x] = 1 + ((x + y) % 5); opaque[y * TW + x] = 1; }
const decodeDt1 = () => ({ tiles: [{ orientation: 13, mainIndex: 0, subIndex: 0, rarity: 0, blocks: [{ x: 0, y: 0, format: 1 }],
  pixels: { width: TW, height: TH, originX: 0, originY: 0, indices, opaque } }] });
const decodeDatPalette = () => Uint8Array.from({ length: 1024 }, (_, i) => (i % 4 === 3 ? 255 : (i * 37) & 255));
const pl2 = Uint8Array.from({ length: 1024 + 49 * 256 + 256 + 256 * 256 }, (_, i) => (i * 131 + 7) & 255);
const tile = { room: 0, layer: 1, ordinal: 0, orientation: 13, mainIndex: 0, subIndex: 0, frameIndex: 0, worldX: 40, worldY: 40,
  screenX: worldToScreen(40, 40).x + 68, screenY: worldToScreen(40, 40).y - 64, width: TW, height: TH, flags: 0, librarySlot: 0, libraryIndex: 0, entryHint: -1, pop: 0 };
const start = () => new Uint8ClampedArray(W * H * 4).map((_, i) => (i * 29 + 3) & 255);

function run() {
  const px = start(), reads = [];
  const ctx = { canvas: { width: W, height: H }, globalAlpha: 1, save() {}, restore() {}, drawImage() {},
    getImageData(x, y, w, h) { reads.push(w * h); const out = new Uint8ClampedArray(w * h * 4);
      for (let r = 0; r < h; r++) out.set(px.subarray(((y + r) * W + x) * 4, ((y + r) * W + x + w) * 4), r * w * 4); return { data: out, width: w, height: h }; },
    putImageData(img, x, y) { for (let r = 0; r < img.height; r++) px.set(img.data.subarray(r * img.width * 4, (r + 1) * img.width * 4), ((y + r) * W + x) * 4); } };
  globalThis.ImageData ??= class { constructor(d, w, h) { this.data = d; this.width = w; this.height = h; } };
  const doc = { createElement: () => { const c = { width: 0, height: 0 }; c.getContext = () => c.ctx ??= { putImageData(i) { c.img = i; }, getImageData: () => c.img }; return c; } };
  const t = createSessionTerrain({ read: (p) => (p === LIB ? new Uint8Array(3) : p.endsWith('.pl2') ? pl2 : new Uint8Array(1024)), decodeDt1, decodeDatPalette, palettePath: PAL, documentObject: doc });
  const list = [];
  globalThis.__d2DrawList = list;
  try { t.draw(ctx, [tile], [LIB], { x: 40, y: 40 }, W, H, [], null, W / 2); } finally { globalThis.__d2DrawList = undefined; }
  return { px, reads, list };
}

test('a shadow tile reads back and blends only its non-transparent box', () => {
  const a = run(), b = run();
  assert.deepEqual(a.px, b.px);
  assert.equal(a.reads.length, 1);
  assert.ok(a.reads[0] <= 7 * 4, `read ${a.reads[0]} pixels`);
  const before = start();
  let changed = 0; for (let i = 0; i < before.length; i++) if (before[i] !== a.px[i]) changed++;
  assert.ok(changed > 0 && changed <= 7 * 4 * 4);
});
